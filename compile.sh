cmake ../ \
-DBUILD_DOCUMENTATION=ON \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/var/remote/software/paraview/4.2.0 \
-DPARAVIEW_ENABLE_FFMPEG=ON \
-DPARAVIEW_ENABLE_PYTHON=ON \
-DPARAVIEW_INSTALL_DEVELOPMENT_FILES=ON \
-DPARAVIEW_USE_MPI=ON \
-DVTK_SMP_IMPLEMENTATION_TYPE=TBB \
-DModule_vtkFiltersParallelGeometry=ON \
-DModule_vtkFiltersReebGraph=ON \
-DModule_vtkFiltersSMP=ON \
-DModule_vtkGUISupportQtOpenGL=ON \
-DModule_vtkGeovisCore=ON \
-DModule_vtkIOGDAL=ON \
-DModule_vtkIOGeoJSON=ON \
-DModule_vtkIOVideo=ON \
-DModule_vtkIOXdmf3=ON \
-DModule_vtkImagingMath=ON \
-DModule_vtkImagingStatistics=ON \
-DModule_vtkImagingStencil=ON \
-DModule_vtkInfovisBoost=ON \
-DModule_vtkInfovisBoostGraphAlgorithms=ON \
-DModule_vtkInfovisLayout=ON \
-DModule_vtkInfovisParallel=ON \
-DModule_vtkRenderingContextOpenGL2=ON \
-DModule_vtkRenderingOpenGL2=ON \
-DModule_vtkRenderingImage=ON \
-DModule_vtkRenderingQt=ON \
-DModule_vtkViewsGeovis=ON \
-DModule_vtkViewsInfovis=ON \
-DModule_vtkWebApplications=ON \
-DModule_vtkViewsQt=ON \
-DModule_vtkWebInstall=ON \
-DModule_vtkglew=ON \
-DModule_vtkxdmf3=ON \
-DOPENGL_gl_LIBRARY=/usr/lib64/nvidia/libGL.so \
-DPARAVIEW_BUILD_CATALYST_ADAPTORS=ON \
-DPARAVIEW_BUILD_PLUGIN_VRPlugin=ON \
-DPARAVIEW_BUILD_WEB_DOCUMENTATION=ON \
-DVTK_Group_Imaging=ON \
-DVTK_Group_MPI=ON \
-DVTK_Group_ParaViewQt=ON \
-DVTK_Group_Qt=ON \
-DVTK_Group_Rendering=ON \
-DVTK_Group_Views=ON \
-DVTK_Group_Web=ON \
-DBoost_INCLUDE_DIR=/var/remote/software/boost/1.55.0-mpich-3.1/include
