declare name      "FMvocoder";
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
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/FMvocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------
freq=3.33;
oct=2;
/*process=fund(freq,oct);*/
process(audio) =
(
  FMvocoder(audio,PitchTracker(audio,enablePitchTracker),doubleOscs),
  reEsser(voice(audio),PitchTracker(audio,enablePitchTracker),enableReEsser)
):>bus(nrOutChan);
// process = chooseResonBP;
/*process(audio) =*/
  /*(pafCenters,(pafFund(freq)<:bus(nrBands)),pafIndexes,analizer(voice(audio),freq,enableDeEsser)):pafOscs:vocoderMixer(ambisonicsOn)*/
  /*:postProc(nrOutChan,ambisonicsOn);*/
