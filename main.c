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

#include "acal.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


/*
 * Output help/usage and exit the program
 */
void usage(int exit_code) {
    puts("");
    puts("USAGE");
    puts("    acal [options] [ [day] [month] [year] ]");
    puts("");
    puts("DESCRIPTION");
    puts("    Another CALendar displays a basic calendar. If no arguments");
    puts("    are specified, the current month is displayed.");
    puts("");
    puts("OPTIONS");
    puts("    -1, --one     - display single month output (default)");
    puts("    -3, --three   - display 3 months spanning the date");
    puts("    -s, --sunday  - use Sunday as the first day of the week");
    puts("    -m, --monday  - use Monday as the first day of the week");
    puts("    -y, --year    - display a calendar for the whole year");
    puts("    -Y, --twelve  - display a calendar for the next twelve months");
    puts("    -h, --help    - display this help");
    puts("    -v, --version - output version information");
    puts("");
    puts("EXAMPLES");
    puts("    acal -3");
    puts("    acal 12 2016");
    puts("    acal -y 20 07 1971");

    exit(exit_code);
}


/*
 * Ouput a version string and exit the program
 */
void version(void) {
    printf("acal - Another CALendar %s (compiled %s)\n", VERSION, __DATE__);
    exit(0);
}


/*
 * Ouput an error message and exit the program with an error statuis code
 */
void error(char *message) {
    puts(message);
    exit(-1);
}


/*
 * Parse the user given command line to set options
 */
int set_command_line_options(
        int argc, char *argv[],
        struct Date *calendar,
        struct Date *highlight,
        int*display_months) {
    int optc = 0;
    int show_calendar_year = 0;

    static struct option const longopts[] = {
        {"one",      no_argument, NULL, '1'},
        {"three",    no_argument, NULL, '3'},
        {"twelve",   no_argument, NULL, 'Y'},
        {"year",     no_argument, NULL, 'y'},
        {"sunday",   no_argument, NULL, 's'},
        {"monday",   no_argument, NULL, 'm'},
        {"help",     no_argument, NULL, 'h'},
        {"version",  no_argument, NULL, 'V'},
        {NULL, 0, NULL, 0}
    };

    while ((optc = getopt_long(argc, argv, "13yYsmhv", longopts, NULL)) != -1) {
        switch (optc) {
            case '1':
                *display_months = 1;
                break;
            case '3':
                *display_months = 3;
                break;
            case 'Y':
                *display_months = 12;
                break;
            case 'y':
                *display_months = 12;
                calendar->month = 1;
                show_calendar_year = -1;
                break;
            case 's':
                set_weekday_start(SUNDAY);
                break;
            case 'm':
                set_weekday_start(MONDAY);
                break;
            case 'h':
                usage(0);
                break;
            case 'v':
                version();
                break;
            default:
                usage (-1);
        }
    }

    if (optind < argc) {
        if (argc-optind == 3) {
            highlight->day = atoi(argv[optind]);
            highlight->month = atoi(argv[optind+1]);
            highlight->year = atoi(argv[optind+2]);
            if (show_calendar_year)
                calendar->month = 1;
            else
                calendar->month = highlight->month;
            calendar->year = highlight->year;
        } else if (argc-optind == 2) {
            if (show_calendar_year)
                calendar->month = 1;
            else
                calendar->month = atoi(argv[optind]);
            calendar->year = atoi(argv[optind+1]);
        } else if (argc-optind == 1) {
            calendar->year = atoi(argv[optind]);
            calendar->month = 1;
            *display_months = 12;
        } else
            usage(-1);
    }

    if (calendar->month < 1 || calendar->month > 12)
        error("acal: illegal month value: use 1-12");

    if (calendar->year < 1 || calendar->year >= MAX_YEAR)
        error("acal: illegal year value: out of range");

    return optc;
}


/*
 * Entry point of the program. Set the defaults then exec as needed from
 * command line options.
 */
int main(int argc, char *argv[]) {

    struct Date highlight_date = today();
    struct Date calendar_date = highlight_date;
    int display_months = 1;

    set_command_line_options(argc, argv,
        &calendar_date, &highlight_date, &display_months);

    set_highlight_date(highlight_date);

    switch (display_months) {
        case 1:
            display_one_month(calendar_date);
            break;
        case 3:
            display_three_months(calendar_date);
            break;
        case 12:
            display_twelve_months(calendar_date);
            break;
    }
    return 0;
}
