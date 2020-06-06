/*
 * dieclock.cpp
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
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrlib/time.h"
#include "avrlib/gpio.h"
#include "avrlib/adc.h"
#include "HardwareConfig.h"
#include "clock.h"
#include "ui.h"

// __Compiler Bug__
int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};
void __cxa_pure_virtual() {};


volatile uint8_t num_clock_ticks = 0;
volatile bool poll = false;
Ui ui;

ISR(TIMER1_COMPA_vect)
{
  PwmChannel1A::set_frequency(clock.Tick());  // N Ticks per Clock from ClockInput
  if(clock.running())
  {
    ++num_clock_ticks;
  }
}

ISR(TIMER2_OVF_vect, ISR_NOBLOCK)
{
  //ca 4kHz
  while (num_clock_ticks)
  {
    --num_clock_ticks;
    ui.onClock(); // reicht Clock an die App weiter
  }
  static int8_t subClock = 0;
  subClock = (subClock + 1) & 3;

  if (subClock == 0)
  { // 1kHz
    poll = true;
  }
}


int main(void)
{
  // Configure the timers.
    static const uint8_t PRESCALER = 3;
    static const uint8_t PRESCALER_VALUE = 64;
    //PWM mit CTC
    Timer<1>::set_mode(0, _BV(WGM12), PRESCALER);
    int16_t startFreq = 1024;
    int16_t counterForStartFreq = (20000000L / PRESCALER_VALUE) / startFreq -1;

    PwmChannel1A::set_frequency(counterForStartFreq);
    Timer<1>::StartCompare();
    //TIMSK |= (1<<OCIE1A);

    //     16MHz / (8 * 510) = 3906,25 Hz
    // prescaler(2)_|
    Timer<2>::set_prescaler(2);
    Timer<2>::set_mode(TIMER_PWM_PHASE_CORRECT);
    Timer<2>::Start();

    sei();

  Adc::StartConversion(AdcChannelPoti);

  ui.init();
  // after initialization of all port extender IOs
  initHW();
  _delay_ms(50);

  while(1)
  {
    static uint8_t step = 3;
    if(poll)
    {
      poll = false;
      ui.poll();
      ui.doEvents();
    }
    ui.doEvents();
  }
}
