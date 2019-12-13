#include "Operator.h"
#include <cmath>
#include <stdlib.h>
#include <random>

#define DETUNE_MAX 0.0001

Operator::Operator() {
    harmonic = 0;
}
Operator::Operator(
    int harmonic, 
    bool allow_detuning, 
    bool allow_fm, 
    bool allow_am
    ) {

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);    
    std::exponential_distribution<> edist(1);

    Operator::allow_detuning = allow_detuning;
    Operator::allow_fm = allow_fm;
    Operator::allow_am = allow_am;

    Operator::harmonic = harmonic;
    Envelope envelope = Envelope(dist(e2) * 5, dist(e2) * 50, 0.5 + dist(e2) * 0.5, 0.0);
    if (harmonic > 0 && dist(e2) > 0.9) {
        detune = edist(e2) * DETUNE_MAX * 2 - 0.05;
    }
    
    fm_multiplier = dist(e2) / 10;
    fm_envelope = Envelope(dist(e2) * 5, dist(e2) * 10, dist(e2), 0.0f);

    am_multiplier = dist(e2);
    if (am_multiplier > 0.5) {
        am_multiplier *= 5;
    }
    am_envelope = Envelope(dist(e2) * 5, dist(e2) * 10, dist(e2), 0.0f);
}

float Operator::getSample(float angle, float time) {
    float fm = 0.0;
    if (allow_fm) {
        fm = std::sin(angle * 5 * fm_multiplier);
        fm *= fm_envelope.getEnvelopeLevel(time);
    }
    float r = 0.0;
    if (allow_detuning) {
        std::sin(angle * 5 * harmonic * detune + fm);
    }
    else {
        std::sin(angle * 5 * harmonic + fm);
    }
    if (allow_am) {
        float am = std::sin(angle * 5 * am_multiplier);
        r = am * am_envelope.getEnvelopeLevel(time) + r * (1 - am_envelope.getEnvelopeLevel(time));
    }
    r = r * envelope.getEnvelopeLevel(time);
    return r;
}

float Operator::getSustainLevel() {
    return envelope.getSustainLevel();
}

void Operator::setSustainLevel(float level) {
    envelope.setSustainLevel(level);
}


void Operator::toggleDetune(bool value) {
    allow_detuning = value;
}
void Operator::toggleAM(bool value) {
    allow_am = value;
}
void Operator::toggleFM(bool value) {
    allow_fm = value;
}
