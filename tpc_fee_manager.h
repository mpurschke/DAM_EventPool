#include "dam_data.h"
#include <set>

class Packet;

class tpc_fee_manager {

 public:

  tpc_fee_manager ();

  int AddPacket ( Packet * p, const int fee);

  int inspect_set() const;

 protected:

#define MAX_CHANNELS 256
  
  std::set<dam_data * , damDataSort> TheSet;

  unsigned int max_bx[MAX_CHANNELS]; 
  unsigned int rollover[MAX_CHANNELS]; 

};


  
