/* vim: sw=8 ts=8 noet */

#include <time.h>

#ifndef _PROJECT__H_
#define _PROJECT__H_

/* Date */
typedef struct {
	int month;
	int day;
	int hour;
} MeetingDate;

/* User command types */
enum Commandtype {
	A,	// add a new meeting
	D,	// delete a meeting
	L,	// print calendar
	W,	// save db to file
	O,	// open db from file
	Q	// quit application
};

/* User command */
typedef struct {
	enum Commandtype type;
	MeetingDate meetingdate;
	char *message;
} Command;

/* Meeting */
typedef struct {
	MeetingDate date;
	char *description;
} Meeting;

void print_calendar(Meeting*, int);

Meeting *add_meeting(Meeting*, int, Meeting);
Meeting *delete_meeting(Meeting*, int, MeetingDate);

#endif //! _PROJECT__H_
