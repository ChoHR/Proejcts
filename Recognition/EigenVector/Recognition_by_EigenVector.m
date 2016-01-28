% training images
n = 20;
x = zeros(n, 45*40);
for k=1:n,
    fname = sprintf('%d.png',k);
    img = double(rgb2gray(imread(fname)));
    x(k,:) = (img(:))';
end;

% pca
c = cov(x);
[v, d] = eig(c);

% average face
face = zeros(45,40);
face(:) = mean(x);
imwrite(uint8(face), 'avg.png');

% eigenfaces (first 20 faces)
for k=1:20,
    fname = sprintf('eig%d.png',k);
    face(:) = v(:,45*40-k+1);
    imwrite(uint8(face/max(face(:))*255), fname);
end;

% reconstruction using only k eigenfaces
cnt = [20, 10, 5, 2];
for k=cnt,
    % K-L transform
    v_k = v(:,45*40-k+1:45*40);
    y_k = x*v_k;
    % reconstruction
    x_recons = v_k*y_k';
    for i=1:n,
        fname = sprintf('%dres%d.png',i,k);
        face(:) = x_recons(:,i);
        imwrite(uint8(face), fname);
    end;
end;
