#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

double MSE_PSNR(int rows, int cols, Mat image, Mat shift_image);
double L(int rows, int cols, Mat image, Mat shift_image);
double C(int rows, int cols, Mat image, Mat shift_image);
double S(int rows, int cols, Mat image, Mat shift_image);
int i, j;
double psnr;
//LUMINANCE參數
double mean_as;//sigma image & shift_image
double mean_a;
double mean_s;
double luminance;
//CONTRAST參數
double contrast_a;
double contrast_s;
double contrast;

int main(int argc, char** argv){
	Mat image = imread("original.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	int cols = image.cols;//width
	int rows = image.rows;//height
	double l;
	double c;
	double s;
	double ssim;
	FILE *temp;
	temp = fopen("SSI_PSNR.txt", "w");
	//printf("rows=%d\n", rows);
	//printf("cols=%d\n", cols);
	Mat shift_image = Mat(rows, cols, CV_8UC1);
	for (j = 0; j<cols-1; j++)//右移
	{
		for (i = 0; i<rows; i++)
		{
			if (j == 0)
			{
				shift_image.at<unsigned char>(i, j + 1) = image.at<unsigned char>(i, j);
				shift_image.at<unsigned char>(i, j) = image.at<unsigned char>(i, j);
			}
			else shift_image.at<unsigned char>(i, j + 1) = image.at<unsigned char>(i, j);
		}
	}
	MSE_PSNR(rows, cols, image, shift_image);
	l = L(rows, cols, image, shift_image);
	c = C(rows, cols, image, shift_image);
	s = S(rows, cols, image, shift_image);
	ssim = l*c*s;//ssim
	//printf("\nSSIM=%f", ssim);
	fprintf(temp, "ssim=%f,psnr=%f\n", ssim, psnr);
	namedWindow("original", CV_WINDOW_AUTOSIZE);//顯示image圖
	imshow("original", image);
	namedWindow("output", CV_WINDOW_AUTOSIZE);//顯示shift_image圖
	imshow("output", shift_image);
	imwrite("gray_image.jpg", image);
	imwrite( "output.jpg", shift_image);
	waitKey(0);
	fclose(temp);
	return 0;
}
double MSE_PSNR(int rows, int cols, Mat image, Mat shift_image)
{
	double difference = 0;
	double sigma = 0;
	double mse;
	for (i = 0; i<rows; i++)
	{
		for (j = 0; j<cols; j++)
		{
			(double)difference = (double)image.at<unsigned char>(i, j) - (double)shift_image.at<unsigned char>(i, j);
			sigma = sigma + (double)difference*(double)difference;
		}
	}
	mse = sigma / (rows*cols);
	psnr = 10 * log10(255 * 255 / mse);//PSNR
	//printf("PSNR=%f\n", psnr);
	return psnr;
}
double L(int rows, int cols, Mat image, Mat shift_image)
{
	double sum_a = 0;
	double sum_s = 0;
	double C1 = 0.1;
	for (i = 0; i<rows; i++)
	{
		for (j = 0; j<cols; j++)
		{
			sum_a = sum_a + (double)image.at<unsigned char>(i, j);
			sum_s = sum_s + (double)shift_image.at<unsigned char>(i, j);
		}
	}
	mean_a = sum_a / (rows*cols);
	mean_s = sum_s / (rows*cols);;
	luminance = (2 * mean_a*mean_s + C1) / (mean_a*mean_a + mean_s*mean_s + C1);
	//printf("\nLUMINANCE=%f\n", luminance);
	return luminance;
}
double C(int rows, int cols, Mat image, Mat shift_image)
{
	double variance_a = 0;//image變異數
	double variance_s = 0;//shift_image變異數
	int C2 = 0.1;
	for (i = 0; i<rows; i++)
	{
		for (j = 0; j<cols; j++)
		{
			variance_a = variance_a + ((double)image.at<unsigned char>(i, j) - mean_a)
				*((double)image.at<unsigned char>(i, j) - mean_a);
			variance_s = variance_s + ((double)shift_image.at<unsigned char>(i, j) - mean_s)
				*((double)shift_image.at<unsigned char>(i, j) - mean_s);
		}
	}
	contrast_a = sqrt(variance_a / (rows*cols));
	contrast_s = sqrt(variance_s / (rows*cols));
	contrast = ((2 * contrast_a*contrast_s) + C2) / (contrast_a*contrast_a + contrast_s*contrast_s + C2);
	//printf("CONTRAST=%f\n", contrast);
	return contrast;
}
double S(int rows, int cols, Mat image, Mat shift_image)
{
	double variance_as = 0;
	double contrast_as;
	double structure;
	double C3 = 0.1;
	for (i = 0; i<rows; i++)
	{
		for (j = 0; j<cols; j++)
		{
			variance_as = variance_as + (image.at<unsigned char>(i, j) - mean_a)
				*(shift_image.at<unsigned char>(i, j) - mean_s);
		}
	}
	contrast_as = variance_as / (rows*cols - 1);
	structure = (contrast_as + C3) / (contrast_a*contrast_s + C3);
	//printf("STRUCTURE=%f\n", structure);
	return structure;

}