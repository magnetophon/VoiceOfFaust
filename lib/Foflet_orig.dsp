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
f0 = hgroup("[1]",nentry("Freq [style:knob]",80,30,350,1) + (vibGain*osc(vibRate))); // fundamental freq (Hz)
// choice of formant structure (vowel a,e,i,o and u)
vow = hgroup("[1]",nentry("[5:]A_E_I_O_U",1,1,5,1));
// vibrato rate
vibRate = hgroup("[1]",nentry("Vib Rate [style:knob]",4,2,7,0.1));
// vibrato gain
vibGain = hgroup("[1]",nentry("Vib Gain [style:knob]",1,0.2,1.2,0.001));
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
A(0) = 0; // a
A(1) = -7;
A(2) = -9;
A(3) = -9;
A(4) = -20;
A(5) = 0; // e
A(6) = -12;
A(7) = -9;
A(8) = -12;
A(9) = -18;
A(10) = 0; // i
A(11) = -30;
A(12) = -16;
A(13) = -22;
A(14) = -28;
A(15) = 0; // o
A(16) = -11;
A(17) = -21;
A(18) = -20;
A(19) = -40;
A(20) = 0; // u
A(21) = -20;
A(22) = -32;
A(23) = -28;
A(24) = -36;
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
sigLen(x) = int(SR * log(0.001) / (-x * PI)) + 1; // foflet T60 in samples

// functions used in foflet calculation
k = (+(1)~_) - 1;
expy(x) = exp(-x * PI * T)^k; // exponential env (x = BW)
envAttack(y) = 0.5 * (1.0 - cos(y * k * SR)); // attack discontinuity smoother (y=beta)
sinus(z) = sin(2.0 * PI * z * k * T); // sinusoid (z=fc)

// functions to calculate fof attack and decay sections
fofStop(x) = k < sigLen(x); // gate
fofAttack(w,x,y,z) = ml.db2linear(w) * expy(x) * envAttack(y) * sinus(z); // first part of fof calculation
fofRemainder(w,x,z) = ml.db2linear(w) * expy(x) * sinus(z); // 2nd part of fof calculation
// function to generate single fof
fof(v,w,x,y,z) = ((k < int(v)) * fofAttack(w,x,y,z)) + ((k >= int(v)) * fofRemainder(w,x,z)); // v = k1
// function to play single fof
playFof(v,w,x,y,z) = (+(fof(v,w,x,y,z) * fofStop(x))~fdelay2(2048,f0Period-1.0));
// function to play 5 fofs in parallel (5 fofs = 1 vowel
allFofs(j) = par(i,5,playFof(k1(i),A((j-1)*5+i),BW((j-1)*5+i),beta(i),fc((j-1)*5+i))) :>_;

// adsr variables and controls
attack = nentry("[1:]attack [style:knob]",1,0,4,0.1);
decay = nentry("[2:]decay[style:knob]",0.3,0,1,0.001);
sustain = nentry("[3:]sustain[style:knob]",0.5,0,1,0.01);
release = nentry("[4:]release[style:knob]",0.5,0,4,0.1);
// display group to group ADSR controls together
volADSR = vgroup("[2]",hgroup("ADSR", 0.75*adsr(attack,decay,sustain,release) : _));

// final process line: feed play button to volADSR to currently active vowel Fofs and then sum
process = vgroup("[3]",button("play")) <: (volADSR <: (par(i,5,*(allFofs(i+1)*((i+1)==vow)))) :>_<:_,_);
