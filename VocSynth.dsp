declare name 		"VocSynth";
declare version 	"0.4";
declare author 		"Bart Brouns";
declare license 	"GNU 3.0";
declare copyright 	"(c) Bart Brouns 2014";
declare credits		"PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and square oscillators adapted from the faust library";

//-----------------------------------------------
// imports
//-----------------------------------------------

import ("oscillator.lib");
import ("maxmsp.lib");
import ("effect.lib");
import ("KarplusStrongFX.lib");
import ("NLFeksFX.lib");
import ("mixer.lib");

//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo
qompander	= component("qompander/qompander.dsp");
//KarplusStrongFX		= component("KarplusStrongFX.dsp");

//-----------------------------------------------
// contants
//-----------------------------------------------
//todo: test or ask about smooth vs line:, efficiency and behaviour
minline				= 3;		// minimum line time in ms
analizerQ			= 7;		// Q of the analizer bp filters 
MinInputPitch		= 61.7354;	// lowest expected note is a B1
MaxInputPitch		= 493.6635;	// highest expected note is a B4
maxTimeWithoutPitch	= 2*SR;		// longest time the OSC pitch tracker can be silent before we switch to the intenal one. (in samples, so 2*SR is 2 seconds)
maxTimeWithoutFidelity	= 265*4 ;	// is 4*6ms @ 44100. the time between 4 fid messages

//-----------------------------------------------
// the GUI
//-----------------------------------------------

OSCgroup(x)  = (hgroup("[0]OSC", x)); // To recieve OSC pitch and other messages
//qompander = [1]
tabs(x) = (tgroup("[1]", x));
synthsGroup(x)  = tabs(hgroup("[0]synths", x));
FXGroup(x)  = tabs(hgroup("[1]effects", x));


OSCpitch	= OSCgroup(nentry("[0]pitch", MinInputPitch, MinInputPitch, MaxInputPitch, 0));
OSCfidelity     = OSCgroup(nentry("[1]fidelity", 0, 0, 1, 0));
OSConset     	= OSCgroup(nentry("[2]onset", 0, 0, 1, 0));
formant		= OSCgroup(nentry("[3]formant", MinInputPitch, MinInputPitch, 12000, 0)):smooth(0.999);
ManualOnset     = OSCgroup(button("[4]trigger")); //button does not seem to recieve osc

cleanGroup(x)	= synthsGroup((hgroup("[0]clean", x)));
cleanVolume	= cleanGroup(vslider("[0]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
cleanNLKS	= cleanGroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
cleanpmFX	= cleanGroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1

subGroup(x)	= synthsGroup((hgroup("[1]sub", x)));
subVolume	= subGroup(vslider("[0]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
subNLKS		= subGroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
subpmFX		= subGroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
subOctave	= subGroup(vslider("[3]octave",	-1, -2, 2, 1):octaveMultiplier);				//set the octave of sub


vocoderGroup(x) 	= synthsGroup((hgroup("[2]vocoder", x)));
vocoderVolume	= vocoderGroup(vslider("[0]volume",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
vocoderNLKS	= vocoderGroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
vocoderpmFX	= vocoderGroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
vocoderOctave	= vocoderGroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of vocoder

vocoderParamsGroup(x)= vocoderGroup((vgroup("[4]parameters", x)));
vocoderTop	= vocoderParamsGroup(vslider("[1]top[style:knob]",	32, 1, 64, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
vocoderBottom	= vocoderParamsGroup(vslider("[2]bottom[style:knob]",	1, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
vocoderQ	= vocoderParamsGroup(vslider("[3]Q[style:knob]",	2, 0.3, 7, 0)<:(_,_):*:smooth(0.999));			//0.1 to 49 logarithmicly,
//todo: research
vocoderN	= 2;//vocoderParamsGroup(vslider("[6]N[style:knob]",	1, 1, 6, 1));
vocoderMix	= vocoderParamsGroup(vslider("[4]mix[style:knob]",	0, 0, 1, 0));								// is smoothed at the synth
vocoderDetune	= vocoderParamsGroup(vslider("[5]detune[style:knob]",	0, 0, 1, 0):smooth(0.999));
vocoderSawPulse = vocoderParamsGroup(vslider("[6]saw-pulse[style:knob]", 0, 0, 1, 0));
vocoderDuty	= vocoderParamsGroup(vslider("[7]PW[style:knob]", 0.5, 0.5, 1, 0):min(0.9996));
vocoderWidth	= vocoderParamsGroup(vslider("[8]width[style:knob]",	1, 0, 2, 0):smooth(0.999));				//wide pan, 0=mono 1=normal 2=full-wide


PAFvocoderGroup(x)  = synthsGroup((hgroup("[3]PAFvocoder", x)));
pafVolume	= PAFvocoderGroup(vslider("[0]volume",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
pafNLKS		= PAFvocoderGroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
pafpmFX		= PAFvocoderGroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
pafOctave	= PAFvocoderGroup(vslider("[3]octave",	0, -2, 2, 1):octaveMultiplier);				//set the octave of paf


PAFparamsGroup(x)= PAFvocoderGroup((vgroup("[4]parameters", x)));
pafTop		= PAFparamsGroup(vslider("[1]top[style:knob]",		12, 1, 64, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
pafBottom	= PAFparamsGroup(vslider("[2]bottom[style:knob]",	1, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
pafIndex	= PAFparamsGroup(vslider("[3]index[style:knob]",	25, 1, 100, 0):smooth(0.999));
pafWidth	= PAFparamsGroup(vslider("[4]width[style:knob]",1, 0, 2, 0):smooth(0.999)); //wide pan, 0=mono 1=normal 2=full-wide


//-----------------------------------------------
// FOF 
//-----------------------------------------------
fofVocoderGroup(x)  = synthsGroup((hgroup("[4]FOFvocoder", x)));
fofVolume	= fofVocoderGroup(vslider("[0]volume",	1, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
fofNLKS		= fofVocoderGroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
fofpmFX		= fofVocoderGroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
fofOctave	= fofVocoderGroup(vslider("[3]octave",	-1, -2, 2, 1):octaveMultiplier);				//set the octave of fof

fofparamsGroup(x)= fofVocoderGroup((vgroup("[4]parameters", x)));
fofTop		= fofparamsGroup(vslider("[1]top[style:knob]",	56.192, 1, 64, 0):smooth(0.999)<:(_,_):*);		//1 to 100 logarithmicly,
fofBottom	= fofparamsGroup(vslider("[2]bottom[style:knob]",	0.694, 0.5, 7, 0):smooth(0.999)<:(_,_):*);			//0.25 to 49 logarithmicly
fofSkirt	= fofparamsGroup(vslider("[3]skirt[style:knob]", 30.359, 3, 500, 0)*0.001:smooth(0.999));
fofDecay	= fofparamsGroup(vslider("[4]decay[style:knob]", 3.462, 0, 18, 0):_<:*:smooth(0.999));
//was used for formant phase:
//fofPhaseRand	= fofparamsGroup((vslider("[5]phase rnd[style:knob]", 1, 0, 1, 0)*0.014)+0.996:smooth(0.999));
fofPhaseRand	= fofparamsGroup((vslider("[5]phase rnd[style:knob]", 1, 0, 1, 0)):pow(2):smooth(0.999));
fofWidth	= fofparamsGroup(vslider("[6]width[style:knob]",2, 0, 2, 0):smooth(0.999)); //wide pan, 0=mono 1=normal 2=full-wide
//width = vslider("width", 3, 3, 100, 0)*0.001:smooth(0.999);
//decay = vslider("decay", 0, 0, 10, 0):_<:*:smooth(0.999);
//fReso= vslider("fReso", 60, 0, 1270, 0):smooth(0.999);


//-----------------------------------------------
// FM 
//-----------------------------------------------

FMgroup(x)	 = synthsGroup((hgroup("[5]FM", x)));
FMvolume	= FMgroup(vslider("[01]volume",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
fmNLKS		= FMgroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
FMpmFX		= FMgroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
//FMOctave	= FMgroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//not needed, we have all octaves! :)
FMparamsGroup(x)= FMgroup((vgroup("[3]parameters", x)));


HHFMparamsGroup(x)	= FMparamsGroup((hgroup("[0]+2 oct", x)));
FMvolHH		= HHFMparamsGroup(vslider("[1]vol[style:knob]",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexHH	= HHFMparamsGroup(vslider("[2]index[style:knob]",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynHH		= HHFMparamsGroup(vslider("[3]dyn[style:knob]",	0, 0, 1, 0):smooth(0.999));

HFMparamsGroup(x)  = FMparamsGroup((hgroup("[1]+1 oct", x)));
FMvolH		= HFMparamsGroup(vslider("[1]vol[style:knob]",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexH	= HFMparamsGroup(vslider("[2]index[style:knob]",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynH		= HFMparamsGroup(vslider("[3]dyn[style:knob]",	0, 0, 1, 0):smooth(0.999));

mFMparamsGroup(x)  = FMparamsGroup((hgroup("[2]0 oct", x)));
FMvol		= mFMparamsGroup(vslider("[1]vol[style:knob]",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindex		= mFMparamsGroup(vslider("[2]index[style:knob]",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdyn		= mFMparamsGroup(vslider("[3]dyn[style:knob]",	0, 0, 1, 0):smooth(0.999));

LFMparamsGroup(x)  = FMparamsGroup((hgroup("[3]-1 oct", x)));
FMvolL		= LFMparamsGroup(vslider("[1]vol[style:knob]",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexL	= LFMparamsGroup(vslider("[2]index[style:knob]",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynL		= LFMparamsGroup(vslider("[3]dyn[style:knob]",	0, 0, 1, 0):smooth(0.999));

LLFMparamsGroup(x) = FMparamsGroup((hgroup("[4]-2 oct", x)));
FMvolLL		= LLFMparamsGroup(vslider("[1]vol[style:knob]",	0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
FMindexLL	= LLFMparamsGroup(vslider("[2]index[style:knob]",	0, 0, 1, 0):smooth(0.999)<:(_,_):*:_*15000);
FMdynLL		= LLFMparamsGroup(vslider("[3]dyn[style:knob]",	0, 0, 1, 0):smooth(0.999));


//-----------------------------------------------
// ringmodualtion with emulated Casio CZ oscilators
//-----------------------------------------------

CZgroup(x)	 = synthsGroup((hgroup("[6]CZ rigmodulator", x)));
CZvolume	= CZgroup(vslider("[01]volume",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
CZNLKS		= CZgroup(vslider("[1]NL-KS",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
CZpmFX		= CZgroup(vslider("[2]PM fx",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1
//CZOctave	= CZgroup(vslider("[2]octave",	0, -2, 2, 1):octaveMultiplier);				//not needed, we have all octaves! :)
CZparamsGroup(x)= CZgroup((vgroup("[3]parameters", x)));


HHCZparamsGroup(x)	= CZparamsGroup((hgroup("[0]+2 oct", x)));
CZsquareHH		= HHCZparamsGroup(vslider("[0]square[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZsquareIxHH		= HHCZparamsGroup(vslider("[1]squareIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseHH		= HHCZparamsGroup(vslider("[2]pulse[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseIxHH		= HHCZparamsGroup(vslider("[3]pulseIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZresHH			= HHCZparamsGroup(vslider("[4]res[style:knob]",		0, 0, 1, 0):smooth(0.999));
CZresMultHH		= HHCZparamsGroup(vslider("[5]resMult[style:knob]",	0, 0, 1, 0):smooth(0.999));

HCZparamsGroup(x)  	= CZparamsGroup((hgroup("[1]+1 oct", x)));
CZsquareH		= HCZparamsGroup(vslider("[0]square[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZsquareIxH		= HCZparamsGroup(vslider("[1]squareIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseH		= HCZparamsGroup(vslider("[2]pulse[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseIxH		= HCZparamsGroup(vslider("[3]pulseIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZresH			= HCZparamsGroup(vslider("[4]res[style:knob]",		0, 0, 1, 0):smooth(0.999));
CZresMultH		= HCZparamsGroup(vslider("[5]resMult[style:knob]",	0, 0, 1, 0):smooth(0.999));

mCZparamsGroup(x) 	 = CZparamsGroup((hgroup("[2]0 oct", x)));
CZsquareM		= mCZparamsGroup(vslider("[0]square[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZsquareIxM		= mCZparamsGroup(vslider("[1]squareIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseM		= mCZparamsGroup(vslider("[2]pulse[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseIxM		= mCZparamsGroup(vslider("[3]pulseIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZresM			= mCZparamsGroup(vslider("[4]res[style:knob]",		0, 0, 1, 0):smooth(0.999));
CZresMultM		= mCZparamsGroup(vslider("[5]resMult[style:knob]",	0, 0, 1, 0):smooth(0.999));

LCZparamsGroup(x)  	= CZparamsGroup((hgroup("[3]-1 oct", x)));
CZsquareL		= LCZparamsGroup(vslider("[0]square[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZsquareIxL		= LCZparamsGroup(vslider("[1]squareIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseL		= LCZparamsGroup(vslider("[2]pulse[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseIxL		= LCZparamsGroup(vslider("[3]pulseIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZresL			= LCZparamsGroup(vslider("[4]res[style:knob]",		0, 0, 1, 0):smooth(0.999));
CZresMultL		= LCZparamsGroup(vslider("[5]resMult[style:knob]",	0, 0, 1, 0):smooth(0.999));

LLCZparamsGroup(x) 	= CZparamsGroup((hgroup("[4]-2 oct", x)));
CZsquareLL		= LLCZparamsGroup(vslider("[0]square[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZsquareIxLL		= LLCZparamsGroup(vslider("[1]squareIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseLL		= LLCZparamsGroup(vslider("[2]pulse[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZpulseIxLL		= LLCZparamsGroup(vslider("[3]pulseIx[style:knob]",	0, 0, 1, 0):smooth(0.999));
CZresLL			= LLCZparamsGroup(vslider("[4]res[style:knob]",		0, 0, 1, 0):smooth(0.999));
CZresMultLL		= LLCZparamsGroup(vslider("[5]resMult[style:knob]",	0, 0, 1, 0):smooth(0.999));






//-----------------------------------------------
// Karplus Strong as an effect
//-----------------------------------------------
//todo: make FX group and routing
KPgroup(x)	 = FXGroup((vgroup("[0]Karplus-Strong", x)));
mainKPgroup(x) = KPgroup((hgroup("[1]main", x)));
KPvolume	= mainKPgroup(vslider("[0]volume [style:knob]",	0, 0, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
//KPattack	= mainKPgroup(vslider("[1]attack [style:knob]",	0.01, 0.01, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
//KPdecay		= mainKPgroup(vslider("[2]decay [style:knob]",	0.01, 0.01, 3, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
//KPsustain	= mainKPgroup(vslider("[3]sustain [style:knob]",	0.5, 0.01, 1, 0):smooth(0.999)<:(_,_):*);			//0 to 1 logarithmicly
KPrelease	= mainKPgroup(vslider("[4]release [style:knob]",	0, 0.01, 1, 0):pow(4)*3);			//0 to 1


HHKPgroup(x) = KPgroup((hgroup("[2]+2 oct", x)));
KPvolHH		= HHKPgroup(vslider("[0]vol [style:knob]",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModHH	= HHKPgroup(nentry("[1]type",0,0,4,1));
t60HH		= HHKPgroup(vslider("[2]feedback [style:knob]", 0, 0, 9, 0.01))<:(_,_):*;  // -60db decay time (sec)
treshHH		= HHKPgroup(vslider("[3] thres [unit:dB] [tooltip: A limiter in the feedback-loop] [style:knob]", 33, -33, 33, 0.1));
nonLinHH	= HHKPgroup(vslider("[4]nonlin [style:knob]",0,0,1,0.01) : smooth(0.999));
brightHH 	= HHKPgroup(vslider("[5]bright [style:knob]", 0.5, 0, 1, 0.01));
frequencyModHH	= HHKPgroup(vslider("[6]mFreq [style:knob]",1,0,8,0) : smooth(0.999));

HKPgroup(x) = KPgroup((hgroup("[3]+1 oct", x)));
KPvolH		= HKPgroup(vslider("[0]vol [style:knob]",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModH	= HKPgroup(nentry("[1]type",0,0,4,1));
t60H		= HKPgroup(vslider("[2]feedback [style:knob]", 0, 0, 9, 0.01))<:(_,_):*;  // -60db decay time (sec)
treshH		= HKPgroup(vslider("[3] thres [unit:dB] [tooltip: A limiter in the feedback-loop] [style:knob]", 33, -33, 33, 0.1));
nonLinH		= HKPgroup(vslider("[4]nonlin [style:knob]",0,0,1,0.01) : smooth(0.999));
brightH 	= HKPgroup(vslider("[5]bright [style:knob]", 0.5, 0, 1, 0.01));
frequencyModH	= HKPgroup(vslider("[6]mFreq [style:knob]",1,0,8,0) : smooth(0.999));

MKPgroup(x)	= KPgroup((hgroup("[4]0 oct", x)));
KPvol		= MKPgroup(vslider("[0]vol [style:knob]",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeMod		= MKPgroup(nentry("[1]type",0,0,4,1));
t60		= MKPgroup(vslider("[2]feedback [style:knob]", 0, 0, 9, 0.01))<:(_,_):*;  // -60db decay time (sec)
tresh		= MKPgroup(vslider("[3] thres [unit:dB] [tooltip: A limiter in the feedback-loop] [style:knob]", 33, -33, 33, 0.1));
nonLin		= MKPgroup(vslider("[4]nonlin [style:knob]",0,0,1,0.01) : smooth(0.999));
bright		= MKPgroup(vslider("[5]bright [style:knob]", 0.5, 0, 1, 0.01));
frequencyMod	= MKPgroup(vslider("[6]mFreq [style:knob]",1,0,8,0) : smooth(0.999));

LKPgroup(x) = KPgroup((hgroup("[5]-1 oct", x)));
KPvolL		= LKPgroup(vslider("[0]vol [style:knob]",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModL	= LKPgroup(nentry("[1]type",0,0,4,1));
t60L		= LKPgroup(vslider("[2]feedback [style:knob]", 0, 0, 9, 0.01))<:(_,_):*;  // -60db decay time (sec)
treshL		= LKPgroup(vslider("[3] thres [unit:dB] [tooltip: A limiter in the feedback-loop] [style:knob]", 33, -33, 33, 0.1));
nonLinL		= LKPgroup(vslider("[4]nonlin [style:knob]",0,0,1,0.01) : smooth(0.999));
brightL 	= LKPgroup(vslider("[5]bright [style:knob]", 0.5, 0, 1, 0.01));
frequencyModL	= LKPgroup(vslider("[6]mFreq [style:knob]",1,0,8,0) : smooth(0.999));



LLKPgroup(x) = KPgroup((hgroup("[6]-2 oct", x)));
KPvolLL		= LLKPgroup(vslider("[0]vol [style:knob]",		0, 0, 1, 0):smooth(0.999))<:(_,_):*; 
typeModLL	= LLKPgroup(nentry("[1]type",0,0,4,1));
t60LL		= LLKPgroup(vslider("[2]feedback [style:knob]", 0, 0, 9, 0.01))<:(_,_):*<:(_,_):*;  // -60db decay time (sec)
treshLL		= LLKPgroup(vslider("[3] thres [unit:dB] [tooltip: A limiter in the feedback-loop] [style:knob]", 33, -33, 33, 0.1));
nonLinLL	= LLKPgroup(vslider("[4]nonlin [style:knob]",0,0,1,0.01) : smooth(0.999));
brightLL 	= LLKPgroup(vslider("[5]bright [style:knob]", 0.5, 0, 1, 0.01));
frequencyModLL	= LLKPgroup(vslider("[6]mFreq [style:knob]",1,0,8,0) : smooth(0.999));

//-----------------------------------------------
// Phase Modulation as an effect
//-----------------------------------------------
pmFXgroup(x)	= FXGroup((vgroup("[1]Phase Modulation", x)));
//pmFXvolume	= pmFXgroup(vslider("[0]volume [style:knob]",	0, 0, 1, 0)<:(_,_):*:smooth(0.999));			//0 to 1 logarithmicly
pmFXi		= pmFXgroup(vslider("[1]depth[style:knob]",1,0,4,0):pow(2):smooth(0.999) );
pmFXr		= pmFXgroup(vslider("[2]freq[style:knob]",4,0,8,0):smooth(0.999) );
PMphase		= pmFXgroup(hslider("[3]phase[style:knob]", 0.9, 0, 1, 0):pow(3)*0.5:smooth(0.999));

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
//PTsmooth = 0.1*(((OSCfidelity*-1)+1):amp_follower(0.026))+0.997:min(0.9996):max(0.9984):vbargraph("[-1]PTsmooth", 0.997, 1);
PTsmooth = 0.9984;
PitchTracker(audio) = ((OSCpitchIsBad , OSCpitch, internal):select2) :smooth(PTsmooth)
//PitchTracker(audio) = ((((isSameTooLong(OSCpitch,maxTimeWithoutPitch) & OSCfidelity>0) | isSameTooLong(OSCfidelity,maxTimeWithoutFidelity)), OSCpitch, internal)|:select2) :smooth(0.99)
with	{
		internal = (audio:dcblockerat(MinInputPitch) : (lowpass(1) : Pitch(a): min(MaxInputPitch) )  ~ max(MinInputPitch*2)) : max(MinInputPitch);
		OSCpitchIsBad  = (isSameTooLong(OSCpitch,maxTimeWithoutPitch) & isSameTooLong(OSCfidelity,maxTimeWithoutFidelity));
		//OSCpitchIsBad  = isSameTooLong(OSCfidelity,1);
		isSameTooLong(x,time) = (x@time)==x;
		same(x,time) =(x@time)==x;
		sames(x,time) = (prod(i,int(time),same(x,i+1)));
		intervalTester(x,nrSamples,interval) = (prod(i,nrSamples,same(x,i*interval+1)));
		};

//-----------------------------------------------
// master index
//-----------------------------------------------
masterIndex(freq)= lf_sawpos(freq/4); // lowest possible pitch, as we can only shift up, using wrap
fund(freq,oct)= (4 * oct * masterIndex(freq)) - floor(4 * oct * masterIndex(freq)); //choose octaves

//-----------------------------------------------
// Sub sine
//-----------------------------------------------

subSine(audio,freq) = fund(freq,subOctave)*2*PI:sin * //(subLevel(audio):lowpass(1,freq*subOctave))<:_,_;
(subLevel(audio))<:_,_;
//-----------------------------------------------
// vocoder analiser
//-----------------------------------------------

subLevel(audio) = audio:lowpass(3,300):amp_follower_ud(0.003,0.005)*6:tanh; 

//subLevel(audio) = audio:lowpass(3,300):amp_follower_ud((vslider("up", 0, 0, 1, 0)*0.1),(vslider("down", 0, 0, 1, 0)*0.1))*6:tanh; 

analizerCenters(freq) = VocoderFreqs(0.853553,128):(par(i,16, _,freq:*:min(SR/2)));
//amp_follower_ud params set for minimal distortion
//also sounds cool to vary between this and 0
bandEnv(freq)=resonbp(freq,analizerQ,1):amp_follower_ud(0.002,0.004);
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
//ported toFaust by Bart Brouns
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

    //*-1 to get it into phase with my other synths
    mixer(mix) = (_*mainmix),par(i, 6, _*detunemix):>_*-1;
    };

//todo:Window Function Synthesis:
//see also: http://dspwiki.com/index.php?title=Physical_Modeling_Synthesis
//todo: vector PM osclators

sawpulseNws(N,fund,freq,SawPulse,duty) = diffdel(sawNws(N,fund,freq),del)*compensate with { 
 // non-interpolated-delay version: diffdel(x,del) = x - x@int(del+0.5);
 // linearly interpolated delay version (sounds good to me):
    diffdel(x,del) = x-SawPulse*(x@int(del)*(1-ml.frac(del))+x@(int(del)+1)*ml.frac(del));
 // Third-order Lagrange interpolated-delay version (see filter.lib):
 // diffdel(x,del) = x - fl.fdelay3(DELPWR2,max(1,min(DELPWR2-2,ddel)));
 compensate = (((SawPulse*-1)+1)*0.2)+0.8; //compensate volume dif between saw and pulse
 DELPWR2 = 2048; // Needs to be a power of 2 when fdelay*() used above.
 delmax = DELPWR2-1; // arbitrary upper limit on diff delay (duty=0.5)
 SRmax = 96000.0; // assumed upper limit on sampling rate
 fmin = SRmax / float(2.0*delmax); // 23.4 Hz (audio freqs only)
 freqC = max(freq,fmin); // clip frequency at lower limit
 period = (float(ml.SR) / freqC); // actual period
 ddel = duty * period; // desired delay
 del = max(0,min(delmax,ddel));
};

sawpulseN(N,freq,SawPulse,duty) = diffdel(sawN(N,freq),del)*compensate  with { 
 // non-interpolated-delay version: diffdel(x,del) = x - x@int(del+0.5);
 // linearly interpolated delay version (sounds good to me):
    diffdel(x,del) = x-SawPulse*(x@int(del)*(1-ml.frac(del))+x@(int(del)+1)*ml.frac(del));
 // Third-order Lagrange interpolated-delay version (see filter.lib):
 // diffdel(x,del) = x - fl.fdelay3(DELPWR2,max(1,min(DELPWR2-2,ddel)));
 compensate = (((SawPulse*-1)+1)*0.2)+0.8; //compensate volume dif between saw and pulse
 DELPWR2 = 2048; // Needs to be a power of 2 when fdelay*() used above.
 delmax = DELPWR2-1; // arbitrary upper limit on diff delay (duty=0.5)
 SRmax = 96000.0; // assumed upper limit on sampling rate
 fmin = SRmax / float(2.0*delmax); // 23.4 Hz (audio freqs only)
 freqC = max(freq,fmin); // clip frequency at lower limit
 period = (float(ml.SR) / freqC); // actual period
 ddel = duty * period; // desired delay
 del = max(0,min(delmax,ddel));
};


//emulation of a roland supersaw, based on: "How to Emulate the Super Saw" by ADAM SZABO
//http://www.nada.kth.se/utbildning/grukth/exjobb/rapportlistor/2010/rapporter10/szabo_adam_10131.pdf 
//ported toFaust by Bart Brouns
supersawpulse(N,fund,freq,detune,mix,SawPulse,duty) = saws(fund,freq,detuner(detune)):mixer(mix) 
with {
    detuner (detune) = 
      (((detune*0.2),(detune<0.6)):*),
      ((((detune-0.6)*0.8+0.12),((detune>=0.6)&(detune<=0.95))):*),
      ((((detune-0.95)*12+0.4),(detune>0.95)):*)
      :>_;
    saws(fund,freq,det) = 
      sawpulseNws(N,fund,freq,SawPulse,duty),
      sawpulseN(N,(det * -0.110023+1)*freq,SawPulse,duty),
      sawpulseN(N,(det * -0.0628844+1)*freq,SawPulse,duty),
      sawpulseN(N,(det * -0.0195236+1)*freq,SawPulse,duty),
      sawpulseN(N,(det * 0.0199122+1)*freq,SawPulse,duty),
      sawpulseN(N,(det * 0.0621654+1)*freq,SawPulse,duty),
      sawpulseN(N,(det * 0.107452+1)*freq,SawPulse,duty);

    mainmix = mix * -0.55366 + 0.99785:smooth(0.999);
    detunemix = (mix:pow(2) * -0.73764)+(mix * 1.2841):smooth(0.999);

    //*-1 to get it into phase with my other synths
    mixer(mix) = (_*mainmix),par(i, 6, _*detunemix):>_*-1;
    };

//todo:Window Function Synthesis:
//see also: http://dspwiki.com/index.php?title=Physical_Modeling_Synthesis
//todo: vector PM osclators

//-----------------------------------------------
// CZ oscilators by Mike Moser-Booth. ported from pd to faust by Bart Brouns
//-----------------------------------------------

//index= 0 to 1
CZsquare(fund, index) =
(fund>=0.5),
(decimal((fund*2)+1)<:_-min(_,(-1*_+1)*((INDEX)/(1-INDEX))))
:+ *PI:cos*0.5
with {
INDEX = (index:pow(0.25)) * 0.98;};

CZsinepulse(fund, index) = (min(fund*((0.5-INDEX)/INDEX),(-1*fund+1)*((.5-INDEX)/(1-INDEX)))+fund)*4*PI:cos
with {
INDEX = ((index*-0.49)+0.5);};

//res= 0.5 to 64. this is multiplied by the fundamental pitch
CZrestrap(fund, res) =  (((-1*(1-fund)):max(-0.5):min(0.5)*2) *  (cos(decimal((fund*(res:max(1)))+1)*2*PI)*-1)+1)+1;

//-----------------------------------------------
// PAF oscilator
//-----------------------------------------------

pafFreq(audio)= PitchTracker(audio)*pafOctave;
pafFund(freq) = fund(freq,pafOctave);
sampleAndHold(sample) = select2((sample!=0):int) ~ _;



paf(c,f,i,v)= (((cos12(c,f))*bell(f,i)) * line (v, minline))
with {
bellcurve(x) = int(x):rdtable(belltablesize+1,curve,_)
with 	{
		belltablesize 	= 199;
		curve 	= time:(((_-100)/25)<:(_*_)*-1);
		};

//bellcurve = (((_-100)/25)<:(_*_)*-1);


wrap=_<:(_>0,(_,1:fmod)+1,(_,1:fmod)):select2;
centerWrap(c,f) = line (c, 300):sampleAndHold(f)<:wrap;
centerMin(c,f) = c:sampleAndHold(f)-centerWrap(c,f);
cos12(c,f) = (centerMin(c,f)*f<:(_*2*PI:cos)<:(_,_((_,(_+f:(_*2*PI:cos))):_-_:(_*centerWrap(c,f))) )):_+_;
bell(f,i)= (((f*0.5)-0.25:(_*2*PI:cos))*line (i, 12))+100:bellcurve;
};

//-----------------------------------------------
// FOF oscilator
//-----------------------------------------------
//fof is based on work by Xavier Rodet on his CHANT program at IRCAM
fof(fReso,fund,skirt,decay,phase,vol) = 
select2((PHfund<skirt),
(0.5*(1-cos((PI/skirt)*PHfund))*exp((-decay/PI)*PHfund)*sin(fReso*PHfund+PH)),
exp((-decay/PI)*PHfund)*sin(fReso*PHfund+PH))*vol
with {
arc(angle) = angle/360 * 2 * PI;
PHfund = decimal(fund+phase);
//warning: phase is currently being re-used for the fundamentalosc phase, this is the formant phase
//PH = arc( sin(fReso*exp((-decay/PI))) / (phase -cos(fReso*exp((-decay/PI))) ));
//sounds cool, but glithes when either the phase or one of the other params is moved, so:
PH = 0;
};


//-----------------------------------------------
// Normal vocoder synthesis
//-----------------------------------------------

vocoderFund(freq)= fund(freq,vocoderOctave);
vocoderOsc(freq) =   supersawpulse(vocoderN,vocoderFund(freq),freq,vocoderDetune,vocoderMix,vocoderSawPulse,vocoderDuty);
//sawNws(vocoderN,vocoderFund(freq),freq*vocoderOctave);
//supersawpulse(N,fund,freq,detune,mix,SawPulse,duty)

volFilter(c,f,v) = f:resonbp(c:min((SR/2)-10),vocoderQ,v):resonbp(c:min((SR/2)-10),vocoderQ,compensate)
with {
//compensate for one filter:
//compensate = (tanh((1/(vocoderQ:min(1)))/2));
//compensate for two filters:
//pow(vslider("[-1]comp",	1, 1, 20, 0)):
//compensate = ((1/(((vocoderQ:min(7))/7):pow(hslider("[-1]comp",	1, 0.001, 1, 0))))-1)*(hslider("[-1]comp",	1, 1, 2000, 0)):hbargraph("[-1]compensate", 0, 99)+1;

//compensate = 1;// ((1/(vocoderQ:min(7)))/7);
//compensate = (1/(((vocoderQ:min(2))/2)))*((((vocoderQ*0.5:min(1)*-1)+1)*10)+1);
//gain = line (v*compensate, minline);
//gain = (v*compensate);
BP1= 1.766;//1/hslider("[-1]bp1",	0, 0, 1, 0); //1.766
BP2 =9.43;//1/hslider("[-1]bp2",	0, 0, 1, 0); //9.43
FAC1 = 1;//hslider("[-1]fac1",	1, 1, 30, 0);//1
FAC2 = 1.7;//hslider("[-1]fac2",	1, 1, 30, 0);//1.7
FAC3 = 4;//hslider("[-1]fac3",	1, 1, 30, 0);//4
com = 1/(vocoderQ/49);
compensate = 
      (((com*FAC1),(com<BP1)):*),
      ((((com-BP1)*FAC2+FAC1*BP1),((com>=BP1)&(com<=BP2))):*),
      ((((com-BP2)*FAC3+((BP2-BP1)*FAC2)+FAC1*BP1),(com>BP2)):*)
      :>_+1;

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

vocoder(audio,freq)= (vocoderCenters(freq),analizer(audio:qompander,freq),vocoderOsc(freq)):volFilterBank:vocoderMixer:par(i, 2, _*0.01):WidePanner(vocoderWidth);



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


pafvocoder(audio,freq)=(pafCenters,analizer(audio:qompander,freq),pafFund(freq)):pafOscs:vocoderMixer:par(i, 2, _):WidePanner(pafWidth);

//-----------------------------------------------
// FOF vocoder synthesis
//-----------------------------------------------

//fofCenters=     par(i,16,   pow((pow((fofTop/fofBottom),1/15)),i)*fofBottom);
fofFund(freq) = fund(freq,fofOctave);
fofRNDfreq = 0.014;

fofCenters = VocoderFreqs(fofBottom,fofTop);

fofOscs(phase,fofCenter1,fofCenter2,fofCenter3,fofCenter4,fofCenter5,fofCenter6,fofCenter7,fofCenter8,fofCenter9,fofCenter10,fofCenter11,fofCenter12,fofCenter13,fofCenter14,fofCenter15,fofCenter16,fofVol1,fofVol2,fofVol3,fofVol4,fofVol5,fofVol6,fofVol7,fofVol8,fofVol9,fofVol10,fofVol11,fofVol12,fofVol13,fofVol14,fofVol15,fofVol16,Fund)=
//fof(fofCenter1,Fund,fofSkirt,fofDecay,lfnoise(0.1*1):lowpass(1,2)*fofPhaseRand*lfnoise(0.03125*fofCenter),fofVol1),

//this part is to make a different (low)freq modulation for each osc.
// *lfnoise(decimal(fofRNDfreq*fofCenter1)
//decimal is so the mod frq stays low
//"phase*" is to make ir differen left and right.
//max(0.0001) is so the modulation freq doesn't go to 0, because that causes silence and xruns...
fof(fofCenter1,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter1):max(0.0001)),fofVol1),
fof(fofCenter2,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter2):max(0.0001)),fofVol2),
fof(fofCenter3,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter3):max(0.0001)),fofVol3),
fof(fofCenter4,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter4):max(0.0001)),fofVol4),
fof(fofCenter5,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter5):max(0.0001)),fofVol5),
fof(fofCenter6,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter6):max(0.0001)),fofVol6),
fof(fofCenter7,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter7):max(0.0001)),fofVol7),
fof(fofCenter8,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter8):max(0.0001)),fofVol8),
fof(fofCenter9,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter9):max(0.0001)),fofVol9),
fof(fofCenter10,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter10):max(0.0001)),fofVol10),
fof(fofCenter11,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter11):max(0.0001)),fofVol11),
fof(fofCenter12,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter12):max(0.0001)),fofVol12),
fof(fofCenter13,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter13):max(0.0001)),fofVol13),
fof(fofCenter14,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter14):max(0.0001)),fofVol14),
fof(fofCenter15,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter15):max(0.0001)),fofVol15),
fof(fofCenter16,Fund,fofSkirt,fofDecay,phase*fofPhaseRand*lfnoise(decimal(fofRNDfreq*fofCenter16):max(0.0001)),fofVol16)
;


//fofvocoder(audio,freq)=(fofCenters,analizer(audio:qompander,freq),fofFund(freq)):fofOscs:vocoderMixer:par(i, 2, min(100):max(-100)):WidePanner(fofWidth);

fofvocoder(audio,freq)=
((fofCenters,analizer(audio:qompander,freq),fofFund(freq)):fofOscs(1):>_),
((fofCenters,analizer(audio:qompander,freq),fofFund(freq)):fofOscs(-1):>_)
;


//fof(fReso,fund,width,decay,vol)

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
):>_<:_,_
;

//-----------------------------------------------
// ringmodualtion with emulated Casio CZ oscilators
//-----------------------------------------------

CZ(fund,freq,square,squareIx,pulse,pulseIx,res,resMult,formant) =
(CZsquare(fund, squareIx)*square),
(CZsinepulse(fund, pulseIx)*(-pulse)),
(CZrestrap(fund, RES)*(-res))
:>_
with{
RES = (formant/freq)* ((resMult:pow(2) * 3.75)+0.25);
};

CZringMod(audio,freq) =
(
CZ(fund(freq,0.25),freq*0.25	,CZsquareLL,CZsquareIxLL,CZpulseLL,CZpulseIxLL,CZresLL,CZresMultLL,formant),
CZ(fund(freq,0.5),freq*0.5	,CZsquareL,CZsquareIxL,CZpulseL,CZpulseIxL,CZresL,CZresMultL,formant),
CZ(fund(freq,1),freq		,CZsquareM,CZsquareIxM,CZpulseM,CZpulseIxM,CZresM,CZresMultM,formant),
CZ(fund(freq,2),freq*2		,CZsquareH,CZsquareIxH,CZpulseH,CZpulseIxH,CZresH,CZresMultH,formant),
CZ(fund(freq,4),freq*4		,CZsquareHH,CZsquareIxHH,CZpulseHH,CZpulseIxHH,CZresHH,CZresMultHH,formant)
):>_*audio<:_,_;




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


Rt60adsr(audio) = tanh(audio:qompander*2:amp_follower(KPrelease)):pow(4);

stringloopBank(freq,audio) = audio<:(
stringloop(_,freq*0.25,typeModLL,t60LL*Rt60adsr(audio),treshLL,nonLinLL,brightLL,frequencyModLL),
stringloop(_,freq*0.5,typeModL,t60L*Rt60adsr(audio),treshL,nonLinL,brightL,frequencyModL),
stringloop(_,freq,typeMod,t60*Rt60adsr(audio),tresh,nonLin,bright,frequencyMod),
stringloop(_,freq*2,typeModH,t60H*Rt60adsr(audio),treshH,nonLinH,brightH,frequencyModH),
stringloop(_,freq*4,typeModHH,t60HH*Rt60adsr(audio),treshHH,nonLinHH,brightHH,frequencyModHH)
):>_*KPvolume
;

//-----------------------------------------------
// Phase Modulation as an effect
//-----------------------------------------------
//x - floor(x)

PHosci(freq,PH)	= s1 + d * (s2 - s1)
		with {
			i = int(phase(freq));
			d = decimal(phase(freq));
			PHi = decimal(i/tablesize+PH)*tablesize;
			s1 = rdtable(tablesize+1,sinwaveform,int(PHi));
			s2 = rdtable(tablesize+1,sinwaveform,int(PHi+1));};

// modulator:
pmFX(fc,r,I,PH,x) = x : fdelay3(1 << 17, dt + 1)
with {
k = 8.0 ; // pitch-tracking analyzing cycles number
//fc = PtchPr(k,x) ;
dt = (0.5 * PHosci(fc * r,PH) + 0.5) * I / (PI * fc) *SR ;
};
//process = pmFX(r,I) ;


//-----------------------------------------------
// VocSynth: Combine all the elements
//-----------------------------------------------

//mixerWithSends(nrChan,nrMonoChan,nrSends)

VocSynth(audio) = 
(
cleanVolume,cleanNLKS,cleanpmFX,
(audio:qompander*2<:_,_),

subVolume,subNLKS,subpmFX,
subSine(audio:qompander,PitchTracker(audio)),

vocoderVolume,vocoderNLKS,vocoderpmFX,
vocoder(audio:qompander,PitchTracker(audio)),

pafVolume,pafNLKS,pafpmFX,
pafvocoder(audio:qompander,PitchTracker(audio)),

fofVolume,fofNLKS,fofpmFX,
fofvocoder(audio:qompander,PitchTracker(audio)),

FMvolume,fmNLKS,FMpmFX,
FMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio),subLevel(audio)),

CZvolume,CZNLKS,CZpmFX,
CZringMod(audio:qompander,PitchTracker(audio)):

mixerWithSends(nrChan,nrMonoChan,nrSends)

:_,_
,((_<:stringloopBank(PitchTracker(audio))),(_<:stringloopBank(PitchTracker(audio))))
,pmFX(PitchTracker(audio),pmFXr,pmFXi,PMphase),pmFX(PitchTracker(audio),pmFXr,pmFXi,0-PMphase)
:interleave(nrMonoChan,nrSends):par(i,nrMonoChan,(bus(nrSends):>_))
//:block  //block out non tonal sounds
:stereoLimiter(audio) //it needas the original audio (the voice) to calculate the pitch, and with that the decay time.
:par(i,2,_<:(_, (envelop :(hbargraph("[2][unit:dB][tooltip: output level in dB]", -70, +6)))):attach)
)
with {
      nrChan = 7;
      nrMonoChan = 2;
      nrSends = 3;
      //is actually dual mono. on purpose; to try and keep the image in the center.
      //todo: make this stereo, and find a better way for  KP-FX to stay centered
      stereoLimiter(audio)= 
      (dcblocker*0.5:compressor_mono(100,-12,decay*0.5,decay)),
      (dcblocker*0.5:compressor_mono(100,-12,decay*0.5,decay)):
      (compressor_mono(100,-6,0.001,decay*0.5)),
      (compressor_mono(100,-6,0.001,decay*0.5));
      decay = (1/(PitchTracker(audio) * subOctave ));
      //isSameTooLong(x,time) = (mem(x))==x:vbargraph("[-2]same", 0, 1);
      same(x,time) =(x@time)==x;
      intervalTester(x,nrSamples,interval) = (prod(i,nrSamples,same(x,i*interval+1)));
      block = par(i,2,(intervalTester(PitchTracker(audio),2,265)*-1+1:smooth(0.999))*_);
      envelop	= abs : max ~ -(1.0/SR) : max(db2linear(-70)) : linear2db;
      }
;

//process(audio) = FMSynth(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio)/2,subLevel(audio));
//FMvoc(audio:highpass3e(400):extremeLimiter, audio:highpass3e(400),PitchTracker(audio)/2,subLevel(audio),FMindex,FMdyn)<:_,_;
//process(audio) =stringloop(audio,PitchTracker(audio),typeMod,2,3,4,5,6.1);
//process(audio) =stringloop(audio,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod);


//process(audio) = fof(400,fofFund(PitchTracker(audio)),fofSkirt,fofDecay,fofPhaseRand,1);

//adsr(KPattack,KPdecay,KPsustain,KPrelease,button("foo")):vbargraph("foo", 0, 1);

//process(audio) = audio<:(stringloopBank(PitchTracker(audio)));




//process(audio) = fof(400,fofFund(PitchTracker(audio)),fofSkirt,fofDecay,fofPhaseRand,1):stringloop(_,PitchTracker(audio)*0.5,typeModL,t60L*Rt60adsr(audio),treshL,nonLinL,brightL,frequencyModL);
//process(audio) = fofvocoder(audio:qompander,PitchTracker(audio)):>min(100):max(-100):stringloop(_,PitchTracker(audio)*0.5,typeModL,t60L*Rt60adsr(audio),treshL,nonLinL,brightL,frequencyModL);



process(audio) = VocSynth(audio);



//process = PHosci(1000,0.5);
//process(audio) = audio<:((_:qompander*2<:stringloopBank(PitchTracker(audio))),(_:qompander*2<:stringloopBank(PitchTracker(audio))));

//process(audio) = audio:stringloop(_,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod);
//process(audio) = audio:(stringloopBank(_,PitchTracker(audio)));
//stringloop(_,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod),
//stringloop(_,PitchTracker(audio)/1,typeMod,t60,tresh,nonLin,bright,frequencyMod);


//process(audio) = pafvocoder(audio:qompander,PitchTracker(audio));

//-----------------------------------------------
// testing cruft
//-----------------------------------------------

/*


*/
