#include "LightScheduler.h"

#define MAX_LIGHT_ID 255
#define MINUTES_IN_A_DAY 1440

LightSchedulerEvents events[MaxEvents];
int eventIndex = 0;
int handle;

bool isEventDayMatch(int eventDay, int currentDay);
bool isEventTimeMatch(int eventMinute, int currentMinute);
int scheduleEvent(int id, Time time, EventState state);
void resetLightSchedulerEvents(void);

void LightScheduler_Init(void)
{
    LightControl_init();
    TimeService_init();
    handle = TimeService_startPeriodicAlarm(60, LightScheduler_Wakeup);
    resetLightSchedulerEvents();
}

void LightScheduler_Destroy(void)
{
    resetLightSchedulerEvents();
    TimeService_stopPeriodicAlarm(handle);
    LightControl_destroy();
    TimeService_destroy();
}

void LightScheduler_Wakeup(void)
{
    Time time;
    TimeService_getTime(&time);
    for (int i = 0; i < MaxEvents; i++)
    {
        if (events[i].state != UNUSED)
        {
            if (isEventDayMatch(events[i].time.dayOfWeek, time.dayOfWeek) &&
                isEventTimeMatch(events[i].time.minuteOfDay, time.minuteOfDay))
            {
                if (events[i].state == TURN_ON)
                    LightControl_on(events[i].lightId);
                else if (events[i].state == TURN_OFF)
                    LightControl_off(events[i].lightId);
            }
        }
    }
}

int LightScheduler_CreateEvent(int id, Time time, EventState state)
{
    if (id < 0 || id > MAX_LIGHT_ID)
        return LIGHT_ID_OUT_OF_RANGE;

    if (time.minuteOfDay < 0 || time.minuteOfDay > MINUTES_IN_A_DAY)
        return INVALID_TIME;

    for (int i = 0; i < MaxEvents; i++)
    {
        if (events[i].state == UNUSED)
        {
            events[i].lightId = id;
            events[i].time = time;
            events[i].state = state;
            eventIndex++;
            return OK;
        }
    }
    return SCHEDULER_FULL;
}

int LightScheduler_RemoveEvent(int id, Time time)
{
    if (eventIndex == 0)
        return SCHEDULER_EMPTY;

    for (int i = 0; i < MaxEvents; i++)
    {
        if (events[i].lightId == id &&
            events[i].time.dayOfWeek == time.dayOfWeek &&
            events[i].time.minuteOfDay == time.minuteOfDay)
        {
            events[i].state = UNUSED;
            eventIndex--;
            return OK;
        }
    }
    return INVALID_SCHEDULE;
}

int removeSchedule(int id, Time time)
{
    return LightScheduler_RemoveEvent(id, time);
}

int scheduleEvent(int id, Time time, EventState state)
{
    return LightScheduler_CreateEvent(id, time, state);
}

int scheduleTurnOn(int id, Time time)
{
    return scheduleEvent(id, time, TURN_ON);
}

int scheduleTurnOff(int id, Time time)
{
    return scheduleEvent(id, time, TURN_OFF);
}

int scheduleTurnOnEveryDay(int id, int minuteOfDay)
{
    Time time = {EVERYDAY, minuteOfDay};
    return scheduleTurnOn(id, time);
}

int scheduleTurnOffEveryDay(int id, int minuteOfDay)
{
    Time time = {EVERYDAY, minuteOfDay};
    return scheduleTurnOff(id, time);
}

int scheduleTurnOnWeekend(int id, int minuteOfDay)
{
    Time time = {WEEKEND, minuteOfDay};
    return scheduleTurnOn(id, time);
}

int scheduleTurnOffWeekend(int id, int minuteOfDay)
{
    Time time = {WEEKEND, minuteOfDay};
    return scheduleTurnOff(id, time);
}

int scheduleTurnOnWeekday(int id, int minuteOfDay)
{
    Time time = {WEEKDAY, minuteOfDay};
    return scheduleTurnOn(id, time);
}

int scheduleTurnOffWeekday(int id, int minuteOfDay)
{
    Time time = {WEEKDAY, minuteOfDay};
    return scheduleTurnOff(id, time);
}

bool isEventDayMatch(int eventDay, int currentDay)
{
    return eventDay == EVERYDAY ||
           eventDay == currentDay ||
           (eventDay == WEEKEND && (currentDay == SATURDAY || currentDay == SUNDAY)) ||
           (eventDay == WEEKDAY && currentDay >= MONDAY && currentDay <= FRIDAY);
}

bool isEventTimeMatch(int eventMinute, int currentMinute)
{
    return eventMinute == currentMinute;
}

void resetLightSchedulerEvents(void)
{
    for (int i = 0; i < MaxEvents; i++)
        events[i].state = UNUSED;
    eventIndex = 0;
}