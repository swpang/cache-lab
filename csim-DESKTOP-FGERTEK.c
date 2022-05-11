#include "cachelab.h"

#include <unistd.h>



typedef struct cache_line {
    int valid_bit;
    int tag;
    int age_counter;
}

int main(int argc, char **argv)
{
    int verbose = 0;
    int s;              /* Number of set index bits, Number of sets S = 2^s */
    int E;              /* Associativity (number of lines per set) */
    int b;              /* Number of block bits, Block size B = 2^b */
    char *t;            /* Name of tracefile */

    // Parse Input Arguments
    while (-1 != getopt(argc, argv, "t:v:s:E:b:")) {
        switch(opt) {
            case 't':
                strcpy(t, optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'h':
            default:
                printf("wrong argument\n");
                exit(0);      
        }
    }

    S = pow(2, s);

    struct cache_line cache[S][E]

    /* 
    I : Instruction Load (ignore)
    L : Data Load
    S : Data Store
    M : Data Modify
    */

    FILE *fptr;
    ptr = fopen(t, "r");

    char instruction;
    unsigned address;
    int size;

    while (fscanf(fptr, " %c %x,%d", &instruction, &address, $size) > 0) {
        if (verbose)
            printf(" %c %x,%d", identifier, address, size);

        tag = (addr >> s) >> b;
        // place / find words in cache -> if no match, missed
    }

    fclose(fptr);

    printSummary(0, 0, 0);
    return 0;
}
