/*=========================================================================

  Program:   Visualization Toolkit
  Module:    pqMultiSliceView.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "pqMultiSliceView.h"

#include <QtCore>
#include <QGridLayout>
#include <QtGui>

#include "QVTKWidget.h"

#include "pqDataRepresentation.h"
#include "pqMultiSliceAxisWidget.h"
#include "pqRepresentation.h"

#include "vtkAxis.h"
#include "vtkChartXY.h"
#include "vtkDataRepresentation.h"
#include "vtkMath.h"
#include "vtkPlot.h"
#include "vtkPVDataInformation.h"
#include "vtkPVRenderView.h"
#include "vtkSMMultiSliceViewProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkSMRepresentationProxy.h"
#include "vtkView.h"

#define MULTI_SLICE_AXIS_THIKNESS 80
#define MULTI_SLICE_AXIS_ACTIVE_SIZE 20
#define MULTI_SLICE_AXIS_EDGE_MARGIN 10

//-----------------------------------------------------------------------------
pqMultiSliceView::pqMultiSliceView(
    const QString& viewType, const QString& group, const QString& name,
    vtkSMViewProxy* viewProxy, pqServer* server, QObject* p)
  : pqRenderView(viewType, group, name, viewProxy, server, p)
{
  // Nothing is changing anything.
  this->UserIsInteracting = false;

  // When data change make sure the bounds are updated
  QObject::connect(this, SIGNAL(updateDataEvent()),
                   this, SLOT(updateAxisBounds()));

  // Make sure if the proxy change by undo-redo, we properly update the Qt UI
  viewProxy->GetProperty("XSlicesValues")->AddObserver(
        vtkCommand::ModifiedEvent, this,
        &pqMultiSliceView::updateViewModelCallBack);
  viewProxy->GetProperty("YSlicesValues")->AddObserver(
        vtkCommand::ModifiedEvent, this,
        &pqMultiSliceView::updateViewModelCallBack);
  viewProxy->GetProperty("ZSlicesValues")->AddObserver(
        vtkCommand::ModifiedEvent, this,
        &pqMultiSliceView::updateViewModelCallBack);

  for(int i=0;i<9;++i)
    {
    this->NormalValuesHolder[i] = this->OriginValuesHolder[i] = 0;
    }
}

//-----------------------------------------------------------------------------
pqMultiSliceView::~pqMultiSliceView()
{
}
//-----------------------------------------------------------------------------
QWidget* pqMultiSliceView::createWidget()
{
  // Get the internal widget that we want to decorate
  this->InternalWidget = qobject_cast<QVTKWidget*>(this->pqRenderView::createWidget());

  // Build the widget hierarchy
  QWidget* container = new QWidget();
  container->setStyleSheet("background-color: white");
  container->setAutoFillBackground(true);

  QGridLayout* gridLayout = new QGridLayout(container);
  this->InternalWidget->setParent(container);

  // Init top axis
  this->AxisX = new pqMultiSliceAxisWidget(container);
  this->AxisX->setAxisType(vtkAxis::LEFT);
  this->AxisX->setRange(-10,10);
  this->AxisX->setTitle("X");
  this->AxisX->SetEdgeMargin(MULTI_SLICE_AXIS_EDGE_MARGIN);
  this->AxisX->SetActiveSize(MULTI_SLICE_AXIS_ACTIVE_SIZE);
  this->AxisX->setFixedWidth(MULTI_SLICE_AXIS_THIKNESS);
  this->AxisX->renderView();

  this->AxisY = new pqMultiSliceAxisWidget(container);
  this->AxisY->setAxisType(vtkAxis::TOP);
  this->AxisY->setRange(-10,10);
  this->AxisY->setTitle("Y");
  this->AxisY->SetEdgeMargin(MULTI_SLICE_AXIS_EDGE_MARGIN);
  this->AxisY->SetActiveSize(MULTI_SLICE_AXIS_ACTIVE_SIZE);
  this->AxisY->setFixedHeight(MULTI_SLICE_AXIS_THIKNESS - 4);
  this->AxisY->renderView();

  this->AxisZ = new pqMultiSliceAxisWidget(container);
  this->AxisZ->setAxisType(vtkAxis::RIGHT);
  this->AxisZ->setRange(-10,10);
  this->AxisZ->setTitle("Z");
  this->AxisZ->SetEdgeMargin(MULTI_SLICE_AXIS_EDGE_MARGIN);
  this->AxisZ->SetActiveSize(MULTI_SLICE_AXIS_ACTIVE_SIZE);
  this->AxisZ->setFixedWidth(MULTI_SLICE_AXIS_THIKNESS);
  this->AxisZ->renderView();

  gridLayout->addWidget(this->AxisY, 0, 1);  // TOP
  gridLayout->addWidget(this->AxisX, 1, 0); // LEFT
  gridLayout->addWidget(this->AxisZ, 1, 2); // RIGHT
  gridLayout->addWidget(this->InternalWidget,1,1);
  gridLayout->setContentsMargins(0,0,0,0);
  gridLayout->setSpacing(0);

  // Properly do the binding between the proxy and the 3D widget
  vtkSMRenderViewProxy* renModule = this->getRenderViewProxy();
  if (this->InternalWidget && renModule)
    {
    this->InternalWidget->SetRenderWindow(renModule->GetRenderWindow());
    }

  // Make sure we are aware when the slice positions changes
  QObject::connect(this->AxisX, SIGNAL(modelUpdated()),
                   this, SLOT(updateSlices()));
  QObject::connect(this->AxisY, SIGNAL(modelUpdated()),
                   this, SLOT(updateSlices()));
  QObject::connect(this->AxisZ, SIGNAL(modelUpdated()),
                   this, SLOT(updateSlices()));

  // Attach click listener to slice marks
  QObject::connect(this->AxisX, SIGNAL(markClicked(int,int,double)),
                   this, SLOT(onSliceClicked(int,int,double)));
  QObject::connect(this->AxisY, SIGNAL(markClicked(int,int,double)),
                   this, SLOT(onSliceClicked(int,int,double)));
  QObject::connect(this->AxisZ, SIGNAL(markClicked(int,int,double)),
                   this, SLOT(onSliceClicked(int,int,double)));

  // Make sure the UI reflect the proxy state
  this->updateViewModelCallBack(NULL, 0, NULL);

  return container;
}

//-----------------------------------------------------------------------------
void pqMultiSliceView::updateAxisBounds()
{
  double bounds[6];
  vtkSMMultiSliceViewProxy* viewPxy = vtkSMMultiSliceViewProxy::SafeDownCast(
    this->getProxy());
  Q_ASSERT(viewPxy);

  viewPxy->GetDataBounds(bounds);
  if (vtkMath::AreBoundsInitialized(bounds))
    {
    this->AxisX->setRange(bounds[0], bounds[1]);
    this->AxisY->setRange(bounds[2], bounds[3]);
    this->AxisZ->setRange(bounds[4], bounds[5]);
    }
  else
    {
    this->AxisX->setRange(-10, 10);
    this->AxisY->setRange(-10, 10);
    this->AxisZ->setRange(-10, 10);
    }

  const char* xlabel = viewPxy->GetAxisLabel(0);
  const char* ylabel = viewPxy->GetAxisLabel(1);
  const char* zlabel = viewPxy->GetAxisLabel(2);
  this->AxisX->setTitle(xlabel? xlabel : "X");
  this->AxisY->setTitle(ylabel? ylabel : "Y");
  this->AxisZ->setTitle(zlabel? zlabel : "Z");

  // Make sure we render the new range
  this->AxisX->renderView();
  this->AxisY->renderView();
  this->AxisZ->renderView();
}

//-----------------------------------------------------------------------------
void pqMultiSliceView::updateSlices()
{
  // Make sure we know that the origin of the changes come from the UI
  this->UserIsInteracting = true;

  int nbSliceX = 0;
  const double* sliceX = this->AxisX->getVisibleSlices(nbSliceX);
  int nbSliceY = 0;
  const double* sliceY = this->AxisY->getVisibleSlices(nbSliceY);
  int nbSliceZ = 0;
  const double* sliceZ = this->AxisZ->getVisibleSlices(nbSliceZ);

  // Update view which will notify its representation that care about those info
  vtkSMViewProxy* view = this->getViewProxy();
  vtkSMPropertyHelper(view, "XSlicesValues").Set(sliceX, nbSliceX);
  vtkSMPropertyHelper(view, "YSlicesValues").Set(sliceY, nbSliceY);
  vtkSMPropertyHelper(view, "ZSlicesValues").Set(sliceZ, nbSliceZ);
  view->UpdateVTKObjects();

  this->render();

  // Get back to an idle mode
  this->UserIsInteracting = false;

  // Notify that the slices location have changed
  emit slicesChanged();
}

//-----------------------------------------------------------------------------
void pqMultiSliceView::updateViewModelCallBack(vtkObject*,unsigned long, void*)
{
  if(this->UserIsInteracting)
    {
    return; // We don't want to update our data model
    }

  std::vector<double> xSlices =
      vtkSMPropertyHelper(this->getViewProxy(), "XSlicesValues").GetDoubleArray();
  std::vector<double> ySlices =
      vtkSMPropertyHelper(this->getViewProxy(), "YSlicesValues").GetDoubleArray();
  std::vector<double> zSlices =
      vtkSMPropertyHelper(this->getViewProxy(), "ZSlicesValues").GetDoubleArray();

  assert("The maximum number of slice can not be bigger than 255" &&
         ( 255 >
           std::max(
             std::max(
               xSlices.size(), ySlices.size()), zSlices.size())));

  // Build a tmp array of visibility set to true
  bool visibility[255];
  memset(visibility, true, 255);

  double emptyDouble = 0;
  if( xSlices.size() > 0 )
    {
    this->AxisX->updateSlices(&xSlices[0], visibility, static_cast<int>(xSlices.size()));
    }
  else // Reset slices
    {
    this->AxisX->updateSlices(&emptyDouble, visibility, 0);
    }

  if( ySlices.size() > 0 )
    {
    this->AxisY->updateSlices(&ySlices[0], visibility, static_cast<int>(ySlices.size()));
    }
  else // Reset slices
    {
    this->AxisY->updateSlices(&emptyDouble, visibility, 0);
    }

  if( zSlices.size() > 0 )
    {
    this->AxisZ->updateSlices(&zSlices[0], visibility, static_cast<int>(zSlices.size()));
    }
  else // Reset slices
    {
    this->AxisZ->updateSlices(&emptyDouble, visibility, 0);
    }

  this->render();
}
//-----------------------------------------------------------------------------
const double* pqMultiSliceView::GetSlices(int axisIndex, int &numberOfSlices)
{
  switch(axisIndex)
    {
  case 0:
    return this->AxisX->getVisibleSlices(numberOfSlices);
  case 1:
    return this->AxisY->getVisibleSlices(numberOfSlices);
  case 2:
    return this->AxisZ->getVisibleSlices(numberOfSlices);
    }

  // Invalid axis
  numberOfSlices = 0;
  return NULL;
}

//-----------------------------------------------------------------------------
const double* pqMultiSliceView::GetSliceNormal(int axisIndex)
{
  if(axisIndex < 0 || axisIndex > 2)
    {
    return NULL;
    }

  const char* propertyNames[3] = {"XSlicesNormal", "YSlicesNormal", "ZSlicesNormal"};
  std::vector<double> values =
      vtkSMPropertyHelper(this->getViewProxy(), propertyNames[axisIndex]).GetDoubleArray();
  for(int i=0;i<3;++i)
    {
    this->NormalValuesHolder[i+ (3*axisIndex)] = values[i];
    }
  return &this->NormalValuesHolder[(3*axisIndex)];
}

//-----------------------------------------------------------------------------
const double* pqMultiSliceView::GetSliceOrigin(int axisIndex)
{
  if(axisIndex < 0 || axisIndex > 2)
    {
    return NULL;
    }

  const char* propertyNames[3] = {"XSlicesOrigin", "YSlicesOrigin", "ZSlicesOrigin"};
  std::vector<double> values =
      vtkSMPropertyHelper(this->getViewProxy(), propertyNames[axisIndex]).GetDoubleArray();
  for(int i=0;i<3;++i)
    {
    this->OriginValuesHolder[i+ (3*axisIndex)] = values[i];
    }
  return &this->OriginValuesHolder[(3*axisIndex)];
}

//-----------------------------------------------------------------------------
void pqMultiSliceView::onSliceClicked(int button, int modifier, double value)
{
  QObject* senderObject = QObject::sender();
  if(senderObject == this->AxisX.data())
    {
    emit sliceClicked(0, value, button, modifier);
    }
  else if(senderObject == this->AxisY.data())
    {
    emit sliceClicked(1, value, button, modifier);
    }
  else if(senderObject == this->AxisZ.data())
    {
    emit sliceClicked(2, value, button, modifier);
    }
}
//-----------------------------------------------------------------------------
void pqMultiSliceView::setCursor(const QCursor &c)
{
  if(this->InternalWidget)
    {
    this->InternalWidget->setCursor(c);
    }
}
//-----------------------------------------------------------------------------
QVTKWidget* pqMultiSliceView::getInternalWidget()
{
  return this->InternalWidget;
}

//-----------------------------------------------------------------------------
bool pqMultiSliceView::getOutlineVisibility()
{
  return vtkSMPropertyHelper(this->getViewProxy(), "ShowOutline").GetAsInt() != 0;
}

//-----------------------------------------------------------------------------
void pqMultiSliceView::setOutlineVisibility(bool visible)
{
  vtkSMPropertyHelper(this->getViewProxy(), "ShowOutline").Set(visible ? 1 : 0);
  this->getViewProxy()->UpdateVTKObjects();
}
