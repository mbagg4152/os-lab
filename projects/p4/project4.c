#include "project4.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

int main(int argc, char **argv) {
    error_check_main(argc, argv);
    while (fgets(input_addr, SZ_LINE, addr_file) != NULL) {
        addr_virtual = atoi(input_addr);
        get_page(addr_virtual);
        trans_addr++;
    }

    fault_rate = faults / (double) trans_addr;
    hit_rate = buff_hits / (double) trans_addr;
    printf("\n\nTranslated addressed: %d | Page faults: %d | Fault rate: %.2f%% | TLB hits: %d | "
           "TLB hit rate: %.2f%%\n", trans_addr, faults, fault_rate, buff_hits, hit_rate);
    fclose(addr_file);
    fclose(store_file);
    return 0;
}

void error_check_main(int argc, char **argv) {
    if (argc != 2) {
        printf("wrong input\n");
        exit(1);
    }
    if ((store_file = fopen(BS_NAME, "rb")) == NULL) {
        printf("err opening backing store\n");
        exit(1);
    }

    if ((addr_file = fopen(argv[1], "r")) == NULL) {
        printf("err reading addresses\n");
        exit(1);
    }

}

void get_page(int addr) {
    int this_frame = INIT;
    int this_page = ((addr & MASK_ADDR) >> BYTE);
    int this_offset = (addr & MASK_OFFSET);

    for (int i = 0; i < SZ_BUFF; i++) {
        if (buff_pages[i] == this_page) {
            this_frame = buff_frames[i];
            buff_hits++;
        }
    }

    if (this_frame == INIT) {
        for (int i = 0; i < avail_entry_index; i++) {
            if (table_pages[i] == this_page) {
                this_frame = table_frames[i];
            }
        }
        if (this_frame == INIT) {
            read_store(this_page);
            faults++;
            this_frame = avail_frame_index - 1;
        }
    }

    buff_insert(this_page, this_frame);
    this_val = physical_mem[this_frame][this_offset];
    addr_physical = (this_frame << BYTE) | this_offset;
    printf("Virtual address: %d | Physical address %d | Value: %d\n", addr_virtual, addr_physical, this_val);
}

void buff_insert(int page, int frame) {
    int counter = 0;
    for (int i = 0; i < buff_entries; i++, counter++) {
        if (buff_pages[i] == page) {
            break;
        }
    }
    if (counter != buff_entries) {
        for (int i = counter; i < buff_entries - 1; i++) { // shift TLB array values right one
            buff_right_shift(i);
        }
        if (buff_entries < SZ_BUFF) { // enough room to add to TLB at the end
            buff_assign_end(frame, page);
            buff_entries++;
        } else { // not enough room to add to TLB at the end
            buff_assign_prev(frame, page);
        }

    } else { // at current number of TLB entries
        if (buff_entries < SZ_BUFF) { // enough room to add to TLB at the end
            buff_assign_end(frame, page);
            buff_entries++;
        } else {
            for (int i = 0; i < SZ_BUFF - 1; i++) { // shift TLB array values right one
                buff_right_shift(i);
            }
            buff_assign_prev(frame, page); // assign values at next to last index
        }
    }
}

/*
 * Assign values for buff_pages & buff_frames at the end of TLB
 */
void buff_assign_end(int frame, int page) {
    buff_frames[buff_entries] = frame;
    buff_pages[buff_entries] = page;

}

/*
 * Assign values for buff_pages & buff_frames at buff_entries -1
 */
void buff_assign_prev(int frame, int page) {
    buff_frames[buff_entries - 1] = frame;
    buff_pages[buff_entries - 1] = page;
}

/*
 * Shift TLB values right once
 */
void buff_right_shift(int index) {
    buff_frames[index] = buff_frames[index + 1];
    buff_pages[index] = buff_pages[index + 1];
}

void read_store(int page) {
    error_check_read_store(page);
    for (int i = 0; i < SZ_STORE; i++) {
        physical_mem[avail_frame_index][i] = bs_data[i];
    }
    table_pages[avail_entry_index] = page;
    table_frames[avail_entry_index] = avail_frame_index;

    avail_frame_index++;
    avail_entry_index++;
}

void error_check_read_store(int page) {
    if (fseek(store_file, page * SZ_STORE, SEEK_SET) != 0) {
        printf("Err looking in backing store\n");
    }

    if (fread(bs_data, sizeof(signed char), SZ_STORE, store_file) == 0) {
        printf("Err looking in backing store\n");
    }
}
