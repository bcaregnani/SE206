#include"LightControl.h"
#include"LightControlSpy.h"

static int lastLightId;
static int lastState;


// Light Control functions

void LightControl_init(void)
{
    lastLightId = LIGHT_ID_UNKNOWN;
    lastState = LIGHT_STATE_UNKNOWN;
    return;
}

void LightControl_destroy(void)
{
    lastLightId = LIGHT_ID_UNKNOWN;
    lastState = LIGHT_STATE_UNKNOWN;
    return;
}

void LightControl_on(int id)
{
    lastLightId = id;
    lastState = LIGHT_ON;
    return;
}

void LightControl_off(int id)
{
    lastLightId = id;
    lastState = LIGHT_OFF;
    return;
}


// Light Control Spy functions

int LightControlSpy_getLastLightId(void)
{
    return lastLightId;
}

int LightControlSpy_getLastState(void)
{
    return lastState;
}