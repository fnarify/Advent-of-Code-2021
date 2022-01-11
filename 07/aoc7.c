#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_POS  1000
#define MAX_LINE 5000
#define DELIN ","

void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * linear summation formula for seuqnece 0 1 2 ... n
 */
int sum(int n)
{
    return (n * n + n) / 2;
}

int smaller(int a, int b)
{
    return (a < b ? a: b);
}

int main()
{
    int i, j, min, npos, index, median, average;
    int pos[MAX_POS], diff[2];
    char *parse, *input;
    
    input = malloc(sizeof(*input) * MAX_LINE);
    if (!input) {return -1;}

    npos = 0;
    if (fgets(input, sizeof(*input) * MAX_LINE, stdin) && npos < MAX_POS)
    {
        parse = strtok(input, DELIN);
        while (parse != NULL)
        {
            pos[npos++] = atoi(parse);
            parse = strtok(NULL, DELIN);
        }
    }
    free(input);

    // selection sort
    for (i = 0; i < npos; i++)
    {
        min = pos[i];
        index = i;
        for (j = i + 1; j < npos; j++)
        {
            if (pos[j] < min)
            {
                min = pos[j];
                index = j;
            }
        }
        if (index != i) {swap(&pos[i], &pos[index]);}
    }
    
    // calculate median of sorted array
    if (npos % 2) {median = pos[npos / 2];}
    else          {median = (pos[npos / 2] + pos[(npos / 2) - 1]) / 2;}

    // distance from the median, both the floor AND ceiling of the median are correct
    for (i = 0, diff[0] = 0; i < npos; i++) {diff[0] += abs(median - pos[i]);}
    printf("fuel used (constant rate): %d\n", diff[0]);

    // calculate mean of array
    for (i = 0, average = 0; i < npos; i++) {average += pos[i];}
    average /= npos;

    // need to check floor and ceiling (+1) around the average for the smallest value
    diff[0] = diff[1] = 0;
    for (i = 0; i < npos; i++)
    {
        diff[0] += sum(abs(average - pos[i]));
        diff[1] += sum(abs(average + 1 - pos[i]));
    }
    printf("fuel used (non-constant rate): %d\n", smaller(diff[0], diff[1]));

    return 0;
}
