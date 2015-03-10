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
    pafvocoder(audio,PitchTracker(audio)),

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

    //:block  //block out non tonal sounds
    :stereoLimiter(audio) //it needs the original audio (the voice) to calculate the pitch, and with that the decay time.
    :VuMeter
    )
    with {
          nrChan     = 7;
          nrMonoChan = 2;
          nrSends    = 3;
          //is actually dual mono. on purpose; to try and keep the image in the center.
          //todo: make this stereo, and find a better way for  KP-FX to stay centered
          stereoLimiter(audio)=
              (dcblocker*0.5:compressor_mono(100,-12,decay*0.5,decay)),
              (dcblocker*0.5:compressor_mono(100,-12,decay*0.5,decay)):
              (compressor_mono(100,-6,0.001,decay*0.5)),
              (compressor_mono(100,-6,0.001,decay*0.5));

          decay                                = (1/(PitchTracker(audio) * subOctave ));
          same(x,time)                         = (x@time) == x;
          intervalTester(x,nrSamples,interval) = (prod(i,nrSamples,same(x,i*interval+1)));
          block                                = par(i,2,(intervalTester(PitchTracker(audio),2,265)*-1+1:smooth(0.999))*_);
          };


//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VocSynth(audio);


//-----------------------------------------------
// testing cruft
//-----------------------------------------------

//process(audio) = FMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio)/2,subLevel(audio));
//FMvoc(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio)/2,subLevel(audio),FMindex,FMdyn)<:_,_;
//process(audio) =stringloop(audio,PitchTracker(audio),nlfOrder,2,3,4,5,6.1);
//process(audio) =stringloop(audio,PitchTracker(audio)/1,nlfOrder,feedback,tresh,nonLin,bright,frequencyMod);

//process(audio) = fof(400,fofFund(PitchTracker(audio)),fofSkirt,fofDecay,fofPhaseRand,1);

//adsr(KPattack,KPdecay,KPsustain,KPrelease,button("foo")):vbargraph("foo", 0, 1);

//process(audio) = audio<:(stringloopBank(PitchTracker(audio)));

//fof(fofCenter1,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*(noises(16,1):smooth(tau2pole(1))),fofVol1)
//process(audio) = fof(444,222,fofSkirt,fofDecay,1*fofPhaseRand*(noises(16,1):smooth(tau2pole(1))),1);
//process(audio) = fofvocoder(voice,PitchTracker(audio)):>min(100):max(-100):stringloop(_,PitchTracker(audio)*0.5,nlfOrderL,feedbackL*feedbackADSR(audio),treshL,nonLinL,brightL,frequencyModL);

//process(audio) = volFilterBank:vocoderMixer:par(i, 2, _*0.01):WidePanner(vocoderWidth);
//process(audio) = (stringloopBank(PitchTracker(audio),audio,audio,phaseLL,phaseL,phaseM,phaseH,phaseHH));
//process(audio) = stringloopFBpath(1,0.25,feedbackLL*feedbackADSR(audio),phaseLL,nonLinLL,frequencyModLL);
//process(audio) = KPvocoder(audio,voice(audio),PitchTracker(audio));

//process = PHosci(1000,0.5);
//process(audio) = audio<:((_:qompander(factor,threshold,attack,release)*2<:stringloopBank(PitchTracker(audio))),(_:qompander(factor,threshold,attack,release)*2<:stringloopBank(PitchTracker(audio))));

//process(audio) = audio:stringloop(_,PitchTracker(audio)/1,nlfOrder,feedback,tresh,nonLin,bright,frequencyMod);
//process(audio) = audio:(stringloopBank(_,PitchTracker(audio)));
//stringloop(_,PitchTracker(audio)/1,nlfOrder,feedback,tresh,nonLin,bright,frequencyMod),
//stringloop(_,PitchTracker(audio)/1,nlfOrder,feedback,tresh,nonLin,bright,frequencyMod);

//process(audio) = vocoder(audio,PitchTracker(audio));
