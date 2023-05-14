//-----------------------------------------------
// guide controller
//-----------------------------------------------

declare name      "guide";
declare version   "1.1.4";
declare author    "Bart Brouns";
declare license   "GNU 3.0";
declare copyright "(c) Bart Brouns 2014";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and square oscillators adapted from the Faust library" ;

import ("../lib/common.lib");
import ("../lib/FullGUI.lib");
import("../lib/guide.lib");

process(audio) =
  audio,
  guideIndex(freq,index),
  fidelity
with {
  // we want to tell the external pitchtracker the min and max
  // so don't optimize it out
  freq = (guidePitch(audio,index):attach(OSCminPitch):attach(OSCmaxPitch));
};
