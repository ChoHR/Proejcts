% ����Ÿ���̽��� �������� gray�� ��ȯ�Ͽ� �����Ѵ�.
numOfPicture = 10;
data_x = zeros(numOfPicture, 40*40);
for k=1:numOfPicture,
	fname = sprintf('%d.png',k);
	data_img = double(rgb2gray(imread(fname)));
	data_x(k,:) = (data_img(1:1600))';
end;

% ��ǲ���� ���� �������� gray�� ��ȯ�Ͽ� �����Ѵ�.
input_x = zeros(1, 40*40);
fname = sprintf('input.png');
input_img = double(rgb2gray(imread(fname)));
input_x(1,:) = (input_img(1:1600))';

% pca
data_c = cov(data_x);
[data_v, data_d] = eig(data_c);

% K-L ��ȯ
data_v_k = data_v(:,40*40-20*1:40*40);
data_y_k = data_x*data_v_k;

% ��ǲ������ K-L ��ȯ
input_y_k = input_x*data_v_k;

% ����Ÿ���̽��� i��° ������ ��ǲ ������ eigenvector�� ����� ��
% �� ���̰� ���� ���� ������ ��ǲ ������ �ι��� �������� ���� ����.
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

% ����Ÿ���̽��� �ִ� ���� �� ���° ������ ������� �˷��� ����
count

% ������ ����� �´��� Ȯ���ϱ� ���Ͽ� �̹������� ����

% ����Ÿ���̽� ���� �� �������� ����

data_x_recons = data_v_k*data_y_k';
for i=1:numOfPicture,
	fname = sprintf('%dres.png',i);
	data_face(:) = data_x_recons(:,i);
	imwrite(uint8(data_face),fname);
end;

% ��ǲ ���� ���� �� �������� ����
input_x_recons = data_v_k*input_y_k';
fname = sprintf('input_res.png');
data_face(:) = input_x_recons(:,1);
imwrite(uint8(data_face),fname);