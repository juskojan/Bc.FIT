%%%%% Projekt do predmetu ISS %%%%%
%%%%% 			2014		  %%%%%
%%%%%		 Ján Jusko		  %%%%%
%%%%%		  xjusko00		  %%%%%

%%%%% Skript pre MATLAB R2012b%%%%%

% INIT, NACITANIE OBRAZKU
iptsetpref('UseIPPL',false);
original_src = imread('xjusko00.bmp');


% 1) ZAOSTENI OBRAZU
step1_src = imread('xjusko00.bmp');
H1 = [-0.5 -0.5 -0.5; -0.5 5.0 -0.5; -0.5 -0.5 -0.5];	%matica zaostrenia obrazu
step1_IMG = imfilter(step1_src, H1);
imwrite(step1_IMG, 'step1.bmp');


% 2) OTOCENIE OBRAZU
step2_IMG = fliplr(step1_IMG);
imwrite(step2_IMG, 'step2.bmp');


% 3) MEDIANOVY FILTER
step3_IMG = medfilt2(step2_IMG, [5 5]);
imwrite(step3_IMG, 'step3.bmp');


% 4) ROZMAZANI OBRAZU
H2 = [1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49;
step4_IMG = imfilter(step3_IMG, H2);
imwrite(step4_IMG, 'step4.bmp');


% 5) CHYBA
original_flipped = fliplr(original_src);
original_flippedD = double(original_flipped);
step4_IMGD = double(step4_IMG);
chyba = 0;

for (x = 1:512)
	for (y = 1:512)
		chyba = chyba + (abs(original_flippedD(x,y) - step4_IMGD(x,y)));
	end;
end;

chyba = chyba / 512 / 512;


% 6) ROZTIAHNUTIE HISTOGRAMU
step4_IMGD = im2double(step4_IMG);
minimum = min(min(step4_IMGD));
maximum = max(max(step4_IMGD));

step5_IMG = imadjust(step4_IMG, [minimum maximum], [0 1]);
step5_IMG = im2uint8(step5_IMG);
imwrite (step5_IMG, 'step5.bmp');


% 7) STREDNA HODNOTA, SMERODATNA ODCHYLKA
step4_IMGD = double(step4_IMG);
mean_no_hist = mean2(step4_IMGD)
std_no_hist = std2(step4_IMGD)

step5_IMGD = double(step5_IMG);
mean_hist = mean2(step5_IMGD)
std_hist = std2(step5_IMGD)


% 8) KVANTIZACE OBRAZU
a = 0;
b = 255;
N = 2;

tmp = double(step5_IMG);
step6_IMG = zeros(512,512);
for x=1:512
   for y=1:512
      step6_IMG(x,y) = round(((2^N)-1)*(tmp(x,y)-a)/(b-a))*(b-a)/((2^N)-1) + a;
   end
end

step6_IMG=uint8(step6_IMG);
imwrite(step6_IMG, 'step6.bmp');


 