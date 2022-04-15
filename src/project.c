#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "project.h"

int compare_int(int a, int b)
{
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

int compare_meeting(const void *meeting0, const void *meeting1)
{
	Meeting *a = (Meeting*) meeting0;
	Meeting *b = (Meeting*) meeting1;

	int months = compare_int(a->date.month, b->date.month);
	if (months != 0) return months;

	int days = compare_int(a->date.day, b->date.day);
	if (days != 0) return days;

	return compare_int(a->date.hour, b->date.hour);
}

/**
 * \brief Prints all meetings in the calendar.
 *
 * \details The function first sorts the meetings in the calendar according to
 * `date`, and then prints each `Meeting` in the following format:
 * `"%s %02d.%02d at %02d",
 * 	Meeting.description,
 * 	Meeting.date.day,
 * 	Meeting.date.month,
 * 	Meeting.date.hour`
 *
 * For example: `Example meeting 24.03 at 20`
 *
 * After all of the meetings have been printed, a line containing `SUCCESS` is
 * printed.
 *
 * \param calendar Pointer to an array of meetings.
 * \param num Number of meetings in calendar.
 */
void print_calendar(Meeting *calendar, int num)
{
	qsort((void*) calendar, num, sizeof(Meeting), compare_meeting);
	for (int i = 0; i < num; i++) {
		printf("%s %02d.%02d at %02d\n",
				calendar[i].description,
				calendar[i].date.day,
				calendar[i].date.month,
				calendar[i].date.hour);
	}
	printf("SUCCESS\n");
}

int check_timeslot(Meeting* calendar, int num, Meeting newmeeting)
{
	for (int i = 0; i < num; i++) {
		if (compare_meeting((void*) &calendar[i], (void*) &newmeeting) == 0) {
			return i;
		}
	}
	return -1;
}

Meeting *add_meeting(Meeting *calendar, int num, Meeting newmeeting)
{
	// indicate caller that the meeting timeslot is already taken
	if (check_timeslot(calendar, num, newmeeting) != -1)
		return NULL;

	calendar = (Meeting*) realloc(calendar, (num + 1) * sizeof(Meeting));
	calendar[num] = newmeeting;
	return calendar;
}

Meeting *delete_meeting(Meeting *calendar, int num, MeetingDate timeslot)
{
	// a dummy meeting struct to check the calendar against in
	// `check_timeslot`
	Meeting timeslot_dummy;
	timeslot_dummy.description = NULL;
	timeslot_dummy.date = timeslot;

	int del_i = check_timeslot(calendar, num, timeslot_dummy);
	
	// indicate caller that the timeslot to delete was not found
	if (del_i == -1)
		return NULL;

	Meeting *newcalendar = (Meeting*) calloc(num - 1, sizeof(Meeting));

	for (int i = 0; i < del_i; i++) {
		newcalendar[i] = calendar[i];
	}

	for (int i = del_i; i < num - 1; i++) {
		newcalendar[i] = calendar[i + 1];
	}
	free(calendar);
	return newcalendar;
}


int main()
{
	Meeting *calendar = (Meeting*) calloc(2, sizeof(Meeting));
	if (!calendar) {
		printf("Error: failed to alloc for calendar!");
		return 1;
	}

	Meeting meet0;
	Meeting meet1;

	MeetingDate date0;
	MeetingDate date1;

	date0.month = 1;
	date0.day = 1;
	date0.hour = 12;

	date1.month = 6;
	date1.day = 14;
	date1.hour = 20;

	meet0.date = date0;
	meet1.date = date1;

	meet0.description = "Meeting 0";
	meet1.description = "Meeting 1";

	calendar = add_meeting(calendar, 0, meet1);
	calendar = add_meeting(calendar, 1, meet0);

	print_calendar(calendar, 2);
	MeetingDate del_date;
	del_date.month = 6;
	del_date.day = 14;
	del_date.hour = 21;

	Meeting *deleted = delete_meeting(calendar, 2, del_date);
	if (!deleted)
		printf("Could not delete, such meeting does not exists\n");
	print_calendar(calendar, 2);

	del_date.hour = 20;

	calendar = delete_meeting(calendar, 2, del_date);
	print_calendar(calendar, 1);
	free(calendar);
	return 0;
}
