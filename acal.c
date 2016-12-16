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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>

#define VERSION "0.1"

#define ANSI_BLUE     "\x1b[34m"
#define ANSI_REVERSE  "\x1b[7m"
#define ANSI_RESET    "\x1b[0m"

#define CAL_HEIGHT 8
#define CAL_WIDTH 36 /* Visible is 21,allow control chars(2*7) and term(1) */
#define MAX_YEAR (INT_MAX / 12 - 1)  /* Limited by the inc_month function */

enum DaysOfWeek {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

struct Weekday {
    char *longName;
    char *shortName;
} weekdays[] = {
    {"Sunday", "Su"},
    {"Monday", "Mo"},
    {"Tuesday", "Tu"},
    {"Wednesday", "We"},
    {"Thursday", "Th"},
    {"Friday", "Fr"},
    {"Saturday", "Sa"}
};

struct Month {
    char *longName;
    int num_days;
} months[] = {
    {"January", 31},
    {"February", 28},
    {"March", 31},
    {"April", 31},
    {"May", 31},
    {"June", 30},
    {"July", 31},
    {"August", 31},
    {"September", 30},
    {"October", 31},
    {"November", 30},
    {"December", 31}
};

struct Date {
    int day;
    int month;
    int year;
} highlight_date = { 0, 0, 0 };

struct Options {
    int show_number_of_months;
    int month_to_start_year;
    enum DaysOfWeek weekday_start;
    int month;
    int year;
} options = { 1, 1, MONDAY, 0, 0 };


/*
 * Convert a string of digits to int
 */
int strToInt(char *str) {
    char *p;
    return strtol(str, &p, 10);
}


/*
 * Add -/+num to month and inc/dec year as necessary. To simplify the
 * calculation, months are converted to 0 base and years converted to months.
 */
void inc_month(int num, int *month, int *year) {
    int tempYear  = *year;
    int tempMonth = *month - 1;

    *year = (tempYear * 12 + tempMonth + num) / 12;
    *month = (tempYear * 12 + tempMonth + num) % 12 + 1;
}


/*
 * Given month and year, calculate the day of week of the first day.
 * Day is returned as an integer 0-6, Sun-Sat
 * Based on Zeller's congruence;
 * https://en.wikipedia.org/wiki/Zeller's_congruence
 */
int first_day_of_month(int month, int year) {
    if (month < 3) {
        month += 12;
        year--;
    }
    int century = year / 100;
    year = year % 100;
    return (((13 * (month + 1)) / 5) +
            (century / 4) + (5 * century) +
            year + (year / 4)) % 7;
}


/*
 * Given month and year, populate the string array with a calendar month.
 */
void get_month(char calendar[CAL_HEIGHT][CAL_WIDTH], int month, int year) {
    int day, first_day, line=0;

    /* Adjust Februarys days for leap year */
    if ((month == 2) &&
            ((!(year % 4) && (year % 100)) || !(year % 400))) {
        months[month-1].num_days = 29;
    }

    /* Initialize calenVdar strings - needed when appending */
    for (line=0; line<CAL_HEIGHT; line++)
        calendar[line][0]='\0';

    /* Calendar month/year header */
    sprintf(calendar[0], "    %s %d", months[month-1].longName, year);

    /* Day column headers */
    strcpy(calendar[1], ANSI_BLUE);
    for (day=0; day <7; day++) {
        sprintf(calendar[1], "%s%s ",
                calendar[1],
                weekdays[(day+options.weekday_start) % 7].shortName);
    }
    strcat(calendar[1], ANSI_RESET);

    /* Pad line to 1st day of month */
    first_day = first_day_of_month(month, year);
    for (day = 0; day < (first_day-(int)options.weekday_start+7)%7; day++)
        strcat(calendar[2], "   ");

    /* Add each day of month, incrementing to next line on weekend */
    line = 2;
    for (day = 1; day <= months[month-1].num_days; day++) {
        if (day == highlight_date.day &&
                year == highlight_date.year &&
                month == highlight_date.month) {
            sprintf(calendar[line],
                    "%s%s%2d%s ",
                    calendar[line],
                    ANSI_REVERSE, day, ANSI_RESET);
        } else {
            sprintf(calendar[line],
                    "%s%2d ",
                    calendar[line],
                    day);
        }

        /* if current_day == weekend */
        if (((first_day+day - 1) % 7) == 
                (((int)options.weekday_start + 6) % 7))
            line++;
    }
}


/*
 * Output a single calendar month
 */
void print_one_month() {
    char calendar[CAL_HEIGHT][CAL_WIDTH];
    int line;

    get_month(calendar, options.month, options.year);
    for (line=0; line<CAL_HEIGHT; line++)
        puts(calendar[line]);
}


/*
 * Output 3 calendar months in vertical row
 */
void print_three_months(int month, int year) {
    int i;
    char calendar[3][CAL_HEIGHT][CAL_WIDTH];

    inc_month(-1, &month, &year);

    for (i=0; i<3; i++) {
        get_month(calendar[i], month, year);
        inc_month(1, &month, &year);
    }

    for (i=0; i<CAL_HEIGHT; i++) {
        printf("%-21s  %-21s  %-21s\n",
                calendar[0][i],
                calendar[1][i],
                calendar[2][i]);
    }
}


/*
 * Output a 12 month calendar arranged as a 3x4
 */
void print_twelve_months(int month, int year) {
    int i;

    inc_month(1, &month, &year);
    for (i=0; i<4; i++) {
        print_three_months(month, year);
        inc_month(3, &month, &year);
    }
}


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
void version() {
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
int decode_switches(int argc, char *argv[]) {
    int optc;

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
                options.show_number_of_months = 1;
                break;
            case '3':
                options.show_number_of_months = 3;
                break;
            case 'Y':
                options.show_number_of_months = 12;
                options.month_to_start_year = options.month; /* TODO: FIX ME */
                break;
            case 'y':
                options.show_number_of_months = 12;
                options.month_to_start_year = 1;
                break;
            case 's':
                options.weekday_start = SUNDAY;
                break;
            case 'm':
                options.weekday_start = MONDAY;
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
            highlight_date.day = strToInt(argv[optind++]);
            highlight_date.month = strToInt(argv[optind++]);
            highlight_date.year = strToInt(argv[optind++]);
            options.month = highlight_date.month;
            options.year = highlight_date.year;
        } else if (argc-optind == 2) {
            options.month = strToInt(argv[optind++]);
            options.year = strToInt(argv[optind++]);
        } else if (argc-optind == 1) {
            options.year = strToInt(argv[optind++]);
            options.show_number_of_months = 12;
        } else
            usage(-1);
    }

    if (options.month < 1 || options.month > 12)
        error("acal: illegal month value: use 1-12");

    if (options.year < 1 || options.year >= MAX_YEAR)
        error("acal: illegal year value: out of range");

    return optc;
}


/*
 * Entry point of the program. Set the defaults then exec as needed from
 * command line options.
 */
int main(int argc, char *argv[]) {

    /* Initialize highlight to todays date */
    time_t t = time(0);
    struct tm *now = localtime(&t);
    highlight_date.day= now->tm_mday;
    highlight_date.month = now->tm_mon+1;
    highlight_date.year = now->tm_year+1900;

    /* Default calendar to current month/year */
    options.month = highlight_date.month;
    options.year = highlight_date.year;

    decode_switches(argc, argv);

    switch (options.show_number_of_months) {
        case 1:
            print_one_month();
            break;
        case 3:
            print_three_months(options.month, options.year);
            break;
        case 12:
            print_twelve_months(options.month_to_start_year, options.year);
            break;
    }
    return 0;
}
