VoiceOfFaust
============

A port of https://github.com/magnetophon/VocSynth into faust.
Work in progress.
A rough idea of the end goal: https://github.com/magnetophon/VocSynth/blob/master/A%20realtime%20synthesizer%20controlled%20by%20singing%20and%20beatboxing.pdf

Currently it consists of:

* pitch tracker
* compressor/expander
* normal vocoder with a "supersaw"
* filter-free vocoder based on PAF oscilators
* filter-free vocoder based on FOF oscilators
* FM, with modulation by the voice
* Karplus-Strong used as an effect
* PM used as an effect
