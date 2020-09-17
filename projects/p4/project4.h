#include <bits/types/FILE.h>
#define BS_NAME             "BACKING_STORE.bin"
#define BYTE                8
#define FRAMES              256
#define INIT                -1
#define MASK_ADDR           0xFFFF
#define MASK_OFFSET         0xFF
#define SZ_BUFF             16
#define SZ_FRAME            256
#define SZ_LINE             10
#define SZ_STORE            256
#define SZ_TABLE            256


FILE *addr_file;                    // file containing addresses
FILE *store_file;                   // backing store file
char input_addr[SZ_LINE];           // used to hold one address value at a time
double fault_rate;                  // rate of page faults
double hit_rate;                    // TLB hit rate
int addr_physical;                  // physical address
int addr_virtual;                   // virtual address
int avail_entry_index = 0;          // keep track of first free table entry
int avail_frame_index = 0;          // keep track of first free frame
int buff_entries = 0;               // counter for TLB entries
int buff_frames[SZ_BUFF];           // hold TLB frame numbers
int buff_hits = 0;                  // keep track of TLB hits
int buff_pages[SZ_BUFF];            // hold TLB page numbers
int faults = 0;                     // page fault counter
int physical_mem[FRAMES][SZ_FRAME]; // physical memory
int table_frames[SZ_TABLE];         // hold page table frame numbers
int table_pages[SZ_TABLE];          // hold page table page numbers
int trans_addr = 0;                 // counter for translated addresses
signed char bs_data[SZ_STORE];      // hold data from backing store
signed char this_val;               // used to store current value in memory

void buff_assign_end(int frame, int page);
void buff_assign_prev(int frame, int page);
void buff_insert(int page, int frame);
void buff_right_shift(int index);
void error_check_main(int argc, char **argv);
void error_check_read_store(int page);
void get_page(int addr);
void read_store(int page);