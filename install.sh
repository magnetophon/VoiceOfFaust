#!/usr/bin/env sh
# ./faust2jack  -osc VocSynth.dsp -time -sch -vs 1024 -lv 1 -lt -t 99999
masters=(
    classicVocoder.dsp
    CZringmod.dsp
    czVocoder.dsp
    FMsinger.dsp
    FMvocoder.dsp
    FOFvocoder.dsp
    Karplus-StrongSinger.dsp
    Karplus-StrongSingerMaxi.dsp
    master.dsp
    PAFvocoder.dsp
    # softSyncVocoder.dsp
    stringSinger.dsp
    subSinger.dsp
)


slaves=(
    classicVocoderSlave.dsp
    CZringmodSlave.dsp
    czVocoderSlave.dsp
    FMsingerslave.dsp
    FMvocoderSlave.dsp
    FOFvocoderSlave.dsp
    Karplus-StrongSingerMaxiSlave.dsp
    Karplus-StrongSingerSlave.dsp
    PAFvocoderSlave.dsp
    stringSingerSlave.dsp
    subSingerSlave.dsp
)

for i in "${masters[@]}"
do
    echo "compiling standalone from" $i
	  echo "time faust2jack -t 99999 -time -osc -vec" $i
	  time faust2jack -t 99999 -time -osc -vec $i
done

for i in "${slaves[@]}"
do
    echo "compiling standalone from" $i
	  echo "time faust2jack -t 99999 -time -osc -vec" $i
	  time faust2jack -t 99999 -time -osc -vec $i
done

# workaround for a bug in faust2lv2:
# https://bitbucket.org/agraef/faust-lv2/issues/10/tabs-break-lv2s
echo "patching tgroup for lv2:"
sed -i "s|tabs(x)               = VoFgroup(tgroup|tabs(x)               = VoFgroup(hgroup|" "lib/FullGUI.lib"


for i in "${slaves[@]}"
do
    echo "compiling lv2 from:" $i
    echo "time faust2lv2 -gui -t 99999 -time -osc -vec" $i
    time faust2lv2 -gui -t 99999 -time -osc -vec $i
done

echo "undo patching tgroup for lv2:"
sed -i "s|tabs(x)               = VoFgroup(hgroup|tabs(x)               = VoFgroup(tgroup|" "lib/FullGUI.lib"
