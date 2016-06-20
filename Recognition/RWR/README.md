# step 1: RWR.cpp(random walk with restart)

RWR을 이용하여, item을 ranking한다.

parameter는 다음과 같다.

* numOfUser, numOfItem, r(probability of restart), threshold(dosen't work), n(number of iterating), rFileName(input file name), wFileName(output file name)

input은 다음과 같다.

* [user number]\t[item number]\t
* [user number]\t[item number]\t
* [user number]\t[item number]\t

output은 다음과 같다.

* [user number]\t[item number]\t[probability]
* [user number]\t[item number]\t[probability]
* [user number]\t[item number]\t[probability]

# step 2: getParameter.cpp

Graphchi에 넣을 input file인 User-Oriented.base 와 OCCF.test 를 만드는 프로그램.
user가 어떤 item에 rating을 했는지 표시된 u.base 파일을 input으로 삼는다.
input 파일을 읽어, 최대 user number와 최대 item number를 자동으로 계산해준다.

parameter는 다음과 같다.

* u4.base u4_User-Oriented.base u4_OCCF.test 
* u4.base: user가 어떤 item에 rating을 했는지 표시한 파일
* u4_User-Oriented.base: user-oriented weight를 구한다.
* u4_OCCF.test: 구해야 하는 모든 (user,item) 쌍을 출력한다.

# step 3: step3.cpp

Graphchi에 User-Oriented.base 와 OCCF.test를 넣어서 나온 output file인 u4_OCCF.test.predict와 처음 우리에게 주어진 u4.base를 갖고 interesting item만으로 RWR 결과를 출력하고, uninteresting item만으로 RWR 결과를 출력한다. 
uninteresting item은 10% 단위로 구분해서 RWR 결과를 출력한다.

parameter는 다음과 같다.

* u4_OCCF.test.predict u4.base
* u4_OCCF.test.predict: Graphchi의 결과. 
* u4.base: user가 어떤 item에 rating을 했는지 표시한 파일

output
* u4_Positive.txt: interesting한 item들만 사용한 RWR 결과.
* u4_0.1.txt: 10%의 uninteresting한 item들만 사용한 RWR 결과.

# step 4: step4.cpp

Graphchi에 User-Oriented.base 와 OCCF.test를 넣어서 나온 output file인 u4_OCCF.test.predict와 처음 우리에게 주어진 u4.base를 갖고 interesting item만으로 RWR 결과를 출력하고, uninteresting item만으로 RWR 결과를 출력한다. 
uninteresting item은 10% 단위로 구분해서 RWR 결과를 출력한다.

parameter는 다음과 같다.

* 943 1682 0.15 0.0001 10 u4_Positive.txt output.txt
* 943: # of user
* 1682: # of item
* 0.15: probability of restart
* 0.0001: change threshold(not use)
* 10: RWR iterator
* u4_Positive.txt: step3.cpp의 결과. 
* output.txt: not use

output
* result_u4.0.1.txt: interesting한 item들만 사용한 RWR 결과값에서 10%의 uninteresting한 item들만 사용한 RWR 결과값을 뺀 상황에서, 각 user당 top-50개의 item을 추천한 결과.

# step 5: step5.cpp

step4.cpp에서 나온 결과의 정확도를 테스트하기 위한 소스이다.

parameter는 없다.

output
* test_u4.0.1.txt: MRR, nDCG, Precision, Recall and F1의 평가 결과.

school : Hanyang univ

major  : CSE

writed by CHR
