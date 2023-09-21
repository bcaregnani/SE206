#ifndef LIGHT_SCHEDULER_H
#define LIGHT_SCHEDULER_H

#include "LightControl.h"
#include "TimeService.h"

// TODO: Define the light scheduler interface here

typedef struct {
    Time time;
    int lightID;
    int action; // 1 for turn on, 0 for turn off
} Event;

int LightScheduler_init(void);
void LightScheduler_destroy(int handle);
void LightScheduler_wakeUP(void);
int LightScheduler_ScheduleOn(int lightID, Time time);
int LightScheduler_ScheduleOff(int lightID, Time time);
int LightScheduler_RemoveSchedule(int lightID, Time time);

int lightIDinRange_EventInRange(int id);
int scheduleMoreThanOneDay(int lightID, Time time, int action);


#endif