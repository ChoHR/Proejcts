#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <deque>
#include <fstream>
#include <string.h>
#include <math.h>
#include <list>

// parameter : 943 1682 0.15 0.0001 10 input.txt output.txt
using namespace std;

class Cmp{
public:
	bool operator()(pair<int, double> a, pair<int, double> b){
		return a.second > b.second;
	}
};

const int MAX_NUM_FILE = 1;

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
	double **P,**Q;
	double **uP[MAX_NUM_FILE],**uQ[MAX_NUM_FILE];
	/************************************
	 * Adjacency matrix	: A
	 ************************************/
	double** A;
	double** B[MAX_NUM_FILE];

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

	for(i = 0; i < MAX_NUM_FILE; i++){	
		uP[i] = (double**)malloc(sizeof(double*)*(numOfUser+1));
		uQ[i] = (double**)malloc(sizeof(double*)*(numOfUser+1));
	}
	P = (double**)malloc(sizeof(double*)*(numOfUser+1));
	Q = (double**)malloc(sizeof(double*)*(numOfUser+1));
	for(i = 0; i < numOfUser+1; i++){
		for(j = 0; j < MAX_NUM_FILE; j++){
			uP[j][i] = (double*)malloc(sizeof(double)*(numOfNode+1));
			uQ[j][i] = (double*)malloc(sizeof(double)*(numOfNode+1));
		}
		P[i] = (double*)malloc(sizeof(double)*(numOfNode+1));
		Q[i] = (double*)malloc(sizeof(double)*(numOfNode+1));
	}

	A = (double**)malloc(sizeof(double*)*(numOfNode+1));
	
	for(i = 0; i < MAX_NUM_FILE; i++){
		B[i] = (double**)malloc(sizeof(double*)*(numOfNode+1));
	}

	for(i = 1; i <= numOfNode; i++){
		A[i] = (double*)malloc(sizeof(double)*(numOfNode+1));
		for(k = 0; k < MAX_NUM_FILE; k++){
			B[k][i] = (double*)malloc(sizeof(double)*(numOfNode+1));
			for(j = 1; j <= numOfNode; j++){
				B[k][i][j] = 0.0;
			}
		}
		for(k = 0; k < numOfUser+1; k++){
			P[k][i] = 0.0;
			Q[k][i] = 0.0;
			for(j = 0; j < MAX_NUM_FILE; j++){
				uP[j][k][i] = 0.0;
				uQ[j][k][i] = 0.0;
			}
		}
		for(j = 1; j <= numOfNode; j++){
			A[i][j] = 0.0;
		}
	}

	cout << "init end" << endl;

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
	
	if(fclose(f) != 0){
		cout << "file close error" << endl;
		return 0;
	}

	cout << " array A set finish " << endl;

	////////////////////////////////////

	FILE *ff[MAX_NUM_FILE];

	char u1[MAX_NUM_FILE][20] = {"u1_0.9.txt"};//,"u1_0.2.txt","u1_0.3.txt","u1_0.4.txt","u1_0.5.txt","u1_0.6.txt","u1_0.7.txt","u1_0.8.txt","u1_0.9.txt"};

	for(int INDEX = 0; INDEX < MAX_NUM_FILE; INDEX++){
		ff[INDEX] = fopen(u1[INDEX],"rt");
		if(ff[INDEX] == NULL){
			cout << "file open error" << endl;
			return 0;
		}

		while(fgets(buf,128,ff[INDEX]) != NULL){
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

			B[INDEX][a][numOfUser+b] = 1.0;
			B[INDEX][numOfUser+b][a] = 1.0;
		}
		
		if(fclose(ff[INDEX]) != 0){
			cout << "file close error" << endl;
			return 0;
		}
	}

	cout << " array B set finish " << endl;

	///////////////////////////
	
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

	cout << " dangling A finish " << endl;

	//////////////////////////////////

	for(int INDEX = 0; INDEX < MAX_NUM_FILE; INDEX++){
		for(i = 1; i <= numOfNode; i++){
			int count = 0;
			for(j = 1; j <= numOfNode; j++){
				if(B[INDEX][i][j] == 1){
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
						B[INDEX][i][j] = Ig;
					}
					else{
						B[INDEX][i][j] = Ug;
					}
				}
			}
			// normalizing
			else{
				for(j = 1; j <= numOfNode; j++){
					B[INDEX][i][j] /= (double)count;
				}
			}
		}
	}

	cout << " dangling B finish " << endl;
	
	/////////////////////////////////////////////////////////////
	
	double* _P = (double*)malloc(sizeof(double)*(numOfNode+1));

	for(i = 1; i <= numOfUser; i++){
		cout << "array P user " << i << " start " << endl;
		// init P and Q
		for(j = 1; j <= numOfNode; j++){
			P[i][j] = 0.0;
			Q[i][j] = 0.0;
		}
		P[i][i] = 1.0;
		Q[i][i] = 1.0;
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
			for(j = 1; j <= numOfNode; j++){
				_P[j] = 0.0;
			}

			// P*A
			for(j = 1; j <= numOfNode; j++){
				for(k = 1; k <= numOfNode; k++){
					_P[j] += (P[i][k] * A[k][j]);
				}	
			}
				
			// (1-r)*PA
			for(j = 1; j <= numOfNode; j++){
				_P[j] *= (1.0-r);
			}
			// we can restart to only user i
			_P[i] += r;
			for(j = 1; j <= numOfNode; j++){
				P[i][j] = _P[j];
			}
		}
	}

	cout << " array P finish " << endl;

	/////////////////////////////////////////////
	for(int INDEX = 0; INDEX < MAX_NUM_FILE; INDEX++){
		cout << "index " << INDEX << " start " << endl;
		for(i = 1; i <= numOfUser; i++){
			cout << " INDEX : " << INDEX << " array uP user " << i << " start " << endl;
			// init P and Q
			for(j = 1; j <= numOfNode; j++){
				uP[INDEX][i][j] = 0.0;
				uQ[INDEX][i][j] = 0.0;
			}
			uP[INDEX][i][i] = 1.0;
			uQ[INDEX][i][i] = 1.0;
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
				for(j = 1; j <= numOfNode; j++){
					_P[j] = 0.0;
				}

				// P*A
				for(j = 1; j <= numOfNode; j++){
					for(k = 1; k <= numOfNode; k++){
						_P[j] += (uP[INDEX][i][k] * B[INDEX][k][j]);
					}	
				}

				// (1-r)*PA
				for(j = 1; j <= numOfNode; j++){
					_P[j] *= (1.0-r);
				}
				// we can restart to only user i
				_P[i] += r;
				for(j = 1; j <= numOfNode; j++){
					uP[INDEX][i][j] = _P[j];
				}
			}
		}
	}

	cout << "array uP finish " << endl;

	/////////////////////////////////////////////
	
	char u2[MAX_NUM_FILE][23] = {"result_u1_0.9.txt"};//,"result_u1_0.2.txt","result_u1_0.3.txt","result_u1_0.4.txt","result_u1_0.5.txt","result_u1_0.6.txt","result_u1_0.7.txt","result_u1_0.8.txt","result_u1_0.9.txt"};

	for(int INDEX = 0; INDEX < MAX_NUM_FILE; INDEX++){
		ff[INDEX] = fopen(u2[INDEX],"wt");
		if(ff[INDEX] == NULL){
			cout << "file open error" << endl;
			return 0;
		}
		for(i = 1; i < numOfUser+1; i++){
			for(j = 1; j < numOfNode+1; j++){
			//	cout << "uP : " << uP[INDEX][i][j] << "  P : " << P[i][j] << endl;
				uP[INDEX][i][j] = P[i][j] - uP[INDEX][i][j];
			}
		
			list< pair<int,double> > C;
			for(j = numOfUser+1; j <= numOfNode; j++){
				C.push_back( make_pair(j,uP[INDEX][i][j]) );
			}
			C.sort(Cmp());
			//while(C.empty() == false){
			for(j = 0; j < 50;){
				if(A[i][C.front().first] == 0.0 || A[i][C.front().first] == Ig || A[i][C.front().first] == Ug){
					j++;
					fprintf(ff[INDEX],"%d\t%d\t%0.16lf\n",i,C.front().first - numOfUser, C.front().second);
				}
				C.pop_front();
			}
			C.clear();
		}
		if( fclose(ff[INDEX]) != 0){
			cout <<"file close error" << endl;
			return 0;
		}
		cout << " " << INDEX << " finish " << endl;
	}

	return 0;
}
