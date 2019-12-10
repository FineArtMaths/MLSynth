# MLSynth

Machine learning experiments.

* Use pyoSoundMaker.py to generate some 1-second samples. NB despite the name, pyoSoundMaker uses only standard Python libraries, not Pyo.

* Run tSNE-audio.py on the directory of samples to generate a JSON file. The command is `python tSNE-audio.py --input_dir synth-samples --output_file out-tSNE.json`

* Run the Openframeworks t-SNE audio viewer with the generated JSON file: https://ml4a.github.io/guides/AudioTSNEViewer/. In the version committed here, the JSON file is hard-coded to `C:\Programs\tSNE\out-tSNE.json`.
