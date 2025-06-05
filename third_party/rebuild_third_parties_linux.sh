#! /bin/bash

# FFmpeg
cd FFmpeg
make clean
rm -rf output_linux
./configure --enable-shared --enable-static --prefix=./output_linux --disable-programs --disable-doc \
            --disable-avdevice --disable-swscale --disable-postproc --disable-avfilter --disable-x86asm \
            --disable-network --disable-dwt --disable-lsp --disable-faan --disable-iamf 
make -j8 && make install
cd ..