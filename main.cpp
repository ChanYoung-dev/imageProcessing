#include <opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv\cxcore.h>

#include <stdio.h>

#define unsigned char uchar


uchar** uc_alloc(int size_x, int size_y) // ��� �޸� �Ҵ� x,y
{

	uchar** m;
	int i;

	if ((m = (uchar * *)calloc(size_y, sizeof(uchar*))) == NULL)
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
//평균값
double average(uchar** img, int size_x, int size_y)
{
	double sum = 0, avg;
	int i, j;

	for (i = 0; i < size_x; i++)
	{
		for (j = 0; j < size_y; j++)
		{
			sum += img[i][j];
		}
	}
	avg = sum / ((double)size_x * size_y);
	printf("Average of Image %lf \n", avg);

	return avg;
}
//분산값 구하기
double bunsan(uchar** img, int size_x, int size_y)
{
	double avg = average(img, size_x, size_y);
	double total = 0, valance = 0;
	int i, j;
	for (i = 0; i < size_x; i++)
	{
		for (j = 0; j < size_y; j++)
		{
			valance += (img[i][j] - avg) * (img[i][j] - avg);
		}
	}
	total = valance / (size_x * size_y);
	printf("�л��� %lf \n", total);
	printf("ǥ�������� %lf \n", sqrt(total));

	return avg;

}

void makeBinary(uchar** img, uchar** out, int size_x, int size_y, double avg)
{
	int i, j;

	for (i = 0; i < size_x; i++)
	{
		for (j = 0; j < size_y; j++)
		{
			if (img[i][j] > avg)
				out[i][j] = 255;      // chang value for book with avg-30
			else
				out[i][j] = 0;
		}
	}
}

void PowImg(uchar** img, uchar** Result, int Row, int Col, double gamma)
{
	int i, j;
	double tmp;

	for (i = 0; i < Row; i++)//��
		for (j = 0; j < Col; j++)//��
		{
			tmp = pow(img[j][i] / 255., 1 / gamma);

			if (tmp * 255 > 255)tmp = 1;
			else if (tmp * 255 < 0) tmp = 0;

			tmp = tmp * 255;

			Result[j][i] = tmp;
		}
}

void BitSlicing(uchar** img, uchar** Result, int Row, int Col, int position)
{
	int i, j;
	uchar mask = 0x01;
	mask <<= position;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			if ((mask & img[j][i]) == pow(2, position))
			{
				Result[j][i] = pow(2, position);
			}
			else
			{
				Result[j][i] = 0;
			}

		}
}
void BitImage(uchar** img, uchar** Result, int Row, int Col, int sum)
{
	int i, j, k;
	uchar mask = 0x80;

	for (k = 1; k < sum; k++)
	{
		mask = mask + (0x80 >> k);
	}

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			Result[i][j] = img[i][j] & mask;
		}
}
double myaver(uchar** img, int Row, int Col) {
	int i, j, k;
	double pp[256] = { 0 };
	double ssum = 0;

	for (k = 0; k < 256; k++) {
		for (i = 0; i < Row; i++) {
			for (j = 0; j < Col; j++) {
				if (img[i][j] == k) {
					pp[k] += 1;
				}
			}
		}
		ssum += ((pp[k] * k) / (Row * Col));

	}
	return ssum;

}


int main(int argc, char* argv[])
{

	double avg = 0;
	double gamma = 0.5;
	IplImage* cvImg;
	CvSize imgSize;
	uchar** img;
	uchar** rimg;

	if (argc != 5)
	{
		printf("Exe imgData x_size y_size resultimg \n");
		exit(0);
	}
	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);
	img = uc_alloc(imgSize.width, imgSize.height);
	rimg = uc_alloc(imgSize.width, imgSize.height);  //결과 이미지를 저장할 곳을 할당해줌
	read_ucmatrix(imgSize.width, imgSize.height, img, argv[1]);
	BitImage(img, rimg, imgSize.width, imgSize.height, 3);
	//addImage(img, rimg, imgSize.width, imgSize.height, 3);
	write_ucmatrix(imgSize.width, imgSize.height, rimg, argv[4]);  //결과값 저장
	average(rimg, imgSize.width, imgSize.height);

	/*cvImg = cvCreateImage(imgSize, 8, 1);
	for (i = 0; i < imgSize.height; i++)
	   for (j = 0; j < imgSize.width; j++)
	   {

		  ((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

	   }
	   cvNamedWindow(argv[1], 1);
	   cvShowImage(argv[1], cvImg);


	cvWaitKey(0);

	cvDestroyWindow("image");
	cvReleaseImage(&cvImg);

	getchar();
	getchar();*/

	return 0;

}