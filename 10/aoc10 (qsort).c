#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_IN   200
#define MAX_LINE 100

const int illegal[] = {
    [(int) ')'] = 3,
    [(int) ']'] = 57,
    [(int) '}'] = 1197,
    [(int) '>'] = 25137
};

const int complete[] = {
    [(int) '('] = 1,
    [(int) '['] = 2,
    [(int) '{'] = 3,
    [(int) '<'] = 4
};

const char invert[] = {
    [(int) '('] = ')',
    [(int) '['] = ']',
    [(int) '{'] = '}',
    [(int) '<'] = '>'
};

struct Stack {
    char *val;
    int loc, size;
};
typedef struct Stack Stack;

int sempty(Stack *s)
{
    return s->loc < 1;
}

void push(Stack *s, int item)
{
    if (s->loc < s->size) {s->val[s->loc++] = item;}
    else                  {printf("Stack is full\n");}
}

int pop(Stack *s)
{
    char ret = 0;
    if (!sempty(s)) {ret = s->val[--s->loc];}
    else            {printf("Stack is empty\n");}
    return ret;
}

int cmp(const void *a, const void *b)
{
    int ret;
    if      (*(int64_t *)a > *(int64_t *)b) {ret = 1;}
    else if (*(int64_t *)a < *(int64_t *)b) {ret = -1;}
    else                                    {ret = 0;}
    return ret;
}

/**
 * needs -std=c99 for printing 64-bit ints
 */
int main()
{
    int n, len, invalid, syntax, nsum;
    int64_t autosums[MAX_LINE];
    char test;
    char input[MAX_IN];
    Stack *match;

    match = malloc(sizeof(*match));
    if (!match) {return -1;}
    match->size = MAX_IN;
    match->val = malloc(sizeof(*match->val) * match->size);
    if (!match->val) {return -2;}

    nsum = syntax = 0;
    while (fgets(input, sizeof(input), stdin))
    {
        invalid = 0;
        match->loc = 0;
        len = strcspn(input, "\n");

        // find corrupted lines (non-matching closing characters)
        for (n = 0; n < len && !invalid; n++)
        {
            switch (input[n])
            {
                case '(': case '[': case '{': case '<':
                    push(match, input[n]);
                    break;
                default:
                    test = pop(match);
                    if (input[n] != invert[(int) test])
                    {
                        syntax += illegal[(int) input[n]];
                        invalid = 1;
                    }
                    break;
            }
        }

        // find incomplete lines (missing closing characters)
        if (!invalid)
        {
            autosums[nsum] = 0;
            while (!sempty(match))
            {
                test = pop(match);
                autosums[nsum] *= 5;
                autosums[nsum] += complete[(int) test];
            }
            nsum++;
        }
    }
    printf("syntax error score: %d\n", syntax);

    // sort autosums via selection sort, then take midpoint (always odd number of items)
    qsort(autosums, nsum, sizeof(*autosums), cmp);
    printf("middle score: %lld\n", autosums[nsum / 2]);

    free(match->val);
    free(match);

    return 0;
}
