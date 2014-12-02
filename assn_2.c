#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	int noOfKeys;
	int *nodeKeys;
	long *nodeChildOffsets;

}bTreeNode;

long searchForKey(int primaryKeyInInt, long rootNodeInLong, int bTreeOrder, long* nodeOffset);
void insertIntoBTree(long rootNodeInLong, int primaryKeyInInt, int bTreeOrder);
void initializeNode(bTreeNode *node, int bTreeOrder);
void insertIntoBTree_SplitNode(long nodeInLong, int position, int bTreeOrder);
void insertIntoBtree_NonSplitNode(long nodeInLong, int primaryKeyInInt, int bTreeOrder);
void printBtree(long nodeInLong, int bTreeOrder);


int main(int argc, char *argv[])
{
	 char *inputFileName, *readLine, *funcToPerform = NULL, *primaryKeyInString = NULL;
	 int bTreeOrder = 0;
	 int primaryKeyInInt = 0, getVal = 0;
	 bTreeNode rootNode;
	 FILE *inputFile = NULL;
	 long rootNodeOffset = 0, tempNodeOffset = 0;


	inputFileName = argv[1];
	bTreeOrder = atoi(argv[2]);

	initializeNode(&rootNode, bTreeOrder);
	funcToPerform = 	(char*)malloc(sizeof(char)*100);
	primaryKeyInString= (char*)malloc(sizeof(char)*100);
	readLine = (char*)malloc(sizeof(char)*100);
	
	
	//if( (inputFile = fopen(inputFileName, "r+b")) == NULL )
	{
		//File doesn't exist, Open in write mode
		inputFile = fopen(inputFileName, "w+b");
		//rootNodeOffset = -1;
		rootNodeOffset = (long)&rootNode;
		printf("\nRoot Node : %ld", &rootNode);
		printf("\nRoot Node Offset : %ld", rootNodeOffset);

	}
	//else
	{
		//read the index file data into btreeNode
		//but first read root node offset
		//fread(&rootNodeOffset, sizeof(long), 1, inputFile);
	}

	
	while( (fgets(readLine, 100, stdin)) != NULL )
	{
		funcToPerform = strtok(readLine, " ");
		printf("\n--------------------------\nFunc to perform val: %s\n", funcToPerform);
		if( strcmp(funcToPerform, "add") == 0 )
		{
			
			primaryKeyInString = strtok(NULL, "\n");
			printf("\nGoing to check file..add %s", primaryKeyInString);
			
			printf("\nGoing to check file10\n");
			
			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);
			
			
			tempNodeOffset = 0;
			
			printf("Calling search\n");
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, &tempNodeOffset);
			printf("Finishing search, getVal: %d & tempNodeOffset: %ld\n", getVal, tempNodeOffset);
			//printf("Finishing search, getVal: %d & tempNodeOffset: %ld\n", getVal, tempNodeOffset);
			//printf("Finishing search, getVal: %d & tempNodeOffset: %ld\n", getVal, tempNodeOffset);
			if( (getVal == -1) && (tempNodeOffset != -2))
			{
				//means key not found or pointer to next level is being returned
				printf("\nCalling INSERT() from MAIN() with root long: %ld", rootNodeOffset);
				insertIntoBTree(rootNodeOffset, primaryKeyInInt, bTreeOrder);
				printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");
				printBtree(rootNodeOffset, bTreeOrder);
				printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");
			}
			else
			{
				printf("Entry with key=%d already exists\n", primaryKeyInInt);
			}
		}
		else if( strcmp(funcToPerform, "find") == 0 )
		{
			primaryKeyInString = strtok(NULL, "\n");
			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);
			
			tempNodeOffset = 0;
			
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, &tempNodeOffset);
			printf("Finishing search, getVal: %d & tempNodeOffset: %ld\n", getVal, tempNodeOffset);
			if( (getVal == -1) && (tempNodeOffset != -2))
			{
				//means key not found or pointer to next level is being returned
				printf("Entry with key=%d does not exist\n", primaryKeyInInt);
			}
			else
			{
				printf("Entry with key=%d exists\n", primaryKeyInInt);
			}
		}

		
	}


}


void initializeNode(bTreeNode *node, int bTreeOrder)
{
	node->noOfKeys = 0;
	node->nodeKeys = (int*)calloc((bTreeOrder-1), sizeof(int));
	node->nodeChildOffsets = (long*)calloc(bTreeOrder, sizeof(long));
	
	printf("\n\n\nINITIALIZE()\n");
	printf("\n node->childoffset: %ld", node->nodeChildOffsets);
	printf("\n node->childoffset[0]: %ld\n\n\n", node->nodeChildOffsets[0]);
}



long searchForKey(int primaryKeyInInt, long rootNodeInLong, int bTreeOrder, long* nodeOffset)
{
	
	int s = 0;

	bTreeNode *rootNode = (bTreeNode*)rootNodeInLong;

	//printf("\nBreak..rootNodeInLong Val ..%ld", rootNodeInLong);
	//printf("\nBreak..rootNode Val %ld", rootNode);
	//printf("\nBreak..key Val %d", rootNode->noOfKeys);
	
	while( s< (rootNode->noOfKeys) )
	{
		//printf("\nWhile..%d", s);
		if( primaryKeyInInt == rootNode->nodeKeys[s] )
		{
			*nodeOffset = -2;
			return((long)rootNode->nodeChildOffsets[s]);
		}
		else if( primaryKeyInInt < rootNode->nodeKeys[s])
			break;
		else
			s++;
	}

	//printf("\nBreak..AILA ");
	
	if (rootNode->nodeChildOffsets[s] != 0 )
		return(searchForKey(primaryKeyInInt, (long)rootNode->nodeChildOffsets[s], bTreeOrder, nodeOffset));
	else
	{
		*nodeOffset = (long)rootNode;
		return(-1);
	}

}




void insertIntoBTree(long rootNodeInLong, int primaryKeyInInt, int bTreeOrder)
{

	bTreeNode *rootNode = (bTreeNode*)rootNodeInLong;
	printf("\nInside insert main()");
	//printf("\nInside insert main()");
	
	if( rootNode->noOfKeys == (bTreeOrder-1) )
	{
		bTreeNode newRootNode;
		initializeNode(&newRootNode, bTreeOrder);
		printf("\nSPLIT()%%%%%%%%%%%%%%%%% : node new keys : %d", newRootNode.noOfKeys);
		newRootNode.nodeChildOffsets[0] = (long)rootNode;
		//make newrightchild node as root node

		insertIntoBTree_SplitNode( (long)&newRootNode, 0, bTreeOrder);
		printf("\nSPLIT()%%%%%%%%%%%%%%%%% : node new keys : %d", newRootNode.noOfKeys);
		printf("\n\nHELLO THERE\n\nSPLIT NODE()-----*******   newRootNode.nodeChildOffsets: %ld",newRootNode.nodeChildOffsets);
		printf("\nSPLIT NODE() ============   newRootNode.nodeChildOffsets[1]: %ld", newRootNode.nodeChildOffsets[1]);
		printf("\nSPLIT NODE() ============   (newRootNode.nodeChildOffsets[1])->nodeChildOffsets: %ld", ((bTreeNode*)(newRootNode.nodeChildOffsets[1]))->nodeChildOffsets);
		printf("\nSPLIT NODE() ============   (newRootNode.nodeChildOffsets[1])->nodeKeys: %ld", ((bTreeNode*)(newRootNode.nodeChildOffsets[1]))->nodeKeys);
		//printf("\nSPLIT NODE() ============   (newRootNode.nodeChildOffsets[1])->nodeChildOffsets: %ld", ((bTreeNode*)(newRootNode.nodeChildOffsets[1]))->nodeChildOffsets);
		
	
		//printBtree((long)&newRootNode, bTreeOrder);
		insertIntoBtree_NonSplitNode( (long)&newRootNode, primaryKeyInInt, bTreeOrder);

	}
	else
	{
		printf("\nInsert to non split(): %ld, %ld", rootNodeInLong, (long)rootNode);
		insertIntoBtree_NonSplitNode( (long)rootNode, primaryKeyInInt, bTreeOrder);
	}
}



void insertIntoBTree_SplitNode(long nodeInLong, int position, int bTreeOrder)
{
	bTreeNode *node = (bTreeNode*)nodeInLong;

	int midValue = 0, i = 0;
	bTreeNode *childNode = NULL;
	bTreeNode newRightNode;
	
	printf("\nInside insert Split Node()");
	printf("\nInside insert Split Node()");
	
	initializeNode(&newRightNode, bTreeOrder);
	midValue = (bTreeOrder/2);
	newRightNode.noOfKeys = ((bTreeOrder-1)- midValue -1);
	printf("\nSPLIT NODE()-----*******   newrightchild->noofkeys: %d  -----------", newRightNode.noOfKeys);

	childNode = (bTreeNode*)node->nodeChildOffsets[position];

	printf("\nInside insert Split Node()+++++++++++++++");
	printf("\nInside insert Split Node()");
	
	for( i = 0; i<(newRightNode.noOfKeys); i++)
	{
		printf("\nInside insert Split Node()>>>>>>>>>>>>>>>>>>> %d, mid : %d", i, (midValue+i));
		printf("\nInside insert Split Node()");
		newRightNode.nodeKeys[i] = childNode->nodeKeys[midValue+i+1];
	}

	printf("\nInside insert Split Node()-------------------");
	printf("\nInside insert Split Node()");
	
	
	if(childNode->nodeChildOffsets[0] != 0)
	{
		printf("\n*********************************");
		printf("\n=================================");
		for( i = 0; i<midValue; i++)
		{
			newRightNode.nodeChildOffsets[i] = childNode->nodeChildOffsets[midValue+i];
		}
	}

	printf("\nInside insert Split Node()=======================");
	printf("\nInside insert Split Node()");
	
	childNode->noOfKeys = (midValue-1);
	
	for( i = (node->noOfKeys); i>position; i--)
	{
		node->nodeChildOffsets[i+1] = node->nodeChildOffsets[i];
	}

	node->nodeChildOffsets[i+1] = (long)&newRightNode;
	printf("\n\nSPLIT NODE()-----*******   Root Node child right offset[%d]: %ld", (i+1), node->nodeChildOffsets[i+1]);
	printf("\nSPLIT NODE()-----*******   Root Node child right offset->offset[%d]: %ld", (i+1), ((bTreeNode*)(node->nodeChildOffsets[i+1]))->nodeChildOffsets);
	printf("\nSPLIT NODE(): .......... newRightNode->childoffset... %ld, keys %ld, address: %ld", newRightNode.nodeChildOffsets, newRightNode.nodeKeys, &(newRightNode.nodeChildOffsets));
	printf("\nSPLIT NODE()-----*******   newrightchild address: %ld\n\n", &newRightNode);
	for( i =(node->noOfKeys-1); i>=position; i-- )
	{
		node->nodeKeys[i+1] = node->nodeKeys[i];
	}

	node->nodeKeys[i+1] = childNode->nodeKeys[midValue];
	node->noOfKeys = (node->noOfKeys + 1);
	printf("\nSPLIT()@@@@@@@@@@@@@@@@ new root node keys now: %d", node->noOfKeys);

}

void insertIntoBtree_NonSplitNode(long nodeInLong, int primaryKeyInInt, int bTreeOrder)
{

	int i = 0;
	bTreeNode *childOffset = NULL;
	bTreeNode *node = (bTreeNode*)nodeInLong;
	
	printf("\nInside insert Node ---- No Split(): %ld", node);
	
	printf("\nInside insert Node ---- No Split(): %ld, keys: %ld", node->nodeChildOffsets, &(node->noOfKeys));
	printf("\nInside insert Node ---- No Split(): %ld", node);
	
	if( node->nodeChildOffsets[0] == 0 )	//means the node is a leaf, it has no child
	{
		i = (node->noOfKeys - 1);
		printf("\n ZERO child Offsets(): BEFORE While");
		printf("\n ZERO child Offsets(): BEFORE While");
		while( (i >= 0 ) && (node->nodeKeys[i] > primaryKeyInInt ) ) //shifting values > primarykey to one position right
		{
			node->nodeKeys[i+1] = node->nodeKeys[i];
			i--;
		}

		printf("\n ZERO child Offsets(): After While");
		printf("\n ZERO child Offsets(): After While");
		node->nodeKeys[i+1] = primaryKeyInInt;
		node->noOfKeys = (node->noOfKeys + 1);

		//write to file
	}
	else
	{
		i = (node->noOfKeys - 1);
		printf("\n GREater > ZERO child Offsets(): BEFORE While");
		//printf("\n GREater > ZERO child Offsets(): BEFORE While");
		while( (i >= 0 ) && (node->nodeKeys[i] > primaryKeyInInt ) ) //finding the right offset to put key value into
		{
			i--;
		}
		i++;

		printf("\nValue if %d position to insert: ", i);
		//read from file the offset of node at i
		childOffset = (bTreeNode*)node->nodeChildOffsets[i];
		printf("\nRoot Node child right offset: %ld", childOffset);
		printf("\nRoot Node child right offset: %ld", childOffset);
		printf("\nYes PROBLEM");
		printf("\nYes PROBLEM, node child offset[%d...].... %ld, .... %ld", i, node->nodeChildOffsets[i], ((bTreeNode*)(node->nodeChildOffsets[1]))->nodeChildOffsets);
		printf("\nRoot Node child right no of keys____: %ld", &(childOffset->noOfKeys));
		printf("\nNO PROBLEM!!!");
		printf("\nNO PROBLEM!!!");
		printf("\nRoot Node child right offset: %ld, keys %ld, address: %ld", (childOffset->nodeChildOffsets), (childOffset->nodeKeys), &(childOffset->nodeChildOffsets));
		printf("\nNO PROBLEM");
		printf("\nNO PROBLEM");

		if( childOffset->noOfKeys == (bTreeOrder-1) )
		{
			insertIntoBTree_SplitNode( (long)node, i, bTreeOrder);
			if( primaryKeyInInt > node->nodeKeys[i])
				i++;
		}
		printf("\nNO PROBLEM.....----------");
		printf("\nNO PROBLEM");
		insertIntoBtree_NonSplitNode( (long)childOffset, primaryKeyInInt, bTreeOrder);
	}
}



void printBtree(long nodeInLong, int bTreeOrder)
{
	int i = 0;
	bTreeNode *node = (bTreeNode*)nodeInLong;
	
	printf("\nNode address: %ld", node);
	printf("\nNode NoOfKeys %d, and &node->noOfKeys: %ld", node->noOfKeys, &(node->noOfKeys) );
	for( i =0; i< node->noOfKeys; i++)
	{
		printf("\nNode Keys[%d]: %d, and address: %ld", i, node->nodeKeys[i], &(node->nodeKeys[i]) );
		//printf("\nNode childOffsets[%d]: %ld, and address: %ld", i, node->nodeChildOffsets[i], &(node->nodeChildOffsets[i]) );
		if( i > 4 )
		{
			printf("\nITS TOO MUCH>>>BREAKING");
			break;
		}
	}
	for( i =0; i<= node->noOfKeys; i++)
	{
		//printf("\nNode Keys[%d]: %d, and address: %ld", i, node->nodeKeys[i], &(node->nodeKeys[i]) );
		printf("\nNode childOffsets[%d]: %ld, and address: %ld", i, node->nodeChildOffsets[i], &(node->nodeChildOffsets[i]) );
		if( i > 4 )
		{
			printf("\nITS TOO MUCH>>>BREAKING");
			break;
		}
	}
	
	for( i =0; i<= node->noOfKeys; i++)
	{
		printf("\nCalling Node Child Offsets");
		if( node->nodeChildOffsets[i] != 0)
			printBtree((long)node->nodeChildOffsets[i], bTreeOrder);
		if( i > 4 )
		{
			printf("\nITS TOO MUCH>>>BREAKING");
			break;
		}
	}
}


/*int insertIntoBTreeLeaf(int primaryKeyInInt, long *rootNode, int bTreeOrder)
{
	
	int s = 0, i=0, newKeyPosition = 0, midValue = 0;
	int tempKeyList[bTreeOrder];

	//Create a new list one greater than noOfKeys and sort them
	//and add new key to the new list
	for( i = 0; i<rootNode->noOfKeys; i++)
	{
		if(primaryKeyInInt < rootNode->nodeKeys[i])
		{
			newKeyPosition = s;
			tempKeyList[s] = primaryKeyInInt;
			s++;

		}
		
		tempKeyList[s] = rootNode->nodeKeys[i];
		s++;		
	}
	

	//if new list is smaller or equal to btreeOrder, then we are fine
	//no need to break
	//copy the new contents to node's keys and update offsets
	if( s < bTreeOrder )
	{
		rootNode->noOfKeys = (rootNode->noOfKeys + 1);
		for(i =0; i<rootNode->noOfKeys; i++)
		{
			rootNode->nodeKeys[i] = tempKeyList[i];
		}
		return(1);
	}
	

	//this means the node is now equal to btree order i.e. one extra node is there

	midValue = bTreeOrder/2;
	bTreeNode rightNode;
	initializeNode(rightNode, bTreeOrder);
	rightNode->noOfKeys = bTreeOrder - (midValue + 1);

	for( i = 0; i<rightNode->noOfKeys; i++)
	{
		rightNode->nodeKeys[i] = tempKeyList[midValue+1+i];
	}

	reInitializeNode(rootNode, bTreeOrder);
	rootNode->noOfKeys = midValue;
	for( i = 0; i < rootNode->noOfKeys; i++)
	{
		rootNode->nodeKeys[i] = tempKeyList[i];
	}


	insertSplitChild(tempKeyList[midValue], &rootNode, &rightchild);
}


void insertSplitChild(int valueToInsert, long *leftChildOffset, long *rightChildOffset)
{

}*/