#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <deque>
#include <fstream>
#include <string.h>
#include <math.h>
#include <list>
#include <vector>
#include <set>

using namespace std;

int main(int argc, char* argv[]){
	// parameter
	int i,j,k;

	/************************************
	 * number of user		: numOfUser, 1 ~ numOfUser
	 * number of item		: numOfItem, numOfUser+1 ~ (numOfItem+numOfUser)
	 * number of node		: numOfNode
	 ************************************/
	int n, numOfUser, numOfItem, numOfNode;
	/************************************
	 * input file name	: rFileName
	 * output file name	: wFileName
	 * FILE structure	: f
	 ************************************/
	char *rFileName,*wTrainFile,*wTestFile;
	FILE* f;
	char buf[128];

	/************************************
	 * Adjacency matrix	: A
	 ************************************/
	double** A;


	/************************************
	 * test number of argument
	 ************************************/
	if(argc != 4){
		cout << "number of argument is not 4" << endl;
		return 0;
	}

	rFileName = argv[1];
	wTrainFile = argv[2];
	wTestFile = argv[3];
	/****************************************
	 * open input file
	 * read input file
	 * each line is checked for tokenizing
	 * so, we get metadata, [max_user_id, max_item_id] using tokenizer
	 ****************************************/
	f = fopen(rFileName,"rt");
	if(f == NULL){
		cout << "file open error" << endl;
		return 0;
	}

	int max_User = 0;
	int max_Item = 0;

	while(fgets(buf,128,f) != NULL){
		int count = 0;
		int start = 0;
		int a,b;
		
		for(i = 0; buf[i] != '\0'; i++){
			if(buf[i] == '\t'){
				count++;
			
				if(count == 1){
					char temp[128];
					strncpy(temp,buf,i);
					temp[i] = '\0';
					a = atoi(temp);
					if(max_User < a)
						max_User = a;
					start = i+1;
				}
				else if(count == 2){
					char temp[128];
					strncpy(temp,buf+start,i-start);
					temp[i-start] = '\0';
					b = atoi(temp);
					if(max_Item < b)
						max_Item = b;
					break;
				}

			}
		}
	}
	
	numOfUser = max_User;
	numOfItem = max_Item;
	numOfNode = numOfUser + numOfItem;

	if(fclose(f) != 0){
		cout << "file close error" << endl;
		return 0;
	}
	////////////////////////////////////////////////////////////////

	A = (double**)malloc(sizeof(double*)*(numOfNode+1));
	for(i = 1; i <= numOfNode; i++){
		A[i] = (double*)malloc(sizeof(double)*(numOfNode+1));
		for(j = 1; j <= numOfNode; j++){
			A[i][j] = 0.0;
		}
	}

	int* numOfRating = (int*)malloc(sizeof(int)*(numOfUser+1));
	memset(numOfRating,0,sizeof(int)*(numOfUser+1));
	f = fopen(rFileName,"rt");
	if(f == NULL){
		cout << "file open error" << endl;
		return 0;
	}

	while(fgets(buf,128,f) != NULL){
		int count = 0;
		int start = 0;
		int a,b;
		
		for(i = 0; buf[i] != '\0'; i++){
			if(buf[i] == '\t'){
				count++;
			
				if(count == 1){
					char temp[128];
					strncpy(temp,buf,i);
					temp[i] = '\0';
					a = atoi(temp);
					numOfRating[a]++;
					start = i+1;
				}
				else if(count == 2){
					char temp[128];
					strncpy(temp,buf+start,i-start);
					temp[i-start] = '\0';
					b = atoi(temp);
					break;
				}

			}
		}

		A[a][numOfUser+b] = 1.0;
		A[numOfUser+b][a] = 1.0;
	}
	
	if(fclose(f) != 0){
		cout << "file close error" << endl;
		return 0;
	}
	/////////////////////////////////////////////////////////
	
	
	f = fopen(wTestFile,"wt");

	fprintf(f,"%%%%MatrixMarket matrix coordinate real general\n%% Generated 28-Aug-2011\n");
	fprintf(f,"%d\t%d\t%d\n",numOfUser,numOfItem,numOfUser*numOfItem);
	for(int j = numOfUser+1; j <= numOfNode; j++){
		for(int i = 1; i < numOfUser+1; i++){
			if(A[i][j] == 1.0){
				fprintf(f,"%d\t%d\t%d\n",i,j-numOfUser,1);
			}
			else{
				fprintf(f,"%d\t%d\t%d\n",i,j-numOfUser,1);
			}
		}
	}
	if( fclose(f) != 0){
		cout <<"file close error" << endl;
		return 0;
	}

	////////////////////////////////////////////////////////////
	
	int min_arr = numOfUser;
	int max_arr = 0;
	for(int i = 1; i <= numOfUser; i++){
		if( numOfRating[i] < min_arr)
			min_arr = numOfRating[i];
		if( numOfRating[i] > max_arr)
			max_arr = numOfRating[i];
	}

	f = fopen(wTrainFile,"wt");
	fprintf(f,"%%%%MatrixMarket matrix coordinate real general\n");
	fprintf(f,"%d\t%d\t%d\n",numOfUser,numOfItem,numOfUser*numOfItem);
	for(int j = numOfUser+1; j <= numOfNode; j++){
		for(int i = 1; i < numOfUser+1; i++){
			if(A[i][j] == 1.0){
				fprintf(f,"%d\t%d\t%d\t%d\n",i,j-numOfUser,1,1);
			}
			else{
				fprintf(f,"%d\t%d\t%0.16lf\t%d\n",i,j-numOfUser,(numOfRating[i] - min_arr) / (double)(max_arr - min_arr),0);
			}
		}
	}
	if( fclose(f) != 0){
		cout <<"file close error" << endl;
		return 0;
	}

	
	/////////////////////////////////////////////////////////
	return 0;

}
