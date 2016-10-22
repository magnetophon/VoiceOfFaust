declare name      "Karplus-StrongSingerMaxi";
declare version   "1.1.1";
declare author    "Bart Brouns";
declare license   "GNU 3.0";
declare copyright "(c) Bart Brouns 2014";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and os.square oscillators adapted from the Faust library" ;

//-----------------------------------------------
// imports
//-----------------------------------------------
//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo

import ("lib/common.lib");
import("lib/slave.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/classicVocoder.lib");
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

VoiceOfFaust(audio,index,fidelity) =
  (voice(audio,freq)<:_,_)
  :
  (
   pmFX(freq,pmFXr,pmFXi,PMphase)
  ,pmFX(freq,pmFXr,pmFXi,0-PMphase)
  )
  :
  (
   (_<:(DryPath(phaseDry,DCnonlinDry+DCleftRightDry),stringloopBank(freq,fidelity,audio,index,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH)):>_)
  ,(_<:(DryPath(0-phaseDry,DCnonlinDry-DCleftRightDry),stringloopBank(freq,fidelity,audio,index,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH)):>_)
  )
  :stereoLimiter(freq * 0.25) //needs the pitch to adjust the decay time.
  :VuMeter(2,enableVUmeter)
  with {
    DryPath(phase,DC) =  MyNonLinearModulator(nonLinDry,frequencyModDry*freq,phase,DC)*KPvolDry;
    freq = masterPitch(audio,index);
  };
//-----------------------------------------------
// process
//-----------------------------------------------

process(audio,index,fidelity) = VoiceOfFaust(audio,index,fidelity);
