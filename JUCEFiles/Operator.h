#pragma once
#include "Envelope.h"


enum EnvelopeStage {
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
};

class Operator
{
public:
	Operator();
	Operator(int harmonic,
		bool allow_detuning = false,
		bool allow_fm = false,
		bool allow_am = false);
	float getSample(float angle, float time);
	float getSustainLevel();
	void setSustainLevel(float level);
	void toggleDetune(bool value);
	void toggleAM(bool value);
	void toggleFM(bool value);
private:
	int harmonic;
	float detune;
	Envelope envelope;
	float fm_multiplier;
	Envelope fm_envelope;
	float am_multiplier;
	Envelope am_envelope;
	bool allow_detuning;
	bool allow_fm;
	bool allow_am;
};
