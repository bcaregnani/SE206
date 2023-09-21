#include "LightScheduler.h"
#include "LightControlSpy.h"

#define MAX_EVENTS 256
static Event event[MAX_EVENTS];
static int ne; // number of events

int LightScheduler_init(void)
{
    int handle = TimeService_startPeriodicAlarm(60, LightScheduler_wakeUP );
    ne = 0;
    for (int i = 0; i < MAX_EVENTS; i++)
    {
        event[i].time.dayOfWeek = NONE;
        event[i].time.minuteOfDay = -1;
        event[i].lightID = LIGHT_ID_UNKNOWN;
        event[i].action = -1;
    };
    return handle;
};

void LightScheduler_destroy(int handle)
{
    TimeService_stopPeriodicAlarm(handle);
    return;
};

void LightScheduler_wakeUP()
{   
    Time actual_time;
    TimeService_getTime(&actual_time);

    // Cover all the events
    for (int i = 0; i < ne; i++)
    {
        // Check if the actual time is the same as an scheduled event
        if ( (actual_time.dayOfWeek == event[i].time.dayOfWeek) & (actual_time.minuteOfDay == event[i].time.minuteOfDay) )
        {
            if (event[i].action){
                LightControl_on(event[i].lightID);
            }
            else{
                LightControl_off(event[i].lightID);
            }
        }
    }

    return;
}

int LightScheduler_ScheduleOn(int lightID, Time time){
    if (lightIDinRange_EventInRange(lightID)<0){
        return -1; 
        };
    if ( (time.dayOfWeek == EVERYDAY) | (time.dayOfWeek == WEEKEND) | (time.dayOfWeek == WEEKDAY) ){
        int scheduled = scheduleMoreThanOneDay(lightID, time, 1);
        return scheduled;
    }
    else{
        event[ne].time.dayOfWeek = time.dayOfWeek;
        event[ne].time.minuteOfDay = time.minuteOfDay;
        event[ne].lightID = lightID;
        event[ne].action = 1;
        ne += 1;
    }
    return 0;
}

int LightScheduler_ScheduleOff(int lightID, Time time){
    if (lightIDinRange_EventInRange(lightID)<0){
        return -1;
    };
    if ( (time.dayOfWeek == EVERYDAY) | (time.dayOfWeek == WEEKEND) | (time.dayOfWeek == WEEKDAY) ){
        int scheduled = scheduleMoreThanOneDay(lightID, time, 0);
        return scheduled;
    }
    else{
        event[ne].time.dayOfWeek = time.dayOfWeek;
        event[ne].time.minuteOfDay = time.minuteOfDay;
        event[ne].lightID = lightID;
        event[ne].action = 0;
        ne += 1;
    }
    return 0;
}

int LightScheduler_RemoveSchedule(int lightID, Time time){
    if ((lightID < 0) | (lightID > 256)){
        return -1;
    };
    // Check if the event is in the list, loop over the array
    for (int i = 0; i < ne; i++)
    {
        if ( (event[i].lightID == lightID) & (event[i].time.dayOfWeek == time.dayOfWeek) & (event[i].time.minuteOfDay == time.minuteOfDay) )
        {
            for (int j = i; j < ne-1; j++)
            {
                event[j].time.dayOfWeek = event[j+1].time.dayOfWeek;
                event[j].time.minuteOfDay = event[j+1].time.minuteOfDay;
                event[j].lightID = event[j+1].lightID;
                event[j].action = event[j+1].action;
            }
            ne -= 1;
            event[ne].time.dayOfWeek = NONE;
            event[ne].time.minuteOfDay = -1;
            event[ne].lightID = LIGHT_ID_UNKNOWN;
            event[ne].action = LIGHT_STATE_UNKNOWN;
        }
    }
    return 0;
}




int lightIDinRange_EventInRange(int id)
{
    if ( (id >= 0) & (id <= 256) & ( ne < MAX_EVENTS) ){
            return 0;
    }
    else{
        return -1;
    }
}


// Function helper to schedule all weekday events,
// for the weekend, for everyday
int scheduleMoreThanOneDay(int lightID, Time time, int action)
{
    if ((time.dayOfWeek == EVERYDAY) & (ne + 7 <= MAX_EVENTS)){
        for (int day = MONDAY; day <= SUNDAY; day++){
            event[ne].time.dayOfWeek = day ;
            event[ne].time.minuteOfDay = time.minuteOfDay;
            event[ne].lightID = lightID;
            event[ne].action = action;
            ne += 1;
        }
    }
    else if ((time.dayOfWeek == WEEKDAY) & (ne + 5 <= MAX_EVENTS)){
        for (int day = MONDAY; day <= FRIDAY; day++)
        {
            event[ne].time.dayOfWeek = day ;
            event[ne].time.minuteOfDay = time.minuteOfDay;
            event[ne].lightID = lightID;
            event[ne].action = action;
            ne += 1;
        }
    }
    else if ((time.dayOfWeek == WEEKEND) & (ne + 2 <= MAX_EVENTS)){
        for (int day = SATURDAY; day <= SUNDAY; day++)
        {
            event[ne].time.dayOfWeek = day ;
            event[ne].time.minuteOfDay = time.minuteOfDay;
            event[ne].lightID = lightID;
            event[ne].action = action;
            ne += 1;
        }
    }
    else{
        return -1; // Not enough space in the array
    }
    
    return 0;
}