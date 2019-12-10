# MLSynth

Machine learning experiments.

* Use soundmaker.py to generate some 1-second samples. 

This is a very simple synth engine. It produces a stack of harmonics as sine waves, which are integer multiples of the fundamental frequency with optional detuning. Some operators may be frequency modulated by others in the stack and some may be ring modulated by arbitrary frequencies. They all have an envelope consisiting of an attack from 0 to the harmonic's maximum level, followed by a decay back to zero. The attach is linear but the release can follow a power curve.

The script will create a predetermined number of short samples, each using a completely random patch and played at A=440Hz. The randomization is a bit hacky but is designed to provide a good variety of timbres for t-SNE to work with. You need to set the dir variable at the top to something suitable, and the numSamples variable near the bottom. 

* Run tSNE-audio.py on the directory of samples to generate a JSON file. The command is `python tSNE-audio.py --input_dir synth-samples --output_file out-tSNE.json`

The input_dir is wherever soundmaker.py put its samples; the output_file is the JSON file we'll use in the next step.

* Run the Openframeworks t-SNE audio viewer with the generated JSON file: https://ml4a.github.io/guides/AudioTSNEViewer/. Warning: In the version committed here, the JSON file is hard-coded to `C:\Programs\tSNE\out-tSNE.json`.

