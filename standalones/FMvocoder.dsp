declare name      "FMvocoder";
declare version   "1.1.5";
declare author    "Bart Brouns";
declare license   "AGPL-3.0-only";
declare copyright "(c) Bart Brouns 2023";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and os.square oscillators adapted from the Faust library" ;

//-----------------------------------------------
// imports
//-----------------------------------------------
//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo

import ("../lib/common.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/FMvocoder.lib");
import("../lib/guide.lib");

//-----------------------------------------------
// process
//-----------------------------------------------
process(audio) =
  FMvocoder(audio, guidePitch(audio,index),index,fidelity,doubleOscs);
