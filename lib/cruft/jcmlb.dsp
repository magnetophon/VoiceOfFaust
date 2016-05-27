declare name 		"jcmlb";
declare version 	"1.0";
declare author 		"cc";
declare license 	"BSD";
declare copyright 	"stk";

import("filter.lib");

//form(f0,a,b,c) = (even) : *(a) // jc
form(f0,a,b,c,w) = (even + odd) : *(a) // mlb
with
{

  f1cr = if((c<f0),1.0,(c/f0));

f1ci = if((w==0),float(int(f1cr)),floor(f1cr));
//  f1ci = float(int( f1cr )); // jc
//  f1ci = floor( f1cr ); // mlb

  f1cm = fmod(f1ci, 2);

isEven = if((w==0), 1, if ((f1cm < 1.0),1,0) );
//  isEven = 1; // jc
//  isEven = if ((f1cm < 1.0),1,0); // mlb

  evenfreq = if (isEven, f1ci, (1 + f1ci) );
  oddfreq = if (isEven, (1 + f1ci), f1ci );
  ampfrac = (f1cr - f1ci);
  oddamp = if (isEven, ampfrac, (1.0 - ampfrac) );

evenamp = if((w==0), 1, if (isEven, (1.0 - ampfrac), ampfrac ) );
//  evenamp = 1; // jc
//  evenamp = if (isEven, (1.0 - ampfrac), ampfrac ); // mlb

  mod = osc(f0) : *(f0 * b); // jc mlb

  even = evenamp :   *(osc((f0 * evenfreq) + mod));

odd = if((w==0), 0, oddamp:*(osc((f0 * oddfreq) + mod)) );
//  odd = oddamp :   *(osc((f0 * oddfreq) + mod));
};
frame(c) = (w ~ _ )
with {
  rst(y)= if(c,-y,1);
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
filt = tf2s(0,0,1,sqrt(2),1,PI*knee/2);
// process = _ <: par(i,nf,formant(i)) :> dcblocker : filt : filt;
process = form(freq,amp,bandwidth,center);


freq = vslider("freq", 110, 55, 440, 1):smooth(0.999);
amp = vslider("amp", 0, 0, 1, 0.001):smooth(0.999);
bandwidth = vslider("bandwidth", 1, 0, 100, 0.001):smooth(0.999);
center =vslider("center", 110, 55, 440, 1):smooth(0.999);