#ifndef LIGHT_SCHEDULER_H
#define LIGHT_SCHEDULER_H

#include "LightControl.h"
#include "TimeService.h"
#include <stdio.h>
#include <stdbool.h>

#define MaxEvents 256

typedef enum
{
    OK = 0,
    GENERIC_ERROR = -1,
    SCHEDULER_FULL = -2,
    SCHEDULER_EMPTY = -3,
    INVALID_TIME = -4,
    LIGHT_ID_OUT_OF_RANGE = -5,
    INVALID_SCHEDULE = -6
} ErrorCodes;

typedef enum
{
    UNUSED = -1,
    TURN_ON,
    TURN_OFF
} EventState;

typedef struct
{
    int lightId;
    Time time;
    EventState state;
} LightSchedulerEvents;

void LightScheduler_Init(void);
void LightScheduler_Destroy(void);
void LightScheduler_Wakeup(void);
int LightScheduler_CreateEvent(int id, Time time, EventState state);
int LightScheduler_RemoveEvent(int id, Time time);

int scheduleTurnOn(int id, Time time);
int scheduleTurnOff(int id, Time time);
int scheduleTurnOnEveryDay(int id, int minuteOfDay);
int scheduleTurnOffEveryDay(int id, int minuteOfDay);
int scheduleTurnOnWeekend(int id, int minuteOfDay);
int scheduleTurnOffWeekend(int id, int minuteOfDay);
int scheduleTurnOnWeekday(int id, int minuteOfDay);
int scheduleTurnOffWeekday(int id, int minuteOfDay);
int removeSchedule(int id, Time time);

#endif
