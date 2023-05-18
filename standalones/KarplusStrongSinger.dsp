declare name      "KarplusStrongSinger";
declare version   "1.1.6";
declare author    "Bart Brouns";
declare license   "AGPL-3.0-only";
declare copyright "2014 - 2023, Bart Brouns";
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
import ("../lib/KarplusStrongFX.lib");
import ("../lib/pmFX.lib");

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
  ,(_<:(DryPath(0-phaseDry,DCnonlinDry-DCleftRightDry),stringloop(freq*KPoctave,audio,index,feedbackAmount,_,0-phaseM,DCnonlin-DCleftRight)):>_)
  )
  :stereoLimiter(freq * KPoctave) //needs the pitch to adjust the decay time.
  :VuMeter(2,enableVUmeter)
  with {
    DryPath(phase,DC) =  MyNonLinearModulator(nonLinDry,frequencyModDry*freq,phase,DC)*KPvolDry;

    // add octave slider:
    KPoctave       = mainKPgroup( vslider("[-2]octave",-1, -2, 2, 1):octaveMultiplier);
    // is used in stringloop. has a different default value than feedbackM
    // so when we only have one FB loop, turn it on!
    feedbackAmount  = MKPgroup(    vslider("[1]feedback[style:knob][tooltip: feedback amount for this octave]"   , 1, 0, 1, 0.001)):volScale; // -60db decay time (sec)
    freq           = guidePitch(audio,index);
};
//-----------------------------------------------
// process
//-----------------------------------------------

process(audio) = VoiceOfFaust(audio,index);
