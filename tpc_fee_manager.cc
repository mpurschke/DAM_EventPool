#include "tcp_fee_manager.h"

tpc_fee_manager::tpc_fee_manager()
{
  for ( int i = 0; i < MAX_CHANNELS; i++)
    {
      max_bx[i] = 0;
      rollover[i] = 0;
    }
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
	      TheSet[fee].insert(dd);
	      
	      dd->identify();
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
	  TheSet[fee].insert(dd);
	  dd->identify();
	  sample_index = 0;
	}
      
    }
}


