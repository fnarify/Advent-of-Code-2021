#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 500
#define MAX_ENTR 200
#define GRID_WIDTH 5
#define DELIN " ,"

struct Bingo {
    int grid[GRID_WIDTH][GRID_WIDTH];
    bool fnd[GRID_WIDTH][GRID_WIDTH];
    bool ignore;
};
typedef struct Bingo Bingo;

/**
 * flag = 0 - print grid
 * flag = 1 - print fnd
 */
void prettyprint(Bingo *b, int bsize, bool flag)
{
    int x, y, n;
    for (n = 0; n < bsize; n++)
    {
        printf("\niteration %d:\n", n);
        for (y = 0; y < GRID_WIDTH; y++)
        {
            for (x = 0; x < GRID_WIDTH; x++)
            {
                printf("%d ", flag ? b[n].fnd[y][x] : b[n].grid[y][x]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

/**
 * marks all positions of val in b's grids as found
 */
void markgrids(Bingo *b, int val, int bsize)
{
    int x, y, n;
    for (n = 0; n < bsize; n++)
    {
        if (b[n].ignore) {continue;}

        for (y = 0; y < GRID_WIDTH; y++)
        {
            for (x = 0; x < GRID_WIDTH; x++)
            {
                if (b[n].grid[y][x] == val)
                {
                    b[n].fnd[y][x] = true;
                }
            }
        }
    }
}

/**
 * returns the index in b of the first grid it finds that has a bingo
 * horizontally or vertically
 */
int isbingo(Bingo *b, int bsize)
{
    int x, y, n, cnt, bingo;

    bingo = -1;
    for (n = 0; n < bsize && bingo == -1; n++)
    {
        if (b[n].ignore) {continue;}

        // horizontal check
        for (y = 0; y < GRID_WIDTH; y++)
        {
            for (x = 0, cnt = 0; x < GRID_WIDTH; x++)
            {
                cnt += b[n].fnd[y][x];
            }
            if (cnt == GRID_WIDTH) {bingo = n;}
        }

        // vertical check
        for (x = 0; x < GRID_WIDTH; x++)
        {
            for (y = 0, cnt = 0; y < GRID_WIDTH; y++)
            {
                cnt += b[n].fnd[y][x];
            }
            if (cnt == GRID_WIDTH) {bingo = n;}
        }
    }

    return bingo;
}

/**
 * sums all the numbers in grid at position loc of b which haven't been marked
 */
int sumgrid(Bingo *b, int loc, int bsize)
{
    int x, y, sum;
    sum = 0;
    if (loc < bsize)
    {
        for (y = 0; y < GRID_WIDTH; y++)
        {
            for (x = 0; x < GRID_WIDTH; x++)
            {
                if (b[loc].fnd[y][x] == false) {sum += b[loc].grid[y][x];}
            }
        }
    }
    return sum;
}

int main()
{
    bool winner;
    int x, lsize, ygrid, mark, bingo, nwins, lastmark, lastbingo;
    char *parse;
    char draw[MAX_LINE], input[MAX_LINE];

    Bingo *list = calloc(MAX_ENTR, sizeof(struct Bingo));
    if (!list) {return -1;}

    if (fgets(draw, sizeof(draw), stdin) == NULL)
    {
        printf("input mangled\n");
        return -2;
    }

    lsize = -1;
    while (fgets(input, sizeof(input), stdin))
    {
        if (input[0] == '\n')
        {
            lsize++;
            ygrid = 0;
        }
        else if (ygrid < GRID_WIDTH)
        {
            x = 0;
            parse = strtok(input, DELIN);
            while (parse != NULL)
            {
                list[lsize].grid[ygrid][x++] = atoi(parse);
                parse = strtok(NULL, DELIN);
            }
            ygrid++;
        }
    }
    lsize++; // last item

    // if mark is in an of Bingo's grids, mark it as true in its respective fnd
    lastmark = lastbingo = nwins = 0;
    winner = false;
    bingo = -1;
    parse = strtok(draw, DELIN);
    while (parse != NULL && nwins < lsize)
    {
        mark = atoi(parse);
        markgrids(list, mark, lsize);
        // multiple boards can win in the same round
        while (true)
        {
            bingo = isbingo(list, lsize);
            if (bingo == -1) {break;}

            list[bingo].ignore = true;
            if (!winner)
            {
                printf("first winner: %d\n", mark * sumgrid(list, bingo, lsize));
                winner = true;
            }
            nwins++;

            lastbingo = bingo;
            lastmark = mark;
        }
        parse = strtok(NULL, DELIN);
    }
    printf("last winner: %d\n", lastmark * sumgrid(list, lastbingo, lsize));

    free(list);

    return 0;
}
