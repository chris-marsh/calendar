#include <stdio.h>
#include <stdlib.h>
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

void print_month(int weekday_start, int highlight_day, int month, int year) {
    int day, first_day, current_day;

    if ((month == 2) &&
            ((!(year % 4) && (year % 100)) || !(year % 400))) {
        months[month-1].num_days = 29;
    }

    printf("    %s %d\n" ANSI_COLOR_BLUE, months[month-1].longName, year);
    for (day=0; day <7; day++) {
        printf(" %s", weekdays[(day+weekday_start) % 7].shortName);
    }
    puts(ANSI_COLOR_RESET);

    first_day = first_day_of_month(month, year);
    for (day = 0; day < (first_day-weekday_start+7)%7; day++) printf("   ");
    for (day = 1; day <= months[month-1].num_days; day++) {
        if (day == highlight_day)
            printf(ANSI_COLOR_GREEN " %2d" ANSI_COLOR_RESET, day);
        else
            printf(" %2d", day);
        current_day = (first_day+day-1) % 7;
        if (current_day == (weekday_start+6)%7) putchar('\n');
    }
    putchar('\n');
}

int main(int argc, char *argv[]) {
    int day, month, year;
    if (argc == 3) {
        day = 1;
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
    print_month(MONDAY, day, month, year);

    return 0;
}

