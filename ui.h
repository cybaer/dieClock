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

#ifndef UI_H_
#define UI_H_

#include "HardwareConfig.h"

using namespace avrlib;
class Ui
{
public:
  //Ui(void);
  void init();
  void poll();
  void doEvents();
  void onClock();

private:
  int8_t m_AdcChannel;
  int8_t m_Tempo;
  int8_t m_Mode;
  int8_t m_204;
  int8_t m_BpmOffset;
  bool m_Stop;
};

class DividerFarm;
class Divider
{
  public:
  Divider(uint8_t divider);

  void reset()
  {
    m_Counter = 0;
    m_OldValue = false;
    m_Output = false;
  }
  bool Do(bool in)
  {
    if(in != m_OldValue)
    {
      m_OldValue = in;
      if(m_Counter-- == 0)
      {
        m_Counter = m_Divider - 1;
        m_Output = !m_Output;
      }
    }
    return m_Output;
  }
  bool getValue() { return m_Output; } const

  uint8_t m_Divider;
  uint8_t m_Counter;
  bool m_OldValue;
  bool m_Output;
};

class DividerFarm
{
  static const int8_t DividerCount = 12;
  static const int8_t MaxDivider = 16;

public:
  static void registerDivider(Divider* div)
  {
    if(m_Counter < MaxDivider)
    {
      m_Divider[m_Counter] = div;
      m_Counter++;
    }
  }
  static void reset(void)
  {
      for(int8_t i=0; i<m_Counter; i++)
      {
        (void)m_Divider[i]->reset();
      }
    }
  static void clock(bool in)
  {
    for(int8_t i=0; i<m_Counter; i++)
    {
      (void)m_Divider[i]->Do(in);
    }
  }
private:
  static Divider* m_Divider[MaxDivider];
  static int8_t m_Counter;
};

extern Ui ui;

#endif /* UI_H_ */
