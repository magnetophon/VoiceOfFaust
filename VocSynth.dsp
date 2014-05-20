import ("oscillator.lib");
import ("maxmsp.lib");
import ("effect.lib");
minline=3; //minimum line time in ms
analizerQ=7; //Q of the analizer bp filters
analizerFreq(audio) = audio:component("../PitchTracker/PitchTracker.dsp"); //detect the pitch

pafBottom=hslider("bottom", 1, 0.5, 7, 0.001):smooth(0.999)<:_,_:*; //0.25 to 49 logarithmicly
pafTop=hslider("top", 8, 1, 10, 0.01):smooth(0.999)<:_,_:*;//1 to 100 logarithmicly, todo: check why it was 1 to 4000 in pd
pafFreq(audio)= analizerFreq(audio);
//pafFreq=hslider("pafFreq", 10, 5, 20, 0.001):smooth(0.999)<:_,_:*;
pafIndex=hslider("pafIndex", 25, 1, 100, 1):smooth(0.999);

/*
bellcurve =rdtable(199,curve,_)
with {
	curve= (((_-100)/25)<:(_*_)*-1);
	};
*/
bellcurve = (((_-100)/25)<:(_*_)*-1);

pafFund(audio)= lf_sawpos(audio:pafFreq);
sampleAndHold(sample) = select2((sample!=0):int) ~ _;
wrap=_<:(_>0,(_,1:fmod)+1,(_,1:fmod)):select2;
centerWrap(c,f) = line (c, 300):sampleAndHold(f)<:wrap;
centerMin(c,f) = c:sampleAndHold(f)-centerWrap(c,f);
cos12(c,f) = (centerMin(c,f)*f<:(_*2*PI:cos)<:(_,_((_,(_+f:(_*2*PI:cos))):_-_:(_*centerWrap(c,f))) )):_+_;
bell(f,i)= (((f*0.5)-0.25:(_*2*PI:cos))*line (i, 12))+100:bellcurve;

//paf(c,f,i,v)= pafFund:resonbp(c*pafFreq,analizerQ,1) * line (v, minline);
paf(c,f,i,v)= (_<: ((f:cos12(c))*bell(f,i)) * line (v, minline));

pafCenters=     par(i,16,   pow((pow((pafTop/pafBottom),1/15)),i)*pafBottom);
analizerCenters=par(i,16,  (pow((pow((128   /0.853553) ,1/15)),i)*0.853553 )*analizerFreq);
bandEnv(freq)=resonbp(freq,analizerQ,1):amp_follower_ud(0.001,0.001);  
analizers(freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,freq12,freq13,freq14,freq15,freq16)=
_<:
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
analizer=_<:(analizerCenters,_):analizers;

pafOscs(pafCenter1,pafCenter2,pafCenter3,pafCenter4,pafCenter5,pafCenter6,pafCenter7,pafCenter8,pafCenter9,pafCenter10,pafCenter11,pafCenter12,pafCenter13,pafCenter14,pafCenter15,pafCenter16,pafVol1,pafVol2,pafVol3,pafVol4,pafVol5,pafVol6,pafVol7,pafVol8,pafVol9,pafVol10,pafVol11,pafVol12,pafVol13,pafVol14,pafVol15,pafVol16,pafFund)=
paf(pafCenter1,pafFund,pafIndex,pafVol1),
paf(pafCenter2,pafFund,pafIndex,pafVol2),
paf(pafCenter3,pafFund,pafIndex,pafVol3),
paf(pafCenter4,pafFund,pafIndex,pafVol4),
paf(pafCenter5,pafFund,pafIndex,pafVol5),
paf(pafCenter6,pafFund,pafIndex,pafVol6),
paf(pafCenter7,pafFund,pafIndex,pafVol7),
paf(pafCenter8,pafFund,pafIndex,pafVol8),
paf(pafCenter9,pafFund,pafIndex,pafVol9),
paf(pafCenter10,pafFund,pafIndex,pafVol10),
paf(pafCenter11,pafFund,pafIndex,pafVol11),
paf(pafCenter12,pafFund,pafIndex,pafVol12),
paf(pafCenter13,pafFund,pafIndex,pafVol13),
paf(pafCenter14,pafFund,pafIndex,pafVol14),
paf(pafCenter15,pafFund,pafIndex,pafVol15),
paf(pafCenter16,pafFund,pafIndex,pafVol16)
;

//this should be process:
pafvocoder=_<:(pafCenters,analizer,pafFund):pafOscs:>_<:_,_;
//just to debug paf
tst=(pafCenters,par(i,16, 0.1),100):pafOscs:>_;
process = pafFund;
//paf(pafTop,pafFund,pafIndex,1);
//pafvocoder<:_,_;
//pafCenters:par(i,16, vbargraph("foo", 0, 144));
//paf(pafTop,pafFund,pafIndex,1)<:_,_;
//analizer:par(i,16, vbargraph("foo", 0, 1));
//paf(center/10,pafFund,pafIndex,1); // 

