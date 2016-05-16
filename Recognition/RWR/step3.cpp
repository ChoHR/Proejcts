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

class Data{
public:
	int user,item;
	double weight;
	void setData(int u,int i,double w){
		user = u;
		item = i;
		weight = w;
	}
};

class Cmp{
public:
	bool operator()(Data a, Data b){
		return (a.weight > b.weight) ;
	}
};

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
	char *rFileName,*wFileName;
	FILE* f;
	char buf[1024];

	/************************************
	 * Adjacency matrix	: A
	 ************************************/
	double** A;


	/************************************
	 * test number of argument
	 ************************************/
	if(argc != 3){
		cout << "number of argument is not 3" << endl;
		return 0;
	}

	rFileName = argv[1];
	wFileName = argv[2];
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

	fgets(buf,1024,f);
	fgets(buf,1024,f);
	fgets(buf,1024,f);

	{
		int count = 0;
		int start = 0;
		int a,b;
		
		cout << buf << endl;

		for(i = 0; buf[i] != '\0'; i++){
			if(buf[i] == ' '){
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

		cout << a << " " << b << endl;

		numOfUser = a;
		numOfItem = b;
		numOfNode = numOfUser + numOfItem;
	}

	cout << numOfUser << " " << numOfItem << endl;

	cout << "C make" << endl;

	// print p matrix
	list< Data > C,D;

	while(fgets(buf,128,f) != NULL){
		int count = 0;
		int start = 0;
		int a,b;
		double c;
		//cout << buf;
		for(i = 0; buf[i] != '\0'; i++){
			if(((count != 2) && (buf[i] == ' ')) || ((count == 2) && (buf[i] == '\n'))){
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
					start = i+1;
					b = atoi(temp);
				}
				else if(count == 3){
					char temp[128];
					int ii = 0;
					while(buf[start+ii] == ' '){
						ii++;
					}
					strncpy(temp,buf+start+ii,i-start-ii);
					temp[i-start-ii] = '\0';
					//cout << "111111111111111111111111111111111111" << endl;
					//cout << temp << endl;
					c = atof(temp);	
					break;
				}
			}
		}
		Data d;
		d.setData(a,b,c);
		
		C.push_back(d);
	}

	if(fclose(f) != 0){
		cout << "file close error" << endl;
		return 0;
	}
	////////////////////////////////////////////////////////////////
	
	cout << "sort start " << endl;

	// 내림차순 정렬
	C.sort(Cmp());
	
	f = fopen(wFileName,"rt");
	if(f == NULL){
		cout << "file open error" << endl;
		return 0;
	}
	
	while(fgets(buf,128,f) != NULL){
		int count = 0;
		int start = 0;
		int a,b;
		double c;
		//cout << buf;
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
					start = i+1;
					b = atoi(temp);
					break;
				}
			}
		}
		Data d;
		d.setData(a,b,1);
		
		D.push_front(d);
	}

	
		if( fclose(f) != 0){
			cout <<"file close error" << endl;
			return 0;
		}

		f = fopen(wFileName,"wt");
		
	/////////////////////////////////////////////////////////
	while(D.empty() == false){
		fprintf(f,"%d\t%d\t%d\n",D.back().user,D.back().item,1);
		D.pop_back();
	}	
	
		//f = fopen(wFileName,"wt");
		//fseek(f,-1,SEEK_END);
		
		
		int iterator = C.size()/(double)10;
		for(int i = 0; i < iterator; i++){
			fprintf(f,"%d\t%d\t0\n",C.back().user,C.back().item);
			C.pop_back();
		}
		if( fclose(f) != 0){
			cout <<"file close error" << endl;
			return 0;
		}
	
	/////////////////////////////////////////////////////////
	return 0;

}
