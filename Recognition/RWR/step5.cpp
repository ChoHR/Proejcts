#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <map>
#include <list>
#include <math.h>
#include <vector>
#include <fstream>

using namespace std;

const int numOfUser = 943;

int topks[5] = {5,10,15,20,25};

/*
 * test의 결과를 확인하기 위한 source
 *
 */

int MAX_INDEX = 9;

char filename[10][30] = {"result_u4_0.1.txt","result_u4_0.2.txt","result_u4_0.3.txt","result_u4_0.4.txt",
						"result_u4_0.5.txt","result_u4_0.6.txt","result_u4_0.7.txt","result_u4_0.8.txt","result_u4_0.9.txt"};

char o_filename[10][30] = {"test_u4_0.1.txt","test_u4_0.2.txt","test_u4_0.3.txt","test_u4_0.4.txt",
						"test_u4_0.5.txt","test_u4_0.6.txt","test_u4_0.7.txt","test_u4_0.8.txt","test_u4_0.9.txt"};


struct Score{
	int item_id;
};

map< int, vector<Score> > correct_user_item;




void ComputeMRR(map< int, vector<Score> > recommend_user_item,FILE* fp)//, StreamWriter output)
{
	double mrr = 0;
	for(map<int, vector<Score> >::iterator it = correct_user_item.begin(); it != correct_user_item.end(); it++)
	//for each(int user in correct_user_item._Key)
	{
		int user = it->first;
		int c_size = correct_user_item[user].size();
		double rank = 0;
		if (!recommend_user_item.count(user)) 
			continue;//.ContainsKey(user)) continue;
		int size = recommend_user_item[user].size();
		for (int i = 0; i < size; i++)
		{
			int item_id = recommend_user_item[user][i].item_id;
			bool flag = false;
			for(int j = 0; j < c_size; j++){
				if(correct_user_item[user][j].item_id == item_id){
					flag = true;
					break;
				}
			}
			if(flag == true){
				rank = i + 1;
				mrr += (double)1 / rank;
				break;
			}
		}
	}
	fprintf_s(fp,"MRR@\t%0.16lf\n", mrr / (double)correct_user_item.size());
}

void ComputeNDCG(map< int, vector<Score> > recommend_user_item,FILE *fp)
{
	double ndcg[5] = {0,};
	for(map<int, vector<Score> >::iterator it = correct_user_item.begin(); it != correct_user_item.end(); it++)
	{
		int user = it->first;
		int c_size = correct_user_item[user].size();
		if(c_size == 0)
			continue;
		int r_size = recommend_user_item[user].size();
		//cout << " user : " << user << endl;
		if (!recommend_user_item.count(user))
			continue;
		for (int i = 0; i < 5; i++)
		{
			double dcg = 0;
			double idcg = 0;

			int size = min(c_size , topks[i]);
			for (int j = 0; j < size; j++)
				idcg += (1 / (log10((double)j + 2.0)/log10(2.0)));

			size = min(r_size, topks[i]);
			for (int j = 0; j < size; j++)
			{
				bool flag = false;
				int item_id = recommend_user_item[user][j].item_id;
				for(int k = 0; k < c_size; k++){
					if (correct_user_item[user][k].item_id == item_id){
						flag = true;
						break;
					}
				}

				if(flag != true){
					continue;
				}

				int rank = j + 1;
				dcg += (1 / (log10((double)rank + 1.0)/log10(2.0)));
			}
			ndcg[i] += (dcg / idcg);
		//	cout << "ndcg " << i << " " << ndcg[i] << endl;
		}
	}

	int c_size = correct_user_item.size();
	for (int i = 0; i < 5; i++)
		fprintf_s(fp,"NDCG@%d\t%0.16lf\n", topks[i], ndcg[i] / (double)c_size);
}

void ComputeAccuary(map< int, vector<Score> > recommend_user_item, FILE* fp)
{
	double precision[5];
	double recall[5];
	double f1[5];
	double hit[5];

	for(int i = 0; i < 5; i++){
		precision[i] = 0.0;
		recall[i] = 0.0;
		f1[i] = 0.0;
		hit[i] = 0.0;
	}

	for(map<int, vector<Score> >::iterator it = correct_user_item.begin(); it != correct_user_item.end(); it++)
	{
		int rank = 0;
		int user = it->first;
		int c_size = correct_user_item[user].size();
		int r_size = recommend_user_item[user].size();
		if(c_size == 0)
			continue;
		double user_hit[5];
		for(int i = 0; i < 5; i++){
			user_hit[i] = 0.0;
		}
		int size = min(r_size, topks[4]);

		for (int i = 0; i < size; i++)
		{
			int item_id = recommend_user_item[user][i].item_id;

			bool flag = false;
			for(int k = 0; k < c_size; k++){

				if (correct_user_item[user][k].item_id == item_id){
					flag = true;
					break;
				}

			}

			if(flag == false){
				continue;
			}

			rank = i + 1;
			for (int j = 0; j < 5; j++)
				if (rank <= topks[j])
					user_hit[j]++;
		}

		for (int i = 0; i < 5; i++)
		{
			hit[i] += user_hit[i];
			recall[i] += user_hit[i] / (double)c_size;
		}
	}

	int c_size = correct_user_item.size();
	for (int i = 0; i < 5; i++)
	{
		precision[i] = hit[i] / (double)(c_size * topks[i]);
		recall[i] = recall[i] / (double)c_size;
		f1[i] = (2 * precision[i] * recall[i]) / (precision[i] + recall[i]);
	}

	// precision, recall, F1 score 계산            
	for (int i = 0; i < 5; i++)
		fprintf_s(fp,"Prec@%d\t%0.16lf\t%lf\t%d\n", topks[i], precision[i], hit[i], c_size * topks[i]);
	for (int i = 0; i < 5; i++)
		fprintf_s(fp,"Recall@%d\t%0.16lf\n", topks[i], recall[i]);
	for (int i = 0; i < 5; i++)
		fprintf_s(fp,"F1@%d\t%0.16lf\n", topks[i], f1[i]);
}

int main(void){

	int i,j,k;


	{
		FILE *f;
		char buf[512];

		f = fopen("u4.test","rt");
		if(f == NULL){
			cout << "file open error" << endl;
			return 0;
		}

		while(fgets(buf,512,f) != NULL){
			int count = 0;
			int start = 0;
			int a,b;

			for(i = 0; buf[i] != '\0'; i++){
				if(buf[i] == '\t'){
					count++;

					if(count == 1){
						char temp[512];
						strncpy(temp,buf,i);
						temp[i] = '\0';
						a = atoi(temp);
						start = i+1;
					}
					else if(count == 2){
						char temp[512];
						strncpy(temp,buf+start,i-start);
						temp[i-start] = '\0';
						b = atoi(temp);
						break;
					}

				}
			}

			if(correct_user_item.count(a) != 0){
				struct Score sc;
				sc.item_id = b;
				correct_user_item[a].push_back(sc);
			}
			else{
				struct Score sc;
				sc.item_id = b;
				vector<Score> v_sc;
				v_sc.push_back(sc);
				correct_user_item[a] = v_sc;
			}

		}

		if(fclose(f) != 0){
			cout << "file close error" << endl;
			return 0;
		}

	}

	for(int INDEX = 0; INDEX < MAX_INDEX; INDEX++){
		cout << "INDEX : " << INDEX << endl;
		FILE *f;
		char buf[512];

		map< int, vector<Score> > recommend_user_item;

		f = fopen(filename[INDEX],"rt");
		if(f == NULL){
			cout << "file open error" << endl;
			return 0;
		}

		while(fgets(buf,512,f) != NULL){
			int count = 0;
			int start = 0;
			int a,b;

			for(i = 0; buf[i] != '\0'; i++){
				if(buf[i] == '\t'){
					count++;

					if(count == 1){
						char temp[512];
						strncpy(temp,buf,i);
						temp[i] = '\0';
						a = atoi(temp);
						start = i+1;
					}
					else if(count == 2){
						char temp[512];
						strncpy(temp,buf+start,i-start);
						temp[i-start] = '\0';
						b = atoi(temp);
						break;
					}

				}
			}

			if(recommend_user_item.count(a) != 0){
				struct Score sc;
				sc.item_id = b;
				recommend_user_item[a].push_back(sc);
			}
			else{
				struct Score sc;
				sc.item_id = b;
				vector<Score> v_sc;
				v_sc.push_back(sc);
				recommend_user_item[a] = v_sc;
			}



		}

		if(fclose(f) != 0){
			cout << "file close error" << endl;
			return 0;
		}

		FILE *of;
		of = fopen(o_filename[INDEX],"wt");
		if(of == NULL){
			cout << "file open error" << endl;
			return 0;
		}
		cout << "start MRR" << endl;
		ComputeMRR(recommend_user_item,of);
		cout << "end MRR" << endl;

		cout << "start nDCG " << endl;
		ComputeNDCG(recommend_user_item,of);
		cout << "end nDCG" << endl;

		cout << "start Accuary" << endl;
		ComputeAccuary(recommend_user_item,of);
		cout << "end Accuary" << endl << endl;


		if(fclose(of) != 0){
			cout << "file close error" << endl;
			return 0;
		}
	}
	return 0;
}