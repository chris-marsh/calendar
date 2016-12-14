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
 *                                                                                *
 *                            WORK IN PROGRESS                                    *
 *                                                                                *
 **********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
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

struct Options {
    int show_number_of_months;
    enum DaysOfWeek weekday_start;
    int day;
    int month;
    int year;
} options = { 1, MONDAY, 0, 0, 0 };

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
    int century = year / 101;
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
    for (day = 0; day < (first_day-(int)options.weekday_start+7)%7; day++) strcat(output[2], "   ");
    for (day = 1; day <= months[month-1].num_days; day++) {
        if (day == options.day && year == options.year && month == options.month)
            sprintf(temp, "\x1b[34m%2d\x1b[0m ", day);
        else
            sprintf(temp,"%2d ", day);
        strcat(output[2+line], temp);
        current_day = (first_day+day-1) % 7;
        if (current_day == (options.weekday_start+6)%7) line++;
    }
}

void print_one_month() {
    char calendar_month[8][40];
    int line;

    get_month(calendar_month, options.month, options.year);
    for (line=0; line<8; line++)
        puts(calendar_month[line]);
}

void print_three_months(int month, int year) {
    int line;
    char calendar[3][8][40];

    get_month(calendar[1], month, year);

    if (month - 1 == 0)
        get_month(calendar[0], 12, year-1);
    else
        get_month(calendar[0], month-1, year);

    if (month + 1 == 13)
        get_month(calendar[2], 1, year+1);
    else
        get_month(calendar[2], month+1, year);

    for (line=0; line<8; line++) {
        printf("%-21s  %-21s  %-21s\n", calendar[0][line], calendar[1][line], calendar[2][line]);
    }
}

void print_twelve_months(int month, int year) {
    int line;

    inc_month(1, &month, &year);
    for (line=0; line<4; line++) {
        print_three_months(month, year);
        inc_month(3, &month, &year);
    }
}

void usage(int exit_code) {
    /* TODO */
    exit(exit_code);
}

void version() {
    /* TODO */
    exit(0);
}

int decode_switches(int argc, char *argv[]) {
    int optc;

    static struct option const longopts[] = {
        {"one",      no_argument, NULL, '1'},
        {"three",    no_argument, NULL, '3'},
        {"twelve",   no_argument, NULL, 'Y'},
        {"year",     no_argument, NULL, 'y'},
        {"saturday", no_argument, NULL, 's'},
        {"monday",   no_argument, NULL, 'm'},
        {"help",     no_argument, NULL, 'h'},
        {"version",  no_argument, NULL, 'V'},
        {NULL, 0, NULL, 0}
    };

    while ((optc = getopt_long(argc, argv, "13yYsmhV", longopts, NULL)) != -1) {
        switch (optc) {
            case '1':
                options.show_number_of_months = 1;
                break;
            case '3':
                options.show_number_of_months = 3;
                break;
            case 'Y':
                options.show_number_of_months = 12;
                break;
            case 'y':
                /* TODO */
                break;
            case 's':
                /* TODO */
                break;
            case 'm':
                /* TODO */
                break;
            case 'h':
                usage(0);
                break;
            case 'V':
                version();
                break;
            default:
                usage (-1);
        }
    }

    if (optind < argc) {
        if (argc-optind == 3) {
            options.day = strToInt(argv[optind++]);
            options.month = strToInt(argv[optind++]);
            options.year = strToInt(argv[optind++]);
        } else if (argc-optind == 2) {
            options.month = strToInt(argv[optind++]);
            options.year = strToInt(argv[optind++]);
        } else if (argc-optind == 1) {
            options.year = strToInt(argv[optind++]);
            options.show_number_of_months = 12;
        } else
            usage(-1);
    }

    return optc;
}

int main(int argc, char *argv[]) {

    /* Get todays date */
    time_t t = time(0);
    struct tm *now = localtime(&t);
    options.day = now->tm_mday;
    options.month = now->tm_mon+1;
    options.year = now->tm_year+1900;

    decode_switches(argc, argv);

    switch (options.show_number_of_months) {
        case 1:
            print_one_month();
            break;
        case 3:
            print_three_months(options.month, options.year);
            break;
        case 12:
            print_twelve_months(options.month, options.year);
            break;
    }
}
