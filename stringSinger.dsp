declare name      "stringSinger";
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
import ("lib/classicVocoder.lib");
import ("lib/chorus.lib");
import ("lib/pmFX.lib");

5axNrInRoutings = 5;

//-----------------------------------------------
// GUI changes
//-----------------------------------------------


//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrOutChan,nrSends)


VoiceOfFaust(audio) =
  (
  cleanVolume,cleanChorus,cleanpmFX, //output volumes. The number of parameters should be nrSends
  (voice(audio)<:bus(nrOutChan))
  ,
  vocoderVolume,vocoderChorus,vocoderpmFX,
  StereoVocoder(audio,PitchTracker(audio))

  : mixerWithSends(nrChan,nrOutChan,nrSends)

  :bus(nrOutChan)//No effect

  ,par(i,nrOutChan/2,stereoChorus)

  ,par(i,nrOutChan/2,
    (pmFX(PitchTracker(audio),pmFXr,pmFXi,PMphase)
    ,pmFX(PitchTracker(audio),pmFXr,pmFXi,0-PMphase))
  )

  :interleave(nrOutChan,nrSends):par(i,nrOutChan,(bus(nrSends):>_)) // mix the clean and FX

  //:stereoLimiter(PitchTracker(audio) * vocoderOctave) //needs the pitch to adjust the decay time.
  //:VuMeter
  )
  with {
    nrChan     = 2;
    nrSends    = 3;

    cleanChorus             = cleanGroupLevel(vslider("[1]chorus[tooltip: constant detune chorus][style:knob]",	0, 0, 1, 0.001):volScale);
    cleanpmFX               = cleanGroupLevel(vslider("[2]PM[tooltip: phase modulation][style:knob]",	0.5, 0, 1, 0.001):volScale);

    vocoderVolume           = vocoderGroupLevel(vslider("[0]volume[style:knob]",	0.75, 0, 1, 0.001):volScale);
    vocoderChorus           = vocoderGroupLevel(vslider("[1]chorus[tooltip: constant detune chorus][style:knob]",	0.5, 0, 1, 0.001):volScale);
    vocoderpmFX             = vocoderGroupLevel(vslider("[2]PM[tooltip: phase modulation][style:knob]",	0.5, 0, 1, 0.001):volScale); // 0 to 1
    };


//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio);
//process(audio) = StereoVocoder(audio,PitchTracker(audio));
