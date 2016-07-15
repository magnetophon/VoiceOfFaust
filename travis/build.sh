#!/usr/bin/env bash

DSPs=(
    classicVocoder
    CZringmod
    czVocoder
    FMsinger
    FMvocoder
    FOFvocoder
    Karplus-StrongSinger
    Karplus-StrongSingerMaxi
    master
    PAFvocoder
    # softSyncVocoder
    stringSinger
    subSinger
    # slaves:
    classicVocoderSlave
    CZringmodSlave
    czVocoderSlave
    FMvocoderSlave
    FOFvocoderSlave
    Karplus-StrongSingerMaxiSlave
    Karplus-StrongSingerSlave
    PAFvocoderSlave
    stringSingerSlave
    subSingerSlave
)

function buildPlugins {
    echo "building lv2 $1"
    time faust2lv2 -osc -vec  -t 9999999 -time "$1.dsp"
    cp -r $1.lv2 $HOME/.lv2/
    echo "building ladspa $1"
    time faust2ladspa  -t 9999999 -time "$1.dsp"
}
BUILD_DEPS=0
if [[ $BUILD_DEPS == 0 ]];
then
    faust -v

    cd ..
    mkdir -p $HOME/.lv2
    for i in "${DSPs[@]}"
    do
        buildPlugins $i
    done
    echo "benchmark all LV2s"
    lv2bm `lv2ls`
    echo "torture ladspas"
    for i in "${DSPs[@]}"
    do
        plugin-torture --evil -d --ladspa --plugin $i.so
    done
else
    exit 0
fi
