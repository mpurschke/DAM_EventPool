// dam_data.cc

#include "dam_data.h"

#include <iomanip>

using namespace std;

dam_data::dam_data ( const unsigned int bx,
		     const unsigned short fee,
		     const unsigned short channel,
		     const unsigned short * samples , const unsigned int nr_samples)
{

  _bx = bx;
  _fee = fee;
  _channel = channel;

  for (int i = 0; i < nr_samples; i++)
    {
      _samples.push_back(samples[i]);
    }
}


void dam_data::identify( std::ostream &os, const int flag)
{
  os << hex << "bx =  " << std::setw(6) << _bx << dec 
     << " fee " << std::setw(4) << _fee
     << " channel " << std::setw(4) << _channel  << " nr_samples  " << get_nr_samples() << endl;
}

unsigned short dam_data::get_sample(const unsigned int s)
{
  if ( s >= _samples.size() ) return 0;
  return _samples.at(s);
}

