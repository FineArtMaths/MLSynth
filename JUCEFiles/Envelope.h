#pragma once

class Envelope
{
public:
	Envelope();
	Envelope(float a, float d, float s, float r);
	float getEnvelopeLevel(float time);
	float getSustainLevel();
	void setSustainLevel(float level);
private:
	float attack;
	float decay;
	float sustain;
	float release;
};