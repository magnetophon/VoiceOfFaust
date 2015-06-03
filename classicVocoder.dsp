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
/*process = vocoderMixer  ;*/
nrBands =16; // number of bands for the vocoders.
nrOutChan = 2; // number of output
ambisonicsOrder = 3;
width = hslider("radius", 0, 0, 1, 0.001) : smooth(tau2pole(0.02));
angleTop = hslider("angleTop", 62831, 0, 6.28318530717959, 0.001) : smooth(tau2pole(0.02));
angleBottom = hslider("angleBottom", 62831, 0, 6.28318530717959, 0.001) : smooth(tau2pole(0.02));
rotation = hslider("rotation", 62831, -6.28318530717959, 6.28318530717959, 0.001) : smooth(tau2pole(0.02));

//
vocoderMixer  =(angles,bus(nrBands)): interleave(nrBands,2) : par(i,nrBands,myMap):>wider(ambisonicsOrder,width) :rotate(ambisonicsOrder, rotation) :optimInPhase(ambisonicsOrder) :decoderStereo(ambisonicsOrder)
with {
  myMap(a) = _<:encoder(ambisonicsOrder, _, a);
  angles = par(i,nrBands/2,   ((angleTop-angleBottom)*(i/(nrBands/2)))+angleBottom)<:(bus(nrBands/2),par(i,nrBands/2,_*-1)):interleave(nrBands/2,2);
};

tmp = vocoderOsc(2):vocoderMixer;

vocoderMixerOld =bus(nrBands)<:(
               (interleave(nrOutChan,nrBands/nrOutChan):par(i, nrOutChan,(bus((nrBands/nrOutChan)):>_*(channelLayout==0))))
               ,(par(i,nrOutChan, bus(nrBands/nrOutChan):>(channelLayout==1) *_))
               ,(bandsToAmbisonics:>par(i,nrOutChan, (channelLayout==2) *_))
             ):>bus(nrOutChan);

bandsToAmbisonics = bus(nrBands);
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
