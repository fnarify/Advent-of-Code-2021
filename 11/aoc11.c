#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define DIREC  8
#define FLASH  9
#define DIM    10
#define ROUNDS 100
#define MAX_IN 100

const int vec[][2] = {
    { 0, -1}, // up
    { 0,  1}, // down
    {-1,  0}, // left
    { 1,  0}, // right
    {-1, -1}, // upper left
    { 1, -1}, // upper right
    {-1,  1}, // lower left
    { 1,  1}  // lower right
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

bool valid(int n)
{
    return (n >= 0 && n < DIM);
}

void bfs(int grid[DIM][DIM], bool flashed[DIM][DIM], Queue *que)
{
    int i, cx, cy, nx, ny;

    while (!qempty(que))
    {
        cx = deq(que);
        cy = deq(que);

        if (grid[cy][cx] > FLASH && !flashed[cy][cx])
        {
            flashed[cy][cx] = true;

            // add all adjacents (including diagonals)
            for (i = 0; i < DIREC; i++)
            {
                nx = cx + vec[i][0];
                ny = cy + vec[i][1];
                if (valid(nx) && valid(ny) && !flashed[ny][nx])
                {
                    enq(que, nx);
                    enq(que, ny);
                    grid[ny][nx]++;
                }
            }
        }
    }
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
 * needs -std=c99 for printing 64-bit ints
 */
int main()
{
    int n, i, j, sum, nflash;
    int grid[DIM][DIM];
    char input[MAX_IN];
    bool flashed[DIM][DIM];
    Queue *que;

    n = 0;
    while (fgets(input, sizeof(input), stdin))
    {
        for (i = 0; input[i] != '\n'; i++) {grid[n][i] = input[i] - '0';}
        n++;
    }

    que = malloc(sizeof(*que));
    if (!que) {return -1;}
    que->size = DIM * DIM * DIM;
    que->val = malloc(sizeof(*que->val) * que->size);
    if (!que->val) {return -2;}

    n = sum = 0;
    while (++n)
    {
        que->front = 0;
        que->rear = -1;
        memset(flashed, 0, sizeof(flashed));

        // increment whole grid by 1, add any flashed locations to queue
        for (i = 0; i < DIM; i++)
        {
            for (j = 0; j < DIM; j++)
            {
                grid[i][j]++;
                if (grid[i][j] > FLASH)
                {
                    enq(que, j); // x
                    enq(que, i); // y
                }
            }
        }
        
        // for every flashed location, increment all adjacents by 1 (including diagonals)
        bfs(grid, flashed, que);

        // set all flashed locations back to 0
        nflash = 0;
        for (i = 0; i < DIM; i++)
        {
            for (j = 0; j < DIM; j++)
            {
                if (flashed[i][j])
                {
                    grid[i][j] = 0;
                    nflash++;
                    sum++;
                }
            }
        }

        if (n == ROUNDS) {printf("after %d rounds: %d\n", ROUNDS, sum);}
        if (nflash == DIM * DIM)
        {
            printf("synchronised at: %d\n", n);
            break;
        }
    }

    free(que->val);
    free(que);

    return 0;
}
