#include "MockTimeService.h"
#include "LightControlSpy.h"
#include "LightScheduler.h"
#include "unity.h"

#define HANDLER 0

const char* getErrorName(int errorCode)
{
    switch (errorCode)
    {
        case GENERIC_ERROR:
            return "GENERIC_ERROR";
        case SCHEDULER_FULL:
            return "SCHEDULER_FULL";
        case SCHEDULER_EMPTY:
            return "SCHEDULER_EMPTY";
        case INVALID_TIME:
            return "INVALID_TIME";
        case INVALID_SCHEDULE:
            return "INVALID_SCHEDULE";
        case LIGHT_ID_OUT_OF_RANGE:
            return "LIGHT_ID_OUT_OF_RANGE";
        default:
            return "UNKNOWN_ERROR";
    }
}

const char* printError(int errorCode, char* message)
{
    static char errorMessage[256]; // Static to return safely
    snprintf(errorMessage, sizeof(errorMessage), "Error %s occurred during: %s", getErrorName(errorCode), message);
    return errorMessage;
}

void setUp(void)
{
    TimeService_init_Expect();
    TimeService_startPeriodicAlarm_ExpectAndReturn(60, LightScheduler_Wakeup, HANDLER);
    LightScheduler_Init();
}

void tearDown(void)
{
    TimeService_destroy_Expect();
    TimeService_stopPeriodicAlarm_Expect(HANDLER);
    LightScheduler_Destroy();
}

void wakeUp(Time time)
{
    TimeService_getTime_ExpectAnyArgs();
    TimeService_getTime_ReturnThruPtr_time(&time);
    LightScheduler_Wakeup();
}

void testScheduleLightOnSpecificDayAndTime(void)
{
    Time time = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test = { .dayOfWeek = SUNDAY, .minuteOfDay = 1000 };
    
    int result = scheduleTurnOn(3, time);
    wakeUp(time_test);
    wakeUp(time);

    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light on during specific day and time"));
    TEST_ASSERT_EQUAL(3, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleLightOffSpecificDayAndTime(void)
{
    Time time = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test = { .dayOfWeek = SUNDAY, .minuteOfDay = 1000 };
    
    int result = scheduleTurnOff(1, time);
    wakeUp(time_test);
    wakeUp(time);

    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light off during specific day and time"));
    TEST_ASSERT_EQUAL(1, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
}

void testScheduleLightOnEveryday(void)
{
    Time time = { .dayOfWeek = SUNDAY, .minuteOfDay = 1200 };
    Time time_test_0 = { .dayOfWeek = MONDAY, .minuteOfDay = 1400 };
    Time time_test_1 = { .dayOfWeek = THURDSDAY, .minuteOfDay = 1400 };
    
    int result = scheduleTurnOnEveryDay(2, 1400);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light on during everyday"));

    wakeUp(time);
    TEST_ASSERT_NOT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
    
    wakeUp(time_test_0);
    TEST_ASSERT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());

    LightControl_destroy();

    wakeUp(time_test_1);
    TEST_ASSERT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleLightOffEveryday(void)
{
    Time time = { .dayOfWeek = SUNDAY, .minuteOfDay = 1200 };
    Time time_test_0 = { .dayOfWeek = MONDAY, .minuteOfDay = 600 };
    Time time_test_1 = { .dayOfWeek = THURDSDAY, .minuteOfDay = 600 };
    
    int result = scheduleTurnOffEveryDay(2, 600);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light off during everyday"));

    wakeUp(time);
    TEST_ASSERT_NOT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
    
    wakeUp(time_test_0);
    TEST_ASSERT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());

    LightControl_destroy();

    wakeUp(time_test_1);
    TEST_ASSERT_EQUAL(2, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
}

void testScheduleLightOnWeekend(void)
{
    Time time = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_0 = { .dayOfWeek = SATURDAY, .minuteOfDay = 800 };
    Time time_test_1 = { .dayOfWeek = SUNDAY, .minuteOfDay = 800 };
    
    int result = scheduleTurnOnWeekend(4, 800);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light on during weekend"));

    wakeUp(time);
    TEST_ASSERT_NOT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
    
    wakeUp(time_test_0);
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());

    LightControl_destroy();

    wakeUp(time_test_1);
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleLightOffWeekend(void)
{
    Time time = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_0 = { .dayOfWeek = SATURDAY, .minuteOfDay = 300 };
    Time time_test_1 = { .dayOfWeek = SUNDAY, .minuteOfDay = 300 };
    
    int result = scheduleTurnOffWeekend(4, 300);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light off during weekend"));
    
    wakeUp(time);
    TEST_ASSERT_NOT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
    
    wakeUp(time_test_0);
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());

    LightControl_destroy();

    wakeUp(time_test_1);
    TEST_ASSERT_EQUAL(4, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
}

void testScheduleLightOnWeekday(void)
{
    Time time_test_0 = { .dayOfWeek = SATURDAY, .minuteOfDay = 1200 };
    Time time_test_1 = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_2 = { .dayOfWeek = THURDSDAY, .minuteOfDay = 900 };
    
    int result = scheduleTurnOnWeekday(5, 900);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light on during weekday"));

    wakeUp(time_test_0);
    TEST_ASSERT_NOT_EQUAL(5, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
    
    wakeUp(time_test_1);
    TEST_ASSERT_NOT_EQUAL(5, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());

    wakeUp(time_test_2);
    TEST_ASSERT_EQUAL(5, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleLightOffWeekday(void)
{
    Time time_test_0 = { .dayOfWeek = SATURDAY, .minuteOfDay = 1200 };
    Time time_test_1 = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_2 = { .dayOfWeek = THURDSDAY, .minuteOfDay = 1000 };
    
    int result = scheduleTurnOffWeekday(5, 1000);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light off during weekday"));

    wakeUp(time_test_0);
    TEST_ASSERT_NOT_EQUAL(5, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
    
    wakeUp(time_test_1);
    TEST_ASSERT_NOT_EQUAL(5, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());

    wakeUp(time_test_2);
    TEST_ASSERT_EQUAL(5, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());
}

void testScheduleMultipleEventsForSameLight(void)
{
    Time time_test_0 = { .dayOfWeek = SATURDAY, .minuteOfDay = 1200 };
    Time time_test_1 = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_2 = { .dayOfWeek = THURDSDAY, .minuteOfDay = 500 };
    int result;

    result = scheduleTurnOn(6, time_test_1);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light on during multiple events"));
    
    result = scheduleTurnOff(6, time_test_2);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "turning the light off during multiple events"));

    wakeUp(time_test_0);
    TEST_ASSERT_NOT_EQUAL(6, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());

    wakeUp(time_test_1);
    TEST_ASSERT_EQUAL(6, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());

    LightControl_destroy();

    wakeUp(time_test_2);
    TEST_ASSERT_EQUAL(6, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_OFF, LightControlSpy_getLastState());   
}

void testRemoveSchedule(void)
{
    Time time_test_0 = { .dayOfWeek = SATURDAY, .minuteOfDay = 1200 };
    int result;

    result = scheduleTurnOn(7, time_test_0);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "adding the schedule to remove it later"));
    
    wakeUp(time_test_0);
    TEST_ASSERT_EQUAL(7, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());

    LightControl_destroy();

    result = removeSchedule(7, time_test_0);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "removing the schedule"));

    wakeUp(time_test_0);
    TEST_ASSERT_NOT_EQUAL(7, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleTooManyEvents(void)
{
    Time time_test_0 = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_1 = { .dayOfWeek = WEDNESDAY, .minuteOfDay = 200 };
    int result;
    
    for (int i = 0; i < MaxEvents; i++)
    {
        result = scheduleTurnOn(i, time_test_0);
        TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "adding schedules"));
    }

    result = scheduleTurnOn(10, time_test_1);
    TEST_ASSERT_EQUAL_MESSAGE(SCHEDULER_FULL, result, printError(result, "adding schedule when scheduler is full"));
    
    wakeUp(time_test_1);

    TEST_ASSERT_NOT_EQUAL(MaxEvents, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleInvalidLightId(void)
{
    Time time_test_0 = { .dayOfWeek = MONDAY, .minuteOfDay = 1200 };
    Time time_test_1 = { .dayOfWeek = WEDNESDAY, .minuteOfDay = 200 };
    int result;

    result = scheduleTurnOn(-5, time_test_0);
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ID_OUT_OF_RANGE, result, printError(result, "adding schedule with invalid light id"));

    result = scheduleTurnOn(256, time_test_1);
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ID_OUT_OF_RANGE, result, printError(result, "adding schedule with invalid light id"));
    
    wakeUp(time_test_0);
    wakeUp(time_test_1);

    TEST_ASSERT_NOT_EQUAL(-5, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(256, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testScheduleInvalidTime(void)
{
    Time time_test_0 = { .dayOfWeek = MONDAY, .minuteOfDay = -1 };
    Time time_test_1 = { .dayOfWeek = WEDNESDAY, .minuteOfDay = 5000 };
    int result;

    result = scheduleTurnOn(8, time_test_0);
    TEST_ASSERT_EQUAL_MESSAGE(INVALID_TIME, result, printError(result, "adding schedule with invalid time"));

    result = scheduleTurnOn(9, time_test_1);
    TEST_ASSERT_EQUAL_MESSAGE(INVALID_TIME, result, printError(result, "adding schedule with invalid time"));
    
    wakeUp(time_test_0);
    wakeUp(time_test_1);

    TEST_ASSERT_NOT_EQUAL(8, LightControlSpy_getLastLightId());
    TEST_ASSERT_NOT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}

void testRemoveNonExistingScheduledEvent(void)
{
    Time time_test_0 = { .dayOfWeek = WEDNESDAY, .minuteOfDay = 1200 };
    Time time_test_1 = { .dayOfWeek = TUESDAY, .minuteOfDay = 300 };
    int result;

    result = removeSchedule(10, time_test_0);
    TEST_ASSERT_EQUAL_MESSAGE(SCHEDULER_EMPTY, result, printError(result, "removing schedule with empty scheduler"));

    result = scheduleTurnOn(10, time_test_0);
    TEST_ASSERT_EQUAL_MESSAGE(OK, result, printError(result, "adding schedule to remove it later"));

    result = removeSchedule(10, time_test_1);
    TEST_ASSERT_EQUAL_MESSAGE(INVALID_SCHEDULE, result, printError(result, "removing schedule with non-existing event"));

    wakeUp(time_test_0);
    TEST_ASSERT_EQUAL(10, LightControlSpy_getLastLightId());
    TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
}