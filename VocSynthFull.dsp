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
import ("lib/PAFvocoder.lib");
import ("lib/FOFvocoder.lib");
import ("lib/inputFM.lib");
import ("lib/CZringmod.lib");
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VocSynth: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrMonoChan,nrSends)

VocSynth(audio) =
    (
    cleanVolume,cleanNLKS,cleanpmFX,
    (voice(audio)*4<:_,_),

    subVolume,subNLKS,subpmFX,
    subSine(audio,PitchTracker(audio)),

    vocoderVolume,vocoderNLKS,vocoderpmFX,
    StereoVocoder(audio,PitchTracker(audio)),

    pafVolume,pafNLKS,pafpmFX,
    PAFvocoder(audio,PitchTracker(audio)),

    fofVolume,fofNLKS,fofpmFX,
    fofvocoder(audio,PitchTracker(audio)),

    FMvolume,fmNLKS,FMpmFX,
    FMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio),subLevel(audio)),

    CZvolume,CZNLKS,CZpmFX,
    CZringMod(audio,PitchTracker(audio))

    : mixerWithSends(nrChan,nrMonoChan,nrSends)

    :_,_//No effect

    ,(stringloopBank(PitchTracker(audio),audio,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH))
    ,(stringloopBank(PitchTracker(audio),audio,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH))

    ,pmFX(PitchTracker(audio),pmFXr,pmFXi,PMphase)
    ,pmFX(PitchTracker(audio),pmFXr,pmFXi,0-PMphase)

    :interleave(nrMonoChan,nrSends):par(i,nrMonoChan,(bus(nrSends):>_))

    :stereoLimiter(audio) //it needs the original audio (the voice) to calculate the pitch, and with that the decay time.
    :VuMeter
    )
    with {
          nrChan     = 7;
          nrMonoChan = 2;
          nrSends    = 3;
          };


//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VocSynth(audio);



