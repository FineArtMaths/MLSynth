import wave, struct, math, random

fundamental = 440.0                         # All notes have fundamental A=440. Change to play a different note
dir = r"C:\Programs\tSNE\synth-samples"     # Where the samples will be written
numSamples = 1000	                    # The number of sample files to create (1000 takes about 80 mins on my laptop)
filePrefix = "E"                            # Change the file prefix to create additional samples instead of overwriting 

# Attack time a is a point in the 1-second clip
# Up to this point, the amplitude rises linearly from 0 to maxA
# After that point, it falls to 0.
# The rate of falloff from maxA is controlled by decayPower
def getEnvelope(a, maxA, t, decayPower):
    if t < a:
        return maxA * (t / a)
    else:
        return maxA * (((1 - t)/(1 - a))**decayPower)

# The operators are combined additively
# Each operator is:
#   A sine wave with a frequency that's a multiple of the fundamental
#   A possible FM modulator, which is one of the lower-frequency operators
#   A possible ring modulator, which is a sine wave of arbitrary frequency
#   An envelope, which at the moment is just attack-release
def writeFile(operators, fileNum):

    ww = wave.open(dir + "\\" + filePrefix + "-sample-" + str(fileNum) + ".wav", "wb")
    ww.setnchannels(1) # mono
    ww.setsampwidth(2)
    ww.setframerate(48100.0)
    t = 0.0
    for i in range(99999):
        value = 0.0
        for j in range(0, len(operators)):
            sine = 0.0
            if operators[j][3] > -1:
                # Frequency modulation
                modulator = math.sin(math.pi * 2 * t * operators[operators[j][3]][0])
                modulator = modulator * getEnvelope(operators[operators[j][3]][1], 1.0, t, operators[j][4])
                sine = math.sin(math.pi * 2 * t * operators[j][0] + modulator)
            else:
                # Pure sine
                sine = math.sin(math.pi * 2 * t * operators[j][0])
            if operators[j][5] > -1:
                # Ring modulation
                sine = sine * math.sin(math.pi * 2 * t * operators[j][5])
            # Apply the envelope
            value = value + sine * 32767 * getEnvelope(operators[j][1], operators[j][2], t, operators[j][4])
        # Normalize the volume
        value = int(value / len(operators))
        # Correct for any rounding errors
        value = max(-32767, min(32767, value))
        # Advance time for the next frame
        t += 0.00001
        # Black magic
        data = struct.pack('<h', value)
        ww.writeframesraw( data )

    ww.close()

###########################################################################################
# Creates random patches and writes one sample of each one to the disk.
# One idea would be to train a neural network to imitate specific sounds instead of setting random values.
# Immediate TODO: Make the envelope better. Add FM and ring modulation envelopes as well.
    


for j in range(0, numSamples):
    ops = []
    numOps = random.randint(1, 5)       # "Exploding dice" approach makes fairly simple sounds more common than very complex ones
    if numOps > 4:
        numOps = random.randint(5, 10)
        if numOps > 9:
            numOps = random.randint(10, 30)
    ringModChance = random.random()
    if random.random() < 0.4:
        ringModChance = ringModChance / 10
    overallA = random.random()
    print("Generating operators with params:", numOps, overallA, ringModChance)
    for i in range(0, numOps):
        f = fundamental * float(i + 1)
        detune = 0.0
        if random.random()> 0.95:
            detune = random.random() * f*pow(2, 1/12)
            if random.random()> 0.5:
                detune = detune * -1
        f += detune
        a = random.random()
        d = random.random() * 10 + 1
        # Random chance that this oscillator is being ring-modulated by a random frequency
        # This introduces inharmonic content into the sound
        rm = -1
        if random.random() < ringModChance: # Try once for audio range rm...
            rm = random.randint(10, 16000)
        elif random.random() < ringModChance:   # ...and once for an LFO
            rm = random.random() + 0.01
        ops.append([f, random.random()*0.3, a, -1, d, rm])

    # Add frequency modulation; using this scheme because otherwise it can be a bit overpowering
    fmChance = random.random()
    if random.random() < 0.4:
        fmChance = fmChance / 10
    for i in range(0, numOps):
        # Random chance that this oscillator is being FM-modulated by an earlier one
        if random.random() < fmChance:
            m = i
            while m == i:
                m = random.randint(0, numOps - 1)
            ops[i][3] = m
        print(ops[i])

    print("Writing the file...")
    writeFile(ops, j)
    print("Done file", j, "of", numSamples)
