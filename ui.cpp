/*
 * ui.cpp
 *
 *  Created on: 17.05.2020
 *      Author: cybaer
 */

#include "ui.h"
#include "clock.h"

Divider Div_2(2);
Divider Div_3(3);
Divider Div_4(4);
Divider Div_5(5);
Divider Div_7(7);
Divider Div_8(8);
Divider Div_12(12);
Divider Div_16(16);
Divider Div_24(24);
Divider Div_32(32);

void Ui::poll()
{
  Button::Read();
  if(Button::lowered()) DividerFarm::reset();
}

void Ui::onClock()
{
  volatile bool clk = clock.getClock();
  LED::set_value(clk);
  bool out1 = false;
  if(Button::high())
  {
    DividerFarm::clock(clk);
    out1 = clk;
  }
  Output_1::set_value(out1);
  Output_5::set_value(Div_2.getValue());
  Output_6::set_value(Div_4.getValue());
  Output_7::set_value(Div_8.getValue());
  Output_8::set_value(Div_12.getValue());
  Output_9::set_value(Div_24.getValue());
  Output_10::set_value(Div_32.getValue());


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
