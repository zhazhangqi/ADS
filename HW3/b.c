/* * * * * * * * * * * * * * * * * * * * * *
* CSC 541 Assignment 3
* By Zhangqi Zha zzha@ncsu.edu
 * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <math.h>
#define SIZE 1000
#define PP 0
#define MULSIZE 15
/* * * * * * * * * * * * * * * * * * * * * *
* timeval timediff()
* Calculate two timeval difference, return timeval
 * * * * * * * * * * * * * * * * * * * * * */
struct timeval timediff(struct timeval tms, struct timeval tme)
{
	struct timeval tm;
	tm.tv_usec = tme.tv_usec - tms.tv_usec;
	tm.tv_sec = tme.tv_sec - tms.tv_sec;
	if (tm.tv_usec < 0)
	{
		tm.tv_usec += 1000000;
		tm.tv_sec -= 1;
	}
	return tm;
}

/* * * * * * * * * * * * * * * * * * * * * *
* int cmpfunc()
* compare function of items in two pointer
 * * * * * * * * * * * * * * * * * * * * * */
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

/* * * * * * * * * * * * * * * * * * * * * *
* long getRuns()
* load the input file, split into runs, return total number of runs
 * * * * * * * * * * * * * * * * * * * * * */
long getRuns(char *filename)
{
	int input_buffer[SIZE];
	FILE *fp = fopen(filename, "r");
	long i = 0;
	if (fp != NULL) {
	    if (fseek(fp, 0L, SEEK_END) == 0) {
	        long bufsize = ftell(fp)/sizeof(int);
			printf("total size is: %ld\n", bufsize);

	     	char *runfilename = malloc((strlen(filename) + 4) * sizeof(char));

	     	for (i = 0; i < bufsize; i = i + SIZE)
	     	{
	     		int count = SIZE;
	     		if (i + SIZE > bufsize){count = bufsize - i;}
	     		fseek(fp, sizeof(int)*i, SEEK_SET);
	     		size_t len = fread(input_buffer, sizeof(int), count, fp);
	     		qsort(input_buffer, (size_t) count, sizeof(int), cmpfunc);

	     		char *index = malloc(4 * sizeof(char));
        		sprintf(index, ".%03ld", i/SIZE);
        		strcpy(runfilename, filename);
        		strcat(runfilename, index);

	     		FILE *runFile;
    			runFile = fopen(runfilename, "wb" );
    			fwrite(input_buffer, sizeof(int), count, runFile);
    			fclose(runFile);

                if (PP)
                {
                    //print out data
                    printf("%s\n", runfilename);
                    for (int j = 0; j < count; ++j) {
                        printf("%d\n", input_buffer[j]);
                    }
                }
            }
	    }
	    fclose(fp);
	}
	return i/SIZE;
}

/* * * * * * * * * * * * * * * * * * * * * *
* void merge()
* load runs, then merge one by one until all runs exhaust
 * * * * * * * * * * * * * * * * * * * * * */

void merge(char *filename, int start, long numRuns, char *out_file_name)
{
    FILE *outFile;
    outFile = fopen(out_file_name, "wb");
	int *ptr[numRuns];   //array of pointers to indicate each runs current value to be compare
	int input_buffer[SIZE];
	int output_buffer[SIZE];
	int runSize = SIZE/numRuns;
	FILE *runFiles[numRuns];
	int runEnd[numRuns]; //array of integers to indicate each runs end location, at the end of runs, it could less than run size.

    //read all runs with runSize into input_buffer
	for (int i = 0; i < numRuns; ++i)
	{
	    char *runfilename = malloc((strlen(filename) + 4) * sizeof(char));
	    char *index = malloc(4 * sizeof(char));
		sprintf(index, ".%03d", i + start);
		strcpy(runfilename, filename);
		strcat(runfilename, index);

		runFiles[i] = fopen(runfilename, "r" );
		fseek(runFiles[i], 0L, SEEK_SET);
		size_t len = fread(input_buffer + i * runSize, sizeof(int), runSize, runFiles[i]);

		runEnd[i] = len;

		ptr[i] = input_buffer + i * runSize;
	}

	int empty_runs = 0;
	int index = 0;   //index to indicate location of output_buffer

	while (empty_runs < numRuns)
	{
        int min_val = INT_MAX;
		int index_minval = 0;

        for (int i = 0; i < numRuns; ++i) // compare of each runs current value
		{
			if (ptr[i] == NULL){ continue; }

            //this run empty, load more
			if (ptr[i] >= input_buffer + i * runSize + runEnd[i])
			{
				size_t len = fread(input_buffer + i * runSize, sizeof(int), runSize, runFiles[i]);

                if (len == 0)
				{
					ptr[i] = NULL;
					empty_runs++;
				}
				else
				{
					runEnd[i] = len;
					ptr[i] = input_buffer + i * runSize; // reset ptr
				}
			}

            if (ptr[i] != NULL && *ptr[i] < min_val)
			{
				min_val = *ptr[i];
				index_minval = i;
			}
		}

		output_buffer[index++] = min_val;
        ptr[index_minval]++;

        if (index == SIZE)
		{
			fwrite(output_buffer, sizeof(int), index, outFile);

            if (PP)
            {
                //print out data
                for (int j = 0; j < index; ++j) {
                    printf("%d\n", output_buffer[j]);
                }
            }

            index = 0;
		}
	}

    fwrite(output_buffer, sizeof(int), index-1, outFile);  //write index-1 integer into file instead of index

    if (PP)
    {
        //print out data
        for (int j = 0; j < index-1; ++j) {
            printf("%d\n", output_buffer[j]);
        }
    }

    fclose(outFile);

}

/* * * * * * * * * * * * * * * * * * * * * *
* void mult_merge()
* load runs, generate super runs first, then merge them until all runs exhaust
 * * * * * * * * * * * * * * * * * * * * * */
void mult_merge(char *filename, int numRuns, char *out_file_name)
{
    //create super runs
    int num_super_runs = (int) ceil((double)numRuns / MULSIZE);
    for (int j = 0; j < num_super_runs; ++j) {
            long merge_size = MULSIZE;
            if (j == num_super_runs-1)
            {
                merge_size = numRuns-j*MULSIZE;
            }
            char *super_filename;
            super_filename = malloc((strlen(filename) + 10) * sizeof(char));
            char *index = malloc(4 * sizeof(char));
            sprintf(index, ".%03d", j);
            strcpy(super_filename, filename);
            strcat(super_filename, ".super");
            strcat(super_filename, index);

            merge(filename, j * MULSIZE, merge_size, super_filename);
        }
    //merge super runs
    char *super_filename;
    super_filename = malloc((strlen(filename) + 6) * sizeof(char));
    strcpy(super_filename, filename);
    strcat(super_filename, ".super");
    merge(super_filename, 0, num_super_runs, out_file_name);
}

int main(int argc, char **argv)
{
	struct timeval tms;
	struct timeval tme;
	//get program start time stamps
	gettimeofday( &tms, NULL );

	long numRuns = getRuns("input.bin");
	merge("input.bin", 0, numRuns, "out.bin");
//    mult_merge("input.bin",numRuns, "outs.bin");
    // Basic Mergesort
//    if (strcmp(argv[1],"--basic")==0)
//    {
//        long numRuns;
//        numRuns = getRuns(argv[2]);
//	 	merge(argv[2], 0, numRuns, argv[3]);
//    }
//    //Multistep Mergesort
//    else if (strcmp(argv[1], "--multistep") == 0) {
//        long numRuns = getRuns(argv[2]);
//        mult_merge(argv[2], numRuns, argv[3]);
//    }
    // //Replacement selection mergesort
	// if (strcmp(argv[1],"--replacement")==0)
	// {
	// 	long numRuns = getRuns(argv[2]);
	// 	merge(argv[2], numRuns, argv[3]);
	// }
	//get program stop time stamps and calculate time elaspe
	gettimeofday( &tme, NULL );
	struct timeval time_total;
	time_total = timediff(tms, tme);
	printf( "Time: %ld.%06d \n", time_total.tv_sec, time_total.tv_usec );
	// printf("sssssMY TESTLEF\n");
	return 0;
}