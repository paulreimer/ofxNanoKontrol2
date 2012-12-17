/*
 * Copyright Paul Reimer, 2012
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
 * To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc/3.0/
 * or send a letter to
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
**/

#pragma once

#include "MidiMessage.h"
#include "RtMidi.h"

#include "ofImage.h"
#include "ofEvents.h"
#include <sys/types.h>
#include <vector>

class MixerChannel
{
public:
  MixerChannel()
  : slider(0.0)
  , knob(0.0)
  {
    for (size_t i=0; i<NumButtons; ++i)
      button[i] = false;
  }

  ~MixerChannel()
  {}

  float slider;
  float knob;

  ofRectangle sliderRect;
  ofVec2f knobOffset;
  
  const static float knobRadius;

  enum MixerChannelButtons
  {
    SoloButton,
    MuteButton,
    RecordButton,
    NumButtons=3
  };
  bool button[NumButtons];
  ofRectangle buttonRect[NumButtons];

  ofEvent<bool> sliderChangedEvent;
  ofEvent<bool> knobChangedEvent;
  ofEvent<bool> buttonChangedEvent;
};

class NanoKontrol2
{
public:
  NanoKontrol2();
  ~NanoKontrol2();

  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  
  void setup();
  void draw();
  
  void show(bool shouldShow=true);
  void hide();
  
  bool hidden;
  ofEvent<const MidiMessage> midiReceiveEvent;

  const static size_t MixerChannels = 8;
  MixerChannel channel[MixerChannels];

  ofEvent<bool> trackNextEvent;
  ofEvent<bool> trackPrevEvent;

  ofEvent<bool> cycleEvent;
  ofEvent<bool> rewindEvent;
  ofEvent<bool> fastForwardEvent;
  ofEvent<bool> stopEvent;
  ofEvent<bool> playEvent;
  ofEvent<bool> recordEvent;

  ofEvent<bool> markerSetEvent;
  ofEvent<bool> markerPrevEvent;
  ofEvent<bool> markerNextEvent;
  
  size_t midiChannel;
  static ofImage templateImage;
  ofRectangle drawRect;

  void _mouseMoved(ofMouseEventArgs &e);
	void _mousePressed(ofMouseEventArgs &e);
	void _mouseDragged(ofMouseEventArgs &e);
	void _mouseReleased(ofMouseEventArgs &e);  

protected:
  void midiReceive(const MidiMessage& msg);
  RtMidiIn* midiin;
};
