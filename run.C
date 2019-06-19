#include "dam.h"

R__LOAD_LIBRARY(libdam.so)

//int run(const char *filename = "/mac_home/data/SAMPA/tpcfee-00000039-0000.evt")
//int run(const char *filename = "/mac_home/data/SAMPA/tpc_beam_00000274-0000.evt")

int run(const char *filename = "$DATA/tpc_beam_00000274-0000.evt")
{
  pidentify(0);  // identify all (rcdaq) events
  
  pfileopen(filename);
  prun(5);
  return 0;
}

