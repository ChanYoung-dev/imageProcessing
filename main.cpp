#include <iostream>

#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv\cxcore.h>
#include<stdio.h>

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

void mosaic(uchar** img, uchar** out, int Row, int Col, int Block) {
    int i, j, x, y, tmp, count;
    for (i = 0; i < Row; i += Block) {
        for (j = 0; j < Col; j += Block) {
            tmp = 0;
            count = 0;
            for (y = 0; y < Block; y++) {
                for (x = 0; x < Block; x++) {
                    tmp += img[i + y][j + x];
                    count++;
                }


                tmp /= count;
                for (y = 0; y < Block; y++) {
                    for (x = 0; x < Block; x++) {
                        out[i + y][j + x] = tmp;
                    }

                }
            }
        }
    }
}

//중앙 원모양만큼 모자이크 하기
//diameter는 원의 크기, block은 모자이크의 강도이다.
void mosaicCircle(uchar** img, uchar** Result, int Row, int Col, double diameter, int Block)
{
    int i, j, x, y, tmp2, c;
    double tmp, xSqure, ySqure;
    //rimg이미지 생성
    uchar** rimg;
    rimg = uc_alloc(Row, Col);
    //img를 모자이크하여 rimg로 저장
    mosaic(img, rimg, Row, Col, Block);
    //모든비트에 작업을 해준다.
    for (i = 0; i < Row; i++)
        for (j = 0; j < Col; j++)
        {
            // 공식을 통하여 원이 되는 좌표를 만들어 준다. r2 = x2 + y2
            ySqure = (abs(Row / 2 - i)) * (abs(Row / 2 - i));
            xSqure = (abs(Col / 2 - j)) * (abs(Col / 2 - j));
            //루트를 씌어주는 함수 r = 루트 x2 + y2
            tmp = sqrt(ySqure + xSqure);
            //원 안쪽은 모자이크 작업
            if (tmp < diameter) Result[i][j] = rimg[i][j];
                //원 바깥쪽은 원본 이미지를 씌운다.
            else Result[i][j] = img[i][j];
        }
}

int main(int argc, char* argv[])
{

    int i, j, circle_size, mosaic_value;
    IplImage* cvImg;
    CvSize imgSize;
    uchar** img, ** result_img, ** result2_img, ** circle_img, ** circle2_img;

    if (argc != 6)
    {
        printf("Exe imgData x_size y_size \n");
        exit(0);
    }

    imgSize.width = atoi(argv[2]);
    imgSize.height = atoi(argv[3]);
    circle_size = atoi(argv[4]);
    mosaic_value = atoi(argv[5]);

    img = uc_alloc(imgSize.width, imgSize.height);
    result_img = uc_alloc(imgSize.width, imgSize.height);
    read_ucmatrix(imgSize.width, imgSize.height, img, argv[1]);

    cvImg = cvCreateImage(imgSize, 8, 1);

    mosaicCircle(img, result_img, imgSize.width, imgSize.height, circle_size, mosaic_value);

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
