# Report for TDD Exercise

Raynner Schnneider Carvalho

## Test list

1. **Scheduling a Light On on Specific Day and Time**: When a light is scheduled to be switched on at a specific time on a specific day, it should turn on at the scheduled time on the scheduled day.

2. **Scheduling a Light Off on Specific Day and Time**: When a light is scheduled to be switched off at a specific time on a specific day, it should turn off at the scheduled time on the scheduled day.

3. **Scheduling a Light On on Every Day at Specific Time**: When a light is scheduled to be switched on at a specific time every day, it should turn on at the scheduled time every day.

4. **Scheduling a Light Off on Every Day at Specific Time**: When a light is scheduled to be switched off at a specific time every day, it should turn off at the scheduled time every day.

5. **Scheduling a Light On on Weekends at Specific Time**: When a light is scheduled to be switched on at a specific time on weekends, it should turn on at the scheduled time on weekends.

6. **Scheduling a Light Off on Weekends at Specific Time**: When a light is scheduled to be switched off at a specific time on weekends, it should turn off at the scheduled time on weekends.

7. **Scheduling a Light On on Weekdays at Specific Time**: When a light is scheduled to be switched on at a specific time on weekdays, it should turn on at the scheduled time on weekdays.

8. **Scheduling a Light Off on Weekdays at Specific Time**: When a light is scheduled to be switched off at a specific time on weekdays, it should turn off at the scheduled time on weekdays.

9. **Scheduling Multiple Events for the Same Light**: When a light is scheduled to be switched on then switched off at a specific time on a specific day, it should turn on then turn off at the scheduled time on the scheduled day.

10. **Removing a Scheduled Event**: When a scheduled event for a light to be switched on at a specific time on a specific day is removed, the light should not turn on at the scheduled time on the scheduled day.

11. **Scheduling Maximum Number of Events**: When more than the maximum number of events are scheduled for a light, the scheduler should not allow scheduling more than the maximum number of events.

12. **Scheduling an Event for Non-Existing Light**: When an event is scheduled for a non-existing light, the scheduler should not allow scheduling events for non-existing lights.

13. **Scheduling an Event for Invalid Time**: When an event is scheduled for an invalid time, the scheduler should not allow scheduling events for invalid times.

14. **Removing a Non-Existing Scheduled Event**: When a non-existing scheduled event is removed, the scheduler should not allow removing non-existing scheduled events. If the scheduler is empty, it should not allow removing any scheduled events.