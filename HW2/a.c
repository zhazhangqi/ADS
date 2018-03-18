/* * * * * * * * * * * * * * * * * * * * * * 
* CSC 541 Assignment 2
* By Zhangqi Zha zzha@ncsu.edu
 * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define IDX "index.bin"
#define AVA "avail.bin"

typedef struct { 
	int key; /* Record's key */ 
	long off; /* Record's offset in file */ 
} index_S;

typedef struct { 
int siz; /* Hole's size */ 
long off; /* Hole's offset in file */ 
} avail_S;

//write index into index.db file
void writeIndex(index_S *prim, int lens)
{
    FILE *idxFile;
    idxFile = fopen( IDX, "wb" );
    fwrite(prim, sizeof( index_S ), lens, idxFile);
    fclose(idxFile);
}

//read index from file 
void readIndex(index_S *prim){
	FILE *idxFile = fopen(IDX, "rb");; /* input file stream */ 
	if (idxFile != NULL) {
	    if (fseek(idxFile, 0L, SEEK_END) == 0) {
	        long bufsize = ftell(idxFile);
			fseek(idxFile, 0L, SEEK_SET);
	        size_t len = fread(prim, sizeof(index_S), bufsize/sizeof(index_S), idxFile);
	    }
	    fclose(idxFile);
	}
}

//write avail list to file avail.db
void writeAvail(avail_S *ava, int lens)
{
    FILE *avaFile;
    avaFile = fopen( AVA, "wb" );
    fwrite(ava, sizeof(avail_S), lens, avaFile);
    fclose(avaFile);
}

//read avail list from file
void readAvail(avail_S *ava){
	FILE *avaFile = fopen(AVA, "rb");; /* input file stream */ 
	if (avaFile != NULL) {
	    if (fseek(avaFile, 0L, SEEK_END) == 0) {
	        long bufsize = ftell(avaFile);
			fseek(avaFile, 0L, SEEK_SET);
	        size_t len = fread(ava, sizeof(avail_S), bufsize/sizeof(avail_S), avaFile);
	    }
	    fclose(avaFile);
	}
}

// read file list length from file
int readLength(char *fileName, int size){
	FILE *fp = fopen(fileName, "rb");; /* input file stream */ 
	int length;
	if (fp != NULL) {
	    if (fseek(fp, 0L, SEEK_END) == 0) {
	        long bufsize = ftell(fp);
	        length = bufsize/size;
	    }
	    fclose(fp);
	}
	return length;
}

// binary search the index key, if not find, return -1
int findIdx(int key, index_S *index, int indexCT){
	int l = 0;
	int r = indexCT-1;
	while (l <= r)
	{
		int m = (l + r)/2;
		if (key == index[m].key) { return m;}
		else if (key < index[m].key) {r = m - 1; }
		else { l = m + 1; }
	}
	return -1;
}

// linerly search the first available hole index, if not return -1
int findHole(int size, avail_S *avail, int availCT){
	for (int i = 0; i < availCT; ++i)
	{
		if (avail[i].siz >= size){return i;}
	}
	return -1;
}
// comparetor to use for best fit method, which will be used by qsort to ascending order
int bestcmp(const void *a, const void *b)
{
    avail_S *A = (avail_S *)a;
    avail_S *B = (avail_S *)b;
    int res = A->siz - B->siz;
    if (res > 0) return 1;
    else if (res < 0) return -1;
    else return 0;
}
// comparetor to use for best worst method, which will be used by qsort to descending order
int worstcmp(const void *a, const void *b)
{
    avail_S *A = (avail_S *)a;
    avail_S *B = (avail_S *)b;
    int res = B->siz - A->siz;
    if (res > 0) return 1;
    else if (res < 0) return -1;
    else return 0;
}
// comparetor used for index sort, ascending order
int indexcmp(const void *a, const void *b)
{
    index_S *A = (index_S *)a;
    index_S *B = (index_S *)b;
    int res = A->key - B->key;
    if (res > 0) return 1;
    else if (res < 0) return -1;
    else return 0;
}

//wraped qsort according to best, worst, if first fit, no need sort
void zsort(avail_S *avail, int availCT, char *avaOrder){
	if (strcmp(avaOrder,"--best-fit")==0){
		// printf("bestfit order ascending\n");
		qsort(avail, (size_t) availCT, sizeof(avail_S), bestcmp);
	}
	else if (strcmp(avaOrder,"--worst-fit")==0){
		// printf("worstfit order descending\n");
		qsort(avail, (size_t) availCT, sizeof(avail_S), worstcmp);
	}
}

// when add a record, check how much left of the whole, then update the available list
int updateAvail(int size, int holeIdx, avail_S *avail, int availCT, char *avaOrder){
	int newsize = avail[holeIdx].siz - size;
	long offset = avail[holeIdx].off + size;
	
	for (int i = holeIdx; i < availCT - 1; ++i)
	{
		avail[i] = avail[i+1];	
	}
	//if there is not left over, just remove the avail counter
	if (newsize == 0) {
		availCT--;
	}
	else{
		avail[availCT-1].siz = newsize;
		avail[availCT-1].off = offset;
		zsort(avail, availCT, avaOrder);
	}
	return availCT;
}
//when adding a record, add key to index list, then reorder list to ascending order
int updateIndex(int key, long offset, index_S *index, int indexCT){
	indexCT++;
	index[indexCT-1].key = key;
	index[indexCT-1].off = offset;
	qsort(index, (size_t) indexCT, sizeof(index_S), indexcmp);
	return indexCT;
}
//when deleting a record, add the offset and size to avail list.
int addAvail(long offset, int size, avail_S *avail, int availCT, char *avaOrder){
	availCT++;
	avail[availCT-1].siz = size;
	avail[availCT-1].off = offset;
	zsort(avail, availCT, avaOrder);
	return availCT;
}
// when deleting a record, removeing the key form index list.
int delIndex(int key, int keyindex, index_S *index, int indexCT){
	indexCT--;
	for (int i = keyindex; i < indexCT; ++i)
	{
		index[i] = index[i+1];	
	}
	return indexCT;
}

//print the results
void printRes(avail_S *avail, index_S *index, int availCT, int indexCT, FILE *fp){
	printf("Index:\n");
    for(int i = 0; i < indexCT; i++)
    {
        printf( "key=%d: offset=%ld\n", index[i].key, index[i].off );
    }
    
    int numHole = 0;
    printf("Availability:\n");
    for(int i = 0; i < availCT; i++)
    {
        printf("size=%d: offset=%ld\n", avail[i].siz, avail[i].off);
        numHole = numHole + avail[i].siz;
    }
    printf("Number of holes: %d\n", availCT);
    printf("Hole space: %d\n", numHole);

 //    char *db = NULL;
 //    long bufsize;
 //    if (fseek(fp, 0L, SEEK_END) == 0) {
	//         bufsize = ftell(fp);
	//         db = malloc(bufsize);
	// 		fseek(fp, 0L, SEEK_SET);
	//         size_t len = fread(db, sizeof(char), bufsize/sizeof(char), fp);
	//     }
	
	// for (int i = 0; i < bufsize/sizeof(char); ++i)
	// {
	// 	printf("%c", db[i]);
	// }
	// printf("\n");

}
/* * * * * * * * * * * * * * * * * * * * * * 
* int main()
* main function, according input argument
 * * * * * * * * * * * * * * * * * * * * * */

int main(int argc, char * argv[])
{
    if (argc < 2){printf( "argument wrong!!");}
    else
    {
		char *method = argv[1];
		char *dbname = argv[2];
		int key; /* Buffer to hold student record SSID */ 
		FILE *fp; /* Input/output stream */ 
		long offset; /* Record offset */
		int keyindex; 
		int size; /* Record size, in characters */ 
		index_S index[10000];
    	avail_S avail[10000];
    	int indexCT = 0;
    	int availCT = 0;
		// if db exist, open and read coresponding index list and avail list from file, otherwise just creat one 
		if ( ( fp = fopen( dbname, "r+b" ) ) == NULL ) { 
			fp = fopen( dbname, "w+b" ); 
		} 
		else { 
			readIndex(index);
			readAvail(avail);
			indexCT = readLength(IDX, sizeof(index_S));
			availCT = readLength(AVA, sizeof(avail_S));
		}


		//reading the commands from stdin, process each line
		while (1){
			//reading one line commands, stripped the last 0x0a char.
			char *templine = NULL;
		    unsigned long len;
		    int read = getline(&templine, &len, stdin);
		    char *line = malloc(strlen(templine) - 1);
		    strncpy(line, templine, strlen(templine) - 1);

 			//parse line into vector of different conponents
 			// add 503571980 503571980|Short|Name|St
			// find 510542103
			// del 205163635
			// end
 			char *input[4];
 			char *token;
 			int i = 0;
            while ((token = strsep(&line, " ")) != NULL){
                   input[i] = token;
		           // printf("input is : %s, %lu\n", input[i], strlen(input[i]));
		           i++;
				}
	
			//do add command here
			if (strcmp(input[0],"add")==0){ 
				key = atoi(input[1]);
				size = sizeof(int) + sizeof(char) * strlen(input[2]);
				keyindex = findIdx(key, index, indexCT);
				//not in the db, need to add
				if (keyindex == -1){  
					int holeIdx = findHole(size, avail, availCT);
					//if find hole in avail list
					if (holeIdx != -1){ 
						offset = avail[holeIdx].off;
						availCT = updateAvail(size, holeIdx, avail, availCT, method);
						// printf("offset(hole) is %ld\n", offset);
					}
					else{
						fseek(fp, 0L, SEEK_END);
						offset = ftell(fp);
					}
					//seek the offset and write size and record to file
			        // printf("offset is %ld\n", offset);
			        indexCT = updateIndex(key, offset, index, indexCT);
			        fseek(fp, offset, SEEK_SET);
			        fwrite(&size, sizeof(int), 1, fp);
			        fseek(fp, offset+4, SEEK_SET);
			        fwrite(input[2], sizeof(char), (size_t) (strlen(input[2])), fp);
			        // char *readback;
			        // readback = malloc(28);
			        // fseek(fp, offset+4, SEEK_SET);  // every read or write need a seek
			        // size_t len = fread(readback, sizeof(char), 28, fp);
			        // printf("readback: %s\n", readback);
				}
				else printf("Record with SID=%d exists\n", key);
			}
			
			else if (strcmp(input[0],"find")==0){ //do find command here
				// printf("enter find command\n");
				key = atoi(input[1]);
				keyindex = findIdx(key, index, indexCT);
				if (keyindex != -1){ 
					offset = index[keyindex].off;
					int length;
					
					fseek(fp, offset, SEEK_SET);
					fread(&length, sizeof(int), 1, fp);
					
					fseek(fp, offset + 4, SEEK_SET);
    				char *record = malloc((size_t) (length - 3));
					fread(record, sizeof(char), (size_t) (length - 4), fp);
					record[length-4] = '\0';
					
					printf("%s\n", record);
				}
				else{printf("No record with SID=%d exists\n", key);}
			}
			
			else if (strcmp(input[0],"del")==0){
				key = atoi(input[1]);
				// printf("key is: %d\n", key);
				keyindex = findIdx(key, index, indexCT);
				// printf("keyindex is:%d\n", keyindex);
				// find key in index
				if (keyindex != -1){ 
					offset = index[keyindex].off;
					int length;
					fseek(fp, offset, SEEK_SET);
					fread(&length, sizeof(int), 1, fp);
					indexCT = delIndex(key, keyindex, index, indexCT);
					availCT = addAvail(offset,length, avail, availCT, method);
				}
				else{printf("No record with SID=%d exists\n", key);}
			}

			else if (strcmp(input[0],"end")==0){
				// printf("end command\n");
				break;
			}
			else if (strcmp(input[0],"pp")==0){
				printRes(avail, index, availCT, indexCT, fp);
			}
		}
	//ending program, close file, write avail and index list to file, and print the result.
	fclose(fp);
	writeAvail(avail, availCT);
	writeIndex(index, indexCT);
	printRes(avail, index, availCT, indexCT,fp);

    		
	}
	return 0;
}

