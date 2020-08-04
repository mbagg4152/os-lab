/*
    Maggie Horton
    CS-451
    Summer 2020
    mysort.c: takes in number file name in args, reads file to array, 
    sorts then outputs the sorted data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 1000

void sort(int nums[], int len);


int main(int argc, char **argv) {
    if (argc == 1) { // quit if no args supplied
        printf("no file name was supplied in the args.\n");
        return 0;
    }

    int stored = 0, temp; // dont need stored, prob keep temp but rename
    int cap = 10; // initial array capacity
    int *nums = (int *) malloc(sizeof(int) * cap); // array to hold numbers from file
    FILE *file = fopen(argv[1], "r"); // read number file

    while ((!feof(file)) && (stored < MAX)) { // read number file and populate dynamically sized array.
        fscanf(file, "%d", &temp);
        if (stored == cap) {
            cap = cap * 2;
            nums = (int *) realloc(nums, sizeof(int) * (cap));
        }
        nums[stored] = temp; // put number in array
        stored++;
    }

    sort(nums, stored); // sort and print num array
    free(nums);
    return 0;
}


void sort(int nums[], int len) { // implementation of insertion sort
    int this, prev;
    for (int i = 0; i < len; i++) {
        this = nums[i];
        prev = i - 1;

        // check that this is on at least 2nd iteration & only run if previous num is greater than current
        while ((prev >= 0) && (nums[prev] > this)) {
            nums[prev + 1] = nums[prev]; // swap spots
            prev--; // look at number before
        }
        nums[prev + 1] = this;
    }

    for (int i = 0; i < len; i++) {
        printf("%d\n", nums[i]);
    }
}
