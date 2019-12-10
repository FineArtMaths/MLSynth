# MLSynth

Machine learning experiments.

* Use soundmaker.py to generate some 1-second samples. 

You need to set the dir variable at the top to something suitable, and the numSamples variable near the bottom (defaults to 100, which isn't really enough for t-SNE but gives a good range of available sounds). This is a very simple synth engine. It produces a stack of harmonics as sine waves, some of which may be frequency modulated by other harmonics and some of which may be ring modulated by arbitrary frequencies. They all have an envelope consisiting of an attack from 0 to the harmonic's maximum level, followed by a decay back to zero. The attach is linear but the release can follow a power curve.

* Run tSNE-audio.py on the directory of samples to generate a JSON file. The command is `python tSNE-audio.py --input_dir synth-samples --output_file out-tSNE.json`

* Run the Openframeworks t-SNE audio viewer with the generated JSON file: https://ml4a.github.io/guides/AudioTSNEViewer/. In the version committed here, the JSON file is hard-coded to `C:\Programs\tSNE\out-tSNE.json`.
