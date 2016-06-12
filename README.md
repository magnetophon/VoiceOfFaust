VoiceOfFaust
============

A voice synthesizer/effects machine.

It turns any monophonic sound into a synthesizer.
The name was chosen because I use it mostly to turn my voice into a singing robot, and it's made in [faust](http://faust.grame.fr)

It started life as a port of [VocSynth](https://github.com/magnetophon/VocSynth). ([documentation])

More information on how it works can be found here:

https://github.com/magnetophon/VocSynth/blob/master/A%20realtime%20synthesizer%20controlled%20by%20singing%20and%20beatboxing.pdf


Caribean.mp3 is a demo song

Currently it consists of:

* pitch tracker
* compressor/expander
* normal vocoder with a "super-saw" that can be cross-faded to a "super-pulse"
* filter-free vocoder based on PAF oscillators
* filter-free vocoder based on FOF oscillators
* FM, with modulation by the voice
* ring-modulation with an emulation of Casio CZ-oscillators
* Karplus-Strong used as an effect
* Phase modulation used as an effect



For optimal performance it is recommended to run the included puredata pitch tracker.
The built in pitch-tracker is a bit crude, because faust cannot do FFT (yet).
Consider it the backup/try-out pitch tracker.

You need the faust from git, because the release versions have a bug in lf_sawpos.
Otherwise, at least use my_lf_sawpos in lib/master.lib.

The install.sh ‘ script is how I get it to compile for multi-core, but if your processor is powerful enough to run this beast on one core, you can compile it trough a simple:

faust2jack -osc VocSynth.dsp

The -osc is actually only needed if you want to run an external pitch-tracker, or if you want to interact with the parameters trough OSC.
To get a block diagram, run:

faust2firefox Vocsynth.dsp


Enjoy! And don’t forget to tell me what you think of it and send me music you’ve made with it!
