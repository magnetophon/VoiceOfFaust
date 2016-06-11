declare name      "classicVocoder";
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
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/inputFM.lib");
import ("lib/pmFX.lib");

VoiceOfFaust(audio) =
  (
  (
    cleanVolume,cleanpmFX, //output volumes. The number of parameters should be nrSends
    (voice(audio,index)<:_,_)
    ,
    subVolume,subpmFX,
    subSine(audio,masterPitch(audio,index))
    ,
    FMvolume,FMpmFX,
    stereoFMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),masterPitch(audio,index),index,fidelity,doubleOscs,subLevel(audio))
  )
  : mixerWithSends(nrChan,nrOutChan,nrSends)

  :_,_//No effect

  ,pmFX(masterPitch(audio,index),pmFXr,pmFXi,PMphase)
  ,pmFX(masterPitch(audio,index),pmFXr,pmFXi,0-PMphase)

  :interleave(nrOutChan,nrSends):par(i,nrOutChan,(bus(nrSends):>_)) // mix the clean and FX

  :stereoLimiter(masterPitch(audio,index) * vocoderOctave) //needs the pitch to adjust the decay time.
  //:VuMeter
  )
  with {
    nrChan     = 3;
    nrOutChan = 2;
    nrSends    = 2;

    cleanpmFX  = cleanGroupLevel(vslider("[2]PM[tooltip: phase modulation][style:knob]",	0.5, 0, 1, 0.001):volScale);

    subVolume  = subGroupLevel(vslider("[0]volume[style:knob]",	1, 0, 1, 0.001):volScale);

    FMvolM     = HFMparamsGroup(vslider("[1]vol[tooltip: volume][style:knob]",	1, 0, 1, 0.001):volScale);
    FMvolLL    = LLFMparamsGroup(vslider("[1]vol[tooltip: volume][style:knob]", 0, 0, 1, 0.001):volScale);
    };


//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio);
