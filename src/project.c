// SPDX-FileCopyrightText: 2022 Markus Murto (murtoM)
//
// SPDX-License-Identifier: MIT

/** \file project.c
 * \author Markus Murto
 * 
 * \copyright MIT
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "project.h"

/**
 * \brief Integer comparison function
 * 
 * Simple interger comparison function. 
 * 
 * \note Used internally only in compare_meeting() - not designed with `qsort`
 * in mind, but the result is similar with typical `qsort` compar functions.
 * 
 * Returns:
 * - `-1` if `a` < `b`
 * - `1` if `a` > `b`
 * - `0` if `a` == `b`
 * 
 * \param a First integer
 * \param b Second integer
 * 
 * \return `int` Comparison result
 */
int compare_int(int a, int b)
{
	if (a < b)
		return -1;
	if (a > b)
		return 1;
	return 0;
}

/**
 * \brief `Meeting` comparison function
 * 
 * `Meeting` comparison function which defines the order of the `Mettings` by
 * their date. `Meeting` which has date before comes first. Designed to be used
 * with `qsort()`.
 * 
 * \param meeting0 Void pointer to the first `Meeting`
 * \param meeting1 Void pointer to the second `Meeting`
 * 
 * \return `int` Comparision result, as `qsort()` expects
 */
int compare_meeting(const void *meeting0, const void *meeting1)
{
	Meeting *a = (Meeting *)meeting0;
	Meeting *b = (Meeting *)meeting1;

	int months = compare_int(a->date.month, b->date.month);
	if (months != 0)
		return months;

	int days = compare_int(a->date.day, b->date.day);
	if (days != 0)
		return days;

	return compare_int(a->date.hour, b->date.hour);
}

/**
 * \brief Prints all meetings in the calendar.
 *
 * The function first sorts the meetings in the calendar according to
 * `date`, and then prints each `Meeting` to `stdout` in the same plain text
 * format is used as in `write_calendar()`:
 * 
 * `"%s %02d.%02d at %02d", Meeting.description, Meeting.date.day,
 * Meeting.date.month, Meeting.date.hour`
 *
 * For example: `Example meeting 24.03 at 20`
 *
 * \param calendar Pointer to an array of `Meeting`s
 * \param num Number of meetings in calendar
 */
void print_calendar(Meeting *calendar, int num)
{
	qsort((void *)calendar, num, sizeof(Meeting), compare_meeting);
	for (int i = 0; i < num; i++) {
		printf("%s %02d.%02d at %02d\n", calendar[i].description,
		       calendar[i].date.day, calendar[i].date.month,
		       calendar[i].date.hour);
	}
}

/**
 * \brief Checks if the calendar already has the timeslot taken.
 * 
 * Checks if the calendar already has the timeslot taken. If the timeslot is
 * already taken, returns the index in the calendar, which date matches with the
 * new meeting. If the timeslot is free in the calendar, returns `-1`.
 * 
 * Used internally in add_meeting() and delete_meeting().
 * 
 * \param calendar Pointer to the `Meeting` array, "calendar"
 * \param num Number of meetings in the calendar
 * \param newmeeting A new `Meeting`, which timeslot might match with existing
 * meeting
 * 
 * \return `int` Index of matching meeting timeslot in calendar, `-1` if there is
 * no match
 */
int check_timeslot(Meeting *calendar, int num, Meeting newmeeting)
{
	for (int i = 0; i < num; i++) {
		if (compare_meeting((void *)&calendar[i],
				    (void *)&newmeeting) == 0) {
			return i;
		}
	}
	return -1;
}

/**
 * \brief Returns the number of lines in a file.
 *
 * Returns the number of lines in a file, if an error occurs while reading the
 * file, returns `-1`.
 * 
 * \param filename The name of the file
 *
 * \return `int` The number of files in the file, or `-1` if an error occurs
 */
int file_line_count(const char *filename)
{
	FILE *file_ptr = fopen(filename, "r");
	if (!file_ptr)
		return -1;

	int count = 0;
	char buffer[1000];

	while (fgets(buffer, 1000, file_ptr))
		count++;

	fclose(file_ptr);
	return count;
}

/**
 * \brief Validates `MeetingDate` and prints out a helpful error message
 *
 * Validates `MeetingDate` and prints out a helpful error message as a side
 * effect. Returns `0` if there is an validation error, otherwise returns `1`.
 * 
 * \param date A `MeetingDate` to validate
 *
 * \return `int` `0` if there is validation errors, otherwise `0`
 */
int validate_date(MeetingDate date)
{
	if (date.month > 12 || date.month < 1) {
		printf("Month cannot be less than 1 or greater than 12.\n");
		return 0;
	}
	if (date.day > 31 || date.day < 1) {
		printf("Day cannot be less than 1 or greater than 31.\n");
		return 0;
	}
	if (date.hour > 23 || date.hour < 0) {
		printf("Hour cannot be negative or greater than 23.\n");
		return 0;
	}
	return 1;
}

/**
 * \brief Add new `Meeting` to the calendar, if there is a free timeslot for it
 * 
 * Add new `Meeting` to the calendar, if there is a free timeslot for it - if
 * there is no free timeslot, returns `NULL`. The new `Meeting` is appended to
 * the end of the calendar, no sorting is done to the calendar. If the addition
 * is successful, returns pointer to the calendar.
 * 
 * \param calendar Pointer to the `Meeting` array, "calendar"
 * \param num Number of meetings in the calendar
 * \param newmeeting A new `Meeting` to be added into the calendar
 * 
 * \return `Meeting`* Pointer to the new calendar, `NULL` if the new meeting could
 * not be added
 */
Meeting *add_meeting(Meeting *calendar, int num, Meeting newmeeting)
{
	// indicate caller that the meeting timeslot is already taken
	if (check_timeslot(calendar, num, newmeeting) != -1)
		return NULL;

	calendar = (Meeting *)realloc(calendar, (num + 1) * sizeof(Meeting));
	if (!calendar) {
		printf("ERROR: Could not allocate memory for new calendar!\n");
		exit(1);
	}
	calendar[num] = newmeeting;
	return calendar;
}

/**
 * \brief Delete a `Meeting` from the calendar
 * 
 * Delete a `Meeting` from the calendar. If the meeting was not found in the
 * calendar, returns `NULL`. If the deletion was successful, returns pointer to
 * the new calendar.
 * 
 * \param calendar Pointer to the `Meeting` array, "calendar"
 * \param num Number of meetings in the calendar
 * \param timeslot `MeetingDate` used to match with a `Meeting` in the calendar
 * 
 * \return `Meeting`* Pointer to the new calendar, `NULL` if the meeting could not
 * be deleted
 */
Meeting *delete_meeting(Meeting *calendar, int num, MeetingDate timeslot)
{
	// a dummy meeting struct to check the calendar against in
	// `check_timeslot`
	Meeting timeslot_dummy;
	timeslot_dummy.date = timeslot;

	int del_i = check_timeslot(calendar, num, timeslot_dummy);

	// indicate caller that the timeslot to delete was not found
	if (del_i == -1)
		return NULL;

	Meeting *newcalendar = (Meeting *)calloc(num - 1, sizeof(Meeting));
	if (!newcalendar) {
		printf("ERROR: Could not allocate memory for new calendar!\n");
		exit(1);
	}

	for (int i = 0; i < del_i; i++) {
		newcalendar[i] = calendar[i];
	}

	for (int i = del_i; i < num - 1; i++) {
		newcalendar[i] = calendar[i + 1];
	}
	free(calendar);
	return newcalendar;
}

/**
 * \brief Writes the calendar as plain text into a file
 * 
 * Writes the calendar as plain text into a file in the filesystem. Same plain
 * text format is used as in `print_calendar()`:
 * 
 * `"%s %02d.%02d at %02d", Meeting.description, Meeting.date.day,
 * Meeting.date.month, Meeting.date.hour`
 *
 * For example: `Example meeting 24.03 at 20`
 * 
 * \param calendar Pointer to the `Meeting` array, "calendar"
 * \param num Number of meetings in the calendar
 * \param filename Path to filename to which calendar is written
 * 
 * \return `int` `1` if an error occurs during writing, `0` if writing was
 * successful
 */
int write_calendar(Meeting *calendar, int num, const char *filename)
{
	char buffer[1000];
	FILE *file_ptr = fopen(filename, "w");
	if (!file_ptr)
		return 1;

	memset(buffer, '\0', 1000);
	for (int i = 0; i < num; i++) {
		snprintf(buffer, 1000, "%s %02d.%02d at %02d\n",
			 calendar[i].description, calendar[i].date.day,
			 calendar[i].date.month, calendar[i].date.hour);
		fputs(buffer, file_ptr);
	}
	if (ferror(file_ptr)) {
		fclose(file_ptr);
		return 1;
	}
	fclose(file_ptr);
	return 0;
}

/**
 * \brief Loads a calendar plain text file and returns a `Meeting` array,
 * "calendar".
 *
 * Reads a calendar plain text file, which has to be written in the same format
 * as `write_calendar()` uses.
 *
 * Does not return the number of lines in the file, you have to use the function
 * `file_line_count()` to get the number of lines in a file, which is used in
 * other functions.
 * 
 * \param filename Path to filename from which calendar is read from
 *
 * \return `Meeting`* Pointer to the `Meeting` array, "calendar"
 */
Meeting *load_calendar(const char *filename)
{
	char buffer[1000];
	FILE *file_ptr = fopen(filename, "r");
	if (!file_ptr)
		return NULL;

	Meeting *calendar = (Meeting *)malloc(sizeof(Meeting));

	memset(buffer, '\0', 1000);
	int i = 0;
	while (fgets(buffer, 1000, file_ptr)) {
		calendar = (Meeting *)realloc(calendar, ++i * sizeof(Meeting));
		sscanf(buffer, "%s %02d.%02d at %02d",
		       calendar[i - 1].description, &calendar[i - 1].date.day,
		       &calendar[i - 1].date.month, &calendar[i - 1].date.hour);
	}
	if (ferror(file_ptr)) {
		fclose(file_ptr);
		return NULL;
	}
	fclose(file_ptr);
	return calendar;
}

/**
 * \brief Takes a command from the user and returns it as a `Command`
 * 
 * Takes user input in specific format and parses it into a proper `Command`
 * `struct`.
 * 
 * Expected Add meeting (\ref A) command: 
 * 
 * ```
 * A <description> <month> <day> <hour>
 * ```
 * 
 * For example:
 * 
 * ```
 * A Haircut 3 26 14
 * ```
 * 
 * Expected Delete meeting (\ref D) command:
 * 
 * ```
 * D <month> <day> <hour>
 * ```
 * 
 * For example:
 * 
 * ```
 * D 3 26 14
 * ```
 * 
 * Expected Print calendar (\ref L) command:
 * 
 * ```
 * L
 * ```
 * 
 * Expected Save to file (\ref W) command:
 * 
 * ```
 * W <filename>
 * ```
 * 
 * Expected Load from file (\ref O) command:
 * 
 * ```
 * O <filename>
 * ```
 * 
 * Expected Quit program (\ref Q) command:
 * 
 * ```
 * Q
 * ```
 * 
 * If the command is none of these, a `Command` with type \ref ERROR is returned.
 * 
 * \warning There is quite little validation. The function only validates the
 * command type and the number of arguments. `MeetingDate` can be validated
 * with `validate_date()`.
 * 
 * \return Command Parsed `Command`
 */
Command command_parser()
{
	Command command;
	command.meetingdate.month = 0;
	command.meetingdate.day = 0;
	command.meetingdate.hour = 0;
	command.message = (char *)malloc(sizeof(char));
	if (!command.message) {
		printf("ERROR: Could not allocate memory for new command message!\n");
		exit(1);
	}
	memset(command.message, '\0', 1);

	// setup buffers for each parameter and initialize them
	char type[200], param0[200], param1[200], param2[200], param3[200];
	memset(type, '\0', 200);
	memset(param0, '\0', 200);
	memset(param1, '\0', 200);
	memset(param2, '\0', 200);
	memset(param3, '\0', 200);

	// buffer for the whole line and initialize
	char line[1000];
	memset(line, '\0', 1000);

	// read user input and parse it into each param buffer
	fgets(line, 1000, stdin);
	int num_parsed = 0;
	num_parsed = sscanf(line, "%s %s %s %s %s", type, param0, param1, param2, param3);

	if (strcmp(type, "A") == 0) {
		// A command requires 4 arguments
		if (num_parsed == 5) {
			command.type = A;
			command.message = (char *)realloc(command.message,
							  (strlen(param0) + 1) *
								  sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for new command message!\n");
				exit(1);
			}
			strcpy(command.message, param0);
			sscanf(param1, "%d", &command.meetingdate.month);
			sscanf(param2, "%d", &command.meetingdate.day);
			sscanf(param3, "%d", &command.meetingdate.hour);
			return command;
		} else { // not enough arguments
			command.type = ERROR;
			char *error_msg =
				"A should be followed by exactly 4 arguments.\n";
			command.message = (char *)realloc(
				command.message,
				(strlen(error_msg) + 1) * sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for error message!\n");
				exit(1);
			}
			strcpy(command.message, error_msg);
			return command;
		}
	}
	if (strcmp(type, "D") == 0) {
		// D command requires 3 arguments
		if (num_parsed == 4) {
			command.type = D;
			sscanf(param0, "%d", &command.meetingdate.month);
			sscanf(param1, "%d", &command.meetingdate.day);
			sscanf(param2, "%d", &command.meetingdate.hour);
			return command;
		} else { // not enough arguments
			command.type = ERROR;
			char *error_msg =
				"D should be followed by exactly 3 arguments.\n";
			command.message = (char *)realloc(
				command.message,
				(strlen(error_msg) + 1) * sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for error message!\n");
				exit(1);
			}
			strcpy(command.message, error_msg);
			return command;
		}
	}
	if (strcmp(type, "W") == 0) {
		// W command requires 1 argument
		if (num_parsed == 2) {
			command.type = W;
			command.message = (char *)realloc(command.message,
							  (strlen(param0) + 1) *
								  sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for new command message!\n");
				exit(1);
			}
			strcpy(command.message, param0);
			return command;
		} else { // not enough arguments
			command.type = ERROR;
			char *error_msg =
				"W should be followed by exactly 1 argument.\n";
			command.message = (char *)realloc(
				command.message,
				(strlen(error_msg) + 1) * sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for error message!\n");
				exit(1);
			}
			strcpy(command.message, error_msg);
			return command;
		}
	}
	if (strcmp(type, "O") == 0) {
		// O command requires 1 argument
		if (num_parsed == 2) {
			command.type = O;
			command.message = (char *)realloc(command.message,
							  (strlen(param0) + 1) *
								  sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for new command message!\n");
				exit(1);
			}
			strcpy(command.message, param0);
			return command;
		} else { // not enough arguments
			command.type = ERROR;
			char *error_msg =
				"O should be followed by exactly 1 argument.\n";
			command.message = (char *)realloc(
				command.message,
				(strlen(error_msg) + 1) * sizeof(char));
			if (!command.message) {
				printf("ERROR: Could not allocate memory for error message!\n");
				exit(1);
			}
			strcpy(command.message, error_msg);
			return command;
		}
	}
	if (strcmp(type, "L") == 0) {
		command.type = L;
		return command;
	}
	if (strcmp(type, "Q") == 0) {
		command.type = Q;
		return command;
	}
	command.type = ERROR;
	char *error_msg = "Invalid command ";
	command.message = (char *)realloc(
		command.message,
		(strlen(error_msg) + strlen(line) + 1) * sizeof(char));
	strcpy(command.message, error_msg);
	strcat(command.message, line);
	return command;
}

/**
 * \brief Application entry
 * 
 * Application entry, starts taking user input in application command-line
 * (\ref command_parser()) and intrepretes the input into actions.
 * 
 * Prints `SUCCESS\n` into `stdout` after each successfully processed command.
 * If a command was not successfully processed, an error message is printed.
 * 
 * \return `int` Zero if the application quits successfully
 */
int main()
{
	Meeting *calendar = (Meeting *)calloc(2, sizeof(Meeting));
	if (!calendar) {
		printf("Error: failed to alloc for calendar!");
		return 1;
	}

	Command command;
	Meeting *processed;
	Meeting newmeeting;
	int num = 0;
	do {
		command = command_parser();
		switch (command.type) {
		case A:
			if (validate_date(command.meetingdate)) {
				newmeeting.date = command.meetingdate;
				memset(newmeeting.description, '\0', 64);
				strcpy(newmeeting.description, command.message);
				processed =
					add_meeting(calendar, num, newmeeting);
				if (!processed) {
					printf("The time slot %02d.%02d at %02d is already allocated.\n",
					       command.meetingdate.day,
					       command.meetingdate.month,
					       command.meetingdate.hour);
					free(command.message);
					break;
				}
				num++;
				printf("SUCCESS\n");
				calendar = processed;
			}
			free(command.message);
			break;
		case D:
			if (validate_date(command.meetingdate)) {
				processed = delete_meeting(calendar, num,
							   command.meetingdate);
				if (!processed) {
					printf("The time slot %02d.%02d at %02d is not in the calendar.\n",
					       command.meetingdate.day,
					       command.meetingdate.month,
					       command.meetingdate.hour);
					free(command.message);
					break;
				}
				num--;
				printf("SUCCESS\n");
				calendar = processed;
			}
			free(command.message);
			break;
		case L:
			print_calendar(calendar, num);
			printf("SUCCESS\n");
			free(command.message);
			break;
		case W:
			if (write_calendar(calendar, num, command.message)) {
				printf("ERROR: Error while writing a file.\n");
				free(command.message);
				break;
			}
			printf("SUCCESS\n");
			free(command.message);
			break;
		case O:
			processed = load_calendar(command.message);
			if (!processed) {
				printf("Cannot open file %s for reading.\n",
				       command.message);
				free(command.message);
				break;
			}
			printf("SUCCESS\n");
			num = file_line_count(command.message);
			free(calendar);
			calendar = processed;
			free(command.message);
			break;
		case Q:
			printf("SUCCESS\n");
			free(command.message);
			break;
		case ERROR:
			printf("%s", command.message);
			free(command.message);
			break;
		default:
			printf("ERROR: something went horribly wrong!\n");
			free(command.message);
			break;
		}
	} while (command.type != Q);

	free(calendar);
	return 0;
}
