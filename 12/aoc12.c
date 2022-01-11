#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define N_PATH 30
#define N_SIZE 10
#define MAX_IN 50

struct Path {
    char name[N_SIZE];
    bool lower;
    int *joined; // stores location of connected paths
    int njoined, visited;
};
typedef struct Path Path;

void printpath(Path *p, int np)
{
    int i, j;
    for (i = 0; i < np; i++)
    {
        printf("%5s (J - %d V - %d): ", p[i].name, p[i].njoined, p[i].visited);
        for (j = 0; j < p[i].njoined; j++) {printf("%s ", p[p[i].joined[j]].name);}
        printf("\n");
    }
}

void init(Path *p, int loc, char *name)
{
    strcpy(p[loc].name, name);
    p[loc].visited = 0;
    p[loc].lower = (islower(name[0]) ? true : false);
    p[loc].joined = malloc(sizeof(*(p[loc].joined)) * N_PATH);
    p[loc].njoined = 0;
    if (!p[loc].joined) {printf("out of memory\n"); return;}
}

int location(Path *p, int np, char *name)
{
    for (int i = 0; i < np; i++)
    {
        if (strcmp(p[i].name, name) == 0) {return i;}
    }
    return -1;
}

void join(Path *p, int a, int b)
{
    if (a == -1 || b == -1) {printf("out of bounds\n"); return;}

    p[a].joined[p[a].njoined++] = b;
    p[b].joined[p[b].njoined++] = a;
}

/**
 * depth-first search of the paths
 * lowercase locations can only be visited at most once
 * (in pt2 only ONE lowercase location can be visited twice)
 * uppercase locations can be visisted as many times as possible
 *
 * twice = -1 for pt1
 * twice =  0 for pt2
 * start is the location of "start"
 */
int search(Path *p, int np, int loc, int *twice, int start)
{
    int i, sum;
    sum = 0;

    if (!strcmp(p[loc].name, "end"))              {return 1;}
    if (p[loc].lower && p[loc].visited && *twice) {return 0;}

    if (p[loc].lower)
    {
        if (p[loc].visited && *twice != -1) {*twice = loc + 1;}
        else                                {p[loc].visited = 1;}
    }

    // add connected paths
    for (i = 0; i < p[loc].njoined; i++)
    {
        if (p[loc].joined[i] != start)
        {
            sum += search(p, np, p[loc].joined[i], twice, start);
        }
    }

    // reset path
    p[loc].visited = 0;
    if (loc + 1 == *twice)
    {
        p[loc].visited = 1;
        *twice = 0;
    }

    return sum;
}

int main()
{
    int i, n, start, twice;
    char input[MAX_IN], left[MAX_IN], right[MAX_IN];
    Path *paths;

    paths = malloc(sizeof(*paths) * N_PATH);
    if (!paths) {return -1;}

    n = 0;
    start = -1;
    while (fgets(input, sizeof(input), stdin))
    {
        if (sscanf(input, "%[^-]-%s\n", left, right) == 2)
        {
            if (location(paths, n, left) == -1)
            {
                init(paths, n, left);
                n++;
            }

            if (location(paths, n, right) == -1)
            {
                init(paths, n, right);
                n++;
            }

            join(paths, location(paths, n, left), location(paths, n, right));
        }
        else {printf("malformed input\n"); return -3;}
    }

    start = location(paths, n, "start");
    twice = -1;
    printf("amount of paths: %d\n", search(paths, n, start, &twice, start));
    twice = 0;
    printf("amount of paths: %d\n", search(paths, n, start, &twice, start));

    for (i = 0; i < n; i++) {free(paths[i].joined);}
    free(paths);

    return 0;
}
