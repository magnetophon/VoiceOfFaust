declare name 		"VocSynth";
declare version 	"0.1";
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
qompander = component("../qompander/qompander.dsp");

//-----------------------------------------------
// contants
//-----------------------------------------------
minline				= 3;		// minimum line time in ms
analizerQ			= 7;		// Q of the analizer bp filters 
MinInputPitch		= 61.7354;	// lowest expected note is a B1
MaxInputPitch		= 987.767;	// highest expected note is a B5
maxTimeWithoutPitch	= 2*SR;		// longest time the OSC pitch tracker can be silent before we switch to the intenal one. (in samples)

//-----------------------------------------------
// the GUI
//-----------------------------------------------
OSCpitch	= nentry("[1]pitch", MinInputPitch, MinInputPitch, MaxInputPitch, 0); 	// To recieve OSC pitch messages

vocoderGroup(x)  = (vgroup("[2]vocoder", x));
vocoderBottom	= vocoderGroup(hslider("[1]bottom",		1, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
vocoderTop		= vocoderGroup(hslider("[2]top",		8.5, 1, 10, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
vocoderQ		= vocoderGroup(hslider("[3]Q",	1, 0.1, 10, 0):smooth(0.999));
vocoderOctave	= vocoderGroup(hslider("[4]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of paf
vocoderVolume	= vocoderGroup(hslider("[5]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly

PAFvocoderGroup(x)  = (vgroup("[3]PAFvocoder", x));
pafBottom	= PAFvocoderGroup(hslider("[1]bottom",		1, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
pafTop		= PAFvocoderGroup(hslider("[2]top",		8.5, 1, 10, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
pafIndex	= PAFvocoderGroup(hslider("[3]index",	25, 1, 100, 0):smooth(0.999));
pafOctave	= PAFvocoderGroup(hslider("[4]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of paf
pafVolume	= PAFvocoderGroup(hslider("[5]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly


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

//-----------------------------------------------
// Universal Pitch Tracker (a periods measurement)
//-----------------------------------------------

SH(trig,x) = (*(1 - trig) + x * trig) ~_;
a = 8.0;// hslider("n cycles", 1, 1, 100, 1) ;


Pitch(a,x) = a * SR / max(M,1) - a * SR * (M == 0)
with { 
      U = (x' < 0) & (x >= 0) ;
      V = +(U) ~ %(int(a)) ;
      W = U & (V == a) ;
      N = (+(1) : *(1 - W)) ~_;
      M = SH(N == 0, N' + 1) ;
};

// switch to internal pitchtracker if OSC is silent for too long
PitchTracker(audio) = (isSameTooLong(OSCpitch), OSCpitch, internal):select2 
with	{
		internal = (audio:dcblockerat(MinInputPitch) : (lowpass(1) : Pitch(a): min(MaxInputPitch) )  ~ max(MinInputPitch*2)) : max(MinInputPitch);
		isSameTooLong(x) = (x@maxTimeWithoutPitch==x);
		};

//-----------------------------------------------
// the vocoder analiser
//-----------------------------------------------

analizerCenters(freq) = VocoderFreqs(0.853553,128);
bandEnv(freq)=resonbp(freq:min(20000),analizerQ,1):amp_follower_ud(0.01,0.01);  
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
// Normal vocoder synthesis
//-----------------------------------------------

oscFilter(c,f,v) = f:resonbp(c,vocoderQ,line (v, minline));

oscFilterBank(Center1,Center2,Center3,Center4,Center5,Center6,Center7,Center8,Center9,Center10,Center11,Center12,Center13,Center14,Center15,Center16,Volume1,Volume2,Volume3,Volume4,Volume5,Volume6,Volume7,Volume8,Volume9,Volume10,Volume11,Volume12,Volume13,Volume14,Volume15,Volume16,Fund)=
oscFilter(Center1,Fund,Volume1),
oscFilter(Center2,Fund,Volume2),
oscFilter(Center3,Fund,Volume3),
oscFilter(Center4,Fund,Volume4),
oscFilter(Center5,Fund,Volume5),
oscFilter(Center6,Fund,Volume6),
oscFilter(Center7,Fund,Volume7),
oscFilter(Center8,Fund,Volume8),
oscFilter(Center9,Fund,Volume9),
oscFilter(Center10,Fund,Volume10),
oscFilter(Center11,Fund,Volume11),
oscFilter(Center12,Fund,Volume12),
oscFilter(Center13,Fund,Volume13),
oscFilter(Center14,Fund,Volume14),
oscFilter(Center15,Fund,Volume15),
oscFilter(Center16,Fund,Volume16)
;

vocoderCenters(freq) = VocoderFreqs(vocoderBottom,vocoderTop):(par(i,16, _,freq:*));

vocoder(audio,freq)= (vocoderCenters(freq),analizer(audio:qompander,freq),pafFund(freq)):oscFilterBank:>_,vocoderVolume:*<:_,_;

//-----------------------------------------------
// PAF oscilator
//-----------------------------------------------


//curve = (((_-100)/25)<:(_*_)*-1);
//bellcurve = rdtable(200, curve, int(+(1)  ~ _ : -(1)));

//pafFreq(audio)= PitchTracker(audio):hbargraph("freq", 0, 700);
pafFreq(audio)= PitchTracker(audio)*pafOctave;


bellcurve(x) = int(x):rdtable(belltablesize+1,curve,_)
with 	{
		belltablesize 	= 199;
		curve 	= time:(((_-100)/25)<:(_*_)*-1);
		};

//bellcurve = (((_-100)/25)<:(_*_)*-1);

/*
bellcurve =rdtable(199,curve,_)
with {
	curve= (((_-100)/25)<:(_*_)*-1);
	};
*/
//bellcurve = (((_-100)/25)<:(_*_)*-1);

pafFund(freq)= lf_sawpos(freq);
sampleAndHold(sample) = select2((sample!=0):int) ~ _;
wrap=_<:(_>0,(_,1:fmod)+1,(_,1:fmod)):select2;
centerWrap(c,f) = line (c, 300):sampleAndHold(f)<:wrap;
centerMin(c,f) = c:sampleAndHold(f)-centerWrap(c,f);
cos12(c,f) = (centerMin(c,f)*f<:(_*2*PI:cos)<:(_,_((_,(_+f:(_*2*PI:cos))):_-_:(_*centerWrap(c,f))) )):_+_;
bell(f,i)= (((f*0.5)-0.25:(_*2*PI:cos))*line (i, 12))+100:bellcurve;

//use this to get a normal vocoder:
//paf(c,f,i,v)= pafFund:resonbp(c*pafFreq,analizerQ,1) * line (v, minline);

paf(c,f,i,v)= (((cos12(c,f))*bell(f,i)) * line (v, minline));


//-----------------------------------------------
// the paf vocoder synthesis
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


//this is process:
pafvocoder(audio,freq)=(pafCenters,analizer(audio:qompander,freq),pafFund(freq)):pafOscs:>_,pafVolume:*<:_,_;




process(audio) = pafvocoder(audio,pafFreq(audio));
//oscFilter(pafFreq(audio),audio, 1);
//vocoder(audio,pafFreq(audio));

//-----------------------------------------------
// testing cruft
//-----------------------------------------------
//process = pafOctave;//PitchTracker;
//analizer(audio,pafFreq(audio)):par(i,16, vbargraph("foo", 0, 0.01));
//analizerCenters(pafFreq(audio)):par(i,16, vbargraph("foo", 0, 20000));
//
//pafvocoder(audio,pafFreq(audio));
//analizer(audio,pafFreq(audio)):par(i,16, vbargraph("foo", 0, 0.01));
// paf(pafTop,pafFund(pafFreq(x)),pafIndex,1);
//pafvocoder<:_,_;
//analizerCenters(pafFreq(audio)):par(i,16, vbargraph("foo", 0, 1440));
//paf(pafTop,pafFund,pafIndex,1)<:_,_;
//analizer:par(i,16, vbargraph("foo", 0, 1));
//paf(center/10,pafFund,pafIndex,1); // 

