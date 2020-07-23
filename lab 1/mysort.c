#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    
    FILE* numFile = fopen("nums.txt", "r"); // read number file    
    int used, temp;
    int cap = 10; // intial array capacity
    int max = 1000; // max number of vals to read in
    int *nums = (int*) malloc(sizeof(int)*cap); // array to hold nums


    // read number file and populate dynamically sized array.
    while ((!feof(numFile)) && (used < max)){
        fscanf(numFile, "%d", &temp);
        // printf("used: %d, cap: %d, temp val: %d\n", used, cap, temp);
        
        
        // change size of array
        if (used == cap){
            cap = (cap * 2);
            nums = (int*) realloc(nums, sizeof(int)*cap);
        }

        nums[used] = temp; // put number in array
        used++;
    }
    
    

    
    for (int i = 0; i < used; i++) printf("%d\n", nums[i]);
    printf("arr len: %d\n", used);

    free(nums);
    
    return 0;
}
