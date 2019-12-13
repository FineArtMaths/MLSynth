/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SynthUsingMidiInputTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Synthesiser with midi input.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once
#include "Patch.h"

//==============================================================================
struct SineWaveSound   : public SynthesiserSound
{
    SineWaveSound() {}

    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

//==============================================================================
struct SineWaveVoice   : public SynthesiserVoice
{
    SineWaveVoice() {
        p = Patch();
    }
    
    void setPatchDetuning(bool value) {
        p.toggleDetune(value);
    }

    void setPatchFM(bool value) {
        p.toggleFM(value);
    }

    void setPatchAM(bool value) {
        p.toggleFM(value);
    }

    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        time = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int) override      {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        time += 0.01;
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0) // [7]
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = p.getSample(currentAngle, time)* level * tailOff;

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99; // [8]

                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote(); // [9]

                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0) // [6]
                {
                    auto currentSample = p.getSample(currentAngle, time) * level;

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

private:
    Patch p;
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    double time = 0.0;
};

//==============================================================================
class SynthAudioSource   : public AudioSource
{
public:
    SynthAudioSource (MidiKeyboardState& keyState)
        : keyboardState (keyState)
    {
        init();
    }

    void init() {
        synth.clearVoices();
        synth.clearSounds();
        for (auto i = 0; i < 4; ++i)                // [1]
            synth.addVoice(new SineWaveVoice());

        synth.addSound(new SineWaveSound());       // [2]
    }

    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate (sampleRate); // [3]
        midiCollector.reset(sampleRate);
    }

    void releaseResources() override {}

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();

        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
        keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                             bufferToFill.numSamples, true);       // [4]

        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                               bufferToFill.startSample, bufferToFill.numSamples); // [5]
    }

    MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }
private:
    MidiKeyboardState& keyboardState;
    MidiMessageCollector midiCollector;
    Synthesiser synth;
};

//==============================================================================
class MainContentComponent   : public AudioAppComponent,
                               public Button::Listener,
                               private Timer
{
public:
    MainContentComponent()
        : synthAudioSource  (keyboardState),
          keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible (keyboardComponent);
        setAudioChannels (0, 2);

        // MIDI device input list
        addAndMakeVisible(midiInputListLabel);
        midiInputListLabel.setText("MIDI Input:", dontSendNotification);
        midiInputListLabel.attachToComponent(&midiInputList, true);
        addAndMakeVisible(midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
        auto midiInputs = MidiInput::getDevices();
        midiInputList.addItemList(midiInputs, 1);
        midiInputList.onChange = [this] { setMidiInput(midiInputList.getSelectedItemIndex()); };
        for (auto midiInput : midiInputs)
        {
            if (deviceManager.isMidiInputEnabled(midiInput))
            {
                setMidiInput(midiInputs.indexOf(midiInput));
                break;
            }
        }
        if (midiInputList.getSelectedId() == 0)
            setMidiInput(0);
            
        // Button to change to a new, random patch
        newPatch.setButtonText("NEW");
        newPatch.changeWidthToFitText();
        newPatch.setComponentID("NEW");
        newPatch.addListener(this);
        addAndMakeVisible(newPatch);

        // Button to change detuning state
        buttonDetune.setButtonText("Detuning");
        buttonDetune.changeWidthToFitText();
        buttonDetune.setComponentID("DETUNE");
        buttonDetune.addListener(this);
        addAndMakeVisible(buttonDetune);

        // Button to change FM
        buttonFM.setButtonText("FM");
        buttonFM.changeWidthToFitText();
        buttonFM.setComponentID("FM");
        buttonFM.addListener(this);
        addAndMakeVisible(buttonFM);

        // Button to change AM
        buttonAM.setButtonText("AM");
        buttonAM.changeWidthToFitText();
        buttonAM.setComponentID("AM");
        buttonAM.addListener(this);
        addAndMakeVisible(buttonAM);

        setSize (600, 160);
        startTimer (400);
    }

    ~MainContentComponent() override
    {
        shutdownAudio();
    }

    void setMidiInput(int index)
    {
        auto list = MidiInput::getDevices();
        deviceManager.removeMidiInputCallback(list[lastInputIndex], synthAudioSource.getMidiCollector()); // [13]
        auto newInput = list[index];
        if (!deviceManager.isMidiInputEnabled(newInput))
            deviceManager.setMidiInputEnabled(newInput, true);
        deviceManager.addMidiInputCallback(newInput, synthAudioSource.getMidiCollector()); // [12]
        midiInputList.setSelectedId(index + 1, dontSendNotification);
        lastInputIndex = index;
    }

    void resized() override
    {
        midiInputList.setBounds(340, 10, getWidth() - 350, 20);
        newPatch.setBounds(20, 10, 35, 20);
        buttonDetune.setBounds(60, 10, 55, 20);
        buttonAM.setBounds(120, 10, 35, 20);
        buttonFM.setBounds(160, 10, 35, 20);
        int h = getHeight() < 100 ? 50 : getHeight() - 50;
        keyboardComponent.setBounds (10, 40, getWidth() - 20, h);
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        synthAudioSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        synthAudioSource.getNextAudioBlock (bufferToFill);
    }

    void releaseResources() override
    {
        synthAudioSource.releaseResources();
    }
    
    // TODO: To make these work, I think we'll have to 
    //       subclass Synthesizer
    void buttonClicked(Button* button) {
        if (button->getComponentID() == "NEW") {
            synthAudioSource.init();
        } else if (button->getComponentID() == "DETUNE") {
        }
    }

    void buttonStateChanged(Button* button) {

    }

private:
    void timerCallback() override
    {
        keyboardComponent.grabKeyboardFocus();
        stopTimer();
    }

    //==========================================================================
    SynthAudioSource synthAudioSource;
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    ComboBox midiInputList;
    Label midiInputListLabel;
    TextButton newPatch;
    TextButton buttonDetune;
    TextButton buttonFM;
    TextButton buttonAM;
    int lastInputIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

