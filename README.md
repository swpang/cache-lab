# cache-lab
2022-1 SNU CSE System Programming Cache Lab (based on CMU 15-213 Cache Lab)

i. Cache Simulator
<Pseudo Code>
parse_args()
load_cache()
f=fopen(t)
while(fscanf(f)) do:
tag = (address >> s) >> b
set = (address >> b) & (S - 1)
for line in cache do:
if cache is valid and tag matches do:
IS_HIT = 1
time_step()
break
if IS_HIT is false do:
lru_line = find_lru()
IS_EVICTED = is cache at lru_line valid
set valid bit = 1
set tag = tag
time_step()
update_count()
fclose(f)
clean_cache()
printSummary()
When the cache simulator starts, first the arguments are parsed. Given arguments are -s, -
E, -b, -t, -v, -h. Using the parameters of the cache given in the arguments, load_cache() 
is called, which allocates the appropriate space for the cache array. Two malloc functions 
are called, the first a malloc of size # of sets * size of cache line (size of int + size 
of long int + size of int) for the cache array, and the second a malloc of size # of lines 
per set * size of cache line iterated through the size of the array for all the array lines.
Each cache word is initialized to values of all 0, to make sure no garbage data is allocated.
After the cache is loaded, the trace file is loaded as a file pointer. Since the trace 
files are composed of multiple lines of commands, the file is iterated through the lines. 
The ‘I’ instruction is ignored; otherwise, the physical address given in the trace file is 
processed into the tag value and set offset. Then, the cache is checked for matching tags, 
and if the valid bit is valid, IS_HIT Boolean marker is marked as true and last used time 
info updated. If there is a hit in the cache, this line is selected and the evicted, missed, 
and hit counts are updated. If there isn’t a hit in the cache, the LRU cache line is found
using the find_lru() function, and this line is selected. If this selected line is invalid, 
there is no need to be evicted, while if this line is valid, an IS_EVICTED Boolean marker 
is marked as true. As before, the last used time info is updated, and the counts are 
updated. The find_lru() function searches the current cache set for the line with the 
largest lru_counter value, which is then returned and used as the lru line value.
Once the trace file is completed, the file pointer must be closed. Also, the previously 
dynamically allocated memory must be freed, through the clean_cache() function. Finally, 
all the counts are summarized and printed using the printSummary() function.
ii. Matrix Transpose
The problem is divided into three cases: 32x32, 64x64, and 61x67. For 32x32 and 61x67, the 
same algorithm is applied. This algorithm divides the given matrix into blocks (block size 
of 8x8 for 32x32 and block size 16x16 for 61x67), then iterates on each of the blocks, 
saving diagonal values to a local variable while replacing non-diagonal values into the 
transposed matrix B. Diagonal values are placed into the transposed matrix B after each 
loop on the block columns. Since 61x67 is not completely divisible into 16x16 blocks, an 
additional condition checking if the block index is smaller than the matrix size is 
implemented.
In the 64x64 case, a block size of 8 is used. Four local variables are added to save four 
values per each row in each block. These local variables are added into the transposed 
matrix B at once. This method resulted in the least cache misses heuristically.
