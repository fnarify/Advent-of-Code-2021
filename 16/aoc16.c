#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define MAX_IN  2000
#define BASE    2

int64_t versionsum = 0;
int litlen = 0;
int64_t litvals[1000];

const char *hex[] = {
    ['0'] = "0000",
    ['1'] = "0001",
    ['2'] = "0010",
    ['3'] = "0011",
    ['4'] = "0100",
    ['5'] = "0101",
    ['6'] = "0110",
    ['7'] = "0111",
    ['8'] = "1000",
    ['9'] = "1001",
    ['A'] = "1010",
    ['B'] = "1011",
    ['C'] = "1100",
    ['D'] = "1101",
    ['E'] = "1110",
    ['F'] = "1111"
};

enum Packet {
    Sum     = 0,
    Product = 1,
    Min     = 2,
    Max     = 3,
    Literal = 4,
    Greater = 5,
    Less    = 6,
    Equal   = 7
};

int process(char *trans, int len, int index)
{
    int end, nops, n;
    int64_t IDval, verval, subval, sublen, min, max;
    char lenID, litstart;
    char version[4] = {0}, ID[4] = {0}, literal[5] = {0}, subpacket[16] = {0};

    // exit condition
    if (index >= len) {return index;}

    // packet version
    strncpy(version, &trans[index], 3);
    verval = strtol(version, NULL, BASE);
    versionsum += verval;
    index += 3;

    // packet type ID
    strncpy(ID, &trans[index], 3);
    IDval = strtol(ID, NULL, BASE);
    index += 3;

    // operator
    if (IDval != Literal)
    {
        // to act as an accumulator (ACC) for later operators
        litvals[litlen++] = 0;
        // to record how many literal values are stored
        nops = litlen;
        lenID = trans[index++] - '0';
        if (!lenID)
        {
            // next 15 bits is the length in bits of the subpackets in the current packet
            strncpy(subpacket, &trans[index], 15);
            subval = strtol(subpacket, NULL, BASE);
            index += 15;
            sublen = index + subval;
            while (index < sublen) {index = process(trans, len, index);}
        }
        else
        {
            // next 11 bits is how many subpackets are in the current packet
            strncpy(subpacket, &trans[index], 11);
            subval = strtol(subpacket, NULL, BASE);
            index += 11;
            while (subval)
            {
                index = process(trans, len, index);
                subval--;
            }
        }
    }

    // no non-operation (NOP), so don't need to decrement litlen if nops = 0 (removing ACC)
    nops = litlen - nops;
    switch (IDval)
    {
        case Sum:
            n = 0;
            while (n < nops)
            {
                litvals[litlen - nops - 1] += litvals[litlen - nops + n];
                n++;
            }
            litlen -= nops;
            break;
        case Product:
            n = 0;
            if (nops) {litvals[litlen - nops - 1] = 1;}
            while (n < nops)
            {
                litvals[litlen - nops - 1] *= litvals[litlen - nops + n];
                n++;
            }
            litlen -= nops;
            break;
        case Min:
            for (n = 0, min = INT64_MAX; n < nops; n++)
            {
                if (litvals[litlen - nops + n] < min) {min = litvals[litlen - nops + n];}
            }
            litvals[litlen - nops - 1] = min;
            litlen -= nops;
            break;
        case Max:
            for (n = 0, max = INT64_MIN; n < nops; n++)
            {
                if (litvals[litlen - nops + n] > max) {max = litvals[litlen - nops + n];}
            }
            litvals[litlen - nops - 1] = max;
            litlen -= nops;
            break;
        case Literal:
            // read blocks of 5 bits until the first bit is 0
            litvals[litlen] = end = 0;
            while (!end)
            {
                litvals[litlen] <<= 4;
                litstart = trans[index++];
                if (litstart == '0') {end = 1;}

                strncpy(literal, &trans[index], 4);
                litvals[litlen] += strtol(literal, NULL, BASE);
                index += 4;
            }
            litlen++;
            break;
        case Greater: // last three always have 2 sub-packets
            litvals[litlen - nops - 1] = (litvals[litlen - nops] > litvals[litlen - nops + 1] ? 1 : 0);
            litlen -= nops;
            break;
        case Less:
            litvals[litlen - nops - 1] = (litvals[litlen - nops] < litvals[litlen - nops + 1] ? 1 : 0);
            litlen -= nops;
            break;
        case Equal:
            litvals[litlen - nops - 1] = (litvals[litlen - nops] == litvals[litlen - nops + 1] ? 1 : 0);
            litlen -= nops;
            break;
    }

    return index;
}

/**
 * needs -std=c99
 * the answer nearly exceeds the size of a 64-bit signed integer
 * the spec makes it seem that no literal value can be negative,
 * but leave it as signed for now
 */
int main()
{
    int i, len, tsize;
    char input[MAX_IN], transmission[MAX_IN * 4];
    tsize = MAX_IN * 4;

    transmission[0] = '\0';
    while (fgets(input, sizeof(input), stdin))
    {
        versionsum = 0;
        memset(transmission, 0, sizeof(transmission));
        litlen = 0;
        memset(litvals, 0, sizeof(litvals));
        len = strcspn(input, "\n");
        for (i = 0; i < len; i++) {strcat(transmission, hex[(int) input[i]]);}
        tsize = strlen(transmission);
        process(transmission, tsize, 0);
        printf("version sum: %lld\n", versionsum);
        printf("prog output: %lld\n", litvals[0]);
    }

    return 0;
}
