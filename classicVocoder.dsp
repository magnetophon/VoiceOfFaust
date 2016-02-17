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
maxNrInRoutings = 5;

process(audio) = StereoVocoder(audio,PitchTracker(audio));
// process = _<:par(i, nrBands, volFilter);

// process = VocoderFreqsChooser;
/*process = StereoVolFilterBank(16);*/
/*process = (bus(nrBands)<:par(i,nrBands,_<:par(j,nrBands,_*(i!=j))):>((bus(nrBands),(VocoderLinArrayChooser(allFBbottom,allFBmid,allFBband,allFBtop,para):par(i,nrBands,pow(3):dezip))):(interleave(nrBands,2)):par(i,nrBands,*)));*/
/*band=3;*/
/*top=4;*/
/*mid=0.5;*/
/*bottom=1;*/
/*process =((mid>0)*((mid*(top-origMid))+origMid))*/
            /*+ ((mid<0)*(((mid*-1)*(origMid-bottom))+bottom));*/
                        /*origMid(band) = (VocoderFreqs(bottom,top):par(i,nrBands,_*(xfadeSelector(band,nrBands)))):>_;*/
//process = interleave(nrBands,nrBands);
/*process = VocoderLinArrayParametricMid(bt,md,bd,tp);*/
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
