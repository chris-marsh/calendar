/******************************************************************************
 *                                                                            *
 *           Another CALendar Copyright (C) 2016 Chris Marsh                  *
 *               <https://github.com/chris-marsh/calendar                     *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify it    *
 * under the terms of the GNU General Public License as published by the      *
 * Free Software Foundation, either version 3 of the License, or any later    *
 * version.                                                                   *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * at <http://www.gnu.org/licenses/> for more details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef HEADERFILE_ACAL
#define HEADERFILE_ACAL

#include <limits.h>

#define VERSION "0.1"

#define ANSI_BLUE     "\x1b[34m"
#define ANSI_REVERSE  "\x1b[7m"
#define ANSI_RESET    "\x1b[0m"

#define CAL_HEIGHT 8
#define CAL_WIDTH 36 /* Visible is 21,allow control chars(2*7) and term(1) */
#define MAX_YEAR (INT_MAX / 12 - 1)  /* Limited by the inc_month function */

enum Days_of_Week{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

struct Date {
    int day;
    int month;
    int year;
};

struct Date today(void);
void set_weekday_start(enum Days_of_Week day);
void set_highlight_date(struct Date Date);
void display_one_month(struct Date date);
void display_three_months(struct Date date);
void display_twelve_months(struct Date date);

#endif
