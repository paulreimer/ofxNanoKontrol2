/*
 * Copyright Paul Reimer, 2012
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
 * To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc/3.0/
 * or send a letter to
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
**/

#include "NanoKontrol2.h"
#include "ofGraphics.h"

const float MixerChannel::knobRadius = 27.0;

//--------------------------------------------------------------
void _midicallback(double deltatime,
                   std::vector<unsigned char> *buffer,
                   void* userData)
{
  NanoKontrol2* nanoKontrol2 = (NanoKontrol2*)userData;

  if (buffer)
  {
    MidiMessage message(*buffer, deltatime);
    ofNotifyEvent(nanoKontrol2->midiReceiveEvent, message);
  }
  else {
    MidiMessage emptyMessage;
    ofNotifyEvent(nanoKontrol2->midiReceiveEvent, emptyMessage);
  }
}

ofImage NanoKontrol2::templateImage;

//--------------------------------------------------------------
NanoKontrol2::NanoKontrol2()
: hidden(false)
, midiin(NULL)
{
  for (size_t i=0; i<MixerChannels; ++i)
  {
    channel[i].slider = 0.0;
    for (size_t j=0; j<MixerChannel::NumButtons; ++j)
      channel[i].button[j] = false;
  }
}

//--------------------------------------------------------------
NanoKontrol2::~NanoKontrol2()
{
  if (midiin)
  {
//    delete midiin;
    midiin = NULL;
  }
}

//--------------------------------------------------------------
void
NanoKontrol2::setup()
{
  if (!templateImage.isAllocated())
    templateImage.loadImage("images/KorgNanoKontrol2-template.png");

  // Default Draw Size
  float controllerWidth = 500;
  float controllerAspect;
  if (templateImage.isAllocated())
    controllerAspect = ((float)templateImage.getHeight() /
                        (float)templateImage.getWidth());
  else
    controllerAspect = ((float)300 /
                        (float)1190); // stock image size

  drawRect.set(0, 0, controllerWidth, controllerWidth*controllerAspect);

  ofVec2f mixerChannelOffset(356, 23);
  ofVec2f nextChannelOffset (102, 0);
  ofVec2f knobOffset        (64, 27);
  ofRectangle sliderRect    (45, 84, 34, 156);
  ofRectangle buttonRect    (0, 92, 32, 32);
  ofVec2f nextButtonOffset  (0, 57);

  ofVec2f cursor(mixerChannelOffset);
  for (size_t i=0; i<MixerChannels; ++i)
  {
    // slider
    channel[i].sliderRect.set(sliderRect);
    channel[i].sliderRect.x = (sliderRect.x + cursor.x);
    channel[i].sliderRect.y = (sliderRect.y + cursor.y);

    // knob
    channel[i].knobOffset.set(knobOffset + cursor);

    // buttons
    ofRectangle thisButtonRect(buttonRect);
    thisButtonRect.x += cursor.x;
    thisButtonRect.y += cursor.y;
    for (size_t j=0; j<MixerChannel::NumButtons; ++j)
    {
      channel[i].buttonRect[j].set(thisButtonRect);
      thisButtonRect.y += nextButtonOffset.y;
    }

    cursor += nextChannelOffset;
  }

  try {
    RtMidiIn* midiin = new RtMidiIn();
    size_t numPorts = midiin->getPortCount();
    size_t nanoKontrolPort = INT_MAX;
    const std::string nanoKONTROL2("nanoKONTROL2");
    if (midiin->getPortCount() >= 1)
    {
      for (size_t i=0; i<numPorts; ++i)
      {
        if (midiin->getPortName(i).substr(0, nanoKONTROL2.size()) == nanoKONTROL2)
        {
          nanoKontrolPort = i;
          midiin->openPort(nanoKontrolPort);
          midiin->setCallback(&_midicallback, this);

          // Don't ignore sysex, timing, or active sensing messages.
          midiin->ignoreTypes(false, false, false);
          ofAddListener(midiReceiveEvent, this, &NanoKontrol2::midiReceive);
        }
      }
    }
  }
  catch (RtError &error)
  {
    std::cout << "MIDI input exception:" << std::endl;
    error.printMessage();
  }
  
  show();
}

/*
//--------------------------------------------------------------
void
NanoKontrol2::update()
{}
*/
//--------------------------------------------------------------
void
NanoKontrol2::draw()
{
  if (hidden)
    return;

  ofSetColor(255, 255, 255, 255);

  templateImage.draw(drawRect);
  ofPushMatrix();
  {
    ofTranslate(drawRect.x, drawRect.y);
    if (drawRect.width > 0 && drawRect.height > 0)
      ofScale(drawRect.width/templateImage.getWidth(),
              drawRect.height/templateImage.getHeight());

    // Draw special buttons
    {
    }

    // Draw mixer channels
    for (size_t i=0; i<MixerChannels; ++i)
    {
      // Draw slider
      {
        ofRectangle channelSliderRect   = channel[i].sliderRect;
        channelSliderRect.height       *= channel[i].slider;
        channelSliderRect.y            += channel[i].sliderRect.height - channelSliderRect.height;
        ofSetColor(0, 255, 0, 127);
        ofRect(channelSliderRect);
      }

      // Draw knob
      {
        ofVec2f indicatorLine(channel[i].knobOffset);
        indicatorLine.x -= channel[i].knobRadius;
        float angle = ofMap(channel[i].knob, 0.0, 1.0, -45, 225);
        indicatorLine.rotate(angle, channel[i].knobOffset);
        ofSetColor(255, 255, 255, 200);
        ofSetLineWidth(2.0);
        ofLine(channel[i].knobOffset, indicatorLine);
        ofSetLineWidth(1.0);
      }

      // Draw buttons
      {
        for (size_t j=0; j<MixerChannel::NumButtons; ++j)
        {
          if (channel[i].button[j])
          {
            ofSetColor(255, 0, 0, 127);
            ofRect(channel[i].buttonRect[j]);
          }
        }
      }
    }
  }
  ofPopMatrix();
}

//--------------------------------------------------------------
void
NanoKontrol2::midiReceive(const MidiMessage& msg)
{
  if (msg.buffer.empty())
    return;
  
  MidiMessage::MessageType messageType = (MidiMessage::MessageType)msg.buffer.at(0);
  switch ((messageType&0xf0))
  {
    case MidiMessage::ControlChange:
    {
      size_t idx = msg.buffer.at(1);
      float val = ofMap(msg.buffer.at(2), 0, 127, 0.0, 1.0);
      
      if (idx < MixerChannels)
        channel[idx].slider = val;
      else if (0x10 <= idx && idx <= (0x10+MixerChannels))
        channel[idx-0x10].knob = val;

      break;
    }

    case MidiMessage::NoteOn:
    {
      size_t idx = msg.buffer.at(1);
      
      if (0x20 <= idx && idx < (0x20+MixerChannels))
        channel[idx-0x20].button[MixerChannel::SoloButton]   = true;
      else if (0x30 <= idx && idx < (0x30+MixerChannels))
        channel[idx-0x30].button[MixerChannel::MuteButton]   = true;
      else if (0x40 <= idx && idx < (0x40+MixerChannels))
        channel[idx-0x40].button[MixerChannel::RecordButton] = true;

      break;
    }

    case MidiMessage::NoteOff:
    {
      size_t idx = msg.buffer.at(1);

      if (0x20 <= idx && idx < (0x20+MixerChannels))
        channel[idx-0x20].button[MixerChannel::SoloButton]   = false;
      else if (0x30 <= idx && idx < (0x30+MixerChannels))
        channel[idx-0x30].button[MixerChannel::MuteButton]   = false;
      else if (0x40 <= idx && idx < (0x40+MixerChannels))
        channel[idx-0x40].button[MixerChannel::RecordButton] = false;

      break;
    }
      
    default:
    {
    }
  }
/*
  unsigned int nBytes = msg.buffer.size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << std::hex << (int)msg.buffer.at(i) << ", ";
  
  std::cout << "stamp = " << msg.deltatime << std::endl;
*/
}

//--------------------------------------------------------------
void
NanoKontrol2::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void
NanoKontrol2::mouseDragged(int x, int y, int button)
{
  ofVec2f fromCorner(drawRect.getMin());
  ofVec2f toCorner(drawRect.getMax());
  ofVec2f scaledMouse(ofMap(x, fromCorner.x, toCorner.x, 0, templateImage.getWidth()),
                      ofMap(y, fromCorner.y, toCorner.y, 0, templateImage.getHeight()));

  for (size_t i=0; i<MixerChannels; ++i)
  {
    if (channel[i].sliderRect.inside(scaledMouse))
    {
      ofVec2f sliderRelativeMouse(scaledMouse.x - channel[i].sliderRect.x,
                                  scaledMouse.y - channel[i].sliderRect.y);

      channel[i].slider = 1.0 - ofClamp(sliderRelativeMouse.y / channel[i].sliderRect.height, 0., 1.);
    }

    if (channel[i].knobOffset.distance(scaledMouse) < channel[i].knobRadius)
    {
      float angle = channel[i].knobOffset.angle(scaledMouse);
      channel[i].knob = ofMap(angle, -45, 225, 0., 1., true);
    }

    for (size_t j=0; j<MixerChannel::NumButtons; ++j)
    {
      if (channel[i].buttonRect[j].inside(scaledMouse))
        channel[i].button[j] = !channel[i].button[j];
    }
  }
}

//--------------------------------------------------------------
void
NanoKontrol2::mousePressed(int x, int y, int button)
{
  mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void
NanoKontrol2::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void
NanoKontrol2::show(bool shouldShow)
{
  if (shouldShow)
  {
    ofAddListener(ofEvents().mousePressed,      this, &NanoKontrol2::_mousePressed);
    ofAddListener(ofEvents().mouseMoved,        this, &NanoKontrol2::_mouseMoved);
    ofAddListener(ofEvents().mouseDragged,      this, &NanoKontrol2::_mouseDragged);
    ofAddListener(ofEvents().mouseReleased,     this, &NanoKontrol2::_mouseReleased);
  }
  else {
    ofRemoveListener(ofEvents().mousePressed,   this, &NanoKontrol2::_mousePressed);
    ofRemoveListener(ofEvents().mouseMoved,     this, &NanoKontrol2::_mouseMoved);
    ofRemoveListener(ofEvents().mouseDragged,   this, &NanoKontrol2::_mouseDragged);
    ofRemoveListener(ofEvents().mouseReleased,  this, &NanoKontrol2::_mouseReleased);
  }
  hidden = !shouldShow;
}

//--------------------------------------------------------------
void
NanoKontrol2::hide()
{
  show(false);
}

//--------------------------------------------------------------
void
NanoKontrol2::_mouseMoved(ofMouseEventArgs &e)
{
  mouseMoved(e.x, e.y);
}

//--------------------------------------------------------------
void
NanoKontrol2::_mousePressed(ofMouseEventArgs &e)
{
  mousePressed(e.x, e.y, e.button);
}

//--------------------------------------------------------------
void
NanoKontrol2::_mouseDragged(ofMouseEventArgs &e)
{
  mouseDragged(e.x, e.y, e.button);
}

//--------------------------------------------------------------
void
NanoKontrol2::_mouseReleased(ofMouseEventArgs &e)
{
  mouseReleased(e.x, e.y, e.button);
}
