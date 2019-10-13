#include <opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv\cxcore.h>

#include<stdio.h>
#include<time.h>

#define unsigned char uchar


uchar** uc_alloc(int size_x, int size_y)
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

int** i_alloc(int size_x, int size_y)
{

   int** m;
   int i;

   if ((m = (int**)calloc(size_y, sizeof(int*))) == NULL)
   {
      printf("i_alloc error 1\7\n");
      exit(0);
   }

   for (i = 0; i < size_y; i++)
      if ((m[i] = (int*)calloc(size_x, sizeof(int))) == NULL) {
         printf("i_alloc error 2\7\n");
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

double uniform()
{
   return((double)(rand() & RAND_MAX) / RAND_MAX - 0.5);
}

double gaussian()
{
   static int ready = 0;
   static double gstore;
   double v1, v2, r, fac, gaus;
   double uniform();

   if (ready == 0) {
      do {
         v1 = 2. * uniform();
         v2 = 2. * uniform();
         r = v1 * v1 + v2 * v2;
      } while (r > 1.0);

      fac = sqrt(-2. * log(r) / r);
      gstore = v1 * fac;
      gaus = v2 * fac;
      ready = 1;
   }
   else {
      ready = 0;
      gaus = gstore;
   }
   //printf("gaus = %f\n", gaus);
   return(gaus);
}
//이미지에 가우시안 함수의 리턴값인 가우스를 넣어주는 함수
//gaus자체가 값이 작아 차이가 미미하기 때문에 곱하기 30을 해준다.
//각각의 가우스잡음을 넣은 이미지들을 tmp에 더하여 합쳐준다.
//tmp를 repeatnum만큼 나누어서 평균값을 구하면 원본이미지와 비슷해진다.
void noiseGaus(uchar** img, uchar** Result, int Row, int Col, int repeatnum)
{
    int i, j, k, n;
    double gaus;
    int** tmp;
    tmp = i_alloc(Row, Col);
    // repeatnum만큼 반복
    for (k = 0; k < repeatnum; k++)
    {
        //모든 비트에 잡음을 더해준다.
        for (i = 0; i < Row; i++)
            for (j = 0; j < Col; j++)
            {
                gaus = gaussian();
                //잡음을 더한 이미지를 tmp에 더해준다.
                tmp[i][j] += img[i][j] + (gaus * 30);
            }
    }
    //잡음을 더한 이미지들의 평균값을 구해준다.
    //repeatnum을 올리면 올릴수록 결국 원본과 비슷해진다.
    for (i = 0; i < Row; i++)
        for (i = 0; i < Col; i++)
        {
            Result[j][i] = (tmp[j][i] / repeatnum);
        }
}


int main(int argc, char* argv[])
{

   int i, j, amount;
   IplImage* cvImg;
   CvSize imgSize;
   uchar** img, ** result_img;

   if (argc != 5)
   {
      printf("Exe imgData x_size y_size \n");
      exit(0);
   }

   imgSize.width = atoi(argv[2]);
   imgSize.height = atoi(argv[3]);
   amount = atoi(argv[4]);

   img = uc_alloc(imgSize.width, imgSize.height);
   result_img = uc_alloc(imgSize.width, imgSize.height);

   read_ucmatrix(imgSize.width, imgSize.height, img, argv[1]);

   noiseGaus(img, result_img, imgSize.width, imgSize.height, amount);

   cvImg = cvCreateImage(imgSize, 8, 1);

   for (i = 0; i < imgSize.height; i++)
      for (j = 0; j < imgSize.width; j++)
      {

         ((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = result_img[i][j];

      }
   cvNamedWindow(argv[1], 1);
   cvShowImage(argv[1], cvImg);

   cvWaitKey(0);

   cvDestroyWindow("image");
   cvReleaseImage(&cvImg);

   getchar();

   return 0;

}
