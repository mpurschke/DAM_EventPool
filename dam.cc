
#include <iostream>
#include <vector>
#include <set>


#include <pmonitor/pmonitor.h>
#include "dam.h"

#include "dam_data.h"

#include <TH1.h>
#include <TH2.h>

int init_done = 0;

using namespace std;

//TH1F *h1; 
//TH2F *h2; 

typedef std::set<dam_data * , damDataSort> damset;
typedef damset::iterator dam_iter;

damset TheSet[16];                                       

#define MAX_FEECHANNELS 256


int pinit()
{

  if (init_done) return 1;
  init_done = 1;

  // h1 = new TH1F ( "h1","test histogram", 400, -50, 50); 
  // h2 = new TH2F ( "h2","test histogram 2D", 100, -50.5, 49.5, 100, -500, 500);  

  return 0;

}

int process_event (Event * e)
{

  unsigned short sample_vector[10240];
  unsigned int length;
  unsigned int bx;
  
  static unsigned int bx_offset[16] = {0};
  
  Packet *p = e->getPacket(3000);
  if (p)
    {

      cout << "Offset " << bx_offset[0] << endl;
      

      //      const int maxfee = p->iValue(0,"MAX_FEECOUNT");
      const int maxfee = 1; 
      for ( int fee = 0; fee < maxfee; fee++)
	{

	  if ( p->iValue(fee, "IS_PRESENT"))
	    {
	      
	      for ( int ch = 0; ch < MAX_FEECHANNELS; ch++)
		{
		  int sample_index = 0;
		  
		  // get the lowest bx from this channel
		  bx = (unsigned int) p->iValue(fee,ch,0,"BXRAW");  // note we are working here with the local BX number

		  // now we go through all samples of this channel
		  for (int s = 0; s < p->iValue(fee,ch, "NR_SAMPLES"); s++ )
		    {
		      if ( (unsigned int) p->iValue(fee,ch,s,"BXRAW") != bx)
			{
			  dam_data *dd = new dam_data( bx  + bx_offset[fee], fee, ch, sample_vector, sample_index);
			  //  cout << TheSet[fee].size() << "  ";
			  TheSet[fee].insert(dd);
			  //cout << dd->get_bx() << endl;
			  
			  dd->identify();
			  //delete dd;
			  sample_index = 0;
			  bx =  (unsigned int) p->iValue(fee,ch,s,"BXRAW");
			}
		      sample_vector[sample_index++] = p->iValue(fee,ch,s);
		    }
		  if ( sample_index > 0)
		    {
		      dam_data * dd = new dam_data( bx  + bx_offset[fee], fee, ch, sample_vector, sample_index);
		      TheSet[fee].insert(dd);
		      //cout << TheSet[fee].size() << "  ";
		      dd->identify();
		      //cout << dd->get_bx() << endl;
		      //delete dd;
		      sample_index = 0;
		    }
	      
		}
	    }
	  //	  bx_offset[fee] = p->iValue(fee, "HIGHEST_BX") + 1000;
	}
    }  // end if {p}
  
  delete p;
  
  return 0;
}

int traverse_set(const int fee)
{
  dam_iter it;

  for ( it = TheSet[fee].begin(); it != TheSet[fee].end(); ++it)
    {
      //      cout << setw(6) << (*it)->identify()  << "  size " << TheSet[fee].size() << endl;
      //cout <<  "  size " << TheSet[fee].size()  << " pointer " << (*it) << "  ";
      (*it)->identify();
      // TheSet[fee].erase(it);
    }
  return 0;
}

int test_insert()
{
  
  unsigned int bx = 0xfff;
  unsigned short fee = 0;
  unsigned short ch = 10;
  
  unsigned short sample_vector[] = {1,2,3,4,5,6,7,8,9,10};
  
  dam_data * dd = new dam_data( bx, fee, ch, sample_vector, 10);
  TheSet[fee].insert(dd);
  cout << " Pointer " << dd << endl;
  dd->identify();
  
  return 0;
}


  

