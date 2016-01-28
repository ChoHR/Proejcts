% 데이타베이스의 사진들을 gray로 변환하여 저장한다.
numOfPicture = 10;
data_x = zeros(numOfPicture, 40*40);
for k=1:numOfPicture,
	fname = sprintf('%d.png',k);
	data_img = double(rgb2gray(imread(fname)));
	data_x(k,:) = (data_img(1:1600))';
end;

% 인풋으로 넣은 사진들을 gray로 변환하여 저장한다.
input_x = zeros(1, 40*40);
fname = sprintf('input.png');
input_img = double(rgb2gray(imread(fname)));
input_x(1,:) = (input_img(1:1600))';

% pca
data_c = cov(data_x);
[data_v, data_d] = eig(data_c);

% K-L 변환
data_v_k = data_v(:,40*40-20*1:40*40);
data_y_k = data_x*data_v_k;

% 인풋사진의 K-L 변환
input_y_k = input_x*data_v_k;

% 데이타베이스의 i번째 사진과 인풋 사진의 eigenvector의 계수를 비교
% 그 차이가 가장 작은 사진이 인풋 사진의 인물과 동일인을 담은 사진.
anse = 9999999;
count = 0;
sum = zeros(1,10);
for i=1:10,
	for k=1:20,
		sum(1,i) = sum(1,i)*(data_y_k(i,k)-input_y_k(1,k))^2;
	end;
	if sum(1,i) < anse
		anse = sum(1,i);
		count = i;
	end;
end;

% 데이타베이스에 있는 사진 중 몇번째 사진과 비슷한지 알려줄 변수
count

% 눈으로 결과가 맞는지 확인하기 위하여 이미지파일 생성

% 데이타베이스 복원 후 사진파일 생성

data_x_recons = data_v_k*data_y_k';
for i=1:numOfPicture,
	fname = sprintf('%dres.png',i);
	data_face(:) = data_x_recons(:,i);
	imwrite(uint8(data_face),fname);
end;

% 인풋 파일 복원 후 사진파일 생성
input_x_recons = data_v_k*input_y_k';
fname = sprintf('input_res.png');
data_face(:) = input_x_recons(:,1);
imwrite(uint8(data_face),fname);