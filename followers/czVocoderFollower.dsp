declare name      "FOFvocoder";
declare version   "1.1.7";
declare author    "Bart Brouns";
declare license   "AGPL-3.0-only";
declare copyright "2014 - 2025, Bart Brouns";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and os.square oscillators adapted from the Faust library" ;

//-----------------------------------------------
// imports
//-----------------------------------------------
//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo

import ("../lib/common.lib");
import("../lib/follower.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/CZvocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------

process(audio,index,fidelity) =
  czVocoder(audio, guidePitch(audio,index),index,fidelity,doubleOscs);
