#ifndef __DAM_DATA_H__
#define __DAM_DATA_H__

#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>

class dam_data {
  
 public:
  
  dam_data ( const unsigned int bx,
	     const unsigned short fee,
	     const unsigned short channel,
	     const unsigned short * samples , const unsigned int nr_samples);
  
  virtual ~dam_data() {};
  

  virtual void identify( std::ostream &os = std::cout, const int flag = 0);

  virtual unsigned long long get_key() const;
  virtual unsigned int get_bx() const {return _bx; };
  virtual unsigned int get_channel() const {return _channel; };
  virtual int get_nr_samples() const {return _samples.size(); };
  virtual unsigned short get_sample(const unsigned int s) const;
  
  
 protected:

  unsigned int _bx;
  unsigned short _fee;
  unsigned short _channel;

  std::vector <unsigned short > _samples;

};


// this is our sort-helper for the Set
struct damDataSort:
  public std::binary_function<const dam_data *, const dam_data*, bool> {

  bool operator() (const dam_data *lhs, const dam_data *rhs) const
  {
    return  ( lhs->get_key() <= rhs->get_key() );
    
  }
};


#endif
