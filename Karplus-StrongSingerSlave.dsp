declare name      "Karplus-StrongSinger";
declare version   "1.0";
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
import("lib/slave.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/classicVocoder.lib");
import ("lib/KarplusStrongFX.lib");
import ("lib/pmFX.lib");

//-----------------------------------------------
// VoiceOfFaust: Combine all the elements
//-----------------------------------------------

VoiceOfFaust(audio,index) =
  (voice(audio,freq)<:_,_)
  :
  (
   pmFX(freq,pmFXr,pmFXi,PMphase)
  ,pmFX(freq,pmFXr,pmFXi,0-PMphase)
  )
  :
  (
   (_<:(DryPath(phaseDry,DCnonlinDry+DCleftRightDry),stringloop(freq*KPoctave,audio,index,feedbackAmount,_,phaseM,DCnonlin+DCleftRight)):>_)
  ,(_<:(DryPath(0-phaseDry,DCnonlin-DCleftRight),stringloop(freq*KPoctave,audio,index,feedbackAmount,_,0-phaseM,DCnonlin-DCleftRight)):>_)
  )
  :stereoLimiter(freq * KPoctave) //needs the pitch to adjust the decay time.
  :VuMeter(2,enableVUmeter)
  with {
    DryPath(phase,DC) =  MyNonLinearModulator(nonLinDry,frequencyModDry*freq,phase,DC)*KPvolDry;

    // add octave slider:
    KPoctave          = mainKPgroup(vslider("[-2]octave",-1, -2, 2, 1):octaveMultiplier);
    KPvolume          = mainKPgroup(vslider("[0]wet volume [style:knob][tooltip: the output-level of the delay]",	0.4, 0, 1, 0.001):volScale);
    feedbackAmount    = MKPgroup(    vslider("[1]feedback[style:knob][tooltip: feedback amount for this octave]"   , 1, 0, 1, 0.001)):volScale; // -60db decay time (sec)
    freq = masterPitch(audio,index);
  };
//-----------------------------------------------
// process
//-----------------------------------------------

process(audio,index) = VoiceOfFaust(audio,index);