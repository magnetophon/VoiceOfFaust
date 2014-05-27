declare name 		"VocSynth";
declare version 	"0.2";
declare author 		"Bart Brouns";
declare license 	"GNU 3.0";
declare copyright 	"(c) Bart Brouns 2014";
declare coauthors	"PitchTracker by Tiziano Bole, qompander translated from a pd patch by Katja Vetter";

//-----------------------------------------------
// imports
//-----------------------------------------------

import ("oscillator.lib");
import ("maxmsp.lib");
import ("effect.lib");
import ("KarplusStrongFX.lib");
import ("NLFeksFX.lib");

qompander	= component("../qompander/qompander.dsp");
//KarplusStrongFX		= component("KarplusStrongFX.dsp");

//-----------------------------------------------
// contants
//-----------------------------------------------
//todo: test or ask about smooth vs line:, efficiency and behaviour
minline				= 3;		// minimum line time in ms
analizerQ			= 7;		// Q of the analizer bp filters 
MinInputPitch		= 61.7354;	// lowest expected note is a B1
MaxInputPitch		= 987.767;	// highest expected note is a B5
maxTimeWithoutPitch	= 2*SR;		// longest time the OSC pitch tracker can be silent before we switch to the intenal one. (in samples, so 2*SR is 2 seconds)

//-----------------------------------------------
// the GUI
//-----------------------------------------------

OSCgroup(x)  = (hgroup("[0]OSC", x)); // To recieve OSC pitch and other messages
//qompander = [1]
synthsGroup(x)  = (hgroup("[2]", x));
FXGroup(x)  = (hgroup("[3]", x));


OSCpitch	= OSCgroup(nentry("[0]pitch", MinInputPitch, MinInputPitch, MaxInputPitch, 0));
OSCfidelity     = OSCgroup(nentry("[1]fidelity", 0, 0, 1, 0));
OSConset     	= OSCgroup(button("onset"));

subGroup(x)	= synthsGroup((hgroup("[1]sub", x)));
subVolume	= subGroup(vslider("[1]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
subOctave	= subGroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of sub



vocoderGroup(x) 	= synthsGroup((hgroup("[2]vocoder", x)));
vocoderVolume	= vocoderGroup(vslider("[1]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
vocoderOctave	= vocoderGroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of vocoder
vocoderBottom	= vocoderGroup(vslider("[3]bottom",	1, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
vocoderTop	= vocoderGroup(vslider("[4]top",	8.5, 1, 10, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
vocoderQ	= vocoderGroup(vslider("[5]Q",	7, 0.3, 10, 0)<:(_,_):*:smooth(0.999));			//0.1 to 10 logarithmicly,
vocoderN	= 1;//vocoderGroup(vslider("[6]N",	1, 1, 6, 1));
vocoderMix	= vocoderGroup(vslider("[7]mix",	0, 0, 1, 0));								// is smoothed at the synth
vocoderDetune	= vocoderGroup(vslider("[8]detune",	0, 0, 1, 0):smooth(0.999));
vocoderWidth	= vocoderGroup(vslider("[9]width",	1, 0, 2, 0):smooth(0.999));				//wide pan, 0=mono 1=normal 2=full-wide


PAFvocoderGroup(x)  = synthsGroup((hgroup("[3]PAFvocoder", x)));
pafVolume	= PAFvocoderGroup(vslider("[1]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
pafOctave	= PAFvocoderGroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of paf
pafBottom	= PAFvocoderGroup(vslider("[3]bottom",	1, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
pafTop		= PAFvocoderGroup(vslider("[4]top",		8.5, 1, 10, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
pafIndex	= PAFvocoderGroup(vslider("[5]index",	25, 1, 100, 0):smooth(0.999));
pafWidth	= PAFvocoderGroup(vslider("[6]width",1, 0, 2, 0):smooth(0.999)); //wide pan, 0=mono 1=normal 2=full-wide

FMgroup(x)	 = synthsGroup((hgroup("[4]FM", x)));
FMvolume	= FMgroup(vslider("[01]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
//FMOctave	= FMgroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//not needed, we have all octaves! :)

LLFMgroup(x) = FMgroup((hgroup("[1]-2 oct", x)));
FMvolLL		= LLFMgroup(vslider("[1]vol",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexLL	= LLFMgroup(vslider("[2]index",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynLL		= LLFMgroup(vslider("[3]dyn",	0, 0, 1, 0):smooth(0.999));

LFMgroup(x)  = FMgroup((hgroup("[2]-1 oct", x)));
FMvolL		= LFMgroup(vslider("[1]vol",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexL	= LFMgroup(vslider("[2]index",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynL		= LFMgroup(vslider("[3]dyn",	0, 0, 1, 0):smooth(0.999));

mFMgroup(x)  = FMgroup((hgroup("[3]0 oct", x)));
FMvol		= mFMgroup(vslider("[1]vol",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindex		= mFMgroup(vslider("[2]index",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdyn		= mFMgroup(vslider("[3]dyn",	0, 0, 1, 0):smooth(0.999));

HFMgroup(x)  = FMgroup((hgroup("[4]+1 oct", x)));
FMvolH		= HFMgroup(vslider("[1]vol",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexH	= HFMgroup(vslider("[2]index",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynH		= HFMgroup(vslider("[3]dyn",	0, 0, 1, 0):smooth(0.999));

HHFMgroup(x)	= FMgroup((hgroup("[5]+2 oct", x)));
FMvolHH		= HHFMgroup(vslider("[1]vol",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexHH	= HHFMgroup(vslider("[2]index",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynHH		= HHFMgroup(vslider("[3]dyn",	0, 0, 1, 0):smooth(0.999));


//-----------------------------------------------
// Karplus Strong as an effect
//-----------------------------------------------
//todo: make FX group and routing
KPgroup(x)	 = FXGroup((vgroup("[0]Karplus-Strong", x)));
KPvolume	= KPgroup(vslider("[0]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly

LLKPgroup(x) = KPgroup((hgroup("[1]-2 oct", x)));
KPvolLL		= LLKPgroup(vslider("[0]vol",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModLL	= LLKPgroup(nentry("[1]Nonlinear Filter/typeMod",0,0,4,1));
t60LL		= LLKPgroup(vslider("[2]decaytime_T60", 4, 0, 3, 0.01))<:(_,_):*<:(_,_):*;  // -60db decay time (sec)
treshLL		= LLKPgroup(vslider("[3] Threshold [unit:dB] [tooltip: A limiter in the feedback-loop]", 0, -33, 33, 0.1));
nonLinLL	= LLKPgroup(vslider("[4]Nonlinearity",0,0,1,0.01) : smooth(0.999));
brightLL 	= LLKPgroup(vslider("[5]brightness", 0.5, 0, 1, 0.01));
frequencyModLL	= LLKPgroup(vslider("[6]freqMod",1,0,8,0) : smooth(0.999));

LKPgroup(x) = KPgroup((hgroup("[2]-1 oct", x)));
KPvolL		= LKPgroup(vslider("[0]vol",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModL	= LKPgroup(nentry("[1]Nonlinear Filter/typeMod",0,0,4,1));
t60L		= LKPgroup(vslider("[2]decaytime_T60", 4, 0, 3, 0.01))<:(_,_):*;  // -60db decay time (sec)
treshL		= LKPgroup(vslider("[3] Threshold [unit:dB] [tooltip: A limiter in the feedback-loop]", 0, -33, 33, 0.1));
nonLinL		= LKPgroup(vslider("[4]Nonlinearity",0,0,1,0.01) : smooth(0.999));
brightL 	= LKPgroup(vslider("[5]brightness", 0.5, 0, 1, 0.01));
frequencyModL	= LKPgroup(vslider("[6]freqMod",1,0,8,0) : smooth(0.999));

MKPgroup(x)	= KPgroup((hgroup("[2]0 oct", x)));
KPvol		= MKPgroup(vslider("[0]vol",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeMod		= MKPgroup(nentry("[1]Nonlinear Filter/typeMod",0,0,4,1));
t60		= MKPgroup(vslider("[2]decaytime_T60", 4, 0, 3, 0.01))<:(_,_):*;  // -60db decay time (sec)
tresh		= MKPgroup(vslider("[3] Threshold [unit:dB] [tooltip: A limiter in the feedback-loop]", 0, -33, 33, 0.1));
nonLin		= MKPgroup(vslider("[4]Nonlinearity",0,0,1,0.01) : smooth(0.999));
bright		= MKPgroup(vslider("[5]brightness", 0.5, 0, 1, 0.01));
frequencyMod	= MKPgroup(vslider("[6]freqMod",1,0,8,0) : smooth(0.999));

HKPgroup(x) = KPgroup((hgroup("[4]+1 oct", x)));
KPvolH		= HKPgroup(vslider("[0]vol",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModH	= HKPgroup(nentry("[1]Nonlinear Filter/typeMod",0,0,4,1));
t60H		= HKPgroup(vslider("[2]decaytime_T60", 4, 0, 3, 0.01))<:(_,_):*;  // -60db decay time (sec)
treshH		= HKPgroup(vslider("[3] Threshold [unit:dB] [tooltip: A limiter in the feedback-loop]", 0, -33, 33, 0.1));
nonLinH		= HKPgroup(vslider("[4]Nonlinearity",0,0,1,0.01) : smooth(0.999));
brightH 	= HKPgroup(vslider("[5]brightness", 0.5, 0, 1, 0.01));
frequencyModH	= HKPgroup(vslider("[6]freqMod",1,0,8,0) : smooth(0.999));

HHKPgroup(x) = KPgroup((hgroup("[5]+2 oct", x)));
KPvolHH		= HHKPgroup(vslider("[0]vol",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModHH	= HHKPgroup(nentry("[1]Nonlinear Filter/typeMod",0,0,4,1));
t60HH		= HHKPgroup(vslider("[2]decaytime_T60", 4, 0, 3, 0.01))<:(_,_):*;  // -60db decay time (sec)
treshHH		= HHKPgroup(vslider("[3] Threshold [unit:dB] [tooltip: A limiter in the feedback-loop]", 0, -33, 33, 0.1));
nonLinHH	= HHKPgroup(vslider("[4]Nonlinearity",0,0,1,0.01) : smooth(0.999));
brightHH 	= HHKPgroup(vslider("[5]brightness", 0.5, 0, 1, 0.01));
frequencyModHH	= HHKPgroup(vslider("[6]freqMod",1,0,8,0) : smooth(0.999));




//-----------------------------------------------
// Some general functions
//-----------------------------------------------

octaveMultiplier	= _<: (
			(_==-2) * 0.25,
			(_==-1) * 0.5,
			(_==0),
			(_==1) * 2,
			(_==2) * 4
):>_;

VocoderFreqs(bottom,top) =     par(i,16,   pow((pow((top/bottom),1/15)),i)*bottom);
//to make it stereo
//todo: implement http://music.columbia.edu/pipermail/music-dsp/2012-February/070328.html
WidePanner(w,L,R) = (((1+w)*L + (1-w)*R)/2) , (((1+w)*R + (1-w)*L)/2);
vocoderMixer = interleave(2,8):((bus(8):>_),(bus(8):>_));

//-----------------------------------------------
// Universal Pitch Tracker (a periods measurement)
//-----------------------------------------------

SH(trig,x) = (*(1 - trig) + x * trig) ~_;
a = 8.0;// vslider("n cycles", 1, 1, 100, 1) ;


Pitch(a,x) = a * SR / max(M,1) - a * SR * (M == 0)
with { 
      U = (x' < 0) & (x >= 0) ;
      V = +(U) ~ %(int(a)) ;
      W = U & (V == a) ;
      N = (+(1) : *(1 - W)) ~_;
      M = SH(N == 0, N' + 1) ;
};

// switch to internal pitchtracker if OSC is silent for too long
//todo: make a more elaborate version, or kill it alltogether
//for example, make the fidelity be a kill switch
PitchTracker(audio) = ((isSameTooLong(OSCfidelity), OSCpitch, internal):select2) :smooth(0.99)
with	{
		internal = (audio:dcblockerat(MinInputPitch) : (lowpass(1) : Pitch(a): min(MaxInputPitch) )  ~ max(MinInputPitch*2)) : max(MinInputPitch);
		isSameTooLong(x) = (x@maxTimeWithoutPitch==x);
		};

//-----------------------------------------------
// master index
//-----------------------------------------------
masterIndex(freq)= lf_sawpos(freq/4); // lowest possible pitch, as we can only shift up, using wrap
fund(freq,oct)= (4 * oct * masterIndex(freq)) - floor(4 * oct * masterIndex(freq)); //choose octaves

//-----------------------------------------------
// Sub sine
//-----------------------------------------------

subSine(audio,freq) = fund(freq,subOctave)*2*PI:sin * (subLevel(audio)*subVolume:lowpass(1,freq*subOctave))<:_,_;

//-----------------------------------------------
// vocoder analiser
//-----------------------------------------------

subLevel(audio) = audio:lowpass(3,300):amp_follower(0.05)*30:tanh; 

analizerCenters(freq) = VocoderFreqs(0.853553,128):(par(i,16, _,freq:*:min(SR/2)));
bandEnv(freq)=resonbp(freq,analizerQ,1):amp_follower_ud(0.01,0.01);  
analizers(audio,freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,freq12,freq13,freq14,freq15,freq16)=
audio<:
(
bandEnv(freq1),
bandEnv(freq2),
bandEnv(freq3),
bandEnv(freq4),
bandEnv(freq5),
bandEnv(freq6),
bandEnv(freq7),
bandEnv(freq8),
bandEnv(freq9),
bandEnv(freq10),
bandEnv(freq11),
bandEnv(freq12),
bandEnv(freq13),
bandEnv(freq14),
bandEnv(freq15),
bandEnv(freq16)
)
;
analizer(audio,freq)=(analizerCenters(freq)):analizers(audio);


//-----------------------------------------------
// slave oscilators
//-----------------------------------------------

//---------------- Bandlimited Sawtooth sawN, saw2, ... ------------------
// METHOD: Differentiated Polynomial Waves (DPW) (for aliasing suppression)
// REFERENCE: 
// "Alias-Suppressed Oscillators based on Differentiated Polynomial Waveforms",
// Vesa Valimaki, Juhan Nam, Julius Smith, and Jonathan Abel, 
// IEEE Tr. Acoustics, Speech, and Language Processing (IEEE-ASLP),
// Vol. 18, no. 5, May 2010.

//todo: research N

// --- sawN for N = 1 to 6 ---
//ws = waveshaping, so it takes an input saw as index
sawNws(N,lfsawpos,freq) = saw1 : poly(N) : D(N-1) : gate(N-1)
with {
  p0n = float(ml.SR)/float(freq); // period in samples
  //lfsawpos = (_,1:fmod) ~ +(1.0/p0n); // sawtooth waveform in [0,1)
  // To introduce a phase offset here (phase in [0,1]):
  // lfsawpos(phase) = (+(phase*(1-1')), 1 : fmod ) ~ +(1.0/p0n);
  saw1 = 2*lfsawpos - 1; // zero-mean, amplitude +/- 1
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


//emulation of a roland supersaw, based on: "How to Emulate the Super Saw" by ADAM SZABO
//http://www.nada.kth.se/utbildning/grukth/exjobb/rapportlistor/2010/rapporter10/szabo_adam_10131.pdf 
//Implemented in Faust by Bart Brouns
supersaw(N,fund,freq,detune,mix) = saws(fund,freq,detuner(detune)):mixer(mix) 
with {
detuner (detune) = 
(((detune*0.2),(detune<0.6)):*),
((((detune-0.6)*0.8+0.12),((detune>=0.6)&(detune<=0.95))):*),
((((detune-0.95)*12+0.4),(detune>0.95)):*)
:>_;
saws(fund,freq,det) = 
sawNws(N,fund,freq),
sawN(N,(det * -0.110023+1)*freq),
sawN(N,(det * -0.0628844+1)*freq),
sawN(N,(det * -0.0195236+1)*freq),
sawN(N,(det * 0.0199122+1)*freq),
sawN(N,(det * 0.0621654+1)*freq),
sawN(N,(det * 0.107452+1)*freq);

mainmix = mix * -0.55366 + 0.99785:smooth(0.999);
detunemix = (mix:pow(2) * -0.73764)+(mix * 1.2841):smooth(0.999);

mixer(mix) = (_*mainmix),par(i, 6, _*detunemix):>_;
};

//-----------------------------------------------
// Normal vocoder synthesis
//-----------------------------------------------

vocoderFund(freq)= fund(freq,vocoderOctave);
vocoderOsc(freq) =   supersaw(vocoderN,vocoderFund(freq),freq,vocoderDetune,vocoderMix);
//sawNws(vocoderN,vocoderFund(freq),freq*vocoderOctave);
//

volFilter(c,f,v) = f:resonbp(c,vocoderQ,gain)
with {
compensate = (tanh((1/(vocoderQ:min(1)))/2));
gain = line (v*compensate, minline);
};

volFilterBank(Center1,Center2,Center3,Center4,Center5,Center6,Center7,Center8,Center9,Center10,Center11,Center12,Center13,Center14,Center15,Center16,Volume1,Volume2,Volume3,Volume4,Volume5,Volume6,Volume7,Volume8,Volume9,Volume10,Volume11,Volume12,Volume13,Volume14,Volume15,Volume16,Oscilator)=
volFilter(Center1,Oscilator,Volume1),
volFilter(Center2,Oscilator,Volume2),
volFilter(Center3,Oscilator,Volume3),
volFilter(Center4,Oscilator,Volume4),
volFilter(Center5,Oscilator,Volume5),
volFilter(Center6,Oscilator,Volume6),
volFilter(Center7,Oscilator,Volume7),
volFilter(Center8,Oscilator,Volume8),
volFilter(Center9,Oscilator,Volume9),
volFilter(Center10,Oscilator,Volume10),
volFilter(Center11,Oscilator,Volume11),
volFilter(Center12,Oscilator,Volume12),
volFilter(Center13,Oscilator,Volume13),
volFilter(Center14,Oscilator,Volume14),
volFilter(Center15,Oscilator,Volume15),
volFilter(Center16,Oscilator,Volume16)
;


vocoderCenters(freq) = VocoderFreqs(vocoderBottom,vocoderTop):(par(i,16, _,freq * vocoderOctave:*:min(SR/2)));

vocoder(audio,freq)= (vocoderCenters(freq),analizer(audio:qompander,freq),vocoderOsc(freq)):volFilterBank:vocoderMixer:par(i, 2, _*vocoderVolume):WidePanner(vocoderWidth);

//-----------------------------------------------
// PAF oscilator
//-----------------------------------------------

pafFreq(audio)= PitchTracker(audio)*pafOctave;
pafFund(freq) = fund(freq,pafOctave);

bellcurve(x) = int(x):rdtable(belltablesize+1,curve,_)
with 	{
		belltablesize 	= 199;
		curve 	= time:(((_-100)/25)<:(_*_)*-1);
		};

//bellcurve = (((_-100)/25)<:(_*_)*-1);

sampleAndHold(sample) = select2((sample!=0):int) ~ _;
wrap=_<:(_>0,(_,1:fmod)+1,(_,1:fmod)):select2;
centerWrap(c,f) = line (c, 300):sampleAndHold(f)<:wrap;
centerMin(c,f) = c:sampleAndHold(f)-centerWrap(c,f);
cos12(c,f) = (centerMin(c,f)*f<:(_*2*PI:cos)<:(_,_((_,(_+f:(_*2*PI:cos))):_-_:(_*centerWrap(c,f))) )):_+_;
bell(f,i)= (((f*0.5)-0.25:(_*2*PI:cos))*line (i, 12))+100:bellcurve;

paf(c,f,i,v)= (((cos12(c,f))*bell(f,i)) * line (v, minline));


//-----------------------------------------------
// PAF vocoder synthesis
//-----------------------------------------------

//pafCenters=     par(i,16,   pow((pow((pafTop/pafBottom),1/15)),i)*pafBottom);
pafCenters = VocoderFreqs(pafBottom,pafTop);
pafOscs(pafCenter1,pafCenter2,pafCenter3,pafCenter4,pafCenter5,pafCenter6,pafCenter7,pafCenter8,pafCenter9,pafCenter10,pafCenter11,pafCenter12,pafCenter13,pafCenter14,pafCenter15,pafCenter16,pafVol1,pafVol2,pafVol3,pafVol4,pafVol5,pafVol6,pafVol7,pafVol8,pafVol9,pafVol10,pafVol11,pafVol12,pafVol13,pafVol14,pafVol15,pafVol16,Fund)=
paf(pafCenter1,Fund,pafIndex,pafVol1),
paf(pafCenter2,Fund,pafIndex,pafVol2),
paf(pafCenter3,Fund,pafIndex,pafVol3),
paf(pafCenter4,Fund,pafIndex,pafVol4),
paf(pafCenter5,Fund,pafIndex,pafVol5),
paf(pafCenter6,Fund,pafIndex,pafVol6),
paf(pafCenter7,Fund,pafIndex,pafVol7),
paf(pafCenter8,Fund,pafIndex,pafVol8),
paf(pafCenter9,Fund,pafIndex,pafVol9),
paf(pafCenter10,Fund,pafIndex,pafVol10),
paf(pafCenter11,Fund,pafIndex,pafVol11),
paf(pafCenter12,Fund,pafIndex,pafVol12),
paf(pafCenter13,Fund,pafIndex,pafVol13),
paf(pafCenter14,Fund,pafIndex,pafVol14),
paf(pafCenter15,Fund,pafIndex,pafVol15),
paf(pafCenter16,Fund,pafIndex,pafVol16)
;


pafvocoder(audio,freq)=(pafCenters,analizer(audio:qompander,freq),pafFund(freq)):pafOscs:vocoderMixer:par(i, 2, _*pafVolume):WidePanner(pafWidth);

//-----------------------------------------------
// input->FM synth
//-----------------------------------------------
FM(audio,freq)= osc(freq/2+audio* 5000)*subLevel(audio);

//compressor_mono(ratio,thresh,att,rel,x)
extremeLimiter = compressor_mono(100,-54,0.0008,0.002)*70;

equalpower = _<:			//3dB equal power curve
log(_	 * 5.63875+1)/ 1.89292,
log((1-_)* 5.63875+1)/ 1.89292;

//crossfades from no dynamics to normal dynamics to reversed ones
dynamics(limited, unlimited,dyn) = 
((dyn*2:min(1):equalpower),
(dyn-0.5:max(0)*2:equalpower:(_,_*-1):cross(2)))
:interleave(2,2):(bus2:>_),(bus2:>_)
:_*limited,_*unlimited:>_;

FMvoc(limited, unlimited,freq,vol,index,dyn) = osc((dynamics(limited, unlimited,dyn)*index)+freq)*vol;

FMSynth(limited, unlimited,freq,gain) =
(
FMvoc(limited, unlimited,freq*0.25,gain*FMvolLL,FMindexLL,FMdynLL),
FMvoc(limited, unlimited,freq*0.5,gain*FMvolL,FMindexL,FMdynL),
FMvoc(limited, unlimited,freq,gain*FMvol,FMindex,FMdyn),
FMvoc(limited, unlimited,freq*2,gain*FMvolH,FMindexH,FMdynH),
FMvoc(limited, unlimited,freq*4,gain*FMvolHH,FMindexHH,FMdynHH)
):>_*FMvolume<:_,_
;

//-----------------------------------------------
// Karplus-Strong effect 
//-----------------------------------------------
//KarplusStrongFX(x,freq,gain,resonance)
KarplusBank(audio,freq) = (
KarplusStrongFX(audio,freq*0.25,KPvolLL,KPresonanceLL),
KarplusStrongFX(audio,freq*0.5,KPvolL,KPresonanceL),
KarplusStrongFX(audio,freq,KPvol,KPresonance),
KarplusStrongFX(audio,freq*2,KPvolH,KPresonanceH),
KarplusStrongFX(audio,freq*4,KPvolHH,KPresonanceHH)
):>_*KPvolume<:_,_
;

//stringloop(_,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod),
stringloopBank(audio,freq) = audio<:(
stringloop(_,freq*0.25,typeModLL,t60LL,treshLL,nonLinLL,brightLL,frequencyModLL),
stringloop(_,freq*0.5,typeModL,t60L,treshL,nonLinL,brightL,frequencyModL),
stringloop(_,freq,typeMod,t60,tresh,nonLin,bright,frequencyMod),
stringloop(_,freq*2,typeModH,t60H,treshH,nonLinH,brightH,frequencyModH),
stringloop(_,freq*4,typeModHH,t60HH,treshHH,nonLinHH,brightHH,frequencyModHH)
):>_*KPvolume
;

//-----------------------------------------------
// VocSynth: Combine all the elements
//-----------------------------------------------
SynthsMixer = interleave(2,4):(bus(4):>_),(bus(4):>_);

VocSynth(audio) = 
(subSine(audio:qompander,PitchTracker(audio)),
vocoder(audio:qompander,PitchTracker(audio)),
pafvocoder(audio:qompander,PitchTracker(audio))),
FMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio),subLevel(audio)):
SynthsMixer;

//process(audio) = FMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio)/2,subLevel(audio));
//FMvoc(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio)/2,subLevel(audio),FMindex,FMdyn)<:_,_;
//process(audio) =stringloop(audio,PitchTracker(audio),typeMod,2,3,4,5,6.1);
//process(audio) =stringloop(audio,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod);


process(audio) = VocSynth(audio):(stringloopBank(_,PitchTracker(audio)),stringloopBank(_,PitchTracker(audio)));
//process(audio) = VocSynth(audio):(stringloopBank(_,PitchTracker(audio)),stringloopBank(_,PitchTracker(audio)));
//stringloop(_,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod),
//stringloop(_,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod);


//process(audio) = pafvocoder(audio:qompander,PitchTracker(audio));

//-----------------------------------------------
// testing cruft
//-----------------------------------------------

/*
KPvol		= KPgroup(vslider("[0]vol",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeMod		= KPgroup(nentry("[1]Nonlinear Filter/typeMod",0,0,4,1));
t60		= KPgroup(vslider("[2]decaytime_T60", 4, 0, 10, 0.01));  // -60db decay time (sec)
tresh		= LLKPgroup(vslider("[3] Threshold [unit:dB] [tooltip: A limiter in the feedback-loop]", 0, -33, 33, 0.1));
nonLin		= KPgroup(vslider("[4]Nonlinearity",0,0,1,0.01) : smooth(0.999));
bright		= KPgroup(vslider("[5]brightness", 0.5, 0, 1, 0.01));
frequencyMod	= KPgroup(vslider("[6]freqMod",1,0,8,0) : smooth(0.999));

*/