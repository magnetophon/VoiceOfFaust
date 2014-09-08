VoiceOfFaust
============

A voice synthesizer/effects machine.

Partly a port of https://github.com/magnetophon/VocSynth to faust, partly adding in some new stuff that is not possible in puredata.

More information on how it works can be found here:

https://github.com/magnetophon/VocSynth/blob/master/A%20realtime%20synthesizer%20controlled%20by%20singing%20and%20beatboxing.pdf



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
