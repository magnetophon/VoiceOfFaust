import("stdfaust.lib");

// import("lib/master.lib");
import("lib/slave.lib");
import ("lib/common.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/FMvocoder.lib");

process(audio,index,fidelity) =
  // os.lf_sawpos(fr)
  // ,
  // os.lf_sawpos( index2freq(os.lf_sawpos(fr) ))
  // ,
  // my_lf_sawpos(fr)
  // ,
  // my_lf_sawpos( index2freq(my_lf_sawpos(fr)) )
  // ,
  // my_lf_sawpos(fr)

// , (my_lf_sawpos(fr))
  ((soft_sync_lf_sawpos(frequency,syncer)*0.25)+0.5)
  ,

(sync_sawN(2,frequency,syncer)*0.5)
  // index,
  // my_lf_sawpos(freq/4)

with {
  my_lf_sawpos(freq) = ma.decimal ~ +((freq/ma.SR));
  frequency = hslider("frequency", 55, -55, 440, 1);
  frequency2 = hslider("frequency 2", 55, -55, 440, 1);
  depth = hslider("lfo depth", 1, 0, 2, 0.001);
  lfo = os.oscrs(2);
  syncer = os.lf_imptrain(frequency2);
  fr = frequency+(lfo*depth*frequency);
  // index2freq(index) = ((index-index')*ma.SR) : sampleAndHold((index-index')!=-1);
  // sampleAndHold(sample) = select2((sample!=0):int) ~ _;

  soft_sync_lf_sawpos(freq,sync) = ma.decimal ~ +(delta*sign) with {
    delta = freq/ma.SR;
    sign = (_<:select2(sync==1,_*1,_*-1)+impulse)~_;
  };
  // --- sync_sawN for N = 1 to 6 ---
  sync_sawN(N,freq,sync) = sync_saw1 : poly(N) : D(N-1) : gate(N-1)
  with {
    p0n = float(ma.SR)/float(freq); // period in samples
    lfsawpos = (_,1:fmod) ~ +(1.0/p0n); // os.sawtooth waveform in [0,1)
    // To introduce a phase offset here (phase in [0,1]):
    // lfsawpos(phase) = (+(phase*(1-1')), 1 : fmod ) ~ +(1.0/p0n);
    sync_saw1 = 2* soft_sync_lf_sawpos(freq,sync) - 1; // fi.zero-mean, amplitude +/- 1
    poly(1,x) =  x;
    poly(2,x) =  x*x;
    poly(3,x) =  x*x*x - x;
    poly(4,x) =  x*x*(x*x - 2.0);
    poly(5,x) =  pow(x,5) - pow(x,3)*10.0/3.0 + x*7.0/3.0;
    poly(6,x) =  pow(x,6) - 5.0*pow(x,4) + 7.0*poly(2,x);
    diff1(x) =  (x - x')/(2.0/p0n);
    diff(N) = seq(n,N,diff1); // N diff1s in series
    D(0) = _;
    D(1) = diff1/2.0;
    D(2) = diff(2)/6.0;
    D(3) = diff(3)/24.0;
    D(4) = diff(4)/120.0;
    D(5) = diff(5)/720.0;
    gate(N) = *(1@(N)); // delayed step for blanking startup glitch
  };

};
