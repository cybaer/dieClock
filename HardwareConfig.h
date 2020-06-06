/*
 * HardwareConfig.h
 *
 *  Created on: 01.05.2020
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
 *
 */

#ifndef HARDWARECONFIG_H_
#define HARDWARECONFIG_H_

#include "avrlib/base.h"
#include "avrlib/gpio.h"
#include "avrlib/devices/switch.h"
#include "avrlib/adc.h"
#include "lib/ui/EdgeTrigger.h"

// __Compiler Bug__
__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual();


using namespace avrlib;

static const uint8_t STAB_BITS = 1;
static const uint8_t STAB = (1 << STAB_BITS);

class AnalogStabilizer
{
public:
  AnalogStabilizer()
  : m_Count(0)
  {}

  uint16_t operator() (uint16_t in)
  {
    m_Raw[m_Count++ & (STAB - 1)] = in;
    uint16_t sum = 0;
    for(int8_t i=0; i<STAB; i++)
    {
      sum += m_Raw[i];
    }
    return sum / STAB;
  }
private:
  uint16_t m_Raw[STAB];
  uint8_t m_Count;
};

typedef DebouncedSwitch< Gpio<PortD, 0> > Button;  // 0

typedef Gpio<PortD, 1> LED;  // 1

typedef EdgeTrigger<Gpio<PortD, 2>, 0> ClockIn;  // xx
typedef EdgeTrigger<Gpio<PortC, 1>, 0> ResetIn;  // xx


typedef Inverter<Gpio<PortB, 1> > Output_10;  //
typedef Inverter<Gpio<PortD, 5> > Output_9;  //
typedef Inverter<Gpio<PortD, 7> > Output_8;  //
typedef Inverter<Gpio<PortD, 6> > Output_7;  //
typedef Inverter<Gpio<PortB, 0> > Output_6;  //
typedef Inverter<Gpio<PortD, 4> > Output_5;  //
typedef Inverter<Gpio<PortB, 5> > Output_4;  //
typedef Inverter<Gpio<PortB, 2> > Output_3;  //
typedef Inverter<Gpio<PortB, 4> > Output_2;  //
typedef Inverter<Gpio<PortB, 3> > Output_1;  //


static const uint8_t AdcChannelPoti = 5;

extern Adc adc;

template<typename Port, int8_t Divider>
class DividerOutput
{
public:

  static void Do(bool in)
  {
    if(in != m_OldValue)
    {
      m_OldValue = in;
      if(++m_Counter >= Divider)
      {
        m_Counter = 0;
        Port::Toggle();
      }
    }
  }
  static void setActive(bool active)
  {
    m_Active = active;
    if(m_Active)
    {

    }
  }
  static uint8_t m_Counter;
  static uint8_t m_OldValue;
  static bool m_Active;
};
template<typename Port, int8_t Divider>
uint8_t DividerOutput<Port, Divider>::m_Counter;
template<typename Port, int8_t Divider>
uint8_t DividerOutput<Port, Divider>::m_OldValue;
template<typename Port, int8_t Divider>
bool DividerOutput<Port, Divider>::m_Active;

template<typename DO_1, typename DO_2, typename DO_3>
class TripleDividerOutput
{
public:
  static void Do(bool in)
  {
    DO_1::Do(in);
    DO_2::Do(in);
    DO_3::Do(in);
  }
};


//typedef DividerOutput<Output_1, 2> Div_2;

inline void initInputs(void)
{
  Button::Init();
  ClockIn::init();
  ResetIn::init();
}
inline void initOutputs(void)
{
  Output_1::set_mode(DIGITAL_OUTPUT);
  Output_1::set_value(false);
  Output_2::set_mode(DIGITAL_OUTPUT);
  Output_2::set_value(false);
  Output_3::set_mode(DIGITAL_OUTPUT);
  Output_3::set_value(false);
  Output_4::set_mode(DIGITAL_OUTPUT);
  Output_4::set_value(false);
  Output_5::set_mode(DIGITAL_OUTPUT);
  Output_5::set_value(false);
  Output_6::set_mode(DIGITAL_OUTPUT);
  Output_6::set_value(false);
  Output_7::set_mode(DIGITAL_OUTPUT);
  Output_7::set_value(false);
  Output_8::set_mode(DIGITAL_OUTPUT);
  Output_8::set_value(false);
  Output_9::set_mode(DIGITAL_OUTPUT);
  Output_9::set_value(false);
  Output_10::set_mode(DIGITAL_OUTPUT);
  Output_10::set_value(false);

  LED::set_mode(DIGITAL_OUTPUT);
  LED::set_value(false);

}

inline void initAnalogIn(void)
{
  adc.Init();
}

inline void initHW(void)
{
  initInputs();
  initOutputs();
  initAnalogIn();
}

#endif /* HARDWARECONFIG_H_ */
