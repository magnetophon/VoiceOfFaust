declare name      "Karplus-StrongSinger";
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
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------


//KPvolume          = mainKPgroup(vslider("[0]volume [style:knob][tooltip: the output level]",	1, 0, 1, 0.001)*0.2:volScale);

VoiceOfFaust(audio) =
  (voice(audio,index)<:_,_)
  :
  (
   pmFX(masterPitch(audio,index),pmFXr,pmFXi,PMphase)
  ,pmFX(masterPitch(audio,index),pmFXr,pmFXi,0-PMphase)
  )
  :
  (
   /*(_<:(_,stringloopBank(masterPitch(audio,index),audio,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH)):>_)*/
  /*,(_<:(_,stringloopBank(masterPitch(audio,index),audio,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH)):>_)*/
   (_<:(_,stringloop(masterPitch(audio,index)*KPoctave,audio,_,phaseM,DCnonlin+DCleftRight)):>_)
  ,(_<:(_,stringloop(masterPitch(audio,index)*KPoctave,audio,_,0-phaseM,DCnonlin-DCleftRight)):>_)
  )
  :stereoLimiter(masterPitch(audio,index) * KPoctave) //needs the pitch to adjust the decay time.
  //:VuMeter
  with {
    //swap the order of KP and PM, to reflect the routing here. Unfortunately, this doesn't work...
    //pmFXgroup(x)      = FXGroup(vgroup("[1]Phase Modulation1[tooltip: delay lines modulated by an oscilator]",x));
    //KPgroup(x)        = FXGroup(vgroup("[2]Karplus-Strong2[tooltip: a feedback delay line whose delay-time is tuned to the input pitch]", x));
    //remove caption. Unfortunately, this doesn't work either...
    //MKPgroup(x)       = KPgroup(x);
    // add octave slider:
    KPoctave          = mainKPgroup(vslider("[-1]octave",-1, -2, 2, 1):octaveMultiplier);
  };
//-----------------------------------------------
// process
//-----------------------------------------------

    KPoctave          = mainKPgroup(vslider("[-1]octave",-1, -2, 2, 1):octaveMultiplier);
process(audio) = VoiceOfFaust(audio);
//process(audio) = audio:stringloop(masterPitch(audio,index),audio,_,phaseM,DCnonlin);

 //process(audio) =  _<:(_,stringloop(masterPitch(audio,index)*KPoctave,audio,_,phaseM,DCnonlin+DCleftRight)):>_;
