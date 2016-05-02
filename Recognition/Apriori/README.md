# Apriori algorithm
Projects In the Hanyang university.

Apriori algorithm을 이용하여, frequent itemset을 찾는다.
parameter는 다음과 같다.

* min_support, input file name, output file name

input은 다음과 같다.

* 7	14
* 9
* 18	2	4	5	1

각 행은 user를 의미하며, 숫자는 해당 user가 구매한 item을 의미한다.
1번 유저는 7,14를 구입했고, 2번 유저는 9를 구입, 3번 유저는 18,2,4,5,1을 구입했다.

output은 다음과 같다.

* {0}	{1}	6.60	24.63
* {1}	{0}	6.60	22.15

[itemSet_A]\t[itemSet_B]\t[support]\t[confidence]

school : Hanyang univ

major  : CSE

writed by CHR
