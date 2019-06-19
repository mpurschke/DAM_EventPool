#include "dam_data.h"
#include <set>
#include <map>

class Packet;

class tpc_fee_manager {

 public:

  tpc_fee_manager ();

  int AddPacket ( Packet * p, const int fee);

  int inspect_set() const;
  int getSize() const;
  int getCurrentBX() const { return _current_bx;};
  int getNextEvent();
  int getNrChannels() const;
  int getNrSamples(const int channel) const;
  int getSampleValue(const int channel, const int sample) const; 


 protected:

#define MAX_CHANNELS 256


  dam_data *getNextRecord() const;
  int deleteRecord ( dam_data *dd);


  
  std::set<dam_data * , damDataSort> TheSet;

  std::map<unsigned int, dam_data *> TheCrossing;

  unsigned int max_bx[MAX_CHANNELS]; 
  unsigned int rollover[MAX_CHANNELS]; 
  unsigned int _current_bx;

};


  
