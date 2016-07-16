#!/bin/bash

if [ "$BUILD_DEPS" == 1 ];
then
    mkdir -p "$HOME/local/{bin,include,lib,share}"
    echo "install faust"
    git clone git://git.code.sf.net/p/faudiostream/code /tmp/faust
    pushd  /tmp/faust
    sed -i 's@-o "\$tmpdir/\$clsname"@@' tools/faust2appls/faust2lv
    sed -i 's@> /dev/null@@' tools/faust2appls/faust2ladspa
    make
    sudo make PREFIX="$HOME/local" install
    # travis wants to see output!

    popd
    echo "install plugin-torture"
    git clone https://github.com/cth103/plugin-torture.git /tmp/plugin-torture
    pushd /tmp/plugin-torture
    make
    cp plugin-torture "$HOME/local/bin/"
    popd
    echo "install lv2bm"
    git clone https://github.com/moddevices/lv2bm
    cd lv2bm && make && sudo make PREFIX="$HOME/local" install && cd ..
else
    sudo rsync -avzh "$HOME/local/" /usr/local/
fi
