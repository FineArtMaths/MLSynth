#pragma once
#include "Operator.h"

class Patch
{
public:
	Patch();
	float getSample(float angle, float time);
	void toggleDetune(bool value);
	void toggleAM(bool value);
	void toggleFM(bool value);

private:
	Operator* ops;
	Envelope global_envelope;
	int numOps;
};

