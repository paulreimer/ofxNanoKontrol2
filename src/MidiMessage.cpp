/*
 * Copyright Paul Reimer, 2012
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
 * To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc/3.0/
 * or send a letter to
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
**/

#include "MidiMessage.h"
#include <iostream>

//--------------------------------------------------------------
MidiMessage::MidiMessage()
: channel(0)
, deltatime(0)
{}

//--------------------------------------------------------------
MidiMessage::MidiMessage(const std::vector<unsigned char>& _buffer,
                         double _deltatime,
                         size_t _channel)
: channel(_channel)
, deltatime(_deltatime)
, buffer(_buffer)
{}
