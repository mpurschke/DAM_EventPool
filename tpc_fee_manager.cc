#include "tpc_fee_manager.h"
#include <Event/packet.h>

#include <unistd.h>

using namespace std;

tpc_fee_manager::tpc_fee_manager()
{
  for ( int i = 0; i < MAX_CHANNELS; i++)
    {
      max_bx[i] = 0;
      rollover[i] = 0;
    }
  _current_bx = 0;
}


int tpc_fee_manager::AddPacket ( Packet * p, const  int fee)
{

  unsigned short sample_vector[10240];
  unsigned int length;
  unsigned int bx;

  if ( ! p->iValue(fee, "IS_PRESENT")) return 0;

  for ( int ch = 0; ch < MAX_CHANNELS; ch++)
    {
      int sample_index = 0;
		  
      // get the lowest bx from this channel
      bx = (unsigned int) p->iValue(fee,ch,0,"BXRAW");  // note we are working here with the local BX number
      if ( bx > max_bx[ch] )  max_bx[ch] = bx;          
      
      // now we go through all samples of this channel
      for (int s = 0; s < p->iValue(fee,ch, "NR_SAMPLES"); s++ )
	{
	  
	  if ( (unsigned int) p->iValue(fee,ch,s,"BXRAW") != bx)
	    {
	      dam_data *dd = new dam_data( bx  + rollover[ch], fee, ch, sample_vector, sample_index);
	      TheSet.insert(dd);
	      
	      //	      dd->identify();
	      sample_index = 0;

	      // now the rollover business
	      if ( bx < (unsigned int) p->iValue(fee,ch,s,"BXRAW") )
		{
		  rollover[ch] += 0x100000;
		}
	      
	      bx =  (unsigned int) p->iValue(fee,ch,s,"BXRAW");
	      if ( bx > max_bx[ch] )  max_bx[ch] = bx;          
	    }

	  sample_vector[sample_index++] = p->iValue(fee,ch,s);
	}

      // we are done but may have the current record o take care of
      if ( sample_index > 0)
	{
	  dam_data * dd = new dam_data( bx  + rollover[ch], fee, ch, sample_vector, sample_index);
	  TheSet.insert(dd);
	  //dd->identify();
	  sample_index = 0;
	}
      
    }
}

int tpc_fee_manager::getNextEvent()
{
  // first, clear and get rid of the current BX data  
  if ( TheCrossing.size() > 0)
    {
      std::map<unsigned int, dam_data *>::iterator it;
      for ( it = TheCrossing.begin(); it != TheCrossing.end(); ++it)
	{
	  //(*it).second->identify();
	  delete (*it).second;  // this is where we finally delete the dam_data object
	}
      TheCrossing.clear();
    }

  // ok, now we find the next BX record 
  dam_data *dd = getNextRecord();
  if  ( dd == NULL) return 0;
  
  _current_bx = dd->get_bx();
  
  while ( dd && dd->get_bx() == _current_bx)
    {
      TheCrossing[dd->get_channel()] = dd;
      int i = deleteRecord(dd);
      // cout << " deleted " << i << " record from Set: ";
      // dd->identify();
      dd = getNextRecord();
    }
  if ( TheCrossing.size() == 0) return 0;

  return 1;  // indicate that we found an event
  
}

/// retrieve the "oldest"  dam_data* record from the set
dam_data * tpc_fee_manager::getNextRecord() const
{
  std::set<dam_data * , damDataSort>::iterator it= TheSet.begin();
  if ( it == TheSet.end()) return NULL;
  
  return (*it);
}

/// delete a particular dam_data* record from the set
int tpc_fee_manager::deleteRecord ( dam_data *dd) 
{
  std::set<dam_data * , damDataSort>::iterator it;

  int c = 0;
  for ( it = TheSet.begin(); it != TheSet.end(); ++it)
   {
     if ( (*it) == dd )
       {
	 TheSet.erase(it);
	 c++;
       }
   }

  return c;
}

int tpc_fee_manager::getSize () const
{
  return TheSet.size();
}

int tpc_fee_manager::getNrChannels() const
{
  return TheCrossing.size();
}

int tpc_fee_manager::inspect_set () const
{

  std::set<dam_data * , damDataSort>::iterator it;
 
  for ( it = TheSet.begin(); it != TheSet.end(); ++it)
   {
      (*it)->identify();

   }
  return 0;

}

int tpc_fee_manager::getNrSamples(const int channel) const
{
  if ( channel < 0 || channel >= MAX_CHANNELS) return 0;

  if ( TheCrossing.count(channel) > 0) // yes we have that channel
    {
      int x =   (*TheCrossing.find(channel)).second->get_nr_samples();
      return x;
    }

  return 0;
}

int tpc_fee_manager::getSampleValue(const int channel, const int sample) const
{
  if ( channel < 0 || channel >= MAX_CHANNELS || sample < 0 ) return 0;

  if ( TheCrossing.count(channel) > 0) // yes we have that channel
    {
      return  (*TheCrossing.find(channel)).second->get_sample(sample);
    }

  return 0;
}

  

