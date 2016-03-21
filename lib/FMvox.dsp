declare name 		"FMVox";
declare version		"1.0";
declare author		"Chris Chafe";
declare license		"BSD";
declare copyright	"stk";

import("filter.lib");

// ------ Program A of "Glitch Free FM Vocal Synthesis" (minor corrections from article)

// ------ table lookup oscillators -----//
ts 	= 1 << 16;			// table size
fs 	= float(ts);
ts1	= ts+1;
ct 	= (+(1)~_ ) - 1;		// incrementing counter from 0
fct	= float(ct);
sc 	= fct*(2*PI)/fs:sin;		// sine table for carrier
sm	= fct*(2*PI)/fs:sin:/(2*PI);	// sine table for modulator
dec(x)	= x-floor(x);			// fractional remainder
pha(f)	= f/float(SR):(+:dec) ~ _;	// generates a phasor signal
tbl(t,p)= s1+dec(f)*(s2-s1)		// looks up linearly interpolated table value
with {
  f = p*fs;
  i = int(f);
  s1 = rdtable(ts1,t,i);
  s2 = rdtable(ts1,t,i+1);
};

// ------ formant generator using uniform (phase-synchronous) oscillators -----//
// fupho(f0,a,b,c) = ef
// fupho(f0,a,b,c) = tbl(sc,(dec(ef*ph+m)))
// fupho(f0,a,b,c) = tbl(sc,(dec(ef*ph+m)))
// fupho(f0,a,b,c) = (even+odd):*(a)	// outputs the sum of bracketing harmonics
with {					// from f0, amp, bandwidth, center freq
  cf 	= c/f0;
  ci	= floor(cf);			// integer harmonic below center freq
  ci1	= ci+1;				// and above
  isEven= if((fmod(ci,2)<1),1,0);	// below is even?
  ef 	= if(isEven,ci,ci1);		// then set even harmonic to lowest
  of 	= if(isEven,ci1,ci);		// and odd harmonic to highest
  frac	= cf-ci;			// fractional frequency remainder
  comp	= 1-frac;
  oa 	= if(isEven,frac,comp);		// odd harmonic amplitude
  ea 	= if(isEven,comp,frac);		// even harmonic amplitude
  ph 	= pha(f0);			// phasor signal at fundamental
  m 	= tbl(sm,ph):*(b);		// modulator sine signal
  even 	= ea:*(tbl(sc,(dec(ef*ph+m)))); // even harmonic signal with phase modulation
  odd 	= oa:*(tbl(sc,(dec(of*ph+m))));	// odd harmonic signal
};

// ------ parser functions for stream with interleaved formant parameters -----//
frame(c) = (w ~ _ )			// detects a new frame and starts counting
with {
  rst(y)= if(c,-y,1);
  w(x) 	= x+rst(x);
};

demux(i,ctr,x) = coef			// demux'es a given offset in frame
with {
  trig	= (ctr==i);
  coef 	= (*(1-trig)+x*trig) ~ _;
};

// ------ parser for stream with interleaved formant parameters -----//
formant(f_num,ctlStream) = fsig		// applies parameters to formant generator
with {
  ctr	= frame(ctlStream<0);		// negative value flags start of a new frame
  co(i)	= demux(i,ctr,ctlStream);	// demux using frame counter
  f0 	= 1;				// f0 pitch in counter position 1
  a 	= f0+1+f_num*3;			// f1 amp in position 2, f2 amp in 5...
  b 	= a+1;				// f1 bandwidth in 3...
  c 	= a+2;				// f1 center freq in 4...
 fsig 	= fupho(co(f0), co(a),		// signal for formant number f_num
     co(b), co(c));
};

// ------ vox synthesizer with single multiplexed control stream input -----//
nf = 4;					// number of formants
// process = _<: par(i,nf,formant(i)) :>_;	// run formants in parallel, sum their outputs
process = formant(1);
// process = fupho;
