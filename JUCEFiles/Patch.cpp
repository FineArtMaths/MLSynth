#include "Patch.h"
#include <cmath>
#include<random>
#include<iostream>

Patch::Patch() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);
    

    numOps = floor(dist(e2) * 20) + 2;
    bool allow_detuning = dist(e2) > 0.5;
    bool allow_fm = dist(e2) > 0.5;
    bool allow_am = dist(e2) > 0.5;
    ops = new Operator[numOps]();

    float totalLevel = 0.0;
    for (int i = 0; i < numOps; i++) {
        ops[i] = Operator(i, allow_detuning, allow_fm, allow_am);
        totalLevel += ops[i].getSustainLevel();
    }
    // Normalize the peak volume
    for (int i = 0; i < numOps; i++) {
        ops[i].setSustainLevel(numOps * ops[i].getSustainLevel()/ totalLevel);
    }
    global_envelope = Envelope(0.0, 30.0, 1.0, 0.0);
    std::cout << "Patch created with " << numOps << " operators" << std::endl;
    for (int i = 0; i < numOps; i++) {
        std::cout << i << ": " << ops[i].getSustainLevel() << "  |  ";
    }
}

float Patch::getSample(float angle, float time) {
    float r = 0.0f;
    for (int i = 1; i <= numOps; i++) {
        r += ops[i].getSample(angle, time) * global_envelope.getEnvelopeLevel(time);
    }
    r /= numOps;
    return r;
}

void Patch::toggleDetune(bool value) {
    for (int i = 1; i <= numOps; i++) {
        ops[i].toggleDetune(value);
    }
}
void Patch::toggleAM(bool value) {
    for (int i = 1; i <= numOps; i++) {
        ops[i].toggleAM(value);
    }
}
void Patch::toggleFM(bool value) {
    for (int i = 1; i <= numOps; i++) {
        ops[i].toggleFM(value);
    }
}
