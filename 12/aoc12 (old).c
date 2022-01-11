#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#define N_PATH 30
#define N_SIZE 10
#define MAX_IN 50

struct Path {
    char name[N_SIZE];
    bool upper;
    struct Path *joined;
    int njoined, visited;
};
typedef struct Path Path;

struct Queue {
    int *val;
    int front, rear, size;
};
typedef struct Queue Queue;

void printpath(Path *p, int np)
{
    int i, j;
    for (i = 0; i < np; i++)
    {
        printf("%5s (J - %d V - %d): ", p[i].name, p[i].njoined, p[i].visited);
        for (j = 0; j < p[i].njoined; j++) {printf("%s ", p[i].joined[j].name);}
        printf("\n");
    }
}

void init(Path *p, int loc, char *name)
{
    strcpy(p[loc].name, name);
    p[loc].visited = 0;
    p[loc].upper = (isupper(name[0]) ? true : false);
    p[loc].joined = malloc(sizeof(*(p[loc].joined)) * N_PATH);
    p[loc].njoined = 0;
    if (!p[loc].joined) {printf("out of memory\n"); return;}
}

int exists(Path *p, int np, char *name)
{
    int i;
    for (i = 0; i < np; i++)
    {
        if (strcmp(p[i].name, name) == 0) {return i;}
    }
    return -1;
}

void join(Path *p, int a, int b)
{
    if (a == -1 || b == -1) {printf("out of bounds\n"); return;}

    p[a].joined[p[a].njoined++] = p[b];
    p[b].joined[p[b].njoined++] = p[a];
}

Queue *qinit(int size)
{
    Queue *q;
    q = malloc(sizeof(*q));
    if (!q) {printf("out of memory\n"); return NULL;}
    q->front = 0;
    q->rear = -1;
    q->size = size;
    q->val = malloc(sizeof(*q->val) * q->size);
    if (!q->val) {printf("out of memory\n"); return NULL;}

    return q;
}

bool qempty(Queue *q)
{
    return q->front > q->rear;
}

void enq(Queue *q, int item)
{
    if (q->rear == q->size - 1)
    {
        printf("Queue is full\n");
        q->rear = -1;
        q->front = 0;
    }
    q->val[++q->rear] = item;
}

int deq(Queue *q)
{
    if (q->front == q->size)
    {
        printf("Queue is empty\n");
        q->front = 0;
        return INT_MAX;
    }
    return q->val[q->front++];
}

/**
 * lowercase locations can only be visited at most once
 * uppercase locations can be visisted as many times as possible
 *
 * Queue stores the location in Paths of the item
 *
 * NOT PROPERLY RESETTING VISITED PATHS
 */
int bfs(Path *p, int np, Queue *que)
{
    int i, npath, cur, next;

    npath = 0;
    while (!qempty(que))
    {
        cur = deq(que);

        if (!p[cur].visited)
        {
            if (!strcmp(p[cur].name, "end")) {npath++;}
            else
            {
                if (!p[cur].upper) {p[cur].visited = 1;}

                for (i = 0; i < p[cur].njoined; i++)
                {
                    next = exists(p, np, p[cur].joined[i].name);
                    if (!p[next].visited && next != -1) {enq(que, next);}
                }
                // if no points added to the queue, remove visited from this one
            }
        }
    }

    return npath;
}

int main()
{
    int i, n, start;
    char input[MAX_IN], left[MAX_IN], right[MAX_IN];
    Path *paths;
    Queue *que;

    paths = malloc(sizeof(*paths) * N_PATH);
    if (!paths) {return -1;}

    n = 0;
    start = -1;
    while (fgets(input, sizeof(input), stdin))
    {
        if (sscanf(input, "%[^-]-%s\n", left, right) == 2)
        {
            if (exists(paths, n, left) == -1)
            {
                init(paths, n, left);
                n++;
            }

            if (exists(paths, n, right) == -1)
            {
                init(paths, n, right);
                n++;
            }

            join(paths, exists(paths, n, left), exists(paths, n, right));
        }
        else {printf("malformed input\n"); return -3;}
    }

    que = qinit(n * n * 2);
    if (!que) {return -4;}

    start = exists(paths, n, "start");
    enq(que, start);
    printpath(paths, n);
    printf("amount of paths: %d\n", search(paths, n, start));

    free(que->val);
    free(que);

    for (i = 0; i < n; i++) {free(paths[i].joined);}
    free(paths);

    return 0;
}
