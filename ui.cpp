/*
 * ui.cpp
 *
 *  Created on: 17.05.2020
 *      Author: cybaer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "avrlib/op.h"
#include "ui.h"
#include "clock.h"

#include "med5.h"

Divider Div_2(2);
Divider Div_3(3);
Divider Div_4(4);
Divider Div_5(5);
Divider Div_7(7);
Divider Div_8(8);
Divider Div_12(12);
Divider Div_16(16);
Divider Div_24(24);
Divider Div_64(64);
Divider Div_128(128);

Median med;
static const int8_t MODE_EXTERNAL = 0;
static const int8_t MODE_INTERNAL = 2;

void Ui::init()
{
  m_Stop = true;
  m_BpmOffset = 0;
  m_204 = Switch204::getValue();
  m_Tempo = SwitchTempo::getValue();
  m_Mode = SwitchMode::getValue();
  m_AdcChannel = AdcChannelCV;
  Adc::StartConversion(m_AdcChannel);
}
void Ui::poll()
{
  Button::Read();
  if(Button::lowered()) DividerFarm::reset();
}
void Ui::doEvents()
{
  if(ResetIn::isTriggered())
  {
    DividerFarm::reset();
  }
  if(ClockIn::isTriggered())
  {
    if(m_Mode == MODE_EXTERNAL)
    {
      m_Stop = false;
      (void)clock.ClockInEdge();
    }
  }

  if (Adc::ready())
  {
    switch(m_AdcChannel)
    {
      case AdcChannelCV:
      {
        if(m_Mode == MODE_INTERNAL)
        {
          uint32_t val = Adc::Read(AdcChannelCV) / 4;
          // CV calibration -5.0V:255, 0.0V:127, +5.0V:0
          val *= 1083l;
          val /= 1024l;
          val -= 1;
          // mapping to +-30 BPM
          m_BpmOffset = U8U8MulShift8(val, 60) - 30;
        }
        else
        {
          m_BpmOffset = 0;
        }
        m_AdcChannel = AdcChannelPoti;
        break;
      }
      case AdcChannelPoti:
      {
        if(m_Mode == MODE_INTERNAL)
        {
          uint16_t val = med.getMedian(Adc::Read(AdcChannelPoti) / 4);
          uint16_t bpm = m_Tempo == 0 ? U8U8MulShift8(val, 60) + 30 :     // maps to  30 ...  90
                         m_Tempo == 1 ? U8U8MulShift8(val, 120) + 60 :    // maps to 360 ... 180
                         m_Tempo == 2 ? U8U8MulShift8(val, 240) + 120 :   // maps to 120 ... 360
                         120;                                             // default 120 BPM
          clock.update(bpm - m_BpmOffset);
        }
        m_AdcChannel = AdcChannel204;
        break;
      }
      case AdcChannel204:
      {
        m_204 = Switch204::getValue();
        m_AdcChannel = AdcChannelTempo;
        break;
      }
      case AdcChannelTempo:
      {
        m_Tempo = SwitchTempo::getValue();
        m_AdcChannel = AdcChannelMode;
        break;
      }
      case AdcChannelMode:
      {
        int8_t mode = SwitchMode::getValue();
        if(mode == MODE_EXTERNAL && m_Mode != MODE_EXTERNAL)
        {
          m_Stop = true;
          DividerFarm::reset();
        }
        if(mode == MODE_INTERNAL) m_Stop = false;
        m_Mode = mode;
        m_AdcChannel = AdcChannelCV;
        break;
      }
    }
    Adc::StartConversion(m_AdcChannel);
  }
}

void Ui::onClock()
{
  bool clk = m_Stop ? false : clock.getClock();
  LED::set_value(clk);
  bool out1 = false;
  if(Button::high() && ResetIn::getValue() == 0)
  {
    DividerFarm::clock(clk);
    out1 = clk;
  }
  Output_1::set_value(out1);
  switch(m_204)
  {
  case 0:
    {
      Output_2::set_value(Div_2.getValue());
      Output_3::set_value(Div_4.getValue());
      Output_4::set_value(Div_8.getValue());
      break;
    }
  case 1:
    {
      Output_2::set_value(Div_3.getValue());
      Output_3::set_value(Div_5.getValue());
      Output_4::set_value(Div_7.getValue());
      break;
    }
  case 2:
    {
      Output_2::set_value(Div_4.getValue());
      Output_3::set_value(Div_16.getValue());
      Output_4::set_value(Div_64.getValue());
      break;
    }
  }
  Output_5::set_value(Div_2.getValue());
  Output_6::set_value(Div_4.getValue());
  Output_7::set_value(Div_8.getValue());
  Output_8::set_value(Div_12.getValue());
  Output_9::set_value(Div_24.getValue());
  Output_10::set_value(Div_128.getValue());
}

Divider::Divider(uint8_t divider)
: m_Divider(divider)
, m_Counter(0)
, m_OldValue(false)
, m_Output(false)
{
  DividerFarm::registerDivider(this);
}

Divider* DividerFarm::m_Divider[MaxDivider];
int8_t DividerFarm::m_Counter=0;
