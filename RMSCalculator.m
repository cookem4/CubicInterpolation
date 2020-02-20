clear

colorImg = imread('finalImage.jpg');
grayImgCubic = rgb2gray( colorImg );

colorImg = imread('burrito-ground-truth.png');
grayImgGT = rgb2gray( colorImg );


differenceMat = grayImgGT - grayImgCubic;
squareMat= differenceMat.*differenceMat;

s = mean2(squareMat);
RMSCubic = sqrt(s);

colorImg = imread('burrito-ML-prediction.png');
grayImgML = rgb2gray( colorImg );

differenceMat = grayImgGT - grayImgML;
squareMat= differenceMat.*differenceMat;

s = mean2(squareMat);
RMSML = sqrt(s);