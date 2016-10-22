declare name 		"jcmlb";
declare version 	"1.0";
declare author 		"cc";
declare license 	"BSD";
declare copyright 	"stk";

import("stdfaust.lib");

//form(f0,a,b,c) = (even) : *(a) // jc
form(f0,a,b,c,w) = (even + odd) : *(a) // mlb
with
{

  f1cr = ba.if((c<f0),1.0,(c/f0));

f1ci = ba.if((w==0),float(int(f1cr)),floor(f1cr));
//  f1ci = float(int( f1cr )); // jc
//  f1ci = floor( f1cr ); // mlb

  f1cm = fmod(f1ci, 2);

isEven = ba.if((w==0), 1, ba.if ((f1cm < 1.0),1,0) );
//  isEven = 1; // jc
//  isEven = ba.if ((f1cm < 1.0),1,0); // mlb

  evenfreq = ba.if (isEven, f1ci, (1 + f1ci) );
  oddfreq = ba.if (isEven, (1 + f1ci), f1ci );
  ampfrac = (f1cr - f1ci);
  oddamp = ba.if (isEven, ampfrac, (1.0 - ampfrac) );

evenamp = ba.if((w==0), 1, ba.if (isEven, (1.0 - ampfrac), ampfrac ) );
//  evenamp = 1; // jc
//  evenamp = ba.if (isEven, (1.0 - ampfrac), ampfrac ); // mlb

  mod = os.osc(f0) : *(f0 * b); // jc mlb

  even = evenamp :   *(os.osc((f0 * evenfreq) + mod));

odd = ba.if((w==0), 0, oddamp:*(os.osc((f0 * oddfreq) + mod)) );
//  odd = oddamp :   *(os.osc((f0 * oddfreq) + mod));
};
frame(c) = (w ~ _ )
with {
  rst(y)= ba.if(c,-y,1);
  w(x) 	= x+rst(x); };
demux(i,ctr,x) = coef
with {
  trig	= (ctr==i);
  coef 	= (*(1-trig)+x*trig) ~ _; };
nf = 4;
formant(f_num,ctlStream) = fsig
with {
  ctr	= frame(ctlStream<0);
  co(i)	= demux(i,ctr,ctlStream);
  f0 	= 1;
  a 	= f0+1+f_num*3;
  b 	= a+1;
  c 	= a+2;
  which	= f0+1+nf*3;
 fsig 	= form(co(f0), co(a),
     co(b), co(c), co(which));
 };
knee	= 48000.0;
filt = fi.tf2s(0,0,1,sqrt(2),1,ma.PI*knee/2);
// process = _ <: par(i,nf,formant(i)) :> fi.dcblocker : filt : filt;
process = form(freq,amp,bandwidth,center);

freq = vslider("freq", 110, 55, 440, 1):si.smooth(0.999);
amp = vslider("amp", 0, 0, 1, 0.001):si.smooth(0.999);
bandwidth = vslider("bandwidth", 1, 0, 100, 0.001):si.smooth(0.999);
center =vslider("center", 110, 55, 440, 1):si.smooth(0.999);
