import("../lib/follower.lib");
import ("../lib/common.lib");
// specific to this synth:
import ("../lib/FullGUI.lib");
import ("../lib/FMvocoder.lib");

process(audio,index,fidelity) =
  FMvocoder(audio, guidePitch(audio,index),index,fidelity,doubleOscs);

// simpleAnalizer(audio,69);
// outputAnalizer(69, FMvocoderGroup, audio);
// ,
// LpHpAnalizer(audio,69)
// ;



// eqCompensate(69, FMvocoderGroup, 1, 2,3);

// process =
// no.noise
// : ls(100,hslider("gain0", 0, -48, 48, 1):si.smoo)
// : bs(100,184.7849797,hslider("gain1", 0, -48, 48, 1):si.smoo)
// : bs(184.7849797,251,hslider("gain2", 0, -48, 48, 1):si.smoo)
// : bs(251,341,hslider("gain3", 0, -48, 48, 1):si.smoo)
// : hs(341,hslider("gain4", 0, -48, 48, 1):si.smoo)
// : ls(hslider("freq1", 100, 100, 400, 1),hslider("gain_freq1", 0, -48, 48, 1):si.smoo)
// : ls(hslider("freq2", 100, 100, 400, 1),hslider("gain_freq2", 0, -48, 48, 1):si.smoo)
// ;

ls3(f,g) = fi.svf.ls (f, .5, g3) : fi.svf.ls (f, .707, g3) : fi.svf.ls (f, 2, g3) with {g3 = g/3;};
bs3(f1,f2,g) = ls3(f1,-g) : ls3(f2,g);
hs3(f,g) = fi.svf.hs (f, .5, g3) : fi.svf.hs (f, .707, g3) : fi.svf.hs (f, 2, g3) with {g3 = g/3;};

ls6(f,g) =
  fi.svf.ls (f, .5, g6) : fi.svf.ls (f, .707, g6) : fi.svf.ls (f, 2, g6)
  : fi.svf.ls (f, .5, g6) : fi.svf.ls (f, .707, g6) : fi.svf.ls (f, 2, g6)
with {g6 = g/6;};
bs6(f1,f2,g) = ls6(f1,-g) : ls6(f2,g);
hs6(f,g) =
  fi.svf.hs (f, .5, g6) : fi.svf.hs (f, .707, g6) : fi.svf.hs (f, 2, g6)
  : fi.svf.hs (f, .5, g6) : fi.svf.hs (f, .707, g6) : fi.svf.hs (f, 2, g6)
with {g6 = g/6;};

order = hslider("order", 0, 0, 5, 1);

bsN(N,f1,f2,g,x) = fi.lowshelf(N,-g,f1,x) : fi.lowshelf(N,g,f2);

ls(f,g,x) =
  par(i, 6, fi.lowshelf(i*4+7,g,f,x)*(i==order)) :>_;
bs(f1,f2,g,x) =
  par(i, 6, bsN(i*4+7,f1,f2,g,x)*(i==order)) :>_;
hs(f,g,x) =
  par(i, 6, fi.highshelf(i*4+7,g,f,x)*(i==order)) :>_;
