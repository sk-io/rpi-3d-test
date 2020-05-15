#include "global.h"

#include <stdlib.h>
#include <stdio.h>

struct timespec timer_start() {
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

void print_temp() {
	float systemp, millideg;
	FILE *thermal;
	int n;

	thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
	n = fscanf(thermal,"%f", &millideg);
	fclose(thermal);
	systemp = millideg / 1000;

	printf("CPU temperature is %f degrees C\n", systemp);
}

char* read_file(const char* filename) {
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	char* buf = malloc(size + 1);
	fread(buf, 1, size, f);
	fclose(f);
	
	buf[size] = 0;
	return buf;
}
