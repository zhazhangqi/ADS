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


#define BUFFERSIZE 100
#define PP 0
#define MULSIZE 15
#define HEAPSIZE 75;
''
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

void sift(int *heap, int i, int n)
{
    int j, k, sm, tmp;
    while(i * 2 + 1 < n)
    {
        j = i * 2 + 1;
        k = j + 1;
        if(k < n && heap[k] < heap[j])
        {
            sm = k;
        }
        else
        {
            sm = j;
        }
        if(heap[i] < heap[sm])
        {
            return ;
        }
        tmp = heap[i];
        heap[i] = heap[sm];
        heap[sm] = tmp;
        i = sm;
    }
}
void heapify(int *heap, int n)
{
    for(int i = n / 2; i >= 0; i--)
    {
        sift(heap, i, n);
    }
}
void print_array(char *start_name, int *array, int array_start, int array_size)
{
    printf("%s\n", start_name);
    for (int j = array_start; j < array_size; ++j)
    {
        printf("%d\n", array[j]);
    }
}
/* * * * * * * * * * * * * * * * * * * * * *
* long getRuns()
* load the input file, split into runs, return total number of runs
 * * * * * * * * * * * * * * * * * * * * * */
long getRuns(char *filename)
{
    int input_buffer[BUFFERSIZE];
    FILE *fp = fopen(filename, "r");
    long i = 0;
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp)/sizeof(int);
            printf("total size is: %ld\n", bufsize);

            char *runfilename = malloc((strlen(filename) + 4) * sizeof(char));

            for (i = 0; i < bufsize; i = i + BUFFERSIZE)
            {
                int count = BUFFERSIZE;
                if (i + BUFFERSIZE > bufsize){count = bufsize - i;}
                fseek(fp, sizeof(int)*i, SEEK_SET);
                size_t len = fread(input_buffer, sizeof(int), count, fp);
                qsort(input_buffer, (size_t) count, sizeof(int), cmpfunc);

                char *index = malloc(4 * sizeof(char));
                sprintf(index, ".%03ld", i/BUFFERSIZE);
                strcpy(runfilename, filename);
                strcat(runfilename, index);

                FILE *runFile;
                runFile = fopen(runfilename, "wb" );
                fwrite(input_buffer, sizeof(int), count, runFile);
                fclose(runFile);

                if (PP){
                    print_array(runfilename, input_buffer, 0, count);
                }
            }
        }
        fclose(fp);
    }
    return i/BUFFERSIZE;
}

/* * * * * * * * * * * * * * * * * * * * * *
* long get_RS_runs()
* load the input file, using replacement selection method to split into runs, return total number of runs
 * * * * * * * * * * * * * * * * * * * * * */
long get_RS_runs(char *filename)
{
    int input_buffer[BUFFERSIZE];
    int output_buffer[BUFFERSIZE];
    FILE *fp = fopen(filename, "r");
    long i = 0;
    int run_index = 0;
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            // read out the length of file
            long buffer_size = ftell(fp)/sizeof(int);
            printf("total size is: %ld\n", buffer_size);

            int bytes_read, heap_end;
            //read 750 bytes first, if there are 750 bytes in the file
            fseek(fp, 0L, SEEK_SET);
            size_t len = fread(input_buffer, sizeof(int), HEAPSIZE, fp);
            heapify(input_buffer, len);
            bytes_read = (int)len;
            heap_end = (int)len-1;
            //initial input_buffer parameters
            int buffer_index = BUFFERSIZE;
            int buffer_end = BUFFERSIZE;  //buffer side end index

            while (bytes_read <= buffer_size)  //there are left bytes in the file
            {
                //generate run file name
                char *run_file_name = malloc((strlen(filename) + 4) *sizeof(char));
                char *index = malloc(4 * sizeof(char));
                sprintf(index, ".%03d", run_index);
                strcpy(run_file_name, filename);
                strcat(run_file_name, index);
                FILE *out_run_file = fopen(run_file_name,"wb");

                heap_end = HEAPSIZE - 1;
                int output_index = 0;
                while (heap_end >= 0)
                {
                    //sift the heap into order
                    sift(input_buffer, 0, heap_end+1);
                    //if output buffer is full, write to file, reset the index
                    if (output_index >= BUFFERSIZE)
                    {
                        fwrite(output_buffer, sizeof(int), BUFFERSIZE, out_run_file);
                        output_index = 0;
                    }

                    //if input buffer reaches the end, load more from file
                    if (buffer_index >= buffer_end)
                    {
                        size_t len = fread(input_buffer + HEAPSIZE, sizeof(int), BUFFERSIZE-HEAPSIZE, fp);
                        bytes_read += (int)len;
                        buffer_index = HEAPSIZE;
                        buffer_end = HEAPSIZE + (int)len;
                        if (len == 0){
                            int temp = 0;
                            qsort(input_buffer, (size_t)(heap_end+1) , sizeof(int), cmpfunc);
                            while (temp <= heap_end){
                                output_buffer[output_index++] = input_buffer[temp++];
                            }
                            temp = 0;
                            int temp2 = heap_end+1;
                            while (temp2 < HEAPSIZE) {
                                input_buffer[temp++] = input_buffer[temp2++];
                            }
//                            printf("break heap_end %d\n", heap_end);
//                            print_array("--------",input_buffer, 0, BUFFERSIZE);

                            fwrite(output_buffer, sizeof(int), (size_t)output_index, out_run_file);
                            fclose(out_run_file);
                            run_index++;

                            qsort(input_buffer,  (size_t)(heap_end+1), sizeof(int), cmpfunc);
                            char *last_run_file_name = malloc((strlen(filename) + 4) * sizeof(char));
                            char *last_index = malloc(4 * sizeof(char));
                            sprintf(last_index, ".%03d", run_index);
                            strcpy(last_run_file_name, filename);
                            strcat(last_run_file_name, last_index);
                            out_run_file = fopen(run_file_name, "wb");
                            fwrite(input_buffer, sizeof(int), (size_t) heap_end, out_run_file);
                            fclose(out_run_file);

                            return run_index;
                        }
                    }

                    //put the top of heap into output buffer, then load one bytes to heap or decrease heap by one
                    output_buffer[output_index++] = input_buffer[0];
                    if (input_buffer[0] <= input_buffer[buffer_index])
                    {
                        input_buffer[0] = input_buffer[buffer_index++];
                    } else
                    {
                        input_buffer[0] = input_buffer[heap_end];
                        input_buffer[heap_end--] = input_buffer[buffer_index++];
                    }

                }

                fwrite(output_buffer, sizeof(int), (size_t)output_index, out_run_file);
                fclose(out_run_file);
                run_index++;
//                print_array(run_file_name,output_buffer, 0, output_index);
//                printf("heap_end %d\n", heap_end);
//                printf("bytes_read %d\n", bytes_read);
            }
            if ( heap_end >= 0) {
                qsort(input_buffer, sizeof(int), heap_end+1, cmpfunc);
                char *run_file_name = malloc((strlen(filename) + 4) * sizeof(char));
                char *index = malloc(4 * sizeof(char));
                sprintf(index, ".%03d", run_index);
                strcpy(run_file_name, filename);
                strcat(run_file_name, index);
                FILE *out_run_file = fopen(run_file_name, "wb");
                fwrite(input_buffer, sizeof(int), (size_t) heap_end, out_run_file);
                fclose(out_run_file);
                run_index++;
//                printf("last file %d\n", run_index);
//                print_array(run_file_name,output_buffer,0, heap_end);
            }
        }
        fclose(fp);
    }
    return run_index;
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
    int input_buffer[BUFFERSIZE];
    int output_buffer[BUFFERSIZE];
    int runSize = BUFFERSIZE/numRuns;
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

        if (index == BUFFERSIZE)
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

//  long numRuns = getRuns("input.bin");
    long numRuns = get_RS_runs("input.bin");
    merge("input.bin", 0, numRuns, "out1.bin");
//    mult_merge("input.bin",numRuns, "outs.bin");
    // Basic Mergesort
//    if (strcmp(argv[1],"--basic")==0)
//    {
//        long numRuns;
//        numRuns = getRuns(argv[2]);
//      merge(argv[2], 0, numRuns, argv[3]);
//    }
//    //Multistep Mergesort
//    else if (strcmp(argv[1], "--multistep") == 0) {
//        long numRuns = getRuns(argv[2]);
//        mult_merge(argv[2], numRuns, argv[3]);
//    }
    // //Replacement selection mergesort
    // if (strcmp(argv[1],"--replacement")==0)
    // {
    //  long numRuns = getRuns(argv[2]);
    //  merge(argv[2], numRuns, argv[3]);
    // }
    //get program stop time stamps and calculate time elaspe
    gettimeofday( &tme, NULL );
    struct timeval time_total;
    time_total = timediff(tms, tme);
    printf( "Time: %ld.%06d \n", time_total.tv_sec, time_total.tv_usec );
    // printf("sssssMY TESTLEF\n");
    return 0;
}