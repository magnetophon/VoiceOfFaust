declare name      "Karplus-StrongSingerMaxi";
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
import("lib/master.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/classicVocoder.lib");
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------


//KPvolume          = mainKPgroup(vslider("[0]volume [style:knob][tooltip: the output level]",	1, 0, 1, 0.001)*0.2:volScale);

VoiceOfFaust(audio,index) =
  (voice(audio,index)<:_,_)
  :
  (
   pmFX(freq,pmFXr,pmFXi,PMphase)
  ,pmFX(freq,pmFXr,pmFXi,0-PMphase)
  )
  :
  (
   (_<:(DryPath(phaseM,DCnonlin+DCleftRight),stringloopBank(freq,audio,index,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH)):>_)
  ,(_<:(DryPath(0-phaseM,DCnonlin-DCleftRight),stringloopBank(freq,audio,index,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH)):>_)
  )
  :stereoLimiter(freq * 0.25) //needs the pitch to adjust the decay time.
  //:VuMeter
  with {
    DryPath(phase,DC) =  MyNonLinearModulator(nonLin,frequencyMod*freq,phase,DC)*DryVol;

    DryVol =  mainKPgroup(vslider("[-1]dry vol[style:knob]", 0.6, 0, 1, 0.001):volScale);
    // KPoctave          = mainKPgroup(vslider("[-1]octave",-1, -2, 2, 1):octaveMultiplier);
    freq = masterPitch(audio,index);
  };
//-----------------------------------------------
// process
//-----------------------------------------------

process(audio,index) = VoiceOfFaust(audio,index);
