#include "LightScheduler.h"
#include "unity.h"
#include "MockTimeService.h"
#include "LightControlSpy.h"

static int handle;

void setUp(void)
{
    // Mock TimeService
    TimeService_init_Expect();
    TimeService_destroy_Expect();
    TimeService_startPeriodicAlarm_ExpectAndReturn(60, LightScheduler_wakeUP, 0);
    TimeService_stopPeriodicAlarm_Expect(0);

    // Test
    TimeService_init();
    LightControl_init();
    handle = LightScheduler_init();
}

void tearDown(void)
{
    LightScheduler_destroy(handle);
    LightControl_destroy();
    TimeService_destroy();
}



void test_LightScheduler_ScheduleOn(void)
{
    Time time;
    time.dayOfWeek = MONDAY;
    time.minuteOfDay = 1200;
    LightScheduler_ScheduleOn(3, time);

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that LightSchedule_ScheduleOn turns on the light at the right time
    TEST_ASSERT_EQUAL_INT(3, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);
}



void test_LightScheduler_ScheduleOff(void)
{
    Time time;
    time.dayOfWeek = MONDAY;
    time.minuteOfDay = 1200;
    LightScheduler_ScheduleOff(5, time);


    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that LightSchedule_ScheduleOff turns off the light at the right time
    TEST_ASSERT_EQUAL_INT(5, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);
}



void test_LightScheduler_BeforeEvent(void)
{
    Time time;
    time.dayOfWeek = MONDAY;
    time.minuteOfDay = 1200;
    LightScheduler_ScheduleOn(3, time);


    time.minuteOfDay = 1100;
    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that LightSchedule_ScheduleOn turns on the light at the right time
    TEST_ASSERT_EQUAL_INT(LIGHT_ID_UNKNOWN, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_STATE_UNKNOWN, state);
}



void test_LightScheduler_LightIDinRange(void)
{
    Time time;
    time.dayOfWeek = MONDAY;

    for (int i = 0; i < 256; i++)
    {
        time.minuteOfDay = 1200 + i;
        TEST_ASSERT_EQUAL_INT(0, LightScheduler_ScheduleOff(i, time));
    }
}


void test_LightSchedule_LightIDoutOfRange(void)
{
    Time time;
    time.dayOfWeek = MONDAY;
    time.minuteOfDay = 1200;

    TEST_ASSERT_EQUAL_INT(-1, LightScheduler_ScheduleOff(257, time));
    TEST_ASSERT_EQUAL_INT(-1, LightScheduler_ScheduleOn(-10, time));

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that LightSchedule_ScheduleOn turns on the light at the right time
    TEST_ASSERT_EQUAL_INT(LIGHT_ID_UNKNOWN, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_STATE_UNKNOWN, state);
}



void test_LightScheduler_SetMultipleEvents(void)
{
    // Schedule two events

    Time time;

    time.dayOfWeek = MONDAY;
    time.minuteOfDay = 100;
    LightScheduler_ScheduleOn(10, time);

    time.dayOfWeek = SATURDAY;
    time.minuteOfDay = 500;
    LightScheduler_ScheduleOff(50, time);


    // Check that event on Saturday works

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    TEST_ASSERT_EQUAL_INT(50, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);



    // Check that event on Monday works

    time.dayOfWeek = MONDAY;
    time.minuteOfDay = 100;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    id = LightControlSpy_getLastLightId();
    state = LightControlSpy_getLastState();

    TEST_ASSERT_EQUAL_INT(10, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);
}


void test_LightScheduler_SetAlarmEveryDay(void)
{
    Time time;
    time.dayOfWeek = EVERYDAY;
    time.minuteOfDay = 1200;
    LightScheduler_ScheduleOn(10, time);

    time.minuteOfDay = 1500;
    LightScheduler_ScheduleOff(10, time);

    int id;
    int state;

    for (int day = MONDAY; day <= SUNDAY; day++)
    {
        time.dayOfWeek = day;
        time.minuteOfDay = 1200;

        // Mock TimeService
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&time);
        LightScheduler_wakeUP();

        id = LightControlSpy_getLastLightId();
        state = LightControlSpy_getLastState();

        // Check that LightSchedule_ScheduleOn turns on the light at the right time
        TEST_ASSERT_EQUAL_INT(10, id);
        TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);


        time.minuteOfDay = 1500;

        // Mock TimeService
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&time);
        LightScheduler_wakeUP();

        id = LightControlSpy_getLastLightId();
        state = LightControlSpy_getLastState();

        // Check that LightSchedule_ScheduleOn turns off the light at the right time
        TEST_ASSERT_EQUAL_INT(10, id);
        TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);
    }
}




void test_LightScheduler_SetAlarmEveryDay_NotEnoughSpace(void){
    Time time;
    time.dayOfWeek = THURDSDAY;

    int out;

    for (int i = 0; i < 250; i++){
        time.minuteOfDay = 10 + 2*i;
        LightScheduler_ScheduleOn(5, time);
    }

    time.dayOfWeek = EVERYDAY;
    time.minuteOfDay = 1500;
    out = LightScheduler_ScheduleOn(20, time);

    TEST_ASSERT_EQUAL_INT(-1, out);
}



void test_LightScheduler_SetAlarmEveryDayOfWeek(void)
{
    Time time;
    time.dayOfWeek = WEEKDAY;
    time.minuteOfDay = 1200;
    LightScheduler_ScheduleOff(15, time);

    int id;
    int state;

    for (int day = MONDAY; day <= FRIDAY; day++)
    {
        time.dayOfWeek = day;
        time.minuteOfDay = 1200;

        // Mock TimeService
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&time);
        LightScheduler_wakeUP();

        id = LightControlSpy_getLastLightId();
        state = LightControlSpy_getLastState();

        // Check that light are ok
        TEST_ASSERT_EQUAL_INT(15, id);
        TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);

    }

}


void test_LightScheduler_SetAlarmWeekend(void)
{
    Time time;
    time.dayOfWeek = WEEKEND;
    time.minuteOfDay = 120;
    LightScheduler_ScheduleOn(5, time);

    int id;
    int state;

    for (int day = SATURDAY; day <= SUNDAY; day++)
    {
        time.dayOfWeek = day;

        // Mock TimeService
        TimeService_getTime_ExpectAnyArgs();
        TimeService_getTime_ReturnThruPtr_time(&time);
        LightScheduler_wakeUP();

        id = LightControlSpy_getLastLightId();
        state = LightControlSpy_getLastState();

        // Check that lights are ok
        TEST_ASSERT_EQUAL_INT(5, id);
        TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);
    }
}



void test_LightScheduler_SetAlarmMultipleForSameLight(void)
{
    Time time;

    time.dayOfWeek = FRIDAY;
    time.minuteOfDay = 120;
    LightScheduler_ScheduleOn(5, time);

    time.minuteOfDay = 150;
    LightScheduler_ScheduleOff(5, time);

    // Turn on
    time.minuteOfDay = 120;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that the light is on
    TEST_ASSERT_EQUAL_INT(5, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);


    // Turn off
    time.minuteOfDay = 150;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    id = LightControlSpy_getLastLightId();
    state = LightControlSpy_getLastState();

    // Check that light is off
    TEST_ASSERT_EQUAL_INT(5, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);

}


void test_LightScheduler_RemoveAlarm(void)
{
    Time time;
    time.dayOfWeek = THURDSDAY;
    time.minuteOfDay = 120;
    LightScheduler_ScheduleOn(5, time);
    LightScheduler_RemoveSchedule(5, time);

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that LightSchedule_ScheduleOn turns on the light at the right time
    TEST_ASSERT_EQUAL_INT(LIGHT_ID_UNKNOWN, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_STATE_UNKNOWN, state);

}


void test_LightScheduler_RemoveAlarmWrongId(void)
{
    Time time;
    time.dayOfWeek = THURDSDAY;
    time.minuteOfDay = 120;
    LightScheduler_ScheduleOn(5, time);

    int out = LightScheduler_RemoveSchedule(-10, time);

    TEST_ASSERT_EQUAL_INT(-1, out);

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that the alarm is still there
    TEST_ASSERT_EQUAL_INT(5, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);
}


void test_LightScheduler_RemoveAlarmWrongTime(void){
    Time time;
    time.dayOfWeek = THURDSDAY;
    time.minuteOfDay = 120;
    LightScheduler_ScheduleOff(10, time);

    // Remove alarm at wrong time
    time.minuteOfDay = 130;
    LightScheduler_RemoveSchedule(10, time);

    time.minuteOfDay = 120;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    int id = LightControlSpy_getLastLightId();
    int state = LightControlSpy_getLastState();

    // Check that light is off
    TEST_ASSERT_EQUAL_INT(10, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);
}


void test_LightScheduler_RemoveAlarmFirstScheduled(void){
    Time time;
    time.dayOfWeek = THURDSDAY;

    int out;

    for (int i = 0; i < 256; i++){
        time.minuteOfDay = 10 + 2*i;
        LightScheduler_ScheduleOn(5, time);
    }

    time.minuteOfDay = 12;
    out = LightScheduler_RemoveSchedule(5, time);

    TEST_ASSERT_EQUAL_INT(0, out);
}


void test_LightScheduler_ScheduleMultipleEvents(void){
    Time time;
    time.dayOfWeek = THURDSDAY;

    int scheduled;

    for (int i = 0; i < 256; i++){
        time.minuteOfDay = 10 + 2*i;
        scheduled = LightScheduler_ScheduleOn(5, time);

        TEST_ASSERT_EQUAL_INT(0, scheduled); // Scheduled OK
    }
    
    time.minuteOfDay = 1000;
    scheduled = LightScheduler_ScheduleOff(10, time);

    TEST_ASSERT_EQUAL_INT(-1, scheduled); // Schedule failed
}


void test_LightScheduler_LightRetainState(void){
    Time time;
    int id;
    int state;
    time.dayOfWeek = THURDSDAY;
    time.minuteOfDay = 120;
    LightScheduler_ScheduleOn(5, time);

    time.minuteOfDay = 150;
    LightScheduler_ScheduleOff(5, time);

    // Turn on
    time.minuteOfDay = 120;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();


    // Check that light is still on
    time.minuteOfDay = 130;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();
    id = LightControlSpy_getLastLightId();
    state = LightControlSpy_getLastState();
    TEST_ASSERT_EQUAL_INT(5, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_ON, state);

    // Turn off
    time.minuteOfDay = 150;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    // Check that light is still off
    time.minuteOfDay = 170;

    // Mock TimeService
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_wakeUP();

    id = LightControlSpy_getLastLightId();
    state = LightControlSpy_getLastState();
    TEST_ASSERT_EQUAL_INT(5, id);
    TEST_ASSERT_EQUAL_INT(LIGHT_OFF, state);
}