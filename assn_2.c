#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	long index_rec_key;
	int index_rec_off;
}index_S;

typedef struct
{
	int hole_size;
	int hole_off;
}availability;

int binarySearch(index_S *indexArray, long keyValue, int min, int max, int* indexLoc);
int compareIndexFunc(index_S* idx1, index_S* idx2);
int compareAvailFunc(availability* avail1, availability* avail2);
int holeForRecord( int recordLength, char *availListOrder, availability *avail_list, int *offsetLocationInHole, int *countAvailEntry);
int compareBestFitFunc(availability* avail1, availability* avail2);
int compareWorstFitFunc(availability* avail1, availability* avail2);


int main(int argc, char** argv)
{
	FILE *studentFile, *indexFile, *availFile;

	char readRecord[200],*availListOrder, *studentFileName;
	char *funcToPerform = NULL, *primaryKeyInString = NULL, *record = NULL;

	long primaryKeyInInt = 0, newRecordOffset = 0;

	int  recordLength = 0, countIndexEntry = 0, indexLocation = 0, i=0, countAvailEntry = 0;
	int  offsetLocationInHole = 0, keyInIndexFound = 0, holeInAvailListFound = 0, indexLocOfHole = 0;
	int  countTotalHoleSize = 0;

	index_S *primaryIndexList;
	availability *avail_list;


	primaryIndexList = 	(index_S*)malloc(10000*sizeof(index_S));
	avail_list = 		(availability*)malloc(10000*sizeof(availability));

	funcToPerform = 	(char*)malloc(sizeof(char)*100);
	primaryKeyInString= (char*)malloc(sizeof(char)*100);
	record = 			(char*)malloc(sizeof(char)*200);
	availListOrder = 	(char*)malloc(sizeof(char)*50);
	studentFileName = 	(char*)malloc(sizeof(char)*50);

	availListOrder = argv[1];
	studentFileName = argv[2];

	memset(primaryIndexList, 0, ( 10000*sizeof(index_S) ) );
	memset(avail_list, 0, ( 10000*sizeof(availability) ) );
	

	if(argc!=3)
	{
		printf("\nToo few arguments. Expected 2 arguments. Found only %d.\n", (argc-1));
		return 0;
	}
	

	if( (studentFile = fopen(studentFileName, "r+b")) == NULL )
	{
		studentFile = fopen(studentFileName, "w+b");
		indexFile = fopen( "index.db", "w+b");
		availFile = fopen( "avail.db", "w+b");
	}
	else
	{
		indexFile = fopen( "index.db", "r+b");
		availFile = fopen( "avail.db", "r+b");

		//read and store file contents in respective array

		//Index Array
		memset(primaryIndexList, 0, ( 10000*sizeof(index_S) ) );
		fread(&countIndexEntry, sizeof(int), 1, indexFile);
		fread(primaryIndexList, sizeof(index_S), countIndexEntry, indexFile);

		//Availability List Array
		memset(avail_list, 0, ( sizeof(availability)*10000 ) );
		fread(&countAvailEntry, sizeof(int), 1, availFile);
		fread(avail_list, sizeof(availability), countAvailEntry, availFile);

	}

	while( fgets(readRecord, 200, stdin)!=NULL )
	{
			funcToPerform = strtok(readRecord, " \n");
			if( strcmp(funcToPerform, "add") == 0 ) 
			{

				primaryKeyInString = strtok(NULL, " ");
				record = strtok(NULL, "\n");
				primaryKeyInInt = strtoll(primaryKeyInString, NULL, 10);

				keyInIndexFound = binarySearch(primaryIndexList, primaryKeyInInt, 0, countIndexEntry, &indexLocation);
				if( keyInIndexFound == 1 )
				{
					printf("Record with SID=%ld exists\n", primaryKeyInInt);
				}
				else if( keyInIndexFound == 0 )
				{
					//search for hole
					recordLength = (strlen(record));

					//search the availability list for an hole based on first fit best fit and worst fit
					offsetLocationInHole = countAvailEntry;
					holeInAvailListFound = holeForRecord( recordLength, availListOrder, avail_list, &offsetLocationInHole, &countAvailEntry );
					if( holeInAvailListFound == 1 )	//hole found
					{

						fseek( studentFile, offsetLocationInHole, SEEK_SET);
						fwrite( &recordLength, sizeof(int), 1, studentFile );
						
						fwrite( record, sizeof(char), strlen(record), studentFile );
						primaryIndexList[countIndexEntry].index_rec_key = primaryKeyInInt;
						primaryIndexList[countIndexEntry].index_rec_off = offsetLocationInHole;
					}
					else if( holeInAvailListFound == 0 )	//no hole found
					{
						//Write/append record to end of the file
						//add an entry to index holding key and offset
						
						fseek(studentFile, 0, SEEK_END);
						newRecordOffset = ftell(studentFile);

						fwrite( &recordLength, sizeof(int), 1, studentFile );
						fwrite( record, sizeof(char), strlen(record), studentFile );

						primaryIndexList[countIndexEntry].index_rec_key = primaryKeyInInt;
						primaryIndexList[countIndexEntry].index_rec_off = newRecordOffset;			
					}
					
					countIndexEntry++;
					qsort(primaryIndexList, countIndexEntry, sizeof(index_S), compareIndexFunc);

				}
			}
			else if( strcmp(funcToPerform, "find") == 0 )
			{
				primaryKeyInString = strtok(NULL, " \n");
				primaryKeyInInt = strtoll(primaryKeyInString, NULL, 10);
			
				keyInIndexFound = 0;
				keyInIndexFound = binarySearch(primaryIndexList, primaryKeyInInt, 0, countIndexEntry, &indexLocation);

				if( keyInIndexFound == 1 )
				{

					fseek( studentFile, primaryIndexList[indexLocation].index_rec_off, SEEK_SET);
					memset(record, 0, sizeof(record));

					fread(&recordLength, sizeof(int), 1, studentFile);
					fread(record, sizeof(char), recordLength, studentFile);
					record[recordLength] = '\0';

					printf("%s\n", record);

				}
				else if( keyInIndexFound == 0 )
				{
					//search for hole
					printf("No record with SID=%ld exists\n", primaryKeyInInt);
				}
			}
			else if( strcmp(funcToPerform, "del") == 0 )
			{
				primaryKeyInString = strtok(NULL, " ");
				primaryKeyInInt = strtoll(primaryKeyInString, NULL, 10);

				keyInIndexFound = 0;
				keyInIndexFound = binarySearch(primaryIndexList, primaryKeyInInt, 0, countIndexEntry, &indexLocation);

				if( keyInIndexFound == 1 )
				{
					//If an entry is found, use the entry's offset to locate and 
					//read the size of the record. Since the record is being deleted, 
					//it will form a hole in the student file. Add a new entry to the 
					//availability list containing the new hole's location and size. 
					//Remember, the size of the hole is the size of the record being 
					//deleted, plus the size of the integer preceding the record. 
					//Finally, remove the entry for the deleted record from the index.


					fseek( studentFile, primaryIndexList[indexLocation].index_rec_off, SEEK_SET);
					memset(record, 0, sizeof(record));
					fread(&recordLength, sizeof(int), 1, studentFile);

					avail_list[countAvailEntry].hole_size = (recordLength+sizeof(int));
					avail_list[countAvailEntry].hole_off = primaryIndexList[indexLocation].index_rec_off;

					countAvailEntry++;	//Increase avail list by one entry

					if( strcmp( availListOrder, "--best-fit" ) == 0 )
					{
						qsort(avail_list, countAvailEntry, sizeof(availability), compareBestFitFunc);
					}
					else if( strcmp( availListOrder, "--worst-fit" ) == 0 )
					{
						qsort(avail_list, countAvailEntry, sizeof(availability), compareWorstFitFunc);
					}

					//Delete the entry from index list
					for( i = indexLocation ; i < countIndexEntry; i++)
					{
						if( i != 10000)
							primaryIndexList[i] = primaryIndexList[i+1];
					}
					countIndexEntry--;
					qsort(primaryIndexList, countIndexEntry, sizeof(index_S), compareIndexFunc);

				}
				else if( keyInIndexFound == 0 )
				{
					//search for hole
					printf("No record with SID=%ld exists\n", primaryKeyInInt);
				}
			}
			else if( strcmp(readRecord, "end") ==0 )
			{
				printf("Index:\n");
				for( i=0; i<countIndexEntry; i++)
				{
					printf("key=%ld: offset=%d\n", primaryIndexList[i].index_rec_key, primaryIndexList[i].index_rec_off);
				}
				printf("Availability:\n");
				for( i=0; i<countAvailEntry; i++)
				{
					printf("size=%d: offset=%d\n", avail_list[i].hole_size, avail_list[i].hole_off);
					countTotalHoleSize += avail_list[i].hole_size;
				}
				
				printf("Number of holes: %d\n", countAvailEntry);
				printf("Hole space: %d\n", countTotalHoleSize);

				//Write the contents of avail and index to file
				indexFile = fopen( "index.db", "w+b");
				availFile = fopen( "avail.db", "w+b");

				//Index File writing
				fwrite(&countIndexEntry, sizeof(int), 1, indexFile);
				fwrite(primaryIndexList, sizeof(index_S), countIndexEntry, indexFile);
				fclose(indexFile);

				//Avail file writing
				fwrite(&countAvailEntry, sizeof(int), 1, availFile);
				fwrite(avail_list, sizeof(availability), countAvailEntry, availFile);
				fclose(availFile);

				return(0);
			}
	}
	return(0);
}


int binarySearch(index_S *indexArray, long keyValue, int min, int max, int* indexLoc )
{
	int midVal = 0;
	*indexLoc = -1;

	while(max>=min)
	{
		//printf("\nBinary Search");
		midVal = ((max+min)/2);

		if ( indexArray[midVal].index_rec_key == keyValue)
		{
			*indexLoc = midVal;

			return(1);
		}
		else if ( indexArray[midVal].index_rec_key < keyValue)
			min = midVal+1;
		else
			max = midVal - 1;
	}

	return(0);
}


int compareIndexFunc(index_S* idx1, index_S* idx2)
{
	return ( idx1->index_rec_key - idx2->index_rec_key );
}

int compareAvailFunc(availability* avail1, availability* avail2)
{
	return ( avail1->hole_size - avail2->hole_size );
}


int compareBestFitFunc(availability* avail1, availability* avail2)
{
	if( avail1->hole_size == avail2->hole_size)
		return( avail1->hole_off - avail2->hole_off );
	else
		return ( avail1->hole_size - avail2->hole_size );
}




int compareWorstFitFunc(availability* avail1, availability* avail2)
{
	if( avail1->hole_size == avail2->hole_size)
		return( avail1->hole_off - avail2->hole_off );
	else
		return ( avail2->hole_size - avail1->hole_size );
}




int holeForRecord( int recordLength, char *availListOrder, availability *avail_list, int *offsetLocationInHole, int *countAvailEntry)
{
	int i = 0, tempHoleSize = 0, tempHoleOffset = 0, indexLocOfHole = -1, j=0;
	recordLength = recordLength + sizeof(int);


	for( i = 0; i < (*countAvailEntry); i++)
	{
		if( avail_list[i].hole_size >= recordLength )
		{
			indexLocOfHole = i;
			break;
		}
	}

	if( indexLocOfHole != -1 )
	{
		//Hole Found @ i
		tempHoleSize = avail_list[indexLocOfHole].hole_size;
		tempHoleOffset = avail_list[indexLocOfHole].hole_off;

		if( tempHoleSize == recordLength )	//means new record and old are same length, so delete and no new hole made
		{
			for( i = indexLocOfHole; i < (*countAvailEntry); i++ )
			{
				if( i != 10000)
					avail_list[i] = avail_list[i+1];
			}
			(*countAvailEntry)--;
		}
		else if( tempHoleSize > recordLength )
		{				
			//First saving new off and size of hole to last location
			avail_list[(*countAvailEntry)].hole_size = (tempHoleSize - recordLength );
			avail_list[(*countAvailEntry)].hole_off = (tempHoleOffset + recordLength);

			//Now shifting each entry to left by one
			for( i = indexLocOfHole; i < (*countAvailEntry); i++ )
			{
				if( i != 10000)
					avail_list[i] = avail_list[i+1];
			}
			
		}
		*offsetLocationInHole = tempHoleOffset;

		if( strcmp( availListOrder, "--best-fit" ) == 0 )
		{
			qsort(avail_list, (*countAvailEntry), sizeof(availability), compareBestFitFunc);
		}
		else if( strcmp( availListOrder, "--worst-fit" ) == 0 )
		{
			qsort(avail_list, (*countAvailEntry), sizeof(availability), compareWorstFitFunc);
		}

		return(1);
	}
	else
		return(0);
}