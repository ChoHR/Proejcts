#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <deque>
#include <fstream>
#include <string.h>
#include <math.h>
#include <list>

using namespace std;


class Cmp{
public:
	bool operator()(pair<int, double> a, pair<int, double> b){
		return a.second > b.second;
	}
};

int main(int argc, char* argv[]){
	// parameter
	int i,j,k;
	// if sum of row is 0, value of row of item is 1/n ( = g ) 
	double Ug,Ig;

	/************************************
	 * restart probability	: r
	 * iterate number		: n
	 * change threshold		: threshold
	 * number of user		: numOfUser, 1 ~ numOfUser
	 * number of item		: numOfItem, numOfUser+1 ~ (numOfItem+numOfUser)
	 * number of node		: numOfNode
	 ************************************/
	int n, numOfUser, numOfItem, numOfNode;
	double r,threshold;
	/************************************
	 * input file name	: rFileName
	 * output file name	: wFileName
	 * FILE structure	: f
	 ************************************/
	char *rFileName,*wFileName;
	FILE* f;
	char buf[128];

	/************************************
	 * proximity matrix			: P
	 * restart uniform matrix	: Q
	 ************************************/
	double *P,*Q;
	/************************************
	 * Adjacency matrix	: A
	 ************************************/
	double** A;


	/************************************
	 * test number of argument
	 ************************************/
	if(argc != 8){
		cout << "number of argument is not 8" << endl;
		return 0;
	}

	/******************************************
	 * init parameter
	 * put arguments of main into parameter values
	 * calculate numOfNode using numOfItem and numOfUser
	 * memset matrix P,Q and A
	 ******************************************/
	numOfUser = atoi(argv[1]);
	numOfItem = atoi(argv[2]);
	r = (double)atof(argv[3]);
	threshold = (double)atof(argv[4]);
	n = atoi(argv[5]);
	rFileName = argv[6];
	wFileName = argv[7];

	numOfNode = numOfItem + numOfUser;

	Ig = 1 / (double)numOfItem;
	Ug = 1 / (double)numOfUser;

	P = (double*)malloc(sizeof(double)*(numOfNode+1));
	Q = (double*)malloc(sizeof(double)*(numOfNode+1));

	A = (double**)malloc(sizeof(double*)*(numOfNode+1));
	for(i = 1; i <= numOfNode; i++){
		A[i] = (double*)malloc(sizeof(double)*(numOfNode+1));
		P[i] = 0.0;
		Q[i] = 0.0;
		for(j = 1; j <= numOfNode; j++){
			A[i][j] = 0.0;
		}
	}


	/****************************************
	 * open input file
	 * read input file
	 * each line is checked for tokenizing
	 * so, we get data, [user_id, item_id] using tokenizer
	 ****************************************/
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
	
	for(i = 1; i <= numOfNode; i++){
		int count = 0;
		for(j = 1; j <= numOfNode; j++){
			if(A[i][j] == 1){
				count++;
			}
		}
		// dangling
		if(count == 0){
			for(j = 1; j <= numOfNode; j++){
				if(i <= numOfUser && j <= numOfUser)
					continue;
				else if(i > numOfUser && j > numOfUser)
					continue;
				else if(i <= numOfUser){
					A[i][j] = Ig;
				}
				else{
					A[i][j] = Ug;
				}
			}
		}
		// normalizing
		else{
			for(j = 1; j <= numOfNode; j++){
				A[i][j] /= (double)count;
			}
		}
	}

	if(fclose(f) != 0){
		cout << "file close error" << endl;
		return 0;
	}
	
	// iterate N or change is less than threshold
	f = fopen(wFileName,"wt");
	
	for(i = 1; i <= numOfUser; i++){
		// init P and Q
		for(j = 1; j <= numOfNode; j++){
			P[j] = 0.0;
			Q[j] = 0.0;
		}
		P[i] = 1.0;
		Q[i] = 1.0;
		/*************************************
		 * when change is less than threshold 
		 * or
		 * when we iterate as n times
		 * stop the iteration
		 *************************************/
		for(int iterator = 0; iterator < n; iterator++){
			/*********************************
			 * ans(P) is 1.
			 * we have to make P normalize
			 * we have to calculate the equation, P = (1-r)*A*P + r*Q
			 *********************************/
			double* _P = (double*)malloc(sizeof(double)*(numOfNode+1));
			for(j = 1; j <= numOfNode; j++){
				_P[j] = 0.0;
			}

			// P*A
			for(j = 1; j <= numOfNode; j++){
				for(k = 1; k <= numOfNode; k++){
					_P[j] += (P[k] * A[k][j]);
				}	
			}
				
			// (1-r)*PA
			for(j = 1; j <= numOfNode; j++){
				_P[j] *= (1.0-r);
			}
			// we can restart to only user i
			_P[i] += r;
			for(j = 1; j <= numOfNode; j++){
				P[j] = _P[j];
			}
		}
		

		// print p matrix
		list< pair<int,double> > C;
		
		for(j = numOfUser+1; j <= numOfNode; j++){
			C.push_back( make_pair(j,P[j]) );
		}

		cout << " i : " << i << endl;
		
		C.sort(Cmp());

		for(j = 0; j < 50; ){
			if(A[i][C.front().first] == 0.0){
				j++;
				fprintf(f,"%d\t%d\t%0.16lf\n",i,C.front().first -numOfUser,C.front().second);
			}
			C.pop_front();		
		}
		C.clear();
	}
	if( fclose(f) != 0){
		cout <<"file close error" << endl;
		return 0;
	}

	return 0;
}
