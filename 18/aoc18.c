#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IN     100
#define MAX_TREE   100
#define SPLIT_LMT  '9'
#define EXPLODE_AT 4

struct Tree {
    struct Tree *left, *right, *parent;
    char data;
};
typedef struct Tree Tree;

Tree *prev = NULL;

void printtree(Tree *tree)
{
    if (tree->data != -1)
    {
        printf("%d", tree->data - '0');
    }
    else
    {
        printf("[");
        printtree(tree->left);
        printf(",");
        printtree(tree->right);
        printf("]");
    }
}

void clear(Tree *tree)
{
    if (tree->data == -1)
    {
        clear(tree->left);
        clear(tree->right);
    }
    if (tree->parent != NULL) {free(tree);}
}

/**
 * if a tree has data value > '9' then split it into a new tree with:
 * lval = data / 2 (round down)
 * rval = data / 2 (round up)
 * we only split at most once in the tree per iteration
 */
void split(Tree *tree, int *cnt)
{
    if (!*cnt)
    {
        if (tree->data == -1)
        {
            split(tree->left, cnt);
            split(tree->right, cnt);
        }
        else if (tree->data > SPLIT_LMT)
        {
            tree->left = malloc(sizeof(*tree->left));
            tree->left->parent = tree;
            tree->left->data = (tree->data - '0') / 2 + '0';

            tree->right = malloc(sizeof(*tree->right));
            tree->right->parent = tree;
            tree->right->data = (tree->data - '0') / 2 + '0';
            if (tree->data % 2) {tree->right->data++;}

            tree->data = -1;

            *cnt = 1;
        }
    }
}

/**
 * if any pair is nested inside 4 pairs (it's own pair doesn't count) it 'explodes'
 * that pairs lval is added to the first ealier value that can be found in the tree
 * the pairs rval is added to the first later value that can be found in the tree
 * do nothing if a suitable value can't be found
 * -- seems like it can be anything greater than 4 pairs as well
 *
 *  prev isn't always set correctly when iterating, so a global variable is needed
 */
void explode(Tree *tree, int depth, int *cnt, char *rval)
{
    if (!*cnt)
    {
        if (depth > EXPLODE_AT && tree->data != -1 && tree->parent->right->data != -1)
        {
            if (prev != NULL)
            {
                prev->data += tree->data - '0';
                tree->parent->data = '0';
            }
            else {tree->parent->data = '0';}

            *rval = tree->parent->right->data;
            // does not need to be freeded yet, as it's parents data is not -1
            //free(tree->parent->right);
            //free(tree);
            *cnt = 1;
        }
        else if (tree->data != -1) {prev = tree;} // record most recent valid value's location
    }
    else if (*cnt == 2 && tree->data != -1)
    {
        tree->data += *rval - '0';
        *cnt = 3;
    }

    if (tree->data == -1 && *cnt != 3)
    {
        explode(tree->left, depth + 1, cnt, rval);
        // so that if an exploding point is found it doesn't consider it's rval
        if (*cnt != 1) {explode(tree->right, depth + 1, cnt, rval);}
        else           {*cnt = 2;}
    }
}

void parse(Tree *tree, char *input)
{
    if (*input == '\0') {return;}

    if (*input == '[')
    {
        tree->left = malloc(sizeof(*tree->left));
        tree->left->data = -1;
        tree->left->parent = tree;
        parse(tree->left, input + 1);
    }
    else if (*input == ']')
    {
        parse(tree->parent, input + 1);
    }
    else if (*input == ',')
    {
        // since we are at the left tree, move up to the parent
        // and add the new value at the parent's right node
        tree->parent->right = malloc(sizeof(*tree->parent->right));
        tree->parent->right->data = -1;
        tree->parent->right->parent = tree->parent;
        parse(tree->parent->right, input + 1);
    }
    else if (isdigit(*input))
    {
        tree->data = *input;
        parse(tree, input + 1);
    }
}

/**
 * combine two Trees together at the highest level
 * note that this isn't commutative
 */
void combine(Tree *dest, Tree *a, Tree *b)
{
    dest->data = -1;
    dest->parent = NULL;

    dest->left = a;
    dest->right = b;

    dest->left->parent = dest;
    dest->right->parent = dest;
}

/**
 * magnitude of a pair is:
 * 3 * lval + 2 * rval
 * this is recursive
 */
int magnitude(Tree *tree)
{
    int sum = 0;
    if (tree->data == -1)
    {
        if (tree->left->data != -1)  {sum += 3 * (tree->left->data - '0');}
        else                         {sum += 3 * magnitude(tree->left);}

        if (tree->right->data != -1) {sum += 2 * (tree->right->data - '0');}
        else                         {sum += 2 * magnitude(tree->right);}
    }
    return sum;
}

/**
 * each run, explode once if possible, toherwise try to split once
 */
void reduce(Tree *tree)
{
    char val;
    int nops, cnt;

    nops = 1;
    while (nops)
    {
        cnt = val = 0;
        prev = NULL;
        explode(tree, 0, &cnt, &val);
        nops = cnt;

        if (!cnt)
        {
            cnt = 0;
            split(tree, &cnt);
            nops += cnt;
        }
    }
}

int main()
{
    int i, ntree;
    char input[MAX_IN];
    Tree *tree, *cur;

    tree = malloc(sizeof(*tree) * MAX_TREE);
    if (!tree) {return -1;}

    ntree = 0;
    while (fgets(input, sizeof(input), stdin) && ntree < MAX_TREE)
    {
        input[strcspn(input, "\n")] = '\0';

        tree[ntree].data = -1;
        tree[ntree].parent = NULL;

        parse(&tree[ntree], input);

        ntree++;
    }

    cur = malloc(sizeof(*cur) * ntree);
    if (!cur) {return -3;}

    for (i = 0; i < ntree - 1; i++)
    {
        if (i == 0) {combine(&cur[i], &tree[i], &tree[i + 1]);}
        else        {combine(&cur[i], &cur[i - 1], &tree[i + 1]);}

        reduce(&cur[i]);
    }
    printf("final mangnitude: %d\n", magnitude(&cur[i - 1]));

    for (i = 0; i < ntree; i++) {clear(&cur[i]);}
    free(tree);
    free(cur);

    return 0;
}
