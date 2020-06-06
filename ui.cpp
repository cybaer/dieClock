/*
 * ui.cpp
 *
 *  Created on: 17.05.2020
 *      Author: cybaer
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


void Ui::init()
{
  m_204 = Switch204::getValue();
  m_Tempo = SwitchTempo::getValue();
  m_Mode = SwitchMode::getValue();
  m_AdcChannel = AdcChannelPoti;
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
    bool TaktStart = clock.ClockInEdge();
  }

  if (Adc::ready())
  {
    switch(m_AdcChannel)
    {
      case AdcChannelPoti:
      {
        uint16_t val = med.getMedian(Adc::Read(AdcChannelPoti)/4);

        uint16_t bpm = m_Tempo == 0 ? U8U8MulShift8(val, 60) + 30 :
                       m_Tempo == 1 ? U8U8MulShift8(val, 120) + 60 :
                       m_Tempo == 2 ? U8U8MulShift8(val, 240) + 120 :
                       120;
        clock.update(bpm);
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
        m_Mode = SwitchMode::getValue();
        m_AdcChannel = AdcChannelPoti;
        break;
      }
    }
    Adc::StartConversion(m_AdcChannel);
  }
}

void Ui::onClock()
{
  volatile bool clk = clock.getClock();
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
