#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define CHK_N_BASIN 3
#define DIM         100
#define OFF         1
#define IGNORE      '9' + 1

enum Move {North = 1, South = 2, West = 3, East = 4};

const int vec[][2] = {
    [North] = {0, -1},
    [South] = {0, 1},
    [West] = {-1, 0},
    [East] = {1, 0}
};

struct Queue {
    int *val;
    int front, rear, size;
};
typedef struct Queue Queue;

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
 * variant breadth-first search over the grid to get the size of each basin
 *
 * technically could just start from the low points in the grid and search outwards
 * only selecting values > the current value and < '9'
 */
int bfs(char grid[DIM + 2][DIM + 2], bool visit[DIM][DIM], Queue *que)
{
    int i, cx, cy, nx, ny, size;

    size = 0;
    while (!qempty(que))
    {
        cx = deq(que);
        cy = deq(que);
        visit[cy - OFF][cx - OFF] = true; // needed for initial run
        size++;

        for (i = North; i <= East; i++)
        {
            nx = cx + vec[i][0];
            ny = cy + vec[i][1];

            if (grid[ny][nx] < '9' && !visit[ny - OFF][nx - OFF])
            {
                enq(que, nx);
                enq(que, ny);
                visit[ny - OFF][nx - OFF] = true;
            }
        }
    }
    return size;
}

/**
 * only need to check horizontal and vertical adjacents NOT diagonals
 */
char checkadj(char state[DIM + 2][DIM + 2], int x, int y)
{
    char val, ret;

    ret = 0;
    val = state[y][x];
    if (val < state[y][x + 1] &&
        val < state[y][x - 1] &&
        val < state[y + 1][x] &&
        val < state[y - 1][x] )
    {
        ret = 1;
    }
    return ret;
}

/**
 * find smallest value in a and return its index
 */
int findmin(int *a, int len)
{
    int i, min, loc;
    min = INT_MAX;
    loc = -1;
    for (i = 0; i < len; i++)
    {
        if (a[i] < min)
        {
            min = a[i];
            loc = i;
        }
    }
    return loc;
}

int main()
{
    int x, y, sum, nx, ny, nbasin, minloc;
    int basins[CHK_N_BASIN] = {0};
    char grid[DIM + 2][DIM + 2], line[DIM + 2];
    bool visit[DIM][DIM];
    Queue *que;

    // so the bounds outside the grid are more easily treated as empty spaces
    // values are always single digits, so anything > '9' works
    memset(grid, IGNORE, sizeof(grid));

    ny = 0;
    while (fgets(line, sizeof(line), stdin))
    {
        // copy without newline
        nx = strlen(line) - 1;
        strncpy(&grid[ny + OFF][OFF], line, nx);
        ny++;
    }

    sum = 0;
    for (y = OFF; y < ny + OFF; y++)
    {
        for (x = OFF; x < nx + OFF; x++)
        {
            if (checkadj(grid, x, y)) {sum += grid[y][x] - '0' + 1;}
        }
    }
    printf("low points: %d\n", sum);

    que = malloc(sizeof(*que));
    if (!que) {return -1;}
    que->front = 0;
    que->rear = -1;
    que->size = DIM * DIM * 2;
    que->val = malloc(sizeof(*que->val) * que->size);
    if (!que->val) {return -2;}

    memset(visit, 0, sizeof(visit));

    // for basins, just bfs every point that is < '9'
    nbasin = 0;
    for (y = OFF; y < ny + OFF; y++)
    {
        for (x = OFF; x < nx + OFF; x++)
        {
            if (grid[y][x] < '9' && !visit[y - OFF][x - OFF])
            {
                enq(que, x);
                enq(que, y);
                sum = bfs(grid, visit, que);

                // track 3 largest basin sizes
                if (nbasin < CHK_N_BASIN) {basins[nbasin++] = sum;}
                else
                {
                    minloc = findmin(basins, CHK_N_BASIN);
                    if (minloc != -1 && sum > basins[minloc]) {basins[minloc] = sum;}
                }
            }
        }
    }
    printf("sum of largest basins: %d\n", basins[0] * basins[1] * basins[2]);

    free(que->val);
    free(que);

    return 0;
}
