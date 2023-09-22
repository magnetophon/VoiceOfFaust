import("../lib/follower.lib");
import ("../lib/common.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/FMvocoder.lib");

process(audio,index,fidelity) =
  // FMvocoder(audio, guidePitch(audio,index),index,fidelity,doubleOscs);
  // myindex:saw2sine
  // ,
  detuneOsc(myindex,my_amount);

// do not smooth amount, it breaks the algo
my_amount = (course + fine);
course = hslider("note", 0, 0, 12, 1);
fine = hslider("cents", 0, 0, 100, 0.1)/100;

saw2sine(x) = 2*x*ma.PI:sin;

detuneOsc(index,amount) =
  (index+offset(amount)
   :ma.decimal
   :saw2sine)
,
  (index+offset(amount*-1)
   :ma.decimal
   :saw2sine)
with {
  offset(amount) =
    (FB(delta(amount))~_) ;
  FB(d,prev) =
    (prev+ba.sAndH(dontHold,d))
    :ma.decimal
  with {
    // TODO:
    // - when offset < .5, and amount is small: fade down instead of up
    // - implement minimum speed?
    dontHold =
      // don't hold when:
      // we are actively detuning
      hasAmount
      // or the offset wraps around
      | (nextOffset >=1)'
      | (nextOffset <0)
    ;
    hasAmount = abs(amount)>0;
    nextOffset =
      d':ba.sAndH(hasAmount)+prev;
  };
  delta(amount) =
    // if we add the diff for each sample, we go an octave up
    amount * dif(index)
    // to get the detune in notes, devide by 12
    // when amount is negative, halve the delta
    * select2(amount>0 , 1/24 , 1/12)
  ;
  dif(index) = index-index':ma.decimal;
};


lf_freq = hslider("lf freq", 0.1, 0, 10, 0.001):si.smoo;
phase = hslider("phase", 0, 0, 1, 0.001):si.smoo;
asym =
  1/3;
// hslider("asym", 1/3, 0, 1, 0.001);
lf_triangle_phase(freq,phase) = pos2plusMinus(1.0-abs(pos2plusMinus(lf_sawpos_phase(freq, phase)))); // saw1 defined below
lf_sawpos_phase(freq,phase) = os.lf_sawpos(lf_freq)+phase:ma.decimal;
pos2plusMinus(x) = 2 * x -1;

// input: 1 -> mult
// output 1 -> 0
// in-1 => 0 -> (mult-1)
// / (mult-1) =>  0 - 1

asym_lf_triangle_phase(asym,freq,phase) =
  select2(bigSaw > 1
         , bigSaw
         , 1-((bigSaw-1)/(mult-1))
         )
  : pos2plusMinus
with {
  saw = lf_sawpos_phase(freq,phase);
  bigSaw = saw*mult;
  mult = 1/max(asym,ma.EPSILON);
};

// phaseNoises(7,69,1,1,FMvocoderGroup);
// funds(69,myindex,0,FMvocoderGroup);
// simpleAnalizer(audio,69);
// outputAnalizer(69, FMvocoderGroup, audio);
// ,
// LpHpAnalizer(audio,69)
// ;



// eqCompensate(69, FMvocoderGroup, 1, 2,3);
myindex = os.lf_sawpos(hslider("osc freq", 69, 20, 1000, 1));
