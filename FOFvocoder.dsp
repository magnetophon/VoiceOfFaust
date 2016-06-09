declare name      "FOFvocoder";
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
//qompander	= component("qompander/qompander.dsp");
//KarplusStrongFX		= component("KarplusStrongFX.dsp");

import ("lib/common.lib");
import("lib/master.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/FOFvocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------

// compile with faust 1:
// time faust2jack -t 99999 -time -osc -vec  FOFvocoder.dsp && timeout 10 ./FOFvocoder & sleep 2 && jack_connect system:capture_1 FOFvocoder:in_0 & jack_connect FOFvocoder:out_0 system:playback_1 & jack_connect FOFvocoder:out_1 system:playback_2
process(audio) = fofvocoder(audio,masterPitch(audio,index),doubleOscs);
//process = FOFvocoderMixer(ambisonicsOn,outputRoutingEnabled);
