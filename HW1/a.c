#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/* * * * * * * * * * * * * * * * * * * * * * 
* void linSearch()
* memory based linear search
 * * * * * * * * * * * * * * * * * * * * * */
void linSearch(int *keys, int sizeKeys, int *seeks, int sizeSeeks, int *hit)
{
	for (int i = 0; i < sizeSeeks; ++i)
    {
    	for (int j = 0; j < sizeKeys; ++j)
    	{
    		if (seeks[i] == keys[j]) { hit[i] = 1;}
    	}
    }
}

/* * * * * * * * * * * * * * * * * * * * * * 
* void dlinSearch()
* disk based linear search
 * * * * * * * * * * * * * * * * * * * * * */
void dlinSearch(char *keyFileName, int sizeKeys, int *seeks, int sizeSeeks, int *hit)
{
	FILE * fp = fopen(keyFileName,"rb");
	int key[1];
	for (int i = 0; i < sizeSeeks; ++i)
    {
    	long j = 0;
    	for (j = 0; j < sizeKeys; ++j)
    	{
    		fseek(fp,sizeof(int)*j,SEEK_SET);   //seek the key
			int ret_code = fread(key,sizeof(int),1,fp);  //read in the key
    		if (seeks[i] == key[0]) { hit[i] = 1;}
    	}
    }
}

/* * * * * * * * * * * * * * * * * * * * * * 
* void binSearch()
* memory based binary search
 * * * * * * * * * * * * * * * * * * * * * */
void binSearch(int *keys, int sizeKeys, int *seeks, int sizeSeeks, int *hit)
{
	for (int i = 0; i < sizeSeeks; ++i)
    {
    	int l = 0;
    	int r = sizeKeys-1;
    	while (l <= r)
    	{
    		int m = (l + r)/2;
    		if (seeks[i] == keys[m]) { hit[i] = 1;}
    		if (seeks[i] < keys[m]) {r = m - 1; }
    		else { l = m + 1; }
    	}
    }
}

/* * * * * * * * * * * * * * * * * * * * * * 
* void dbinSearch()
* disk based binary search
 * * * * * * * * * * * * * * * * * * * * * */
void dbinSearch(char *keyFileName, int sizeKeys, int *seeks, int sizeSeeks, int *hit)
{
	FILE * fp = fopen(keyFileName,"rb");
	int key[1];
	for (int i = 0; i < sizeSeeks; ++i)
    {
    	int l = 0;
    	int r = sizeKeys-1;
    	while (l <= r)
    	{
    		int m = (l + r)/2;
    		fseek(fp,sizeof(int)*m,SEEK_SET); //seek the key
			int ret_code = fread(key,sizeof(int),1,fp);  //read the key
    		if (seeks[i] == key[0]) { hit[i] = 1;}
    		if (seeks[i] < key[0]) {r = m - 1; }
    		else { l = m + 1; }
    	}
    }
}

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
* Array readFiletoArray()
* read binary file into integer array, return the array pointer
 * * * * * * * * * * * * * * * * * * * * * */
int * readFiletoArray(char *fileName)
{
	int *nums = NULL;
	FILE * readfile = fopen(fileName,"rb");
	if (readfile != NULL) {
	    if (fseek(readfile, 0L, SEEK_END) == 0) {
	        long bufsize = ftell(readfile);
	        nums = malloc(sizeof(int) * (bufsize + 1));
			fseek(readfile, 0L, SEEK_SET);
	        size_t len = fread(nums, sizeof(int), bufsize, readfile);
	    }
	    fclose(readfile);
	}
	return nums;
}

/* * * * * * * * * * * * * * * * * * * * * * 
* int getFileSize()
* read the array size of the file content
 * * * * * * * * * * * * * * * * * * * * * */
int getFileSize(char *fileName)
{
	long bufsize = 0;
	FILE * readfile = fopen(fileName,"rb");
	if (readfile != NULL) {
	    if (fseek(readfile, 0L, SEEK_END) == 0) {
	        bufsize = ftell(readfile);
	        bufsize /= sizeof(int);
	    fclose(readfile);
		}
	}
	return bufsize;
}

/* * * * * * * * * * * * * * * * * * * * * * 
* int main()
* main function, according input argument
 * * * * * * * * * * * * * * * * * * * * * */

int main(int argc, char * argv[])
{
    if (argc > 2)
    {
		struct timeval tms;
		struct timeval tme; 
		//read seeks into memory, initilize hit array
	    int sizeSeeks = getFileSize(argv[3]);
	    int *seeks = readFiletoArray(argv[3]);
		int *hit = malloc(sizeof(int) * (sizeSeeks + 1));
		
		//memory linear search
		if (strcmp(argv[1],"--mem-lin")==0)
		{
			gettimeofday( &tms, NULL );
			int sizeKeys = getFileSize(argv[2]);
	    	int *keys = readFiletoArray(argv[2]); 
			linSearch(keys, sizeKeys, seeks, sizeSeeks, hit);
			gettimeofday( &tme, NULL );
		}
		//memory binary search	
		if (strcmp(argv[1],"--mem-bin")==0)
		{
			gettimeofday( &tms, NULL );
			int sizeKeys = getFileSize(argv[2]);
	    	int *keys = readFiletoArray(argv[2]);  
			binSearch(keys, sizeKeys, seeks, sizeSeeks, hit);
			gettimeofday( &tme, NULL );
		}
	    //disk linear search
		if (strcmp(argv[1],"--disk-lin")==0)
		{
			gettimeofday( &tms, NULL );
			int sizeKeys = getFileSize(argv[2]);
			dlinSearch(argv[2], sizeKeys, seeks, sizeSeeks, hit);
			gettimeofday( &tme, NULL );
		}
		//disk binary search
		if (strcmp(argv[1],"--disk-bin")==0)
		{
			gettimeofday( &tms, NULL );
			int sizeKeys = getFileSize(argv[2]);
			dbinSearch(argv[2], sizeKeys, seeks, sizeSeeks, hit);
			gettimeofday( &tme, NULL );
		}
		//print out the results
		for (int i = 0; i < sizeSeeks; ++i)
		{
			if (hit[i] == 1){printf( "%12d: Yes\n", seeks[i] );}
			else {printf( "%12d: No\n", seeks[i] );}
		}
		//compute time cost for search
		struct timeval time_total; 
		time_total = timediff(tms, tme);
		printf( "Time: %ld.%06d", time_total.tv_sec, time_total.tv_usec );
	}
}
