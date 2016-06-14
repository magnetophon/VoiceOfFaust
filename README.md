# VoiceOfFaust

## A voice synthesizer/effects machine.

It turns any monophonic sound into a synthesizer.
The name was chosen because I use it mostly to turn my voice into a singing robot, and it's made in [faust](http://faust.grame.fr)

[These](http://magnetophon.nl/sounds/magnetophon/oldCombo.flac) [are](https://github.com/magnetophon/VoiceOfFaust/blob/MasterSlave/Caribean.mp3) some silly demo songs.

### Features:

* pitch tracker
* compressor/expander ported from [here](http://www.katjaas.nl/compander/compander.html)
* lot's of different DSP:
  * normal channel vocoder with a "super-saw" that can be cross-faded to a "super-pulse"
  * a couple of vocoders based on oscillators with controllable formants,
    so IOW: oscillators that sound like they have a band pass filter on them.
    The types of oscillator used in these, are:
    * FOF, ported from [here](http://anasynth.ircam.fr/home/english/media/singing-synthesis-chant-program) and extended as inspired by [Csound](https://csound.github.io/docs/manual/fof2.html)
    * PAF, ported from [here](http://msp.ucsd.edu/techniques/v0.11/book-html/node96.html)
    * [FM](http://chrischafe.net/glitch-free-fm-vocal-synthesis/)
    * CZ resonant oscillators, ported from [here](http://forum.pdpatchrepo.info/topic/5992/casio-cz-oscillators)
  * FM, with modulation by the voice
  * ring-modulation with an emulation of Casio CZ-oscillators
  * [Karplus-Strong](https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis) used as an effect
  * Phase modulation used as an effect
* powerful [[#parameter-control-system][parameter control system]]
* powerful [#parameter-control-system][parameter control system]
* powerful [parameter control system](#parameter-control-system)
* All synths and effects are spatialized at their core: sounds are generated in stereo, multichannel or ambisonics format,
  not made in mono and afterwards given a stereo width with effects, as is usually the case.
* formant compression and expansion:
  Make the output spectrum more flat or more resonant, at the twist of a knob.
* flexible [in and output routing][], totally changing the character of your synth.
* multi-band [deEsser][] and [reEsser][]
* Optionally use as a master-slave pair:
  * The master is a small program that receives the audio and the OSC messages from the external pitch tracker,
    and outputs:
    * a copy of the input audio
    * a saw wave between 0 and 1 at 1/4 the frequency of the input
    * the value of fidelity, from the pitch tracker, as audio.
  * The slaves are synths and effects that input the above three signals, and output awesome sounds.
  * The 3 outputs of the master can now be recorded into a looper or DAW,
    and be used as song building blocks, without needing the pitch tracker.
    Now you are 100% free to switch synths, automate parameters, etc.
* [configuration file](lib/constants.lib)
  Through this file, lot's of options can be turned on and off at compile time.
  Some of the highlights:
  * number of bands of the vocoders
  * number of output channels
  * whether we want [ambisonics](https://en.wikipedia.org/wiki/Ambisonics) output
  * whether a vocoder has nrBands or nrBands * nrOutChan oscillators
* Lots more, described in [Detailed features][].

### Usage:


You need faust from git, because version 0.9.73 and earlier have a bug in lf_sawpos.
Otherwise, at least use my_lf_sawpos in lib/master.lib.

Then run ``./install.sh`` to compile. (It doesn't actually install anything.)

It is recommended to run the included puredata pitch tracker.
Run it with:
``pd -nodac -nomidi -jack  -nogui /home/bart/faust/VoiceOfFaust/PureData/OscSendVoc.pd``
The built in pitch-tracker is a bit crude, because faust cannot do FFT (yet).
Consider it the backup/try-out pitch tracker.




### Detailed features

#### Vocoders

##### parameter control system

The parameters for the vocoders use a very flexible control system:
Each parameter has a bottom and a top knob, where the bottom changes the value at the lowest formant band,
and the top the value at the highest formant band.
The rest of the formant bands get values that ere evenly spaced in between.
For some of them that means linear spacing, for others logarithmic spacing.

For even more flexibility there is a parametric mid:
You set it's value and band number and the parameter values are now:
* 'bottom' at the lowest band, going to:
* 'mid value' at band nr 'mid band', going to:
* 'top value' at the highest band.
Kind of like a parametric mid in equalizers.

##### formant compression/expansion

Scale the volume of each band; between 0 = all bands at average volume, 1 = normal and 2 = expansion
expansion means:
* the loudest band stays the same
* soft bands get softer



##### in and output routing

The vocoders can mix their bands together in various ways:
We can send all the low bands left and the high ones right,
we can alternate the bands between left and right,
we can do various mid-side variations
we can even do a full [Hadamard matrix](https://en.wikipedia.org/wiki/Hadamard_matrix).
All of these, and more, can be clicklessly cross-faded between.

In the [classicVocoder][], a similar routing matrix is between the oscillators and the filters.

##### deEsser

To tame harch esses, especially when using some [formant compression/expansion][], there is a deEsser:
It has all the usual controlls, but since we already are working on a signals that are split up in bands, with known volumes,
it was implemented rather differently:
* multiband, yet much cheaper,
* without additional filters, even for the sidechain,
* and with a db per octave knob for the sidechain, from 0dB/oct (bypass), to 60dB/oct (fully ignore the lows).



#### classicVocoder









VoiceOfFaust started life as a port of [VocSynth](https://github.com/magnetophon/VocSynth).
[Here](https://github.com/magnetophon/VocSynth/blob/master/A%20realtime%20synthesizer%20controlled%20by%20singing%20and%20beatboxing.pdf) is some in depth documentation fot that project.

Enjoy! And don’t forget to tell me what you think of it and send me music you’ve made with it!
