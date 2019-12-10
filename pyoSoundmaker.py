import wave, struct, math, random


# Attack time a is a point in the 1-second clip
# Up to this point, the amplitude rises linearly from 0 to 1
# After that point, it falls linearly to 0.
def getEnvelope(a, t):
    if t < a:
        return t / a
    else:
        return (1 - t)/(1 - a)

def writeFile(operators, fileNum):

    ww = wave.open("sample-" + str(fileNum) + ".wav", "wb")

    ww.setnchannels(1) # mono
    ww.setsampwidth(2)
    ww.setframerate(48100.0)
    t = 0.0
    for i in range(99999):
        value = 0.0
        for j in range(0, len(operators)):
            sine = 0.0
            if operators[j][2] > -1:
                modulator = math.sin(math.pi * 2 * t * operators[operators[j][2]][0])
                modulator = modulator * getEnvelope(operators[operators[j][2]][1], t)
                sine = math.sin(math.pi * 2 * t * operators[j][0] + modulator)
            else:
                sine = math.sin(math.pi * 2 * t * operators[j][0])
            value = value + sine * 32767 * getEnvelope(operators[j][1], t)
        value = int(value / len(operators))
        value = max(-32767, min(32767, value))
        t += 0.00001
        data = struct.pack('<h', value)
        ww.writeframesraw( data )

    ww.close()


for j in range(0, 100):
    ops = []
    numOps = random.randint(5, 30)
    for i in range(0, numOps):
        fm = -1
        if random.random() > 0.5 and i > 0:
            fm = random.randint(0, i - 1)
        ops.append([random.randint(100, 10000), random.random()*0.7, fm])
    writeFile(ops, j)
    print("Done file", j)
