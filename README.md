# VoiceOfFaust

Turns any monophonic sound into a synthesizer, preserving the pitch and spectral dynamics of the input.  
The name was chosen because I use it mostly to turn my voice into a singing robot, and it's made in [Faust](http://faust.grame.fr).

[These](http://magnetophon.nl/sounds/magnetophon/oldCombo.flac) [are](https://raw.githubusercontent.com/magnetophon/VoiceOfFaust/master/Caribean.mp3) some silly demo songs.

## Overview

VoiceOfFaust consists of:

* An external pitch tracker: [helmholtz~](http://www.katjaas.nl/helmholtz/helmholtz.html) by Katja Vetter
* compressor/expander, also by [Katja](http://www.katjaas.nl/compander/compander.html), ported to Faust.
* 7 synthesizer and two effect algorithms:
  * [channel vocoder](#classicvocoder) with:
    * a "super-saw" that can be cross-faded to a "super-pulse", free after [Adam Szabo](https://www.nada.kth.se/utbildning/grukth/exjobb/rapportlistor/2010/rapporter10/szabo_adam_10131.pdf)
    * flexible Q and frequency setting for the filters
    * an elaborate feedback and distortion matrix around the filters
  * a couple of vocoders based on oscillators with controllable formants,  
    so IOW: oscillators that sound like they have a band pass filter on them.  
    The types of oscillator used in these, are:
    * [CZ resonant](#czvocoder) oscillators, ported from a pd patch by [Mike Moser-Booth](http://forum.pdpatchrepo.info/topic/5992/casio-cz-oscillators)
    * [PAF](#pafvocoder), ported from a pd patch by [Miller Puckette](http://msp.ucsd.edu/techniques/v0.11/book-html/node96.html)
    * [FM](#fmvocoder), based on code by [Chris Chafe](http://chrischafe.net/glitch-free-fm-vocal-synthesis/)
    * [FOF](#fofvocoder), based on code by [Michael Jørgen Olsen](https://ccrma.stanford.edu/~mjolsen/220a/fp/Foflet.dsp) and extended as inspired by [Csound](https://csound.github.io/docs/manual/fof2.html)
  * [FM](#fmsinger), with modulation by the voice
  * [ring-modulation](#czringmod) with a few other CZ-oscillators
  * [Karplus-Strong](#karplus-strongSinger): The famous [synthesis technique](https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis) used as an effect
  * [Phase modulation](#pmfx) used as an effect
* All oscillators are synchronized to a single saw-wave, so they stay in phase, [unless you don't want them to!](#phase-parameters)
* powerful [parameter mapping system](#parameter-mapping-system)  
  Lets you set different parameter values for each band, without having to set them all separately
* All synths are spatialized at their core: sounds are generated in stereo, multichannel or ambisonics format,  
  not made in mono and afterwards given some stereo width with effects.
* [formant compression/expansion](#formant-compression/expansion):  
  Make the output spectrum more flat or more resonant, at the twist of a knob.
* flexible [in and output routing](#in-and-output-routing), totally changing the character of your synth.
* all parameters, including routing, are step-less, meaning any 'preset' can morph into any other, with the only exception of the octave setting
  (though for some of the synths even that one is continuously variable)
* multi-band [deEsser](#deesser) and [reEsser](#reesser)
* Optionally use as a [master-slave](#master-slave) pair  
  * This is a workaround for the need for an external pitchtracker, making it possible to use these as plugins.  
    It has the nice side effect that your sounds become fully deterministic:  
    because a pitchtracker will always output slightly different data, or at least at slightly different moments relative to the audio, the output audio can sometimes change quite a bit from run to run.
* [configuration file](lib/constants.lib)  
  Through this file, lot's of options can be set at compile time, allowing you to adapt the synth to the amount of CPU power and screen real-estate available.  
  Some of the highlights:
  * number of bands of the vocoders
  * number of output channels
  * whether we want [ambisonics](https://en.wikipedia.org/wiki/Ambisonics) output
  * whether a vocoder has nrBands or nrBands * nrOutChan oscillators. [more info](#doubleOscs) 
* Lots more, described in [Features](#features).

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-generate-toc again -->
**Table of Contents**

- [VoiceOfFaust](#voiceoffaust)
    - [Overview](#overview)
    - [Building and running](#building-and-running)
    - [Features](#features)
        - [Vocoders](#vocoders)
            - [features of all vocoders](#features-of-all-vocoders)
                - [parameter mapping system](#parameter-mapping-system)
                - [formant compression/expansion](#formant-compressionexpansion)
                - [deEsser](#deesser)
                - [reEsser](#reesser)
                - [doubleOscs](#doubleoscs)
                - [in and output routing](#in-and-output-routing)
                - [phase parameters](#phase-parameters)
            - [features of individual vocoders](#features-of-individual-vocoders)
                - [classicVocoder](#classicvocoder)
                - [CZvocoder](#czvocoder)
                - [PAFvocoder](#pafvocoder)
                - [FMvocoder](#fmvocoder)
                - [FOFvocoder](#fofvocoder)
        - [other synthesizers](#other-synthesizers)
            - [FMsinger](#fmsinger)
            - [CZringmod](#czringmod)
            - [Karplus-StrongSinger](#karplus-strongsinger)
            - [Karplus-StrongSingerMaxi](#karplus-strongsingermaxi)
        - [master-slave](#master-slave)

<!-- markdown-toc end -->


## Building and running


You need Faust from git, because version 0.9.73 and earlier have a bug in lf_sawpos. 
Otherwise, at least use my_lf_sawpos in lib/master.lib.

Then run ``./install.sh`` to compile. (It doesn't actually install anything.)

It is recommended to run the included puredata pitch tracker.  
Run it with:
```pd -nodac -nomidi -jack  -nogui /path/to//VoiceOfFaust/PureData/OscSendVoc.pd```
The built in pitch-tracker is a bit crude, because Faust cannot do FFT (yet).  
Consider it the backup/try-out pitch tracker.


## Features

### Vocoders

#### features of all vocoders

##### parameter mapping system


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
If that's all a bit too much, just set ``para`` to 0 in the [configuration file](lib/constants.lib),  
and you'll have just the top and bottom settings.

##### formant compression/expansion

Scale the volume of each band relative to the others:
* 0 = all bands at average volume
* 1 = normal
* 2 = expansion
expansion here means:
* the loudest band stays the same
* soft bands get softer

Because low frequencies contain more energy than high ones, a lot of expansion will make your sound duller.
To counteract that, you can apply a weighting filter, settable from
* 0 = no weighting
* 1 = A-weighting
* 2 = ITU-R 468 weighting

##### deEsser

To tame harsh esses, especially when using some [formant compression/expansion](#formant compression/expansion), there is a deEsser:
It has all the usual controlls, but since we already are working on a signals that are split up in bands, with known volumes,
it was implemented rather differently:
* multiband, yet much cheaper,
* without additional filters, even for the sidechain,
* and with a db per octave knob for the sidechain, from 0dB/oct (bypass), to 60dB/oct (fully ignore the lows).
It also has a (badly named) noise strenght parameter: it uses the fidelity parameter from the external pitchtracker to judge if a sound is an S.
When you turn it up, the deEsser gets disabled when the pitchtracker claims a sound is pitched.
See the [site](#http://www.katjaas.nl/helmholtz/helmholtz.html) of the pitchtracker for more info.

##### reEsser

Disabled by default, but can be enabled in the [configuration file](lib/constants.lib).
It replaces or augments the reduced highs caused by the 

##### doubleOscs

This is a compile option, with two settings:
* 0 = have one oscillators for each formant frequency
* 1 = creates a separate set of oscillators for each output channel, with their [phase](#phase-parameters) modulations reversed.

##### in and output routing

The vocoders can mix their bands together in various ways:  
We can send all the low bands left and the high ones right,  
we can alternate the bands between left and right,  
we can do various mid-side variations  
we can even do a full [Hadamard matrix](https://en.wikipedia.org/wiki/Hadamard_matrix).  
All of these, and more, can be clicklessly cross-faded between.
                       
In the [classicVocoder](#classicvocoder), a similar routing matrix is between the oscillators and the filters.


##### phase parameters

Since all(*) formants are made by separate oscillators that are synced to a single master oscillator, you can set their phases, relative to each other.
(*) except for the [classicVocoder](#classicvocoder)
This allows them to sound like one oscillator when they have static phase relationships, and to sound like many detuned oscillators when their phases are moving.

Together with the [output routing](#in-and-output-routing), it can also create interesting cancellation effects.
For example, with the default settings for the FMvocoder, the formants are one octave up from where you'd expect them to be.
When you change the phase or the output routing, they drop down.

These settings are available:
* static phases
* amount of modulation by low pass filtered noise
* the cutoff frequency of the noise filters

#### features of individual vocoders

##### classicVocoder

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/classicVocoder-svg/process.svg)

The gui of the classicVocoder has two sections:
* oscillators
  Containing the parameters for the carrier oscillators.
  These are regular virtual analog oscillators, with the following parameters:
  * cross-fade between oscillators and noise
  * cross-fade between sawtooth and pulse wave
  * width of the pulse wave
  * mix between a single oscilators and multiple detuned ones
  * detuning amount
* filters  
  Containing the parameters for the synthesis filters:
  * bottom, mid and top set the resonant frequencies
  * Q for bandwidth
  * a feedback matrix. each filter gets fed back a variable amount of:
    * itself
    * it's higher neighbor
    * it's lower neighbor
    * all other filters
  * distortion amount
  * DC offset
  
##### CZvocoder

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/czVocoder-svg/process.svg)

This is the simplest of the vocoders made out of formant oscilators.
You can adjust:
* the formant frequencies
* the [phase parameters](#phase-parameters)

  
##### PAFvocoder

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/PAFvocoder-svg/process.svg)

Also has frequencies and phases, but adds index for brightness.

##### FMvocoder

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/FMvocoder-svg/process.svg)

Same parameters, different sound.

##### FOFvocoder

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/FOFvocoder-svg/process.svg)

Original idea by [Xavier Rodet](http://anasynth.ircam.fr/home/english/media/singing-synthesis-chant-program).
Also has frequencies and phases, but adds:
* skirt and decay:  
  Two settings that influence the brightness of each band
* Octavation index  
  Normally zero. If greater than zero, lowers the effective frequency by attenuating odd-numbered sinebursts. Whole numbers are full octaves, fractions transitional.
  Inspired by [Csound](https://csound.github.io/docs/manual/fof2.html).

### other synthesizers

#### FMsinger

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/FMsinger-svg/process.svg)

A sine wave that modulates its frequency with the input signal.
There are five of these, one per octave, and each one has:
* volume
* modulation index
* modulation dynamics  
  This fades between 3 settings:
  * no dynamics: the amount of modulation stays constant with varying input signal
  * normal dynamics: more input volume equals more modulation
  * inverted dynamics: more input equals less modulation.
  
  
#### CZringmod

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/CZringmod-svg/process.svg)

Ringmodulates the input audio with emulations of Casio CZ oscillators.
Again five octaves, with each octave containing three different oscillators:
* square and pulse, each having volume and index (brightness) controls
* reso, having a volume and a resonance multiplier:  
  This is a formant oscillator, and it's resonant frequency is resMult is multiplied by the formant setting top right.
  It is intended to be used with an external formant tracker.
* There is a global width parameter that controls a delay on the oscillators for one output.
  The delay time is relative to the frequency.
  Because this delay is applied to just the oscillators, and before the ringmodulation, the sound of both output channels arrives simultaneously.
  This creates a mono-compatible widening of the stereo image.

#### Karplus-StrongSinger

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/Karplus-StrongSinger-svg/process.svg)

This takes the idea of a [Karplus Strong algorithm](https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis), but instead of noise, it uses the input signal.
The feedback is ran trough an allpass filter, modulated with an LFO; adapted from the nonLinearModulator in instrument.lib.
To keep the level from going out of control, there is a limiter in the feedback path.
Parallel to the delay is a separate nonLinearModulator.
Globally you can set:
* octave
* output volume
* threshold of the limiter
For the allpass filters you can set:
* amount of phase shift
* difference in phase shift between left and right (yeah, I lied, there are two of everything)
* amount of modulation by the LFO
* frequency of the LFO, relative to the main pitch
* phase offset between the left and right LFO's.
To round things off there is a volume for the dry path and a feedback amount for the delayed one.

#### Karplus-StrongSingerMaxi

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/Karplus-StrongSingerMaxi-svg/process.svg)

To have more voice control of the spectrum, this one has a kind of vocoder in the feedback path.
Since we don't want the average volume of the feedback path changing much, only the volumes relative to the other bands, the vocoder is made out of equalizers, not bandpass filters.
You can adjust it's
* strength: from bypass to 'fully equalized'
* cut/boost; steplessly vary between
  * -1 = all bands have negative gain, except the strongest, which is at 0
  * 0  = the average gain of the bands is 0.
  * +1 = the all bands have positive gain, except the weakest, which is at 0
* top and bottom frequencies
* Q factor

It loses the octave slider, and instead has a separate delay and modulation for each octave.


### master-slave

[Block diagram](https://magnetophon.github.io/VoiceOfFaust/images/master-svg/process.svg)

  * This is a workaround for the need for an external pitchtracker, making it possible to use these as plugins.  
    It has the nice side effect that your sounds become fully deterministic:  
    because a pitchtracker will always output slightly different data, or at least at slightly different moments relative to the audio, the output audio can sometimes change quite a bit from run to run.
  * The master is a small program that receives the audio and the OSC messages from the external pitch tracker,  
    and outputs:
    * a copy of the input audio
    * a saw wave defining the pitch and phase
    * the value of fidelity, from the pitch tracker, as audio.
  * The slaves are synths and effects that input the above three signals.
  * The outputs of the master can be recorded into a looper or DAW,  
    and be used as song building blocks, without needing the pitch tracker.  
    Now you are 100% free to switch synths, automate parameters, etc.  


VoiceOfFaust started life as a port of [VocSynth](https://github.com/magnetophon/VocSynth).
[Here](https://github.com/magnetophon/VocSynth/blob/master/A%20realtime%20synthesizer%20controlled%20by%20singing%20and%20beatboxing.pdf) is some in depth documentation fot that project.

Enjoy! And don’t forget to tell me what you think of it and send me music you’ve made with it!
