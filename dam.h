#ifndef __DAM_H__
#define __DAM_H__

#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>

int process_event (Event *e); //++CINT 
int traverse_set(const int fee);
int analyze_events();


// this is just a test routine
int nextEvent(const int fee);

#endif /* __DAM_H__ */
