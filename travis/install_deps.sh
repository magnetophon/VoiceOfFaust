#!/bin/bash

if [ $BUILD_DEPS == 1 ];
then
    mkdir -p $HOME/local/{bin,include,lib,share}
    echo "install faust"
    git clone git://git.code.sf.net/p/faudiostream/code /tmp/faust
    pushd  /tmp/faust
    make
    sudo make PREFIX=$HOME/local install
    # travis wants to see output!
    sed -i 's@-o "\$tmpdir/\$clsname"@@' $HOME/local/bin/faust2lv2
    sed -i 's@\> /dev/null@@' $HOME/local/bin/faust2ladspa
    cat $HOME/local/bin/faust2lv2
    cat $HOME/local/bin/faust2ladspa

    popd
    echo "install plugin-torture"
    git clone https://github.com/cth103/plugin-torture.git /tmp/plugin-torture
    pushd /tmp/plugin-torture
    make
    cp plugin-torture $HOME/local/bin/
    popd
    echo "install lv2bm"
    git clone https://github.com/moddevices/lv2bm
    cd lv2bm && make && sudo make PREFIX=$HOME/local install && cd ..
else
    sudo rsync -avzh $HOME/local/ /usr/local/
fi
