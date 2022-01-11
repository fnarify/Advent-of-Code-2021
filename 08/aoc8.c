#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_LETTER  7
#define IN_LEN    10
#define OUT_LEN   4
#define MAX_LINE  100
#define DELIN     " "

struct Display {
    // +1 for null-byte
    char in[IN_LEN][N_LETTER + 1];
    // bitmasked value as each display as a unique combination of 8 states
    int bit_in[IN_LEN], bit_out;
    int ilen[IN_LEN];
};
typedef struct Display Display;

const int map[] =
{
    ['a'] = 1 << 0,
    ['b'] = 1 << 1,
    ['c'] = 1 << 2,
    ['d'] = 1 << 3,
    ['e'] = 1 << 4,
    ['f'] = 1 << 5,
    ['g'] = 1 << 6
};

/**
 * finds an entry of length len in disp's input array
 */
int findlen(Display disp, int len, int start)
{
    int n;
    for (n = start; n < IN_LEN; n++)
    {
        if (disp.ilen[n] == len) {return n;}
    }
    return -1;
}

/**
 * given two characters build an array c that contains
 * all the characters that occur nd times between a and b
 * nd = 1 gives us a XOR b
 * nd = 2 gives us a AND b
 */
int diff(char *a, char *b, char *c, int nd)
{
    int i, n;
    char occur[N_LETTER] = {0};

    for (i = 0; a[i] != '\0'; i++) {occur[(int) a[i] - 'a']++;}
    for (i = 0; b[i] != '\0'; i++) {occur[(int) b[i] - 'a']++;}

    for (i = 0, n = 0; i < N_LETTER; i++)
    {
        if (occur[i] == nd) {c[n++] = i + 'a';}
    }
    c[n] = '\0';

    return n;
}

int singlebit(int b)
{
    return b && !(b & (b -1));
}

int contain(int *a, int len, int val)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if (a[i] == val) {return i;}
    }
    return -1;
}

/**
 * input is always 10 unique digits long
 * output is always 4 digits long
 *
 * Digit representations (length)
 *
 *  0(6):   1(2):   2(5):   3(5):   4(4):
 *  aaaa    ....    aaaa    aaaa    ....
 * b    c  .    c  .    c  .    c  b    c
 * b    c  .    c  .    c  .    c  b    c
 *  ....    ....    dddd    dddd    dddd
 * e    f  .    f  e    .  .    f  .    f
 * e    f  .    f  e    .  .    f  .    f
 *  gggg    ....    gggg    gggg    ....
 *
 *  5(5):   6(6):   7(3):   8(7):   9(6):
 *  aaaa    aaaa    aaaa    aaaa    aaaa
 * b    .  b    .  .    c  b    c  b    c
 * b    .  b    .  .    c  b    c  b    c
 *  dddd    dddd    ....    dddd    dddd
 * .    f  e    f  .    f  e    f  .    f
 * .    f  e    f  .    f  e    f  .    f
 *  gggg    gggg    ....    gggg    gggg
 *
 *  Separately, there's a unique mapping of each digit
 *  based on how many times a letter appears in the input
 */
int main()
{
    int i, cnt, len, nparse, startpos, temp, outdigit, sum;
    int loc[IN_LEN];
    char line[MAX_LINE], input[MAX_LINE], output[MAX_LINE], letters[N_LETTER], chardiff[N_LETTER + 1];
    char *parse;
    Display disp;

    sum = cnt = 0;
    while (fgets(line, sizeof(line), stdin))
    {
        if (sscanf(line, "%[^|] | %[^\n]", input, output) == 2)
        {
            memset(&disp, 0, sizeof(disp));
            memset(loc, -1, sizeof(loc));
            memset(letters, -1, sizeof(letters));

            // parse input strings
            nparse = 0;
            parse = strtok(input, DELIN);
            while (parse != NULL)
            {
                disp.ilen[nparse] = strlen(parse);
                //convert to bitmap
                for (i = 0; i < disp.ilen[nparse]; i++) {disp.bit_in[nparse] += map[(int) parse[i]];}

                switch (disp.ilen[nparse])
                {
                    case 2:
                        loc[1] = nparse;
                        break;
                    case 3:
                        loc[7] = nparse;
                        break;
                    case 4:
                        loc[4] = nparse;
                        break;
                    case 7:
                        loc[8] = nparse;
                        break;
                    default:
                        break;
                }

                strcpy(disp.in[nparse], parse);
                parse = strtok(NULL, DELIN);
                nparse++;
            }

            // 1 XOR 7 -> 'a'
            diff(disp.in[loc[7]], disp.in[loc[1]], chardiff, 1);
            letters['a' - 'a'] = disp.bit_in[loc[7]] ^ disp.bit_in[loc[1]];

            // 0 XOR 6 AND 1 -> 'f' || 1 AND 6 -> 'f'
            startpos = 0;
            i = findlen(disp, 6, startpos);
            while (i != -1)
            {
                temp = disp.bit_in[loc[1]] & disp.bit_in[i];
                // found 'f'
                if (singlebit(temp))
                {
                    diff(disp.in[loc[1]], disp.in[i], chardiff, 2);
                    letters['f' - 'a'] = temp;
                    // found 6
                    loc[6] = i;

                    // 1 - 'f' = 'c'
                    letters['c' - 'a'] = disp.bit_in[loc[1]] - temp;
                }

                // 4 AND 9 == 4
                temp = disp.bit_in[loc[4]] & disp.bit_in[i];
                if (temp == disp.bit_in[loc[4]]) {loc[9] = i;}

                // next iteration
                startpos = i + 1;
                i = findlen(disp, 6, startpos);
            }

            // unmarked location is therefore 0
            startpos = 0;
            i = findlen(disp, 6, startpos);
            while (i != -1)
            {
                if (contain(loc, IN_LEN, i) == -1) {loc[0] = i;}
                startpos = i + 1;
                i = findlen(disp, 6, startpos);
            }

            // 8 XOR 0 -> 'd' these two lines aren't needed for the solution
            letters['d' - 'a'] = disp.bit_in[loc[8]] ^ disp.bit_in[loc[0]];
            // 8 XOR 9 -> 'e'
            letters['e' - 'a'] = disp.bit_in[loc[8]] ^ disp.bit_in[loc[9]];

            // search digits of length 5
            startpos = 0;
            i = findlen(disp, 5, startpos);
            while (i != -1)
            {
                temp = disp.bit_in[loc[1]] & disp.bit_in[i];
                // 1 AND 2 == c
                if      (temp == letters['c' - 'a']) {loc[2] = i;}
                // 1 AND 5 == f
                else if (temp == letters['f' - 'a']) {loc[5] = i;}

                startpos = i + 1;
                i = findlen(disp, 5, startpos);
            }

            // unused number is 3
            for (i = 0; i < IN_LEN; i++)
            {
                if (contain(loc, IN_LEN, i) == -1) {loc[3] = i;}
            }

            outdigit = nparse = 0;
            parse = strtok(output, DELIN);
            while (parse != NULL)
            {
                disp.bit_out = 0;

                // digits with unique lengths are 2, 3, 4 and 7
                len = strlen(parse);
                if (len == 2 || len == 3 || len == 4 || len == 7) {cnt++;}

                for (i = 0; i < len; i++) {disp.bit_out += map[(int) parse[i]];}

                outdigit *= 10;
                for (i = 0; i < IN_LEN; i++)
                {
                    if (disp.bit_out == disp.bit_in[loc[i]]) {outdigit += i;}
                }

                parse = strtok(NULL, DELIN);
                nparse++;
            }
            sum += outdigit;
        }
        else {printf("no valid input\n"); break;}
    }
    printf("unique digits in the output: %d\n", cnt);
    printf("total sum of output digits: %d\n", sum);

    return 0;
}
