/*
 * med5.h
 *
 *  Created on: 04.06.2020
 *      Author: cybaer
 */

#ifndef MED5_H_
#define MED5_H_

#include <stdint.h>

class Median
{
public:
  Median(uint8_t init = 127)
  : pos(0)
  , a{init,init,init,init,init}
  , m{0,0,0,0,0}
  {

  }

  inline uint8_t getMedian(uint8_t val)
  {
    insert(val);
    copySorted(0, 1);
    copySorted(2, 3);

    if(m[0] < m[2])
    {
      swap(0, 2);
      swap(1, 3);
    }

    m[4] = a[4];
    if(m[1] < m[4]) swap(1, 4);

    if(m[1] > m[2])
    {
      if(m[2] > m[4])
        return m[2];
      else
        return m[4];
    }
    else
    {
      if(m[1] > m[3])
        return m[1];
      else
        return m[3];
    }
  }

private:
  void insert(uint8_t val)
  {
    a[pos++] = val;
    if(pos == 5) pos = 0;
  }
  void copySorted(uint8_t s, uint8_t t)
  {
    if(a[s] < a[t])
    {
      m[s] = a[t];
      m[t] = a[s];
    }
    else
    {
      m[s] = a[s];
      m[t] = a[t];
    }
  }
  void swap(uint8_t l, uint8_t r)
  {
    uint8_t b = m[l];
    m[l] = m[r];
    m[r] = b;
  }
  int8_t pos;
  uint8_t a[5];
  uint8_t m[5];


};


#endif /* MED5_H_ */
