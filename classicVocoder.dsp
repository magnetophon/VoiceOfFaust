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
import ("hoa.lib");

//-----------------------------------------------
// process
//-----------------------------------------------
/*process = volFilter;*/
/*process = _<:par(i, nrBands, volFilter);*/
process(audio) = StereoVocoder(audio,PitchTracker(audio));
/*process=((decoderStereo(ambisonicsOrder):par(i,2, autoSat)), bus(nrOutChan));*/
/*process = vocoderOsc(3.3333):vocoderMixer(ambisonicsOn):postProc(nrOutChan,ambisonicsOn) ;*/
/*process = (butterfly(nrBands):>par(i,nrOutChan,(outputRouting==4) *_)):>bus(nrOutChan);*/
nrBands =16; // NUmber of bands for the vocoders.
nrOutChan = 2; // number of output
maxNrOutRoutings = select2(ambisonicsOn,6,3);
ambisonicsOn = 1;
ambisonicsOrder = 3;
/*
inputRouting (1==2 with 2chan)
out ,6,7 ,8kaput
*/
vocoderMixer(0) =bus(nrBands)<:(
               (bus(nrBands):>(par(i,nrOutChan,(outputRouting==0:dezip) *_)))
               ,(interleave(nrBands/nrOutChan,nrOutChan):par(i, nrOutChan,(bus((nrBands/nrOutChan)):>_*(outputRouting==1:dezip))))
               ,(par(i,nrOutChan, bus(nrBands/nrOutChan):>(outputRouting==2:dezip) *_))
               ,(butterfly(nrBands):>(par(i,nrOutChan,(outputRouting==3:dezip) *_)))
               ,(interleave(nrBands/nrOutChan,nrOutChan):par(i, nrOutChan,(butterfly((nrBands/nrOutChan)):>_*(outputRouting==4:dezip))))
               ,(bus(nrBands):>butterfly(nrOutChan):par(i,nrOutChan,(outputRouting==5:dezip) *_))
               ,(par(i,nrOutChan, bus(nrBands/nrOutChan):>(outputRouting==6:dezip) *_):butterfly(nrOutChan))
               /*,(hademar:>par(i,nrOutChan, (outputRouting==2) *_))*/
             ):>bus(nrOutChan);

             /*(par(i, 2, (inputRouting==0) *_) <:bus(nrBands))*/
             /*,(par(i, 2, (inputRouting==1) *_) : par(i,2,_<:bus(nrBands/2)) )*/
             /*,(par(i, 2, (inputRouting==2) *_) :( (_<:bus(nrBands/nrOutChan)) ,(_<:bus(nrBands-(nrBands/nrOutChan))) ))*/
             /*,(par(i, 2, (inputRouting==3) *_) :butterfly(2)<:bus(nrBands))*/
             /*,(par(i, 2, (inputRouting==4) *_) :butterfly(2)<:butterfly(nrBands))*/
             /*,(par(i, 2, (inputRouting==5) *_) <:hadamard(nrBands))*/
             /*// ==3 hademar?*/
// Ambisonics mixer.
vocoderMixer(1)  = routing:((angles,bus(nrBands)): interleave(nrBands,2)) : par(i,nrBands,myMap):>bus((ambisonicsOrder*2)+1)
with {
  myMap(a) = _<:encoder(ambisonicsOrder, _, a);
  angles = par(i,nrBands/2,   ((angleTop-angleBottom)*(i/(nrBands/2)))+angleBottom)<:(bus(nrBands/2),par(i,nrBands/2,_*-1)):interleave(nrBands/2,2);
  routing =bus(nrBands)<:(
               (bus(nrBands):(par(i,nrBands,(outputRouting==0:dezip) *_)))
               ,(interleave(nrBands/nrOutChan,nrOutChan):(par(i,nrBands,(outputRouting==1:dezip) *_)))
               ,(butterfly(nrBands):(par(i,nrBands,(outputRouting==2:dezip) *_)))
               ,(butterfly(nrBands):interleave(nrBands/nrOutChan,nrOutChan):(par(i,nrBands,(outputRouting==3:dezip) *_)))
               /*,(hademar:>par(i,nrOutChan, (outputRouting==2) *_))*/
             ):>bus(nrBands);
};


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
