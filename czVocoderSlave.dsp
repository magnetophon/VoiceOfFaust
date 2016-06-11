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

import ("lib/common.lib");
import("lib/slave.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/CZvocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------


process(audio,index,fidelity) =
  czVocoder(audio, masterPitch(audio,index),index,fidelity,doubleOscs);