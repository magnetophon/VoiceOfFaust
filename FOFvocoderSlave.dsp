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
import("lib/slave.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/FOFvocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------

// compile with faust 1:
// time faust2jack -t 99999 -time -osc -vec  FOFvocoder.dsp && timeout 10 ./FOFvocoder & sleep 2 && jack_connect system:capture_1 FOFvocoder:in_0 & jack_connect FOFvocoder:out_0 system:playback_1 & jack_connect FOFvocoder:out_1 system:playback_2
process(audio,index,fidelity) =
fofvocoder(audio,masterPitch(audio,index),index,fidelity,doubleOscs);
// process = fof;

  //the noises part is to make a different (low)freq modulation for each osc.
  //noises(nrBands,0):smooth(tau2pole(32))
  //"(i+1)*" is to make each band different
  // par(i,nrOutChan,
    // (fofNoises(i+1,freq,enablePhasenoise,enablePhaseNoiseFilter),fofCenters(freq),(multiK<:bus(nrBands)),(freq<:bus(nrBands)),fofSkirts(freq),fofDecays,fofOctavations)
    // :
    // fofOscs
    // : gainNormalise
    // :((analizer(voice(audio,index),freq,fidelity,enableDeEsser),par(i, nrBands, _)):interleave(nrBands,2):par(i, nrBands,*))
  // )
  // :vocoderMixer(ambisonicsOn,outputRoutingEnabled,doubleOscs)
  // :postProc(nrOutChan,ambisonicsOn,enableAutosat,volume*0.1,1)
// with {
//   multiK = lf_rawsaw(SR/freq*multi);
//   freq =110;
// };