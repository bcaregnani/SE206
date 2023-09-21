# Report for TDD Exercise

This is the place where you can put your test lists, as well as any additional
explanation or discussion with respect to your implementation.


## TestLightControlSpy:

- testLightControlSpyReturnsLastStateChange:

Test LightControlSpy functions LightControlSpy_getLastLightId and LightControlSpy_getLastState.
Test LightControl function LightControl_on.


- test_State_And_Light_ID_unknown_AfterInit:

Test LightControl functions LightControl_init.


- test_LightControl_off_changes_state

Test LightControl function LightControl_destroy.


- test_LightControl_destroy_changes_state

Test LightControl function LightControl_off(int id).



## TestLightScheduler

# Requirements


0. General Settings.

- setUp

init the TimeService
init the LightScheduler
init the LightControl

- tearDown

destroy the LightControl
destroy the LightScheduler
destroy the TimeService


1. The LightScheduler allows to schedule the switching of room lights (on or off) every week on a specific day at a specific time (hour and minute).


- test_LightScheduler_ScheduleOn:
schedule the switch on every week on some day and some time
check that it turns on in specific day and time

- test_LightScheduler_ScheduleOff:
schedule the switch on every week off some day and some time
check that it turns off in specific day and time

- test_LightScheduler_BeforeEvent:
schedule the switch on every week on some day and some time
check that light is unchanged before the event

2. Each light is identified by an integer between 0 and 255.

- test_LightScheduler_LightIDinRange
schedule a light to switch on/off, with id in range
check that schedule is done

- test_LightSchedule_LightIDoutOfRange
schedule a light to switch on/off, with id out of range
check that schedule fails


3. The user can schedule switching a light on or off every day at a specific time.


4. If an event is scheduled, it will be triggered only on the specified day and within one minute around the specified time.

- test_LightScheduler_AlarmWorksFine
schedule a light to switch on/off
check that the light is on (or off) in specific time and off (or on) otherwise


5. The user can schedule multiple events.

- test_LightScheduler_SetMultipleEvents
schedule two events on two different lights
check that both light accomplish the objective
 

6. The user can schedule switching a light on or off every day at a specific time.

- test_LightScheduler_SetAlarmEveryDay:
schedule the switch on/off every day
check that we get the response from the TimeService

- test_LightScheduler_SetAlarmEveryDay_NotEnoughSpace
schedule 250 events
schedule the switch on/off every day
check that we fail to schedule


7. The user can schedule switching a light on or off on a specific day of the week at a specific time.

- test_LightScheduler_SetAlarmEveryDayOfWeek:
schedule the switch on/off every day of the week
check that we get the response from the TimeService
 

8. The user can schedule switching a light on or off during weekends (Saturday and Sunday).

- test_LightScheduler_SetAlarmWeekend:
schedule the switch on/off on weekend
check that we get the response from the TimeService
 

9. The user can schedule switching a light on or off during week days (Monday till Friday).

<!-- Already tested through the different tests -->

- test_LightScheduler_SetAlarmSomeWeekDay
schedule the switch on/off some day on during the week
check that we get the response from the TimeService
 

10. The user can schedule multiple events for the same light.

- test_LightScheduler_SetAlarmMultipleForSameLight
schedule the switch on/off for a specific light
schedule another switch on/off for the same light
check that we get the response from the TimeService on both calls

 
11. The user can remove a scheduled event.

- test_LightScheduler_RemoveAlarm
schedule an alarm the switch on/off of a light
remove schedule alarm
check that we get the response from the TimeService

- test_LightScheduler_RemoveAlarmWrongId
schedule an alarm the switch on/off of a light
try remove schedule alarm with wrong id
check that the scheduled alarm was not affected

- test_LightScheduler_RemoveAlarmWrongTime
schedule an alarm the switch on/off of a light
try remove schedule alarm in wrong time
check that the scheduled alarm was not affected

- test_LightScheduler_RemoveAlarmFirstScheduled
schedule 256 alarm the switch on/off of a light 
remove first scheduled
check that the scheduled alarm was removed

 

12. The user can schedule a maximum number of 256 events.

- test_LightScheduler_ScheduleMultipleEvents
schedule 257 events
check that we get right response from the TimeService 256 times, but the last schedule fails


13. The light remain on/off after the event is activated.

- test_LightScheduler_LightRetainState
schedule one event
check that it is still the expected state some time later
