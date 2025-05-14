declare name      "VoiceOfFaust";
declare version   "1.1.7";
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
import("../lib/guide.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/classicVocoder.lib");
import ("../lib/PAFvocoder.lib");
import ("../lib/FOFvocoder.lib");
import ("../lib/inputFM.lib");
import ("../lib/CZringmod.lib");
import ("../lib/KarplusStrongFX.lib");
import ("../lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrOutChan,nrSends)

VoiceOfFaust(audio,index,fidelity) =
    (
    cleanVolume,cleanNLKS,cleanpmFX,
    (voice(audio,freq)*4<:_,_),

    subVolume,subNLKS,subpmFX,
    subSine(audio,freq,index),

    vocoderVolume,vocoderNLKS,vocoderpmFX,
    StereoVocoder(audio,freq,index,fidelity),

    pafVolume,pafNLKS,pafpmFX,
    PAFvocoder(audio,freq,index,fidelity,doubleOscs),

    fofVolume,fofNLKS,fofpmFX,
    fofvocoder(audio,freq,index,fidelity,doubleOscs),

    FMvolume,fmNLKS,FMpmFX,
    stereoFMSynth(audio:fi.highpass3e(400):extremeLimiter, audio:fi.highpass3e(400),freq,subLevel(audio,freq)),

    CZvolume,CZNLKS,CZpmFX,
    CZringMod(audio,freq,index)

    : mixerWithSends(nrChan,nrOutChan,nrSends)

    :_,_//No effect

    ,(stringloop(freq*KPoctave,audio,index,feedbackAmount,_,phaseM,DCnonlin+DCleftRight))
    ,(stringloop(freq*KPoctave,audio,index,feedbackAmount,_,0-phaseM,DCnonlin-DCleftRight))
    // too CPU heavy:
    // ,(stringloopBank(freq,audio,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH))
    // ,(stringloopBank(freq,audio,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH))

    ,pmFX(freq,pmFXr,pmFXi,PMphase)
    ,pmFX(freq,pmFXr,pmFXi,0-PMphase)

    :ro.interleave(nrOutChan,nrSends):par(i,nrOutChan,(si.bus(nrSends):>_))

    :stereoLimiter(freq * 0.25) //it needs the lowest pitch to adjust the decay time.
    // :VuMeter
    )
    with {
          KPoctave       = mainKPgroup( vslider("[-2]octave",-1, -2, 2, 1):octaveMultiplier);
          feedbackAmount  = MKPgroup(    vslider("[1]feedback[style:knob][tooltip: feedback amount for this octave]"   , 1, 0, 1, 0.001)):volScale; // -60db decay time (sec)
          nrChan     = 7;
          nrOutChan = 2;
          nrSends    = 3;
          freq = guidePitch(audio,index);
};

//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio,index,fidelity);
