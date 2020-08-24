#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_NUMS 20

int *ve_nums;
int total_nums;
int med, max, min, avg;


int *cmd_nums(int arg_count, char *args[]);
void *num_avg(void *void_nums);
void *max_val(void *void_nums);
void *min_val(void *void_nums);
void *med_val(void *void_nums);
void runner(int *nums);

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Need at least 2 numbers supplied in cmdline\n");
		exit(0);
	} else {
		ve_nums = cmd_nums(argc, argv);
		total_nums = argc - 1;
		runner(ve_nums);
		printf("The average is %d\nThe minimum is %d\nThe maximum is %d\nThe median is %d\n", avg, min, max, med);
	}
	return 0;
}

void runner(int *nums) {
	pthread_t avg_thread, max_thread, min_thread, med_thread;
	pthread_create(&avg_thread, NULL, num_avg, nums);
	pthread_create(&max_thread, NULL, max_val, nums);
	pthread_create(&min_thread, NULL, min_val, nums);
	pthread_create(&med_thread, NULL, med_val, nums);
	pthread_join(avg_thread, NULL);
	pthread_join(max_thread, NULL);
	pthread_join(min_thread, NULL);
	pthread_join(med_thread, NULL);
}

int *cmd_nums(int arg_count, char *args[]) {
	int iter = 1;
	int idx = 0;
	int *nums = malloc(sizeof(int) * MAX_NUMS);
	for (iter; iter < arg_count; iter++, idx++) {
		char *tmp = args[iter];
		if (strlen(tmp) == 0) {
			break;
		} else {
			int tmp_int = atoi(tmp);
			nums[idx] = tmp_int;
		}
	}
	return nums;
}

void *num_avg(void *void_nums) {
	int total = 0, count = 0;
	int *nums = (int *) void_nums;
	for (int i = 0; i < total_nums; i++) {
		int tmp = nums[i];
		if (tmp) {
			count++;
			total += tmp;
		}
	}
	avg = total / count;
	return 0;
}

void *max_val(void *void_nums) {
	max = 0;
	int *nums = (int *) void_nums;
	for (int i = 0; i < total_nums; i++) {
		int tmp = nums[i];
		if (max < tmp) { max = tmp; }
	}
	return 0;
}

void *min_val(void *void_nums) {
	int *nums = (int *) void_nums;
	min = nums[0];
	for (int i = 1; i < total_nums; i++) {
		int tmp = nums[i];
		if (min > tmp) { min = tmp; }
	}
	return 0;
}

void *med_val(void *void_nums) {
	int *nums = (int *) void_nums;
	int count = 1;
	for (int i = 1; i < total_nums; i++) {
		count++;
	}
	if (count % 2 == 0) {
		int half_count = count / 2;
		int f_num = nums[half_count - 1], s_num = nums[half_count];
		med = (f_num + s_num) / 2;
	} else {
		int mid_index = count / 2;
		med = nums[mid_index];
	}
	return 0;
}

