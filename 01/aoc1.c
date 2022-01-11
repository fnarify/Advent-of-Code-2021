#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_LINE 20
#define MAX_IN   3000

int main()
{
    int i, n, prev, cnt;
    int *depth;
    char line[MAX_LINE];

    depth = calloc(MAX_IN, sizeof(*depth));
    if (!depth) {return -1;}

    n = cnt = 0;
    prev = INT_MAX;
    while (fgets(line, sizeof(line), stdin) && n < MAX_IN)
    {
        depth[n] = atoi(line);
        if (depth[n] > prev) {cnt++;}
        prev = depth[n];
        n++;
    }
    printf("pt1: %d increases\n", cnt);

    // only need to compare first and last points, as the rest are shared
    // eg, given a b c d with triples (a b c) and (b c d),
    // if d > a then the sum of set 2 is greater than set 1.
    cnt = 0;
    for (i = 0; i < n - 3; i++)
    {
        if (depth[i] < depth[i + 3]) {cnt++;}
    }
    printf("pt2: %d increase\n", cnt);

    free(depth);

    return 0;
}
