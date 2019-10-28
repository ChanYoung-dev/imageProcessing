/*
Compile : g++ -o $1 $1.c -I/usr/local/include -L/usr/local/lib -lopencv_core
-loencv_highgui -lopencv_imgproc -lopencv_imgcodecs

opencv ���̺귯���� /usr/local/lib �� ��ġ�Ǿ� �ִ� ��쿡 �����ϴ� �������̴�.
*/

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define unsigned char uchar

int histogram[256], cdfOfHisto[256], histogramEqual[256];

uchar** uc_alloc(int size_x, int size_y)
{

	uchar** m;
	int i;

	if ((m = (uchar**)calloc(size_y, sizeof(uchar*))) == NULL)
	{
		printf("uc_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL) {
			printf("uc_alloc error 2\7\n");
			exit(0);

		}
	return m;
}

void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;
	if ((fopen_s(&f, filename, "rb")) != NULL)
	{
		printf("%s File open Error! \n", filename);
		exit(0);

	}
	for (i = 0; i < size_y; i++)
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Read Error! \n");
			exit(0);

		}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "wb")) != NULL)
	{
		printf("%s File open Error! \n", filename);
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Write Error! \n");
			exit(0);
		}
	fclose(f);
}

void get_hist(uchar** img, int X_Size, int Y_Size) {
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;
	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);

	for (i = 0; i < histoSize.height; i++) {
		for (j = 0; j < histoSize.width; j++) {
			((uchar*)(imgHisto->imageData + imgHisto->widthStep * i))[j] = 0;
		}
	}
	for (i = 0; i < cdfSize.height; i++) {
		for (j = 0; j < cdfSize.width; j++) {
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}
	}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;

	// Find the maximum histogram value
	tmp1 = 0;
	for (i = 0; i < 256; ++i) {
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}

	for (i = 0; i < 256; ++i) {
		tmp = (int)255 * (histogram[i] / tmp1);
		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);

	}
	cvShowImage("Histo Line", imgHisto);

	cdfOfHisto[0] = histogram[0];

	for (i = 0; i < 256; i++) {
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
	}

	//Draw CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; i++) {
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);
		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}

	cvShowImage("Original CDF of Histogram", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];
	printf("%d  %d\n", tp, range);
	histogramEqual[0] = 0;

	for (i = 1; i < 256; ++i) {
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}

	cvReleaseImage(&imgHisto);
	cvReleaseImage(&cdfImgHisto);
	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]];

}

void get_hist1(uchar** img, int X_Size, int Y_Size) {
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;
	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);

	for (i = 0; i < histoSize.height; i++) {
		for (j = 0; j < histoSize.width; j++) {
			((uchar*)(imgHisto->imageData + imgHisto->widthStep * i))[j] = 0;
		}
	}
	for (i = 0; i < cdfSize.height; i++) {
		for (j = 0; j < cdfSize.width; j++) {
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}
	}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;

	// Find the maximum histogram value
	tmp1 = 0;
	for (i = 0; i < 256; ++i) {
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}

	for (i = 0; i < 256; ++i) {
		tmp = (int)255 * (histogram[i] / tmp1);
		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);

	}
	cvShowImage("Histo Equal", imgHisto);

	cdfOfHisto[0] = histogram[0];

	for (i = 0; i < 256; i++) {
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
	}

	//Draw CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; i++) {
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);
		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}

	cvShowImage("CDF of Histogram", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];
	printf("%d  %d\n", tp, range);
	histogramEqual[0] = 0;

	for (i = 1; i < 256; ++i) {
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}

	cvReleaseImage(&imgHisto);
	cvReleaseImage(&cdfImgHisto);
	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]];

}

int main(int argc, char* argv[])
{

	int i, j;
	IplImage* cvImg;
	CvSize imgSize;
	uchar** img;

	if (argc != 4)
	{
		printf("Exe imgData x_size y_size \n");
		exit(0);
	}

	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);
	img = uc_alloc(imgSize.width, imgSize.height);
	read_ucmatrix(imgSize.width, imgSize.height, img, argv[1]);
	cvImg = cvCreateImage(imgSize, 8, 1);

	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{

			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		}

	cvNamedWindow(argv[1], 1);
	cvShowImage(argv[1], cvImg);

	get_hist(img, imgSize.width, imgSize.height);

	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{

			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		}

	cvShowImage("HistoGram Equalizer...", cvImg);
	get_hist1(img, imgSize.width, imgSize.height);

	cvWaitKey(0);
	cvDestroyWindow("image");
	cvReleaseImage(&cvImg);

	getchar();

	return 0;

}
