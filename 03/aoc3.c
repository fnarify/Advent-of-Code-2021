#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LINE 20
#define MAX_ENTR 1000

int main()
{
    int i, nentr, len, gamma, epsilon, firstrun;
    int cnt[MAX_LINE][2], cnt2[MAX_LINE][2];
    char (*line)[MAX_LINE];

    line = malloc(sizeof(*line) * MAX_ENTR);
    if (!line) {return -1;}
    memset(cnt, 0, sizeof(cnt));

    nentr = 0;
    firstrun = 1;
    while (fgets(line[nentr], sizeof(line[0]), stdin) && nentr < MAX_ENTR)
    {
        // removing trailing newline
        if (firstrun)
        {
            len = strcspn(line[nentr], "\n");
            firstrun = 0;
        }

        for (i = 0; i < len; i++)
        {
            cnt[i][line[nentr][i] == '1' ? 1 : 0]++;
        }
        nentr++;
    }

    // calculate number comprised of the most common binary digits in all of the numbers
    gamma = 0;
    for (i = 0; i < len; i++)
    {
        if (cnt[i][1] > cnt[i][0]) {gamma += 1 << (len - i - 1);}
    }
    // mask for len number of binary digits, so that we can invert it
    epsilon = gamma ^ (~0u >> (sizeof(gamma) * CHAR_BIT - len));
    printf("power consumption: %d\n", gamma * epsilon);

    int oxyremoved, co2removed, rating;
    char oxy, co2;

    // initialise 'bitmask' spot
    for (i = 0; i < nentr; i++) {line[i][MAX_LINE - 1] = line[i][MAX_LINE - 2] = '0';}
    memcpy(cnt2, cnt, sizeof(cnt2));
    
    /**
     * Oxygen: determine most common digit for each position
     * only keep numbers that have that digit (performed linearly)
     * CO2: the same as above, except for the least common digit
     */
    oxyremoved = co2removed = 0;
    for (i = 0; i < len; i++)
    {
        oxy = co2 = '0';
        if (cnt[i][1] >= cnt[i][0])  {oxy = '1';}
        if (cnt2[i][1] < cnt2[i][0]) {co2 = '1';}

        for (int j = 0; j < nentr; j++)
        {
            // last check is so that it doesn't remove the very last item before checking each digit
            if (line[j][i] != oxy && line[j][MAX_LINE - 1] == '0' && oxyremoved < nentr - 1)
            {
                // bitmask using the empty space in each line, '1' means not valid
                line[j][MAX_LINE - 1] = '1';
                // decrease the digit counts for numbers to be excluded
                for (int m = i + 1; m < len; m++) {cnt[m][line[j][m] - '0']--;}
                oxyremoved++;
            }
            
            // second counter required as the original is modified
            if (line[j][i] != co2 && line[j][MAX_LINE - 2] == '0' && co2removed < nentr  - 1)
            {
                line[j][MAX_LINE - 2] = '1';
                for (int m = i + 1; m < len; m++) {cnt2[m][line[j][m] - '0']--;}
                co2removed++;
            }
            
        }
    }

    rating = 1;
    for (i = 0; i < nentr; i++)
    {
        if (line[i][MAX_LINE - 1] == '0') {rating *= strtol(line[i], NULL, 2);}
        if (line[i][MAX_LINE - 2] == '0') {rating *= strtol(line[i], NULL, 2);}
    }
    printf("life support: %d\n", rating);

    free(line);

    return 0;
}
