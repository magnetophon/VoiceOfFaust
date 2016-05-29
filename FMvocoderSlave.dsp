import("lib/slave.lib");
import ("lib/common.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/FMvocoder.lib");


process(audio,index,fidelity) =
  // FMvocoder(audio,calculateFreq,doubleOscs);
// masterPitch
  // fidelity
  // deEssedAnalizer(audio,freq,fidelity)
  // FMfunds(calculateFreq(index),index,0)
// analizer(voice(audio,index),freq,fidelity,enableDeEsser)
// voice(audio,index)
// masterPitch(audio,index)
// calculateFreq(index)
// PitchTracker(audio,0) = OSCpitch : pitchQuantise(enablePitchQuantise) : smoother;
// qompander(audio:highpassStable(2,masterPitch(audio,index)*0.75),factor,threshold,attack,release)
  // (masterPitch(audio,index))
// simpleAnalizer(audio,masterPitch(audio,index))
  // (FMfunds(freq,index,0), (freq<:bus(nrBands)),analizer(voice(audio,index),freq,fidelity,enableDeEsser),FMindexes,FMcenters(freq)):FMoscs:vocoderMixer(ambisonicsOn,outputRoutingEnabled)
  //          :postProc(nrOutChan,ambisonicsOn,enableAutosat,volume*0.025,FMwidth)
  // analizer(voice(audio,index),freq,fidelity,enableDeEsser)//:par(i, nrBands, meter)
FMvocoder(audio, masterPitch(audio,index),index,fidelity,doubleOscs)
// masterPitch(audio,index)/MaxInputPitch
// lf_sawpos(calculateFreq(index)/4)
// my_lf_sawpos(calculateFreq(index)/4)
with {
// --- lf_sawpos ---
// simple sawtooth waveform oscillator between 0 and 1
freq = masterPitch(audio,index);
my_lf_sawpos(freq) = decimal ~ +(freq/SR);
};


// // makeSlave(audio,masterIndex,fidelity,synth,isSlave) =