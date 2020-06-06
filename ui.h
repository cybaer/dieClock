/*
 * ui.h
 *
 *  Created on: Apr 24, 2019
 *      Author: cybaer
 */

#ifndef UI_H_
#define UI_H_

#include "HardwareConfig.h"



using namespace avrlib;
class Ui
{
public:
  //Ui(void);
  //void init();
  void poll();
  //void doEvents();
  void onClock();


private:
  int8_t m_Button;
  int8_t m_Counter;


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
  /*DividerFarm(int8_t d1=1, int8_t d2=2, int8_t d3=3, int8_t d4=4,
              int8_t d5=5, int8_t d6=7, int8_t d7=8, int8_t d8=12,
              int8_t d9=16, int8_t d10=24, int8_t d11=64, int8_t d12=128)
  : m_Divider{d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12}
  {}*/

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
