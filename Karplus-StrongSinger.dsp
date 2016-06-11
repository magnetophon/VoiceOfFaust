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
import("lib/master.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/classicVocoder.lib");
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

VoiceOfFaust(audio,index) =
  (voice(audio,index)<:_,_)
  :
  (
   pmFX(freq,pmFXr,pmFXi,PMphase)
  ,pmFX(freq,pmFXr,pmFXi,0-PMphase)
  )
  :
  (
   (_<:(DryPath(phaseDry,DCnonlinDry+DCleftRightDry),stringloop(freq*KPoctave,audio,index,_,phaseM,DCnonlin+DCleftRight)):>_)
  ,(_<:(DryPath(0-phaseDry,DCnonlinDry-DCleftRightDry),stringloop(freq*KPoctave,audio,index,_,0-phaseM,DCnonlin-DCleftRight)):>_)
  )
  :stereoLimiter(freq * KPoctave) //needs the pitch to adjust the decay time.
  // :VuMeter
  with {
    DryPath(phase,DC) =  MyNonLinearModulator(nonLinDry,frequencyModDry*freq,phase,DC)*KPvolDry;

    // add octave slider:
    KPoctave          = mainKPgroup(vslider("[-2]octave",-1, -2, 2, 1):octaveMultiplier);
    freq = masterPitch(audio,index);
  };
//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio,index);