declare name      "VocSynth";
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
import ("lib/classicVocoder.lib");
import ("lib/chorus.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VocSynth: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrMonoChan,nrSends)

VocSynth(audio) =
    (
    cleanVolume,cleanNLKS,cleanpmFX,
    (voice(audio)*4<:_,_)
    ,
    vocoderVolume,vocoderNLKS,vocoderpmFX,
    StereoVocoder(audio,PitchTracker(audio))

    : mixerWithSends(nrChan,nrMonoChan,nrSends)

    :_,_//No effect
//    ,_,_//No effect

    ,stereoChorus
//    ,_,_//No effect
    ,pmFX(PitchTracker(audio),pmFXr,pmFXi,PMphase)
    ,pmFX(PitchTracker(audio),pmFXr,pmFXi,0-PMphase)

    :interleave(nrMonoChan,nrSends):par(i,nrMonoChan,(bus(nrSends):>_))

    :stereoLimiter(audio) //needs the original audio (the voice) to calculate the pitch, and with that the decay time.
    :VuMeter
    )
    with {
          nrChan     = 2;
          nrMonoChan = 2;
          nrSends    = 3;
          };


//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VocSynth(audio);
//process(audio) = StereoVocoder(audio,PitchTracker(audio));

