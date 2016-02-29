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
  (voice(audio)<:_,_)
  :
  (
   pmFX(PitchTracker(audio,enablePitchTracker),pmFXr,pmFXi,PMphase)
  ,pmFX(PitchTracker(audio,enablePitchTracker),pmFXr,pmFXi,0-PMphase)
  )
  :
  (
   (_<:(_,stringloopBank(PitchTracker(audio,enablePitchTracker),audio,_,phaseLL,phaseL,phaseM,phaseH,phaseHH,DCnonlinLL+DCleftRightLL,DCnonlinL+DCleftRightL,DCnonlin+DCleftRight,DCnonlinH+DCleftRightH,DCnonlinHH+DCleftRightHH)):>_)
  ,(_<:(_,stringloopBank(PitchTracker(audio,enablePitchTracker),audio,_,0-phaseLL,0-phaseL,0-phaseM,0-phaseH,0-phaseHH,DCnonlinLL-DCleftRightLL,DCnonlinL-DCleftRightL,DCnonlin-DCleftRight,DCnonlinH-DCleftRightH,DCnonlinHH-DCleftRightHH)):>_)
   /*(_<:(_,stringloop(PitchTracker(audio,enablePitchTracker)*KPoctave,audio,_,phaseM,DCnonlin+DCleftRight)):>_)*/
  /*,(_<:(_,stringloop(PitchTracker(audio,enablePitchTracker)*KPoctave,audio,_,0-phaseM,DCnonlin-DCleftRight)):>_)*/
  )
  :stereoLimiter(PitchTracker(audio,enablePitchTracker) * KPoctave) //needs the pitch to adjust the decay time.
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
/*OS = bus(nrBands):>_;*/
/*process = (selector(nrBands,nrBands+1));*/
/*process = (selector(3,nrBands+1),selector(nrBands+1,nrBands+1)):+;*/
/*process = bus(nrBands)<:(bus(nrBands),OS)<:(par(i, nrBands, (selector(i,nrBands+1),selector(nrBands,nrBands+1):+)));*/
/*process = bus(nrBands)<:(par(i, nrBands, (selector(i,nrBands), par(j,nrBands, selector(j,nrBands)))));*/
/*process = bus(4)<:(selector(1,4),selector(2,4),selector(3,4),selector(0,4));*/
/*process = EQbank;*/
/*process =     ((bus(nrBands)<:(bus(nrBands),interleave(nrBands,nrBands)):interleave(nrBands,nrBands+1)),bus(2*nrBands));*/
    /*(par(i,nrBands,_+OffSet),bus(nrBands),(q<:bus(nrBands)));*/
/*process = bus(nrBands)<:(bus(nrBands),interleave(nrBands,nrBands)):interleave(nrBands,nrBands+1);*/
/*process(q,OffSet) =  (bus(nrBands),interleave(4,nrBands),bus(2*nrBands));*/
/*process = eqOfN(9);*/
/*process(x) =     ((x,interleave(nrBands,3)):eqOfN(nrBands));*/
/*process = peak_eq_cq;*/
/*process = seq(i, nrBands, peak_eq_cq(7,8,9));*/
/*process = seq(j,(log(nrBands)/log(2)),par(k,nrBands/(2:pow(j+1)),min));*/
/*process = ((0,_:+),_):+;*/
/*process = (0,_:(+)):seq(i,nrBands,((+),_));*/
/*process = (0:seq(i,nrBands, +))/nrBands;*/
//process(audio) = audio:stringloop(PitchTracker(audio,enablePitchTracker),audio,_,phaseM,DCnonlin);

 //process(audio) =  _<:(_,stringloop(PitchTracker(audio,enablePitchTracker)*KPoctave,audio,_,phaseM,DCnonlin+DCleftRight)):>_;
