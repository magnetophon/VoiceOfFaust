declare name      "classicVocoder";
declare version   "0.7";
declare author    "Bart Brouns";
declare license   "GNU 3.0";
declare copyright "(c) Bart Brouns 2014";
declare credits   "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and square oscillators adapted from the faust library" ;

//-----------------------------------------------
// imports
//-----------------------------------------------
//when cloning from git, checkout the submodules to get qompander
//howto: http://stackoverflow.com/questions/7813030/how-can-i-have-linked-dependencies-in-a-git-repo

import ("lib/common.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/classicVocoder.lib");

//-----------------------------------------------
// process
//-----------------------------------------------
maxNrInRoutings = 6;
/*process = volFilter;*/
/*process = _<:par(i, nrBands, volFilter);*/

/*process(audio) = StereoVocoder(audio,PitchTracker(audio));*/
//process = interleave(nrBands,nrBands);
LinArray(bottom,top,-1) =   0:! ;
LinArray(bottom,top,0) =   0:! ;
LinArray(bottom,top,nrElements) =     par(i,nrElements,   ((top-bottom)*(i/(nrElements-1)))+bottom);
VocoderLinArray(bottom,top) =     LinArray(bottom,top,nrBands);
LinArrayBottom(bottom,top,1) =bottom;
/*LinArrayBottom(bottom,top,2) =bottom,(bottom+(0.5*top));*/
//LinArrayBottom(bottom,top,4) = VocoderLinArray(bottom,top);
//LinArrayBottom(bottom,top,3) = LinArray(bottom,top,3);
LinArrayBottom(bottom,top,nrElements) =
  (
     (LinArray(bottom,top,nrElements+1):(bus(nrElements),!)   :(par(i,nrElements, _*(nrElements<=(nrBands-2)))))
    ,(LinArray(bottom,top,nrElements)                         :par(i,nrElements,_*(nrElements>(nrBands-2)))
  ))
  :>bus(nrElements);
LinArrayTop(bottom,top,1) =top;
LinArrayTop(bottom,top,nrElements) =LinArray(bottom,top,nrElements);
myBus(0)= 0:!;
myBus(nr)= bus(nr);
/*j=1;*/
VocoderLinArrayParametricMid(bottom,mid,band,top) =   
(
  (
  (LinArray(mid,top,nrBands):par(i,nrBands,_*(xfadeSelector(band,1))))
  ,((bottom,LinArray(mid,top,nrBands-1)):par(i,nrBands,_*(xfadeSelector(band,2))))
  )
  ,par(j,nrBands-4,
      (
         (LinArray(bottom,mid,midBand):(bus(midBand-1),!))
         ,LinArrayTop(mid,top,nrBands-midBand+1)
      ):par(i,nrBands,_*(xfadeSelector(band,midBand)))with {midBand=j+3;}
  )
  ,
  (
    ((LinArray(bottom,mid,nrBands-1),top):par(i,nrBands,_*(xfadeSelector(band,nrBands-1))))
    ,(LinArray(bottom,mid,nrBands):par(i,nrBands,_*(xfadeSelector(band,nrBands))))
  )
)

  :>bus(nrBands)
   ;
bt=hslider("[0]bottom", 0, 0, 1, 0.001);
md=hslider("[1]mid", 0, 0, 1, 0.001);
bd=hslider("[2]band", nrBands/2, 1, nrBands, 0.001);
tp=hslider("[3]top", 0, 0, 1, 0.001);
  GRmeter_group(x)  = vgroup("[3] GR [tooltip: gain reduction in dB]", x);
    meter(nr)           = GRmeter_group(  _<:(_,(_:min(1):max(0):( (hbargraph("%nr", 0, 1))))):attach);

process = VocoderLinArrayParametricMid(bt,md,bd,tp):par(i,nrBands,meter(i));
/*nrElements =j;*/
/*process=     LinArray(11,14,nrElements+1):(bus(nrElements),!):(par(i,nrElements, _*(nrElements<(nrBands-2))));*/

/*process = */
/*process=((decoderStereo(ambisonicsOrder):par(i,2, autoSat)), bus(nrOutChan));*/
/*process = vocoderOsc(3.3333):vocoderMixer(ambisonicsOn):postProc(nrOutChan,ambisonicsOn) ;*/
/*process = (butterfly(nrBands):>par(i,nrOutChan,(outputRouting==4) *_)):>bus(nrOutChan);*/
/*
inputRouting (1==2 with 2chan)
out ,6,7 ,8kaput
*/

/*tmp = vocoderOsc(2):vocoderMixer(ambisonicsOn);*/
//normal mixer
/*process = decoderStereo(2);*/

/*process = bus2<:bus(nrBands):vocoderMixer;*/
/*process = bus2<:interleave(2,nrOutChan/2)<:bus(nrBands):vocoderMixer;*/
/*process = (bus(nrBands)<:par(i,nrBands,selector(i-1,nrBands)));*/
/*process = resonbp;*/
/*process = VocoderFreqs(1,2);*/
/*process = StereoVolFilterBank(nrBands);*/
/*process(audio) =  analizers(audio);*/
/*process(audio) = (audio,_:bandEnv);*/
/*process(freq,audio) =  (vocoderCenters(freq),(vocoderOsc(freq)<:bus(nrBands)),analizer(voice(audio),freq), (vocoderQ<:bus(nrBands))):StereoVolFilterBank(nrBands):vocoderMixer:par(i, 2, _*0.01):WidePanner(vocoderWidth,nrOutChan);*/
/*process = (bus(nrBands*4)):par(i, nrBands, volFilter);*/
