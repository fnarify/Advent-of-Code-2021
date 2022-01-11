#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MULT 5
#define DIM_ 100

enum Move {North = 1, South = 2, West = 3, East = 4};

const int vec[][2] = {
    [North] = { 0, -1},
    [South] = { 0,  1},
    [West]  = {-1,  0},
    [East]  = { 1,  0}
};

struct Queue {
    int *val;
    int front, rear, size;
};
typedef struct Queue Queue;

int qempty(Queue *q)
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
        return -1;
    }
    return q->val[q->front++];
}

int valid(int x, int y, int dim)
{
    int ret = 0;
    if (x >= 0 && x < dim && y >= 0 && y < dim) {ret = 1;}
    return ret;
}

/**
 * breadth-first search which creates a distance map of each point
 * from the starting point (whatever the first point in the queue is)
 *
 * almost a variant of Bellman-Ford
 */
void bfs(int *grid, int *dist, int dim, Queue *que)
{
    int i, cx, cy, nx, ny, nextdist;

    while (!qempty(que))
    {
        cx = deq(que);
        cy = deq(que);

        for (i = North; i <= East; i++)
        {
            nx = cx + vec[i][0];
            ny = cy + vec[i][1];

            if (valid(nx, ny, dim))
            {
                // next distance is the smallest value of the current distance
                // and the next risk level value at the grid
                nextdist = dist[cy * dim + cx] + grid[ny * dim + nx];
                if (nextdist < dist[ny * dim + nx] || !dist[ny * dim + nx])
                {
                    dist[ny * dim + nx] = nextdist;
                    enq(que, nx);
                    enq(que, ny);
                }
            }
        }
    }
}

int main()
{
    int i, j, x, nx, ny, dim, val, index;
    int *grid, *dist;
    char line[DIM_ + 2];
    Queue *que;

    dim = DIM_ * MULT;
    grid = calloc(dim * dim, sizeof(*grid));
    dist = calloc(dim * dim, sizeof(*dist));
    if (!dist || !grid) {printf("out of memory\n"); return -1;}

    ny = 0;
    while (fgets(line, sizeof(line), stdin))
    {
        // copy without newline
        nx = strcspn(line, "\n");

        for (x = 0; x < nx; x++)
        {
            val = line[x] - '0';
            for (i = 0; i < MULT; i++)
            {
                for (j = 0; j < MULT; j++)
                {
                    index = (ny + DIM_ * i) * dim + (x + DIM_ * j);
                    grid[index] = val + i + j;
                    // values > 9 wrap to 1 and higher, no 0 values
                    while (grid[index] > 9) {grid[index] -= 9;}
                }
            }
        }

        ny++;
    }

    que = malloc(sizeof(*que));
    if (!que) {return -2;}
    que->front = 0;
    que->rear = -1;
    que->size = dim * dim * dim;
    que->val = malloc(sizeof(*que->val) * que->size);
    if (!que->val) {printf("out of memory\n"); return -2;}

    // initial conditions
    dist[0] = grid[0];
    enq(que, 0);
    enq(que, 0);

    bfs(grid, dist, dim, que);
    
    // shortest path to pt1 endpoint is different if you include the expanded grid
    if (MULT == 1) {printf("pt1: %d\n", dist[(DIM_ - 1) * dim + (DIM_ - 1)] - grid[0]);}
    else           {printf("pt2: %d\n", dist[(dim - 1) * dim + (dim - 1)] - grid[0]);}

    free(que->val);
    free(que);
    free(grid);
    free(dist);

    return 0;
}
