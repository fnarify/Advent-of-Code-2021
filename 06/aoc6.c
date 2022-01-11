#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE 1000
#define TIMER_MAX 9
#define DELIN ","

int64_t spawn(int64_t *fish, int rounds)
{
    int i, n;
    int64_t ret, temp;
    for (n = 0; n < rounds; n++)
    {
        temp = fish[0];
        for (i = 0; i < TIMER_MAX - 1; i++) {fish[i] = fish[i + 1];}
        fish[TIMER_MAX - 3] += temp; // reset fish to pos 6
        fish[TIMER_MAX - 1] = temp;  // spawn new fish at pos 8
    }
    for (i = 0, ret = 0; i < TIMER_MAX; i++) {ret += fish[i];}
    return ret;
}

/**
 * requires -std=c99 for printing 64-bit integers on Windows
 */
int main()
{
    int64_t fish[TIMER_MAX] = {0};
    char *parse;
    char input[MAX_LINE];

    if (fgets(input, sizeof(input), stdin))
    {
        parse = strtok(input, DELIN);
        while (parse != NULL)
        {
            fish[atoi(parse)]++;
            parse = strtok(NULL, DELIN);
        }
    }

    printf("pt1: %lld\n", spawn(fish, 80));
    printf("pt2: %lld\n", spawn(fish, 256 - 80));

    return 0;
}
