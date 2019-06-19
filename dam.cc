
#include <iostream>
#include <vector>
#include <set>


#include <pmonitor/pmonitor.h>
#include "dam.h"

#include "tpc_fee_manager.h"

#include <TH1.h>
#include <TH2.h>

int init_done = 0;

using namespace std;

//TH1F *h1; 
//TH2F *h2; 

typedef std::set<dam_data * , damDataSort> damset;
typedef damset::iterator dam_iter;

tpc_fee_manager FeeManager[16];                           

#define MAX_FEECHANNELS 256

int analyze_events();


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
  
  
  Packet *p = e->getPacket(3000);
  if (p)
    {

      const int maxfee = p->iValue(0,"MAX_FEECOUNT");
      //const int maxfee = 1; 
      for ( int fee = 0; fee < maxfee; fee++)
	{

	  FeeManager[fee].AddPacket(p, fee);
	}
    } 
  
  delete p;

  if ( e->getEvtType() == DATAEVENT)  analyze_events();
  
  return 0;
}

int inspect_set(const int fee)
{

  FeeManager[fee].inspect_set();
  
  return 0;
}


// this is just a leftover test thing 
int nextEvent(const int fee)
{

  int i = FeeManager[fee].getNextEvent();
  cout << hex << setw(8) << FeeManager[fee].getCurrentBX() << dec << endl;
  cout << "nr of channels " <<  FeeManager[fee].getNrChannels() << dec << endl;
  for (int ch = 0; ch < 256; ch++)
    {
      if ( FeeManager[fee].getNrSamples(ch) )    cout << "channel " << setw (4) << ch << " nr samples " << FeeManager[fee].getNrSamples(ch) << endl;
    }

  
  return i;
}

#define MIN_FILL_AMOUNT (3*256)

int analyze_events()
{
  int fee, ch;
  
  while ( 1)  // we go trough events until we are below the minimum 
    {
      int total_fee_count = 0;
      for ( fee = 0; fee < 16; fee++)
	{
	  if ( FeeManager[fee].getSize() > 0) total_fee_count++; //safeguard against all empty
	  
	  // see if a fee is active but hasn't seen event depth yet
	  // let's take 3 crossings worth as the depth for starters
	  // cout  << "fee " << fee << " size  " << FeeManager[fee].getSize() << endl;
	  if ( FeeManager[fee].getSize() > 0 && FeeManager[fee].getSize() < MIN_FILL_AMOUNT )
	    {
	      return 0;
	    }
	}
      
      //safeguard against all empty
      if ( total_fee_count == 0) return 0;

      // ok, so we have enough depth in all FEEs


      for ( fee = 0; fee < 16; fee++)
    	{
	  if ( FeeManager[fee].getSize() > 0 )  // yes, this one is active
	    {
	      // say we want the next crossing from this fee; status ==1 means we have that
	      // return 1 on sucess means we can do a while (status...) later
	      int status = FeeManager[fee].getNextEvent();  
	      if (status)   // should always be true; we did check the depth before
		{
		  cout << endl << "-----------------  new crossing  FEE " << fee 
		       << " --- BX: " << hex << setw(12) << FeeManager[fee].getCurrentBX() << dec
		       << "  nr of channels " <<  FeeManager[fee].getNrChannels() << dec << endl;
		  for (int ch = 0; ch < 256; ch++)
		    {
		      if ( FeeManager[fee].getNrSamples(ch) )
			{
			  cout << hex << setw(12) << FeeManager[fee].getCurrentBX() << dec << "  channel " << setw (4) << ch
			       << " nr samples " << FeeManager[fee].getNrSamples(ch) << " | ";
			  //  do just 20 samples now to not clutter the screen			  for ( int s = 0; s < FeeManager[fee].getNrSamples(ch) ; s++)
			  for ( int s = 0; s < 20 ; s++)
			    {
			      cout << hex << " " << setw(4) << FeeManager[fee].getSampleValue(ch,s) << dec;
			    }
			  cout << endl;
			    
			}
		    }
		}
	    }
	}
    }
  return 0;
}
