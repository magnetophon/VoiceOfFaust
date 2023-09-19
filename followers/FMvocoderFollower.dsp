import("../lib/follower.lib");
import ("../lib/common.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/FMvocoder.lib");

process(audio,indexx,fidelity) =
  // FMvocoder(audio, guidePitch(audio,index),index,fidelity,doubleOscs);
  // myindex:saw2sine
  // ,
  detuneOsc(myindex);

saw2sine(x) = 2*x*ma.PI:sin;

detuneOsc(index) =
  (index+detune(trueDelta(delta))
   :ma.decimal
   :saw2sine)
, (index+detune(trueDelta(delta*-0.5))
   :ma.decimal
   :saw2sine)
with {

  detune(x) =
    (x~_)
  ;
  trueDelta(d,prev) =
    (
      index+
      (
        (prev +d)

      )
      // * ((amount>0):seq(i, 4, si.smooth(0.9999)))
      :ma.decimal
       -index
    ): hbargraph("true delta", -2, 2)
  ;
  delta =
    amount * dif(index);
  dif(index) = index-index':ma.decimal;
  amount = (course + fine);
  course = hslider("note", 0, 0, 12, 1)/12;
  fine = hslider("cents", 0, 0, 100, 0.1)/1200;
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
