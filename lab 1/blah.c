#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    
    FILE* numFile = fopen("nums.txt", "r");
    
    int used;
    int temp;
    int cap = 10;
    int *nums = (int*) malloc(sizeof(int)*cap);



    while ((!feof(numFile)) ){
        fscanf(numFile, "%d", &temp);
        printf("used: %d, cap: %d, temp val: %d\n", used, cap, temp);
        if (used == cap){
            cap = (cap + 30);
            nums = (int*) realloc(nums, sizeof(int)*cap);
        }

        nums[used] = temp;
        used++;

    }
    
    

    printf("arr len: %d\n", used);

    for (int i = 0; i < used; i++) printf("%d\n", nums[i]);
    free(nums);
    
    return 0;
}
