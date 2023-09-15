import("../lib/follower.lib");
import ("../lib/common.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/FMvocoder.lib");

process(audio,indexx,fidelity) =
  // FMvocoder(audio, guidePitch(audio,index),index,fidelity,doubleOscs);
  myindex
, detuneOsc(myindex);

detuneOsc(index) = index+offset(index):ma.decimal
with {
  offset(index) = FB(index)~_*amount;
  FB(index,prevOffset) =
    (dif(index)/(amount:max(ma.EPSILON))*
     // select2(os.lf_squarewavepos(hslider("freq", 0.1, 0, 10, 0.001)),1,-1)
     os.lf_triangle(hslider("freq", 0.1, 0, 10, 0.001))
    )+
    (prevOffset

    );
  dif(index) = index-index':ma.decimal;
  amount = hslider("amount", 0, 0, 0.1, 0.001);
};


// phaseNoises(7,69,1,1,FMvocoderGroup);
// funds(69,myindex,0,FMvocoderGroup);
// simpleAnalizer(audio,69);
// outputAnalizer(69, FMvocoderGroup, audio);
// ,
// LpHpAnalizer(audio,69)
// ;



// eqCompensate(69, FMvocoderGroup, 1, 2,3);
myindex = os.lf_sawpos(69*0.25);
