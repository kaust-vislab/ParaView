#!/bin/bash

# wget ftp://ftp.freedesktop.org/pub/mesa/10.6.1/mesa-10.6.1.tar.gz

make -j4 distclean # if in an existing build

autoreconf -fi

./configure \
    CXXFLAGS="-O2 -g -DDEFAULT_SOFTWARE_DEPTH_BITS=31" \
    CFLAGS="-O2 -g -DDEFAULT_SOFTWARE_DEPTH_BITS=31" \
    --disable-xvmc \
    --disable-glx \
    --disable-dri \
    --with-dri-drivers="" \
    --with-gallium-drivers="swrast" \
    --enable-texture-float \
    --disable-shared-glapi \
    --disable-egl \
    --with-egl-platforms="" \
    --enable-gallium-osmesa \
    --enable-gallium-llvm=yes \
    --enable-llvm-shared-libs \
    --prefix=/home/holstgr/software/mesa/10.6.1/llvmpipe

make -j2
make -j4 install
