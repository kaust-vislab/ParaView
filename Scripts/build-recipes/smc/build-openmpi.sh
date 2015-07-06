#!/bin/bash

# git clone --recursive ssh://git@github.com/kaust-vislab/ParaView.git

## Patch: VTK/ThirdParty/xdmf3/vtkxdmf3/core/CMakeLists.txt
#if(NOT Boost_FOUND)
#find_package(Boost REQUIRED)
#endif()

# NOTE: May not need (load-env.sh may handle this now)
MESA_INSTALL_PREFIX=/home/holstgr/software/mesa/10.6.1/llvmpipe

# NOTE: Configuration and build finds wrong system versions of Python libs
# Use CMAKE_PREFIX_PATH to limit search to existing modules for Python and OSMesa
# See: http://www.cmake.org/Wiki/CMake_Useful_Variables

cmake ../ParaView \
-DCMAKE_C_COMPILER=`which gcc` \
-DCMAKE_CXX_COMPILER=`which g++` \
-DBUILD_DOCUMENTATION=OFF \
-DPARAVIEW_BUILD_QT_GUI=OFF \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/home/holstgr/software/paraview/4.3.1 \
-DPARAVIEW_ENABLE_PYTHON=ON \
-DPARAVIEW_INSTALL_DEVELOPMENT_FILES=ON \
-DPARAVIEW_USE_MPI=ON \
-DVTK_Group_Imaging=ON \
-DVTK_Group_MPI=ON \
-DVTK_Group_Rendering=ON \
-DVTK_Group_Views=ON \
-DVTK_Group_Web=ON \
-DVTK_SMP_IMPLEMENTATION_TYPE=Sequential \
-DVTK_USE_X=OFF \
-DVTK_USE_OFFSCREEN=ON \
-DVTK_OPENGL_HAS_OSMESA=ON \
-DOPENGL_INCLUDE_DIR=${MESA_INSTALL_PREFIX}/include \
-DOPENGL_gl_LIBRARY=${MESA_INSTALL_PREFIX}/lib/libOSMesa.so \
-DOSMESA_INCLUDE_DIR=${MESA_INSTALL_PREFIX}/include \
-DOSMESA_LIBRARY=${MESA_INSTALL_PREFIX}/lib/libOSMesa.so \
-DModule_vtkFiltersParallelGeometry=ON \
-DModule_vtkFiltersReebGraph=ON \
-DModule_vtkFiltersSMP=ON \
-DModule_vtkGeovisCore=ON \
-DModule_vtkIOGeoJSON=ON \
-DModule_vtkIOVideo=ON \
-DModule_vtkIOXdmf3=ON \
-DModule_vtkImagingMath=ON \
-DModule_vtkImagingStatistics=ON \
-DModule_vtkImagingStencil=ON \
-DModule_vtkInfovisBoostGraphAlgorithms=ON \
-DModule_vtkInfovisLayout=ON \
-DModule_vtkRenderingImage=ON \
-DModule_vtkViewsGeovis=ON \
-DModule_vtkViewsInfovis=ON \
-DModule_vtkWebApplications=ON \
-DModule_vtkWebInstall=ON \
-DModule_vtkxdmf3=ON \
-DPARAVIEW_BUILD_CATALYST_ADAPTORS=ON \
