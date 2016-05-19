//-------------------------------------
//   A formant wave function generator
//-------------------------------------
import("music.lib");
import("filter.lib");
import("oscillator.lib");

// Sample period
T = 1.0/ml.SR;

//****************** user inputs ******************/

// fundmental frequency of tone with vibrato
// f0 = 110;
f0 = OSCpitch:smooth(PTsmooth);
// f0 = hgroup("[1]",nentry("Freq [style:knob]",80,30,350,1) + (vibGain*osc(vibRate))):dezip; // fundamental freq (Hz)
// choice of formant structure (vowel a,e,i,o and u)
vow = hgroup("[1]",nentry("[5:]A_E_I_O_U",1,1,5,1));
// vibrato rate
vibRate = hgroup("[1]",nentry("Vib Rate [style:knob]",4,2,7,0.1));
// vibrato gain
vibGain = 0;
// vibGain = hgroup("[1]",nentry("Vib Gain [style:knob]",1,0.2,1.2,0.001));
// display group to group f0 and vow controls together
ctrls = vgroup("",hgroup("[1]",f0,vow));

//****************** formant values **********************//
// peak center frequencies
fc(0) = 600; // a
fc(1) = 1040;
fc(2) = 2250;
fc(3) = 2450;
fc(4) = 2750;
fc(5) = 400; // e
fc(6) = 1620;
fc(7) = 2400;
fc(8) = 2800;
fc(9) = 3100;
fc(10) = 250; // i
fc(11) = 1750;
fc(12) = 2600;
fc(13) = 3050;
fc(14) = 3340;
fc(15) = 400; // o
fc(16) = 750;
fc(17) = 2400;
fc(18) = 2600;
fc(19) = 2900;
fc(20) = 350; // u
fc(21) = 600;
fc(22) = 2400;
fc(23) = 2675;
fc(24) = 2950;
// peak amplitudes
// A(0) = 0; // a
// A(1) = -7;
// A(2) = -9;
// A(3) = -9;
// A(4) = -20;
// A(5) = 0; // e
// A(6) = -12;
// A(7) = -9;
// A(8) = -12;
// A(9) = -18;
// A(10) = 0; // i
// A(11) = -30;
// A(12) = -16;
// A(13) = -22;
// A(14) = -28;
// A(15) = 0; // o
// A(16) = -11;
// A(17) = -21;
// A(18) = -20;
// A(19) = -40;
// A(20) = 0; // u
// A(21) = -20;
// A(22) = -32;
// A(23) = -28;
// A(24) = -36;
// formant peak bandwidths
BW(0) = 60; // a
BW(1) = 70;
BW(2) = 110;
BW(3) = 120;
BW(4) = 130;
BW(5) = 40; // e
BW(6) = 80;
BW(7) = 100;
BW(8) = 120;
BW(9) = 120;
BW(10) = 60; // i
BW(11) = 90;
BW(12) = 100;
BW(13) = 120;
BW(14) = 120;
BW(15) = 40; // o
BW(16) = 80;
BW(17) = 100;
BW(18) = 120;
BW(19) = 120;
BW(20) = 40; // u
BW(21) = 80;
BW(22) = 100;
BW(23) = 120;
BW(24) = 120;
// fof equation attack times (controls formant skirt widths
t(0) = 0.000002;
t(1) = 0.0000015;
t(2) = 0.0000015;
t(3) = 0.000003;
t(4) = 0.000001;

// constants based on user input
k1(0) = t(0) * SR; // attack in samples
k1(1) = t(1) * SR;
k1(2) = t(2) * SR;
k1(3) = t(3) * SR;
k1(4) = t(4) * SR;
beta(0) = PI / float(k1(0)); // attack value needed for attack(i) function
beta(1) = PI / float(k1(1));
beta(2) = PI / float(k1(2));
beta(3) = PI / float(k1(3));
beta(4) = PI / float(k1(4));
f0Period = SR / f0; // fund period length in fractional samples
sigLen(BW) = int(SR * log(0.001) / (-BW * PI)) + 1; // foflet T60 in samples

// functions used in foflet calculation
// k = (+(1)~_) - 1;
maxOctavation = 3;
multi = 2:pow(maxOctavation);
multiK(multi)= lf_rawsaw(f0Period*multi);
k = (((multiK(multi))/f0Period) : decimal)*f0Period; //choose octaves
// k= lf_rawsaw(f0Period);
expy(fund,BW) = exp(-BW * PI * T)^(fund/multi); // exponential env (BW = BW)
// bug in original: we don't want * SR.
envAttack(fund,beta) = 0.5 * (1.0 - cos(beta * (fund) )); // attack discontinuity smoother (beta=beta)
// envAttack(beta) = 0.5 * (1.0 - cos(beta * k * SR)); // attack discontinuity smoother (y=beta)
sinus(fund,fc,phase) = sin((2.0 * PI * fc * (fund) * T)+(phase*0*PI)); // sinusoid (z=fc)

// functions to calculate fof attack and decay sections
fofStop(BW) = k < sigLen(BW); // gate
fofAttack(fund,phase,BW,beta,fc) = expy(fund,BW) * envAttack(fund,beta) * sinus(fund,fc,phase); // first part of fof calculation
fofRemainder(fund,phase,BW,fc) =   expy(fund,BW) * sinus(fund,fc,phase); // 2nd part of fof calculation
// function to generate single fof
// fof(k1,phase,BW,beta,fc) = (fofAttack(phase,BW,beta,fc)); // k1 = k1
// fof(k1,phase,BW,beta,fc) = (k < int(PI/beta)) *fofAttack(phase,BW,beta,fc); // v = k1
fofPart(fund,k1,phase,BW,fc) = (((phasedFund) < int(k1)) * fofAttack(phasedFund,phase,BW,beta,fc)) + (((phasedFund) >= int(k1)) * fofRemainder(phasedFund,phase,BW,fc)) with {
  beta = PI / (float(k1));
  phasedFund = (((fund/(multi*f0Period))+(phase)):decimal)*f0Period*multi;
}; // v = k1
fof(phase,fc,fund,k1,BW,octaviation) =
// fofPart(fundI(0),k1,phase,BW,fc)
par(i,multi,fofPart(fundI(i),k1,phase,BW,fc)*OctMuliply(i)):>_
with {
  fundI(i) = (((fund/(multi*f0Period))+(i/multi)):decimal)*f0Period*multi;
  OctMuliply(i) =
    (OctMuliplyPart(i,floor(octaviation+1))*    decimal(octaviation)) +
    (OctMuliplyPart(i,floor(octaviation  ))* (1-decimal(octaviation)));
  OctMuliplyPart(i,oct) = select2(i>0,1,
    (((i % int(2:pow(oct)))):min(1)*-1)+1
  );

  // OctMuliply(0) = 1;
  // OctMuliply(i) = ((fmod(i,2:pow(octaviation))):min(1)*-1)+1;
};
// fof(k1,phase,BW,beta,fc) = ((k >= int(k1)) * fofRemainder(phase,BW,fc)); // v = k1
// function to play single fof
// playFof(k1,phase,BW,beta,fc) = fof(k1,phase,BW,beta,fc) ;
playFof(k1,phase,BW,beta,fc) = (+(fof(k1,phase,BW,beta,fc) * fofStop(BW))~fdelay2(2048,f0Period-1.0));
// function to play 5 fofs in parallel (5 fofs = 1 vowel
allFofs(j) = par(i,5,playFof(k1(i),phase((j-1)*5+i),BW((j-1)*5+i),beta(i),fc((j-1)*5+i))) :>_;

// adsr variables and controls
// attack = nentry("[1:]attack [style:knob]",1,0,4,0.1);
// decay = nentry("[2:]decay[style:knob]",0.3,0,1,0.001);
// sustain = nentry("[3:]sustain[style:knob]",0.5,0,1,0.01);
// release = nentry("[4:]release[style:knob]",0.5,0,4,0.1);
// // display group to group ADSR controls together
volADSR = vgroup("[2]",hgroup("ADSR", 0.75*adsr(attack,decay,sustain,release) : _));

// final process line: feed play button to volADSR to currently active vowel Fofs and then sum
// process = vgroup("[3]",button("play")) <: (volADSR <: (par(i,5,*(allFofs(i+1)*((i+1)==vow)))) :>_<:_,_);
// process =
// (fof(phase,fc,fund,k1,BW,octaviation)*0.1),
// (fof(phase*-1,fc,fund,k1,BW,octaviation)*0.1)
// ,fund/(multi*f0Period)
// with {
// fund = multiK(multi);
// k1 = hslider("k1",2,0.1,10,0.001)*t(4)*SR*f0Period:dezip;
// BW = hslider("BW",20,2,2000,1)*multi:dezip;
// fc = hslider("fc",2,0.5,128,0.001)*f0:dezip;
// octaviation = hslider("octaviation",0,0,maxOctavation,0.001):dezip;
// phase = hslider("phase",0,0,1,0.001)*2*PI:dezip;
// };

// dezip = smooth(0.999);
