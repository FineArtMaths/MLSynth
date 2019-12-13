#include "Envelope.h"
#include <random>

Envelope::Envelope() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);
    attack = dist(e2);
    decay = dist(e2);
    sustain = dist(e2);
    release = dist(e2);

}

Envelope::Envelope(float a, float d, float s, float r) {
    attack = a;
    decay = d;
    sustain = s;
    release = r;
}

float Envelope::getEnvelopeLevel(float time) {
    if (time < attack) {
        return sustain * time / attack;
    }
    return sustain;
    /*
    else if (time < attack + decay) {
        return sustain - sustain * (time - attack) / decay;
    }
    else if (time < attack + decay + release) {
        return 0.0f;
    }
    else {
        return 0.0f;
    }*/
}

float Envelope::getSustainLevel() {
    return sustain;
}

void Envelope::setSustainLevel(float level) {
    sustain = level;
}
