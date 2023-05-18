declare name      "stringSinger";
declare version   "1.1.6";
declare author    "Bart Brouns";
declare license   "AGPL-3.0-only";
declare copyright "2014 - 2023, Bart Brouns";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and os.square oscillators adapted from the Faust library" ;

//-----------------------------------------------
// imports
//-----------------------------------------------
//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo

import ("../lib/common.lib");
import("../lib/guide.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/classicVocoder.lib");
import ("../lib/chorus.lib");
import ("../lib/pmFX.lib");

maxNrInRoutings = 5;

//-----------------------------------------------
// GUI changes
//-----------------------------------------------

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrOutChan,nrSends)

VoiceOfFaust(audio,index,fidelity) =
  (
  cleanVolume,cleanChorus,cleanpmFX, //output volumes. The number of parameters should be nrSends
  (voice(audio,freq)<:si.bus(nrOutChan))
  ,
  vocoderVolume,vocoderChorus,vocoderpmFX,
  StereoVocoder(audio,freq,index,fidelity)

  : mixerWithSends(nrChan,nrOutChan,nrSends)

  :si.bus(nrOutChan)//No effect

  ,par(i,nrOutChan/2,stereoChorus)

  ,par(i,nrOutChan/2,
    (pmFX(freq,pmFXr,pmFXi,PMphase)
    ,pmFX(freq,pmFXr,pmFXi,0-PMphase))
  )

  :ro.interleave(nrOutChan,nrSends):par(i,nrOutChan,(si.bus(nrSends):>_)) // mix the clean and FX

  //:stereoLimiter(freq * vocoderOctave) //needs the pitch to adjust the decay time.
  //:VuMeter
  )
  with {
    nrChan        = 2;
    nrSends       = 3;

    cleanChorus   = cleanGroupLevel(vslider("[1]chorus[tooltip: constant detune chorus][style:knob]",	0, 0, 1, 0.001):volScale);
    cleanpmFX     = cleanGroupLevel(vslider("[2]PM[tooltip: phase modulation][style:knob]",	0, 0, 1, 0.001):volScale);

    vocoderVolume = vocoderGroupLevel(vslider("[0]volume[style:knob]",	0.75, 0, 1, 0.001):volScale);
    vocoderChorus = vocoderGroupLevel(vslider("[1]chorus[tooltip: constant detune chorus][style:knob]",	0.5, 0, 1, 0.001):volScale);
    vocoderpmFX   = vocoderGroupLevel(vslider("[2]PM[tooltip: phase modulation][style:knob]",	0.5, 0, 1, 0.001):volScale); // 0 to 1
    freq          = guidePitch(audio,index);
};

//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio,index,fidelity);
