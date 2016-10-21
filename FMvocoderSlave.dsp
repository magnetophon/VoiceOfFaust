import("lib/slave.lib");
import ("lib/common.lib");
// specific to this synth:
import ("lib/FullGUI.lib");
import ("lib/FMvocoder.lib");

process(audio,index,fidelity) =
  FMvocoder(audio, masterPitch(audio,index),index,fidelity,doubleOscs);
