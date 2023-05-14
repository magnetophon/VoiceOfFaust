#!/usr/bin/env bash

DSPs=(
    classicVocoder
    CZringmod
    czVocoder
    FMsinger
    FMvocoder
    FOFvocoder
    KarplusStrongSinger
    KarplusStrongSingerMaxi
    guide
    PAFvocoder
    # softSyncVocoder
    stringSinger
    subSinger
    # followers:
    classicVocoderFollower
    CZringmodFollower
    czVocoderFollower
    FMvocoderFollower
    FOFvocoderFollower
    KarplusStrongSingerMaxiFollower
    KarplusStrongSingerFollower
    PAFvocoderFollower
    stringSingerFollower
    subSingerFollower
)

function buildPlugins {
    echo "building lv2 $1"
    time bash -x faust2lv2 -vec  -t 9999999 -time "$1.dsp"
    cp -r "$1.lv2" "$HOME/.lv2/"
    echo "building ladspa $1"
    time bash -x faust2ladspa -vec  -t 9999999 -time "$1.dsp"
}

if [[ $BUILD_DEPS == 0 ]];
then
    faust -v

    mkdir -p "$HOME/.lv2"
    for i in "${DSPs[@]}"
    do
        buildPlugins "$i"
    done
    echo "benchmark all LV2s"
    lv2bm "$(lv2ls)"
    echo "torture ladspas"
    for i in "${DSPs[@]}"
    do
        plugin-torture --evil -d --ladspa --plugin "$i.so"
    done
else
    exit 0
fi
