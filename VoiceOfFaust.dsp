declare name      "VoiceOfFaust";
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
import ("lib/PAFvocoder.lib");
import ("lib/FOFvocoder.lib");
import ("lib/inputFM.lib");
import ("lib/CZringmod.lib");
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrOutChan,nrSends)

VoiceOfFaust(audio) =
    (
    cleanVolume,cleanNLKS,cleanpmFX,
    (voice(audio,index)*4<:_,_),

    subVolume,subNLKS,subpmFX,
    subSine(audio,masterPitch(audio,index)),

    vocoderVolume,vocoderNLKS,vocoderpmFX,
    StereoVocoder(audio,masterPitch(audio,index),index,fidelity),

    pafVolume,pafNLKS,pafpmFX,
    PAFvocoder(audio,masterPitch(audio,index),index,fidelity,doubleOscs),

    fofVolume,fofNLKS,fofpmFX,
    fofvocoder(audio,masterPitch(audio,index),index,fidelity,doubleOscs),

    FMvolume,fmNLKS,FMpmFX,
    stereoFMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),masterPitch(audio,index),index,fidelity,doubleOscs,subLevel(audio)),

    CZvolume,CZNLKS,CZpmFX,
    CZringMod(audio,masterPitch(audio,index),index,fidelity,doubleOscs)

    : mixerWithSends(nrChan,nrOutChan,nrSends)

    :_,_//No effect

    ,(stringloopBank(masterPitch(audio,index),audio,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH))
    ,(stringloopBank(masterPitch(audio,index),audio,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH))

    ,pmFX(masterPitch(audio,index),pmFXr,pmFXi,PMphase)
    ,pmFX(masterPitch(audio,index),pmFXr,pmFXi,0-PMphase)

    :interleave(nrOutChan,nrSends):par(i,nrOutChan,(bus(nrSends):>_))

    :stereoLimiter(masterPitch(audio,index) * 0.25) //it needs the lowest pitch to adjust the decay time.
    :VuMeter
    )
    with {
          nrChan     = 7;
          nrOutChan = 2;
          nrSends    = 3;
          };


//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio);
