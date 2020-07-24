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

void sort(int nums[], int len);
void fileToArray(FILE *file);

int main(int argc, char** argv){    
    if (argc == 1) { // quit if no args supplied
        printf("no file name was supplied in the args.\n");
        return 0;
    }

    FILE* numFile = fopen(argv[1], "r"); // read number file    
    printf("the numbers from the file, sorted:\n\n");
    fileToArray(numFile);
    return 0;
}

// read file to int array, written w/ assumption that file only contains ints w/ one int per line
void fileToArray(FILE* file){ 
    int used, temp;
    int cap = 10; // intial array capacity
    int max = 1000; // max number of vals to read in
    int* nums = (int*) malloc(sizeof(int)*cap); // array to hold numbers from file

    while ((!feof(file)) && (used < max)){ // read number file and populate dynamically sized array.
        fscanf(file, "%d", &temp);
        if (used == cap) nums = (int*) realloc(nums, sizeof(int)*(cap * 2)); // change size of array
        nums[used] = temp; // put number in array
        used++;
    }
    
    sort(nums, used); // sort and print num array
    free(nums);

}

void sort(int nums[], int len){ // implementation of insertion sort
    int this, prev;
    for (int i = 0; i < len; i++){
        this = nums[i];
        prev = i - 1;

        // check that this is on at least 2nd iteration & only run if previous num is greather than current
        while ((prev >= 0) && (nums[prev] > this)){ 
            nums[prev + 1] = nums [prev]; // swap spots
            prev--; // look at number before
        }
        nums[prev + 1] = this;
    }


    int fcounter = 0; // counter used for output formatting    
    for (int i = 0; i < len; i++){
        if (i == (len - 1)) printf("%d\n", nums[i]);
        else if (fcounter == 19){
            printf("%d,\n", nums[i]);
            fcounter = 0;
        }
        else printf("%d, ", nums[i]);        
        fcounter++;
    }
}