#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IN   50

int targets = 0;

int summation(int n)
{
    return (n * (n + 1) / 2);
}

int within(int n, int bounds[2])
{
    return (n >= bounds[0] && n <= bounds[1]);
}

int process(int start, int xbound[2], int ybound[2])
{
    int i, x, y, cx, cy, maxy, fnd;

    // upper bound can be abs(ybound[0]) - 1
    maxy = 0;
    for (i = ybound[0]; i <= abs(ybound[0]) - 1; i++)
    {
        fnd = 0;

        x = 0;
        cx = start;
        y = 0;
        cy = i;

        // note target is negative, this would need to be less than
        // if the target was positive in the y axis
        while (y >= ybound[0] && !fnd)
        {
            x += cx;
            cx = (cx ? cx -1 : 0);
            y += cy;
            cy--;

            if (within(x, xbound) && within(y, ybound))
            {
                if (i > maxy) {maxy = i;}
                if (!fnd)     {targets++;}
                fnd = 1;
            }
        }
    }

    return maxy;
}

int main()
{
    int i, maxy, temp;
    int x[2], y[2];
    char input[MAX_IN];

    while (fgets(input, sizeof(input), stdin))
    {
       if (sscanf(input, "target area: x=%d..%d, y=%d..%d\n",
                &x[0], &x[1], &y[0], &y[1]) == 4)
       {
       }
       else {printf("malformed input\n");}
    }

    /**
     * optimal point should be where x is 0 in the input area
     * x's final location is calculated via summation formula
     *
     * the same method can be used to independantly calculate y
     * or just note that if your initial velocity y > 0 then that
     * velocity when your location y is 0 has velocity -y - 1
     * so given bounds [y1, y2] the highest y value is abs(min([y1, y2])) - 1
     *
     * brute force seems more appropriate with pt2, as for this to work generally
     * you need to consider x locations that fall within one of the steps in the
     * summation rather than the final value of the summation.
     * ergo, for summation(6) is x's bounds fell in 6, 11, 15, 18, 20 or 21
     * rather than only 21
     */
    maxy = targets = 0;
    for (i = 1; i <= x[1]; i++)
    {
        temp = process(i, x, y);
        if (temp > maxy) {maxy = temp;}
    }
    printf("highest y position attained: %d\n", summation(maxy));
    printf("distinct initial velocities: %d\n", targets);

    return 0;
}
