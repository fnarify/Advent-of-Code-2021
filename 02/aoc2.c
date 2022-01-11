#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 100

int main()
{
    int depth[2];
    int units, hor, aim;
    char line[MAX_LINE], dir[MAX_LINE];

    units = hor = aim = 0;
    depth[0] = depth[1] = 0;
    while (fgets(line, sizeof(line), stdin))
    {
        if (sscanf(line, "%s %d\n", dir, &units) == 2)
        {
            switch (dir[0])
            {
                case 'f': // forward
                    hor += units;
                    depth[1] += aim * units;
                    break;
                case 'u': // up
                    depth[0] -= units;
                    aim -= units;
                    break;
                case 'd': // down
                    depth[0] += units;
                    aim += units;
                    break;
                default:
                    printf("unknown command\n");
                    break;
            }
        }
    }
    printf("pt1: %d\npt2: %d\n", hor * depth[0], hor * depth[1]);

    return 0;
}
