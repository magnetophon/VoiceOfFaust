declare name      "FOFvocoder";
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
import("../lib/follower.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/FOFvocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------

// compile with Faust 1:
// time Faust2jack -t 99999 -time -os.osc -vec  FOFvocoder.dsp && timeout 10 ./FOFvocoder & sleep 2 && jack_connect system:capture_1 FOFvocoder:in_0 & jack_connect FOFvocoder:out_0 system:playback_1 & jack_connect FOFvocoder:out_1 system:playback_2
process(audio,index,fidelity) =
  fofvocoder(audio,freq,index,fidelity,doubleOscs)
with {
  freq =
    guidePitch(audio,index)
    *((2:pow(((maxOctavation+1)/2):max(1)))/2);
};
