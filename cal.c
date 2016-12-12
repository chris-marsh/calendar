#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int strToInt(char *str) {
    char *p;
    return strtol(str, &p, 10);
}

void dec_month(int *month, int *year) {
    *month = *month - 1;
    if (*month == 0) { *month=12, *year=*year-1; }
}

void inc_month(int *month, int *year) {
    *month = *month + 1;
    if (*month == 13) { *month=1; *year=*year+1; }
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

void get_month(char output[8][40], int weekday_start, int highlight_day, int month, int year) {
    int day, first_day, current_day, line=0;
    char temp[40];

    if ((month == 2) &&
            ((!(year % 4) && (year % 100)) || !(year % 400))) {
        months[month-1].num_days = 29;
    }

    for (line=0; line<8; line++) output[line][0]='\0'; line=0;

    sprintf(output[0], "    %s %d", months[month-1].longName, year);
    sprintf(output[1], "\x1b[34m");
    for (day=0; day <7; day++) {
        sprintf(temp, "%s ", weekdays[(day+weekday_start) % 7].shortName);
        strcat(output[1], temp);
    }
    strcat(output[1], "\x1b[0m");
    first_day = first_day_of_month(month, year);
    for (day = 0; day < (first_day-weekday_start+7)%7; day++) strcat(output[2], "   ");
    for (day = 1; day <= months[month-1].num_days; day++) {
        if (day == highlight_day)
            sprintf(temp, "\x1b[34m%2d\x1b[0m ", day);
        else
            sprintf(temp,"%2d ", day);
        strcat(output[2+line], temp);
        current_day = (first_day+day-1) % 7;
        if (current_day == (weekday_start+6)%7) line++;
    }
}

void print_one_month(int day, int month, int year) {
    char calendar_month[8][40];
    int line;

    get_month(calendar_month, MONDAY, day, month, year);
    for (line=0; line<8; line++)
        puts(calendar_month[line]);
}

void print_three_months(int day, int month, int year) {
    char calendar[3][8][40];
    int line;

    get_month(calendar[1], MONDAY, day, month, year);

    if (month-1==0)
        get_month(calendar[0], MONDAY, 0, 12, year-1);
    else
        get_month(calendar[0], MONDAY, 0, month-1, year);

    if (month+1==13)
        get_month(calendar[2], MONDAY, 0, 1, year+1);
    else
        get_month(calendar[2], MONDAY, 0, month+1, year);

    for (line=0; line<8; line++) {
        printf("%-21s  %-21s  %-21s\n", calendar[0][line], calendar[1][line], calendar[2][line]);
    }
}

void print_twelve_months(int day, int month, int year) {
    int line;
    inc_month(&month, &year);
    for (line=0; line<4; line++) {
        print_three_months(0, month, year);
        inc_month(&month, &year);
        inc_month(&month, &year);
        inc_month(&month, &year);
    }
}

int main(int argc, char *argv[]) {
    int day, month, year;

    if (argc == 3) {
        day = 0;
        month = strToInt(argv[1]);
        year = strToInt(argv[2]);
    } else {
        /* print calendar month for today */
        time_t t = time(0); /* get time now */
        struct tm *now = localtime(&t);
        day = now->tm_mday;
        month = now->tm_mon+1;
        year = now->tm_year+1900;
    }
    // print_one_month(day, month, year);   
    // print_three_months(day, month, year);
    print_twelve_months(day, month, year);

    return 0;
}

