#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 50
#define MAX_ENTR 500
#define GRID_WIDTH 5

void printgrid(int *grid, int dim)
{
    int i, j;
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
        {
            printf("%d", grid[i * dim + j]);
        }
        printf("\n");
    }
}

int lower(int a[][2], int n)
{
    return (a[n][0] < a[n][1] ? a[n][0] : a[n][1]);
}

int higher(int a[][2], int n)
{
    return (a[n][0] > a[n][1] ? a[n][0] : a[n][1]);
}

int intersections(int *grid, int dim, int val)
{
    int i, j, cnt;
    cnt = 0;
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
        {
            if (grid[i * dim + j] >= val) {cnt++;}
        }
    }
    return cnt;
}

int main()
{
    int n, nentr, xmax, ymax, dim, ret, start, end, diff;
    int *grid;
    int x[MAX_ENTR][2] = {0}, y[MAX_ENTR][2] = {0};
    char input[MAX_LINE];

    nentr = xmax = ymax = 0;
    while (fgets(input, sizeof(input), stdin) && nentr < MAX_ENTR)
    {
        ret = sscanf(input, "%d,%d -> %d,%d\n",
                &x[nentr][0], &y[nentr][0], &x[nentr][1], &y[nentr][1]);

        if (ret != 4) {printf("malformed data - %d\n", ret); return -1;}

        if (x[nentr][0] > xmax) {xmax = x[nentr][0];}
        if (x[nentr][1] > xmax) {xmax = x[nentr][1];}
        if (y[nentr][0] > ymax) {ymax = y[nentr][0];}
        if (y[nentr][1] > ymax) {ymax = y[nentr][1];}

        nentr++;
    }

    dim = (xmax > ymax ? xmax + 1 : ymax + 1);
    grid = calloc(dim * dim, sizeof(grid));
    if (!grid) {return -2;}

    // lines can go backwards in direction
    // directions are inclusive of [start, end]
    for (n = 0; n < nentr; n++)
    {
        if (x[n][0] == x[n][1]) // vertical line
        {
            start = lower(y, n);
            end = higher(y, n);

            while (start <= end)
            {
                grid[start * dim + x[n][0]]++;
                start++;
            }
        }
        else if (y[n][0] == y[n][1]) // horizontal line
        {
            start = lower(x, n);
            end = higher(x, n);

            while (start <= end)
            {
                grid[y[n][0] * dim + start]++;
                start++;
            }
        }
    }
    printf("%d points where lines overlap more than twice\n", intersections(grid, dim, 2));

    // diagonal line, always 45 degrees
    for (n = 0; n < nentr; n++)
    {
        if (x[n][0] != x[n][1] && y[n][0] != y[n][1])
        {
            diff = abs(x[n][0] - x[n][1]) + 1;
            while (diff--)
            {
                grid[y[n][0] * dim + x[n][0]]++;

                if (x[n][0] > x[n][1]) {x[n][0]--;}
                else                   {x[n][0]++;}
                if (y[n][0] > y[n][1]) {y[n][0]--;}
                else                   {y[n][0]++;}
            }
        }
    }
    printf("%d points where lines overlap more than twice\n", intersections(grid, dim, 2));

    free(grid);

    return 0;
}
