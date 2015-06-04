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
/*process = vocoderOsc(3.3333):vocoderMixer(ambisonicsOn):postProc(nrOutChan,ambisonicsOn) ;*/
/*process = (butterfly(nrBands):>par(i,nrOutChan,(outLayout==4) *_)):>bus(nrOutChan);*/
nrBands =16; // NUmber of bands for the vocoders.
nrOutChan = 2; // number of output

ambisonicsOn = 0;
ambisonicsOrder = 3;
/*
bandsLayout (1==2 with 2chan)
out ,6,7 ,8kaput
*/
vocoderMixer(0) =bus(nrBands)<:(
               (bus(nrBands):>(par(i,nrOutChan,(outLayout==0:dezip) *_)))
               ,(interleave(nrBands/nrOutChan,nrOutChan):par(i, nrOutChan,(bus((nrBands/nrOutChan)):>_*(outLayout==1:dezip))))
               ,(par(i,nrOutChan, bus(nrBands/nrOutChan):>(outLayout==2:dezip) *_))
               ,(butterfly(nrBands):>(par(i,nrOutChan,(outLayout==3:dezip) *_)))
               ,(interleave(nrBands/nrOutChan,nrOutChan):par(i, nrOutChan,(butterfly((nrBands/nrOutChan)):>_*(outLayout==4:dezip))))
               ,(bus(nrBands):>butterfly(nrOutChan):par(i,nrOutChan,(outLayout==5:dezip) *_))
               ,(par(i,nrOutChan, bus(nrBands/nrOutChan):>(outLayout==6:dezip) *_):butterfly(nrOutChan))
               /*,(hademar:>par(i,nrOutChan, (outLayout==2) *_))*/
             ):>bus(nrOutChan);

             /*(par(i, 2, (bandsLayout==0) *_) <:bus(nrBands))*/
             /*,(par(i, 2, (bandsLayout==1) *_) : par(i,2,_<:bus(nrBands/2)) )*/
             /*,(par(i, 2, (bandsLayout==2) *_) :( (_<:bus(nrBands/nrOutChan)) ,(_<:bus(nrBands-(nrBands/nrOutChan))) ))*/
             /*,(par(i, 2, (bandsLayout==3) *_) :butterfly(2)<:bus(nrBands))*/
             /*,(par(i, 2, (bandsLayout==4) *_) :butterfly(2)<:butterfly(nrBands))*/
             /*,(par(i, 2, (bandsLayout==5) *_) <:hadamard(nrBands))*/
             /*// ==3 hademar?*/
// Ambisonics mixer.
vocoderMixer(1)  =(angles,bus(nrBands)): interleave(nrBands,2) : par(i,nrBands,myMap):>bus((ambisonicsOrder*2)+1)
with {
  myMap(a) = _<:encoder(ambisonicsOrder, _, a);
  angles = par(i,nrBands/2,   ((angleTop-angleBottom)*(i/(nrBands/2)))+angleBottom)<:(bus(nrBands/2),par(i,nrBands/2,_*-1)):interleave(nrBands/2,2);
};
bandsLayout = hslider(
  "[1] bands layout [tooltip: When this is checked, give mid-side output (or ambisonics in 4 channel mode), otherwise give stereo (surround in 4 channel mode)]",
  1,0,6,1);
outLayout = hslider(
  "[2] out layout [tooltip: When this is checked, give mid-side output (or ambisonics in 4 channel mode), otherwise give stereo (surround in 4 channel mode)]",
  1,0,6,1);
width = hslider("radius", 0, 0, 1, 0.001) : smooth(tau2pole(0.02));
angleTop = hslider("angleTop", 62831, 0, 6.28318530717959, 0.001) : smooth(tau2pole(0.02));
angleBottom = hslider("angleBottom", 62831, 0, 6.28318530717959, 0.001) : smooth(tau2pole(0.02));
rotation = hslider("rotation", 62831, -6.28318530717959, 6.28318530717959, 0.001) : smooth(tau2pole(0.02));


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
