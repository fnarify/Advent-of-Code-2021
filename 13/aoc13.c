#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 2000
#define MAX_IN  50
#define EMPTY   '.'
#define DOT     '#'

void prettyprint(char *a, int nx, int ny)
{
    int i, j;
    for (i = 0; i < ny; i++)
    {
        for (j = 0; j < nx; j++)
        {
            printf("%c", a[i * MAX_LEN + j] == DOT ? DOT : ' ');
        }
        printf("\n");
    }
}

int main()
{
    int x, y, xmax, ymax, loc, fold, sum, first;
    char direc;
    char input[MAX_IN];
    char *paper;

    paper = malloc(sizeof(*paper) * MAX_LEN * MAX_LEN);
    if (!paper) {printf("out of memory\n"); return -1;}
    memset(paper, EMPTY, MAX_LEN * MAX_LEN);

    first = 1;
    fold = xmax = ymax = 0;
    while (fgets(input, sizeof(input), stdin))
    {
        if (input[0] == '\n')
        {
            xmax++;
            ymax++;
            fold = 1;
        }
        else if (!fold)
        {
            sscanf(input, "%d,%d\n", &x, &y);
            paper[y * MAX_LEN + x] = DOT;
            if (x > xmax) {xmax = x;}
            if (y > ymax) {ymax = y;}
        }
        else
        {
            sscanf(input, "fold along %c=%d\n", &direc, &loc);

            // technically naive implementation, as the fold may not be in the centre
            // and it could still technically work as long as the fold is at or below half the width
            if (direc == 'x')
            {
                for (y = 0; y < ymax; y++)
                {
                    for (x = loc; x < xmax; x++)
                    {
                        if (paper[y * MAX_LEN + x] == DOT)
                        {
                            paper[y * MAX_LEN + (loc - (x - loc))] = DOT;
                        }
                    }
                }
                xmax = loc;
            }
            else if (direc == 'y')
            {
                for (y = loc; y < ymax; y++)
                {
                    for (x = 0; x < xmax; x++)
                    {
                        if (paper[y * MAX_LEN + x] == DOT)
                        {
                            paper[(loc - (y - loc)) * MAX_LEN + x] = DOT;
                        }
                    }
                }
                ymax = loc;
            }

            if (first)
            {
                sum = 0;
                for (y = 0; y < ymax; y++)
                {
                    for (x = 0; x < xmax; x++)
                    {
                        if (paper[y * MAX_LEN + x] == DOT) {sum++;}
                    }
                }
                first = 0;
                printf("dots visible after first fold: %d\n", sum);
            }
        }
    }
    prettyprint(paper, xmax, ymax);

    free(paper);

    return 0;
}
