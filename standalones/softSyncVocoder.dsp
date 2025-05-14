declare name      "softSyncVocoder";
declare version   "0.1";
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
// specific to this synth:
/*import ("../lib/FullGUI.lib");*/
import ("../lib/classicVocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------

SyncFreq = (vslider("[1]SyncFreq[frequency of the syncing oscilator as a Piano Key (PK) number (A440 = key 49)][style:knob]",  1,0,16,0.01) *OscFreq):si.smooth(0.999);
OscFreq  = (vslider("[2]OscFreq  [frequency of the synced oscilator as a Piano Key (PK) number (A440 = key 49)][style:knob]",  49,1,88,1) : ba.pianokey2hz):si.smooth(0.999);
SyncDuty =  vslider("[2]SyncDuty[duty cycle of the syncing os.osc][style:knob]",  0.5,0,1,0.001):si.smooth(0.999);

SyncingOsc(freq,duty) = (os.lf_pulsetrainpos(freq,duty)*2)-1;

SyncedOsc(freq,sync) = saw1_sync(freq,sync);//:fi.lowpass(16,ma.SR/2);

// --- lf_sawpos_sync ---
// simple os.sawtooth waveform oscillator between 0 and 1
lf_sawpos_sync(freq,sync) = ((_,periodsamps : fmod) ~ +(1.0*sync)) / periodsamps
with {
  periodsamps = float(ma.SR)/freq; // period in samples (not nec. integer)
};

// --- saw1_sync ---
// simple os.sawtooth waveform oscillator between -1 and 1
saw1_sync(freq,sync) = 2.0 * lf_sawpos_sync(freq,sync) - 1.0; // fi.zero-mean in [-1,1)

lf_triangle(freq) = _~(+(os.lf_squarewave(freq)/periodsamps))
with {
  periodsamps = float(ma.SR)/freq; // period in samples (not nec. integer)
};

lf_saw_hardsync(freq,sync) = select2(sync==1,_,0)~(+(os.lf_squarewave(freq)/periodsamps))
with {
  periodsamps = float(ma.SR)/freq; // period in samples (not nec. integer)
};

lf_triangle_softsync(freq) = ((fi.highpass(1,freq):fi.lowpass(16,ma.SR/4))~(+((os.lf_squarewave(freq)*os.lf_squarewave(SyncFreq)/periodsamps))))
/*lf_triangle_softsync(freq) = (fi.highpass(1,freq)~(+((os.lf_squarewave(freq)*os.lf_squarewave(SyncFreq)/periodsamps))):fi.lowpass(16,ma.SR/2))*/
    ,os.lf_squarewave(freq)
    ,os.lf_squarewave(SyncFreq)
with {
  periodsamps = float(ma.SR)/max(freq,SyncFreq); // period in samples (not nec. integer)
};

process =
lf_triangle_softsync(OscFreq);
/*saw1_sync(100,1);*/
/*SyncedOsc(OscFreq,1);*/
/*SyncedOsc(OscFreq,SyncingOsc(SyncFreq,SyncDuty))*0.1<:si.bus(2);*/
