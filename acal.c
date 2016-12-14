/**********************************************************************************
 * Copyright (C) 2016, Chris Marsh <https://github.com/chris-marsh/calendar>      *
 *                                                                                *
 * This program is free software: you can redistribute it and/or modify it under  *
 * the terms of the GNU General Public License as published by the Free Software  *
 * Foundation, either version 3 of the License, or any later version.             *
 *                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT    *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS  *
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License at               *
 * <http://www.gnu.org/licenses/> for more details.                               *
 **********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_REVERSE  "\x1b[7m"
#define ANSI_COLOR_RESET    "\x1b[0m"

enum DaysOfWeek {
    SUNDAY,
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
} weekdays[7] = {
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
} months[12] = {
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

struct date {
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

int strToInt(char *str) {
    char *p;
    return strtol(str, &p, 10);
}

void dec_month(int num, int *month, int *year) {
    *year = *year - ((*month+num) / 12);
    *month = *month - num;
    if (*month < 0) *month = (12 - *month) % 12;
    if (*month == 0) *month = 12;
}

void inc_month(int num, int *month, int *year) {
    *year = *year + ((*month+num) / 12);
    *month = *month + num;
    if (*month > 12) *month = *month % 12;
}

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

void get_month(char output[8][40], int month, int year) {
    int day, first_day, current_day, line=0;
    char temp[41];

    if ((month == 2) &&
            ((!(year % 4) && (year % 100)) || !(year % 400))) {
        months[month-1].num_days = 29;
    }

    for (line=0; line<8; line++) output[line][0]='\0'; line=0;

    sprintf(output[0], "    %s %d", months[month-1].longName, year);
    sprintf(output[1], "\x1b[34m");
    
    for (day=0; day <7; day++) {
        sprintf(temp, "%s ", weekdays[(day+options.weekday_start) % 7].shortName);
        strcat(output[1], temp);
    }
    
    strcat(output[1], "\x1b[0m");
    first_day = first_day_of_month(month, year);

    for (day = 0; day < (first_day-(int)options.weekday_start+7)%7; day++)
        strcat(output[2], "   ");

    for (day = 1; day <= months[month-1].num_days; day++) {
        if (day == highlight_date.day && year == highlight_date.year && month == highlight_date.month)
            sprintf(temp, "\x1b[7m%2d\x1b[0m ", day);
        else
            sprintf(temp,"%2d ", day);
        strcat(output[2+line], temp);
        current_day = (first_day+day-1) % 7;
        if (current_day == (options.weekday_start+6)%7) line++;
    }
}

void print_one_month() {
    char calendar[8][40];
    int line;

    get_month(calendar, options.month, options.year);
    for (line=0; line<8; line++)
        puts(calendar[line]);
}

void print_three_months(int month, int year) {
    int i;
    char calendar[3][8][40];

    dec_month(1, &month, &year);

    for (i=0; i<3; i++) {
        get_month(calendar[i], month, year);
        inc_month(1, &month, &year);
    }

    for (i=0; i<8; i++) {
        printf("%-21s  %-21s  %-21s\n", calendar[0][i], calendar[1][i], calendar[2][i]);
    }
}

void print_twelve_months(int month, int year) {
    int i;

    inc_month(1, &month, &year);
    for (i=0; i<4; i++) {
        print_three_months(month, year);
        inc_month(3, &month, &year);
    }
}

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
    puts("    -1, --one		- display single month output (default)");
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

void version() {
    puts("acal - Another CALendar 0.1 (14 December 2016)");
    exit(0);
}

void error(char *message) {
    puts(message);
    exit(-1);
}

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
                options.month_to_start_year = options.month;
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

    if (options.month<1 || options.month>12)
        error("acal: illegal month value: use 1-12");

    if (options.year<1 || options.year>2147483646)
        error("acal: illegal year value: out of range");

    return optc;
}

int main(int argc, char *argv[]) {

    time_t t = time(0);
    struct tm *now = localtime(&t);
    highlight_date.day= now->tm_mday;
    highlight_date.month = now->tm_mon+1;
    highlight_date.year = now->tm_year+1900;

    options.month = highlight_date.month;
    options.year = highlight_date.year;

    decode_switches(argc, argv);

    printf("Month: %d    Year: %d\n", options.month, options.year);

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
}
