#include "LightControlSpy.h" 
#include "unity.h" 

void testLightControlSpyReturnsLastStateChange(void) 
{ 
    LightControl_init(); 
    LightControl_on(42); 
    
    TEST_ASSERT_EQUAL( 42, LightControlSpy_getLastLightId() ); 
    TEST_ASSERT_EQUAL( LIGHT_ON, LightControlSpy_getLastState() ); 

    LightControl_destroy();
}

void test_State_And_Light_ID_unknown_AfterInit(void)
{
    LightControl_init();

    TEST_ASSERT_EQUAL_INT( LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId() );
    TEST_ASSERT_EQUAL_INT( LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState() );

    LightControl_destroy();
}

void test_LightControl_off_changes_state(void)
{
    LightControl_init();
    LightControl_off(33);
    
    TEST_ASSERT_EQUAL_INT( 33, LightControlSpy_getLastLightId() );
    TEST_ASSERT_EQUAL_INT( LIGHT_OFF, LightControlSpy_getLastState() );

    LightControl_destroy();
}

void test_LightControl_destroy_changes_state(void)
{
    LightControl_init();

    LightControl_on(1);

    LightControl_destroy();

    TEST_ASSERT_EQUAL_INT( LIGHT_ID_UNKNOWN, LightControlSpy_getLastLightId() );
    TEST_ASSERT_EQUAL_INT( LIGHT_STATE_UNKNOWN, LightControlSpy_getLastState() );
}