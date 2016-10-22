//-------------------------------------
//   A formant wave function generator
//-------------------------------------
import("stdfaust.lib");

// Based on
//https://ccrma.stanford.edu/~mjolsen/220a/fp/Foflet.dsp
// by Michael Jørgen Olsen

// modified for dynamic parameters and octavation by Bart Brouns

// Sample period
T = 1.0/ma.SR;
t(4) = 0.000001;
// functions used in foflet calculation
multi = 2:pow(maxOctavation);
expy(fund,BW) = exp(-BW * ma.PI * T)^(fund/multi); // exponential env (BW = BW)
// bug in original: we don't want * ma.SR.
envAttack(fund,beta) = 0.5 * (1.0 - cos(beta * (fund) )); // attack discontinuity smoother (beta=beta)
// envAttack(beta) = 0.5 * (1.0 - cos(beta * k * ma.SR)); // attack discontinuity smoother (y=beta)
// most interesting sounds with sine at phase 0 and envelope phased
sinus(fund,fc,phase) = sin((2.0 * ma.PI * fc * (fund) * T)+(phase*0*ma.PI)); // sinusoid (z=fc)

// functions to calculate fof attack and decay sections
// fofStop(BW) = k < sigLen(BW); // gate
fofAttack(fund,phase,BW,beta,fc) = expy(fund,BW) * envAttack(fund,beta) * sinus(fund,fc,phase); // first part of fof calculation
fofRemainder(fund,phase,BW,fc) =   expy(fund,BW) * sinus(fund,fc,phase); // 2nd part of fof calculation
// function to generate single fof
// most interesting sounds with sine at phase 0 and envelope phased:
// fofPart(fund,k1,phase,BW,fc) = (((fund) < int(k1)) * fofAttack(fund,phase,BW,beta,fc)) + (((fund) >= int(k1)) * fofRemainder(fund,phase,BW,fc)) with {
fofPart(fund,freq,k1,phase,BW,fc) = (((phasedFund) < int(k1)) * fofAttack(phasedFund,phase,BW,beta,fc)) + (((phasedFund) >= int(k1)) * fofRemainder(phasedFund,phase,BW,fc)) with {
  beta = ma.PI / (float(k1));
  phasedFund = (((fund/(multi*f0Period))+(phase)):ma.decimal)*f0Period*multi;
  f0Period = ma.SR/freq;
}; // v = k1
// combines multiple fofParts to create octavation:
// octaviation index, normally fi.zero. If greater than fi.zero, lowers the effective xfund frequency by attenuating odd-numbered sinebursts. Whole numbers are full octaves, fractions transitional.
// source: https://csound.github.io/docs/manual/fof2.html
fof(phase,fc,fund,freq,k1,BW,octaviation) =
// fofPart(fundI(0),k1,phase,BW,fc)
par(i,multi,fofPart(fundI(i),freq,k1,phase,BW,fc)*OctMuliply(i)):>_
with {
  fundI(i) = (((fund/(multi*f0Period))+(i/multi)):ma.decimal)*f0Period*multi;
  OctMuliply(i) =
    (OctMuliplyPart(i,floor(octaviation+1))*    ma.decimal(octaviation)) +
    (OctMuliplyPart(i,floor(octaviation  ))* (1-ma.decimal(octaviation)));
  OctMuliplyPart(i,oct) = select2(i>0,1,
    (((i % int(2:pow(oct)))):min(1)*-1)+1
  );

  f0Period = ma.SR/freq;
};
