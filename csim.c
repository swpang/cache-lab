#include "cachelab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

/* TEST CASES
./csim -s 1 -E 1 -b 1 -t traces/yi2.trace
./csim-ref -s 1 -E 1 -b 1 -t traces/yi2.trace
./csim -s 4 -E 2 -b 4 -t traces/yi.trace
./csim-ref -s 4 -E 2 -b 4 -t traces/yi.trace
./csim -s 2 -E 1 -b 4 -t traces/dave.trace
./csim-ref -s 2 -E 1 -b 4 -t traces/dave.trace
./csim -s 2 -E 1 -b 3 -t traces/trans.trace
./csim-ref -s 2 -E 1 -b 3 -t traces/trans.trace
./csim -s 2 -E 2 -b 3 -t traces/trans.trace
./csim-ref -s 2 -E 2 -b 3 -t traces/trans.trace
./csim -s 2 -E 4 -b 3 -t traces/trans.trace
./csim-ref -s 2 -E 4 -b 3 -t traces/trans.trace
./csim -s 5 -E 1 -b 5 -t traces/trans.trace
./csim-ref -s 5 -E 1 -b 5 -t traces/trans.trace
./csim -s 5 -E 1 -b 5 -t traces/long.trace
./csim-ref -s 5 -E 1 -b 5 -t traces/long.trace
*/

int hits, misses, evicts;
int verbose = 0;
int s;                  /* Number of set index bits, Number of sets S = 2^s */
int E;                  /* Associativity (number of lines per set) */
int b;                  /* Number of block bits, Block size B = 2^b */
char t[1024];           /* Name of tracefile */
int S;                  /* Number of sets */
int IS_DEBUG = 0;

typedef struct cache_line {
    int valid_bit;
    unsigned long int tag;
    int lru_counter;
} line;

/*
 * parse_args : parse input arguments
 */
void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "t:v:s:E:b:")) != -1) {
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
                printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
                printf("Options:\n");
                printf("  -h\t\tPrint this help message.\n   -v <bool>\tOptional verbose flag.\n  -s <num>\tNumber of set index bits.\n  -E <num>\tNumber of lines per set.\n  -b <num>\tNumber of block offset bits.\n  -t <file>\tTrace file.\n\n");
                printf("Examples:\n");
                printf("  linux> ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
                printf("  linux> ./csim -v 1 -s 8 -E 2 -b 4 -t traces/yi.trace\n");
                exit(0);      
        }
    }
}

/*
 * cache_line : allocate space for the given cache size
 */
struct cache_line** load_cache() {
    struct cache_line **cache;
    if ((cache = (struct cache_line**)malloc(S * sizeof(struct cache_line))) == NULL) {
        printf("Malloc Error\n");
        exit(0);
    }

    for (int i = 0; i < S; i++) {
        if ((cache[i] = (struct cache_line*)malloc(E * sizeof(struct cache_line))) == NULL) {
            printf("Malloc Error\n");
            exit(0);
        }
    }


    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            cache[i][j].lru_counter = 0;
            cache[i][j].tag = 0;
            cache[i][j].valid_bit = 0;
        }
    }

    return cache;
}

/*
 * cache_cache : free the space allocated for the cache array
 */
void clean_cache(struct cache_line** cache) {
    for (int i = 0; i < S; i++)
        free(cache[i]);
    free(cache);
}

/*
 * find_lru : search cache set for least recently used line
 */
int find_lru(struct cache_line **cache, int curr_set) {
    int lru_line, lru_time = -1;
    for (int i = 0; i < E; i++) {
        if (cache[curr_set][i].lru_counter > lru_time) {
            lru_time = cache[curr_set][i].lru_counter;
            lru_line = i;
        }
    }

    return lru_line;
}

/*
 * time_step : updated the least recently used values of the items in the cache
 */
void time_step(struct cache_line **cache, int curr_set, int curr_line) {
    for (int i = 0; i < E; i++) {
        if (i == curr_line)
            cache[curr_set][i].lru_counter = 0; /* Updated item counter is reset */
        else
            cache[curr_set][i].lru_counter++;   /* Unused item counters are incremented */
    }
}

/*
 * update_count : depending on the IS_HIT and IS_EVICTED boolean markers, print the appropriate values
 * and count the appropriate numbers of missed, hit, and evicted lines
 */
void update_count(char instruction, int IS_HIT, int IS_EVICTED) {
    switch (instruction) {
        case 'L':
            if (IS_HIT) {
                hits++;
                if (verbose)
                    printf("hit\n");
            }

            else if (IS_EVICTED) {
                misses++;
                evicts++;
                if (verbose)
                    printf("miss eviction\n");
            }

            else {
                misses++;
                if (verbose)
                    printf("miss\n");
            }
            break;

        case 'S':
            if (IS_HIT) {
                hits++;
                if (verbose)
                    printf("hit\n");
            }

            else if (IS_EVICTED) {
                misses++;
                evicts++;
                if (verbose)
                    printf("miss eviction\n");
            }

            else {
                misses++;
                if (verbose)
                    printf("miss\n");
            }
            break;
        
        case 'M':
            if (IS_HIT) {
                hits += 2;
                if (verbose) 
                    printf("hit hit\n");
            }

            else if (IS_EVICTED) {
                misses++;
                evicts++;
                hits++;
                if (verbose)
                    printf("miss eviction hit\n");
            }

            else {
                misses++;
                hits++;
                if (verbose)
                    printf("miss hit\n");
            } 
            break;
    }
}

int main(int argc, char **argv)
{          
    FILE *tracefile_ptr;    /* Trace file */

    parse_args(argc, argv); /* Parse Input Arguments */
 
    S = pow(2, s);          /* Number of sets S = 2^s */

    /* struct cache_line cache[S][E] */
    struct cache_line **cache;
    cache = load_cache();

    /* F
    I : Instruction Load (ignore)
    L : Data Load
    S : Data Store
    M : Data Modify = Load and Store
    */

    if ((tracefile_ptr = fopen(t, "r")) == NULL) {
        printf("File %s doesn't exist - Terminating", t);
        exit(0);
    }

    char instruction;
    unsigned int address;  
    int size;

    unsigned long int set, tag;
    int lru_line;
    int IS_HIT, IS_EVICTED;

    /* Address : 0 0 1 1 0 1 0 1 0 1 0 0 
     *           <---------><------><-->
     *               Tag       Idx  Off 
     *           addr - b - s   s    b
     */

    /* Iterate on file lines, and read values */
    while (fscanf(tracefile_ptr, " %c %x,%d", &instruction, &address, &size) > 0) {
        /* if verbose option is selected, print the input traces */
        if (verbose)
            printf(" %c %x,%d ", instruction, address, size);

        if (instruction == 'I') {
            printf("\n");
            continue;
        }
        
        /* Use shift operations to shave off parts of the address we don't want */
        tag = (address >> s) >> b;
        set = (address >> b) & (S - 1);                     /* Use bitwise operator */
        
        IS_HIT = 0;
        IS_EVICTED = 0;

        /* search for matching tag in the set */
        for (int i = 0; i < E; i++) {
            /* Check if line is valid and if the tag matches */
            if (cache[set][i].valid_bit && (cache[set][i].tag == tag)) {
                IS_HIT = 1;
                time_step(cache, set, i);           /* Updating the last used time of all items in set */
                break;
            }
        }

        if (!IS_HIT) {
            lru_line = find_lru(cache, set);

            IS_EVICTED = cache[set][lru_line].valid_bit;
                        
            cache[set][lru_line].valid_bit = 1;
            cache[set][lru_line].tag = tag;
            time_step(cache, set, lru_line);
        }

        update_count(instruction, IS_HIT, IS_EVICTED);
    }

    fclose(tracefile_ptr);
    clean_cache(cache);
    printSummary(hits, misses, evicts);

    return 0;
}
