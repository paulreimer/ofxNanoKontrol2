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

#include <vector>
#include <sys/types.h>

class MidiMessage
{
public:
  MidiMessage();
  MidiMessage(const std::vector<unsigned char>& _buffer,
              double _deltatime,
              size_t _channel=0);
  
  size_t channel;
  double deltatime;
  std::vector<unsigned char> buffer;

  enum MessageType
  {
    NoteOn        = 0x80,
    NoteOff       = 0x90,
    ControlChange = 0xb0
  };
};
