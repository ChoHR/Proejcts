# Recognition by EigenVector
Projects In the Hanyang university.

EigenVector(EigenFace)을 이용하여 DB에서 해당하는 사람을 찾는 프로그램 소스코드이다.

다음의 과정으로 프로그램은 동작한다.
* 1. 데이터베이스가 될 사진들을 gray로 변환하여 저장.
* 2. 검색할 대상의 사진을 gray로 변환하여 저장.
* 3. pca
* 4. 가장 값이 높은 20개의 eigenface를 구한다.
* 5. 데이터베이스 사진들의 K-L 변환
* 6. 검색할 대상의 K-L변환
* 7. 데이터베이스의 사진과 검색할 대상의 사진의 eigenvector의 계수를 비교한다. 데이터베이스에서 그 차이가 가장 작은 사진이 인풋 사진의 인물과 동일인이라고 판별한다.
(전혀 다른 인물을 검색해야한다면, 계수차이가 특정값 이상일때만 동일인 판별을 내리게 하면 된다. 미구현)

필요한 사진은 두 개이다. 
* 인물의 얼굴이 꽉차게 찍힌 사진(검색대상). 파일 이름은 input.png로 한다.
* 인물의 얼굴이 꽉차게 찍힌 사진들(DB). 파일 이름은 숫자.png로 한다.(0부터 시작. ex. 0.png, 1.png ...)

school : Hanyang univ

major  : CSE

writed by CHR
