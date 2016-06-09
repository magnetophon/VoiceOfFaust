declare name      "classicVocoder";
declare version   "0.7";
declare author    "Bart Brouns";
declare license   "GNU 3.0";
declare copyright "(c) Bart Brouns 2014";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and square oscillators adapted from the faust library" ;

//-----------------------------------------------
// imports
//-----------------------------------------------
//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo

import ("lib/common.lib");
import("lib/master.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/classicVocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------
maxNrInRoutings = 5;

process(audio) =
StereoVocoder(audio,masterPitch(audio,index),index,fidelity);

// (
//            (vocoderOsc(freq)<:bus(nrBands))
//            ,(vocoderCenters(freq)<:((bus(nrBands)<:((50:max(vocoderBottom*freq*vocoderOctave)),par(i,nrBands,selector(i-1,nrBands)))),(bus(nrBands))))
//            ,analizer(voice(audio,index),freq,fidelity,enableDeEsser)
//            ,(VocoderLinArrayChooser(vocoderQbottom,vocoderQmid,vocoderQband,vocoderQtop,para):par(i,nrBands,dezip))
//          )
//            : StereoVolFilterBank(nrBands,enableFeedback)
//            :gainCompare(audio,freq,enableGainCompare)
//            :vocoderMixer(ambisonicsOn,outputRoutingEnabled,0)
//            // :endGainCompare(audio,freq)
//            :postProc(nrOutChan,ambisonicsOn,enableAutosat,volume*select2(gainCompEnable,0.001,0.02),vocoderWidth)
//            with { freq = masterPitch(audio,index);};
