// FMVoxVib.ck
// Program B of "Glitch Free FM Vocal Synthesis"

/* This example defines a guide shred which executes for 4 seconds. It sets up a DSP graph in which one FMVox instance receives a sample-synchronous control stream and sends its output to the dac. The guide shred sporks child shreds for vibrato and the multiplex control stream. The data float array holds "aaa" vowel coefficients for four formants described by their center frequency and dB amplitude. For this test code formant bandwidths are set the same globally. */

Step stream => FMVox fmv => dac;		// set dsp graph with FMVox
4 => int nFormants;				// FMVox generates 4 formants
1::ms => dur updateRate;			// stream will send frames at updateRate
SinOsc vibLFO => blackhole;			// low-frequency oscillator for vibrato
vibLFO.freq(3);					// vibrato freq
vibLFO.gain(0.1);				// vibrato excursion
Std.mtof(64) => float p => float f0;		// pitch freq

fun void vibrato()				// shred for generating vibrato
{
  while (true)
    { 
      ((vibLFO.last()+1.0) * p) => f0;		// vibrato applied to pitch sets f0
      1::ms => now;				// with 1 ms granularity
  }
}

[ // "aaa"
  [ 349.0,  0.0],[ 918.0,-10.0],
  [2350.0,-17.0],[2731.0,-23.0]
] @=> float data[][]; 

fun void mux(float val)				// inserts the next value into frame
{
  stream.next(val);
  1::samp => now;
}

-1 => int startFrame;				// flag to signal new frame
95 => float db;					// output level in dB (100 = maximum)

fun void muxStream()				// shred for generating control stream
{
  updateRate-14::samp => dur padTime;		// there are 14 coefficients per frame
  while(true){
    padTime => now;				// time to output a new frame
    mux(startFrame);				// insert start flag
    mux(f0);					// insert f0
    for (0 => int f; f<nFormants; f++){		// for each formant
      mux(Math.dbtorms(db+data[f][1]));		// ...insert amp
      mux(0.2);					// ...insert bandwidth
      mux(data[f][0]);				// ...insert center freq
    }
}}

spork ~muxStream();				// spork the control stream shred
spork ~vibrato();				// spork the vibrato shred
4::second => now;				// run for 4 seconds
