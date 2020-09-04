#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <iostream>
#include <limits.h> 
#include <Eigen/Dense>
#include<math.h>
using namespace cv;
using namespace std;
using namespace Eigen;

using Eigen::MatrixXd;
using Eigen::VectorXd;

__int64 **Blue;
__int64 **Green;
__int64 **Red;

__int64**Y;
__int64 **Cb;
__int64 **Cr;

__int64**Y1;
__int64 **Cb1;
__int64 **Cr1;
__int64 **pixelvalue;


int resultcount[25] = { 0 }; // ������ ���� ������ �� �������� �����ϱ� ����

void DynamicAllocation(int A, int B); // �����Ҵ�
void DeleteArray(int A, int B); // �����Ҵ� ������ �޸� ���� ����
void changeRGB(Mat &mat, int cols, int rows, int num); // RGB�� YCbCr�� ��ȯ
void changeYCbCr(Mat &mat, int cols, int rows); // YCbCr�� RGB�� ��ȯ
void firstloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR); // ù ��° loop�ν�, �ּ��ڽ¹��� ����ؼ� filter�� ����
void secondloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR, Mat*filterbank); // �ι�° �����ν�, ������� filter�� ����ؼ� �̹����� ����.
void makefilter(Mat*matrix, Mat*matrixR, Mat*filterbank); // ���͸� ����.
void printimage(Mat&image, int num); // ��� �̹����� �����
double checkpsnr(Mat&image, Mat&image1); // psnr�� üũ�ϱ� ���� �Լ�

int main()
{
	Mat image;
	Mat image1;
	image = imread("0063_40.png", IMREAD_COLOR); // Read the file, ������ �ִ� �̹���
	image1 = imread("0063.png", IMREAD_COLOR); // Read the file ������ ���� �̹���

	//�������� �ʴ� �̹������ ����
	if (image.empty() || image1.empty())
		return 0;

	// ���� �̹����� ��� ���� �����س���
	int A = image.rows;
	int B = image.cols;

	// �� 25���� class�� ������, ���� 9���� ���Ұ� �����ϱ� ������ 9���� Ŭ������ ���� matrix�� �����Ѵ�.
	// ���⼭ matrix�� 9���� ���Ҹ� �������·� �����ϴ� ����̰�, matrixR�� ������ ������ ���� ��ǥġ�� ��Ÿ����. �� ���� �̹����� �ȼ��� ����ȴ�.
	Mat matrix[25];
	for (int i = 0; i < 25; i++)
		matrix[i] = Mat::zeros(9, 5000000, CV_64FC1);

	Mat matrixR[25];
	for (int i = 0; i < 25; i++)
		matrixR[i] = Mat::zeros(5000000, 1, CV_64FC1);

	// filterbank�� 3*3*25�� ũ�⸦ ������. �̸� 0���� �ʱ�ȭ�ϰ� �����Ѵ�.
	Mat *filterbank = new Mat[25];
	for (int i = 0; i < 25; i++)
		filterbank[i] = Mat::zeros(3, 3, CV_64FC1);

	// �ʿ��� �迭�� �����Ҵ�
	DynamicAllocation(A, B);

	changeRGB(image, A, B, 0); // change rgb to ycbcr, Y�� ������ �̹���, Y1�� ��ǥ �̹���, ������ ������ �迭�� �־��� ���� ����� rgb�� YCbCr�� ��ȯ
	changeRGB(image1, A, B, 1); // 0�� ������ �̹���, 1�� ��ǥ �̹����� numbering.

	// ù ��° loop�ν�, filter�� ����� ���ؼ� ������ �ᱣ���� �����ϴ� ����, �� �������� �̹����� Y�� ����
	firstloop(image, image1, A, B, matrix, matrixR);

	// ���� ����� �̹����� psnr�� ��
	cout << "���� �� �̹����� psnr�� : " << checkpsnr(image, image1) << endl;

	// �ּ��ڽ¹��� �̿�, filter�� ����
	makefilter(matrix, matrixR, filterbank);

	// �� ��° loop�ν�, ������� filter�� ����ؼ� ����� scan�ϰ�, �� ������ �̹��� pixel���� ����
	secondloop(image, image1, A, B, matrix, matrixR, filterbank);

	// gray���� �̹��� ����
	printimage(image, 0);

	// filtering�� �ᱣ���� ��. Y�� ����Ǿ��� ������, Y�� ���� ���ؼ� psnr�� �����ϰ�, ����Ѵ�.
	cout << "���� �� �̹����� psnr�� : " << checkpsnr(image, image1) << endl;

	// YCbCr�� ����  RGB�� ����
	changeYCbCr(image, A, B);

	//RGB�� ����� �̹����� ���
	printimage(image, 1);

	//�����Ҵ� ����
	DeleteArray(A, B);

	system("pause");

	return 0;
}


// rgb�� ycbcr�� ��ȯ
void changeYCbCr(Mat &image, int A, int B)
{
	//loop�� A*B��ŭ ȸ��, �־��� ������ ����ؼ� YCbCr���� RGB�� �����Ѵ�.
	for (int i = 0; i < A; i++)
		for (int j = 0; j < B; j++)
		{
			int C = Y[i][j] - 16, D = Cb[i][j] - 128, E = Cr[i][j] - 128;
			Red[i][j] = ((298 * C + 409 * E + 128) >> 8);
			Green[i][j] = ((298 * C - 100 * D - 208 * E + 128) >> 8);
			Blue[i][j] = ((298 * C + 516 * D + 128) >> 8);

			//������ �̹��� �ȼ��� �ִ� 255 �ּ� 0�� �����.
			if (Red[i][j] > 255)
				Red[i][j] = 255;
			else if (Red[i][j] < 0)
				Red[i][j] = 0;

			if (Blue[i][j] > 255)
				Blue[i][j] = 255;
			else if (Blue[i][j] < 0)
				Blue[i][j] = 0;

			if (Green[i][j] > 255)
				Green[i][j] = 255;
			else if (Green[i][j] < 0)
				Green[i][j] = 0;

			// ������ ���� �˸��� ������ �����Ѵ�.
			image.at<Vec3b>(i, j)[0] = Blue[i][j];
			image.at<Vec3b>(i, j)[1] = Green[i][j];
			image.at<Vec3b>(i, j)[2] = Red[i][j];
		}
}
// rgb�� ycbcr�� ��ȯ
void changeRGB(Mat &mat, int cols, int rows, int num)
{
	//loop�� A*B��ŭ ȸ��, �־��� ������ ����ؼ� YCbCr���� RGB�� �����Ѵ�.

	// ù ��°��, ���� RGB���� �̾Ƴ����ϱ� ������, �̹����� ���� scan�ؼ� ������ rgb���� �����Ѵ�.
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			Blue[i][j] = (__int64)mat.at<Vec3b>(i, j)[0];
			Green[i][j] = (__int64)mat.at<Vec3b>(i, j)[1];
			Red[i][j] = (__int64)mat.at<Vec3b>(i, j)[2];
		}
	}
	// num = 0�̸� ����� �ִ� �̹���, 1�̸� ����� ���� �̹����� �ȴ�. ������ ��Ȳ�� �°� YCbCr�� ���Ŀ� ���� �̾Ƴ���.
	if (num == 0)
	{
		for (int i = 0; i < cols; i++)
		{
			for (int j = 0; j < rows; j++)
			{
				Y[i][j] = ((Red[i][j] * 66 + 129 * Green[i][j] + 25 * Blue[i][j] + 128) >> 8) + 16;
				Cb[i][j] = ((Red[i][j] * (-38) + (-74) * Green[i][j] + 112 * Blue[i][j] + 128) >> 8) + 128;
				Cr[i][j] = ((Red[i][j] * 112 + (-94) * Green[i][j] + (-18) * Blue[i][j] + 128) >> 8) + 128;

			}
		}
	}
	else
	{
		for (int i = 0; i < cols; i++)
		{
			for (int j = 0; j < rows; j++)
			{
				Y1[i][j] = ((Red[i][j] * 66 + 129 * Green[i][j] + 25 * Blue[i][j] + 128) >> 8) + 16;
				Cb1[i][j] = ((Red[i][j] * (-38) + (-74) * Green[i][j] + 112 * Blue[i][j] + 128) >> 8) + 128;
				Cr1[i][j] = ((Red[i][j] * 112 + (-94) * Green[i][j] + (-18) * Blue[i][j] + 128) >> 8) + 128;

			}
		}
	}
}
// �����Ҵ�
void DynamicAllocation(int A, int B)
{

	//������ ����. Y,Cb,Cr�� ������ �ִ� �̹���, Y1,Cb1,Cr1�� ������ ���� �̹���, 2���� �����Ҵ��� �Ѵ�.
	pixelvalue = new __int64*[A]();
	Blue = new  __int64*[A]();
	Red = new  __int64*[A]();
	Green = new  __int64*[A]();
	Y = new __int64*[A]();
	Cb = new __int64*[A]();
	Cr = new __int64*[A]();
	Y1 = new __int64*[A]();
	Cb1 = new __int64*[A]();
	Cr1 = new __int64*[A]();
	for (int i = 0; i < A; i++)
	{
		Blue[i] = new  __int64[B]();
		Red[i] = new  __int64[B]();
		Green[i] = new  __int64[B]();
		Y[i] = new __int64[B]();
		Cb[i] = new __int64[B]();
		Cr[i] = new __int64[B]();
		Y1[i] = new __int64[B]();
		Cb1[i] = new __int64[B]();
		Cr1[i] = new __int64[B]();
		pixelvalue[i] = new __int64[B]();
	}
}
// �����Ҵ� ����
void DeleteArray(int A, int B)
{
	// ��� ����� �Ϸ�Ǿ����� �����Ҵ��� �����Ѵ�.
	for (int i = 0; i < A; i++)
	{
		delete[]Blue[i];
		delete[]Red[i];
		delete[]Green[i];
		delete[]Y[i];
		delete[]Cb[i];
		delete[]Cr[i];
		delete[]Y1[i];
		delete[]Cb1[i];
		delete[]Cr1[i];
	}

	delete[]Blue;
	delete[]Red;
	delete[]Green;
	delete[]Y;
	delete[]Cb;
	delete[]Cr;
	delete[]Y1;
	delete[]Cb1;
	delete[]Cr1;
}
// ù ��° loop. filter�� ���ϱ� ���� ������ class�� classify�Ѵ�.
void firstloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR)
{
	// ������ �����ϴ� filter.  �� 90��
	int vertical[3][3] = { -1,0,1,
						   -2,0,2,
							-1,0,1 };


	//������ �����ϴ� filter. �� 0��
	int horizantal[3][3] = { 1,2,1,
							0,0,0,
							-1,-2,-1 };

	// ���� �밢���� �����ϴ� filter. �� 135�� 
	int left[3][3] = { 0,1,2,
					-1,0,1,
					-2,-1,0 };


	// ���� �밢���� �����ϴ� filter. �� 45��
	int right[3][3] = { 2,1,0,
						1,0,-1,
						0,-1,-2 };



	// loop�� ��� pixel�� ���ؼ� ����.
	for (int i = 0; i < A; i++)
	{
		for (int j = 0; j < B; j++)
		{
			int result = 0; // ���� class�� ������
			int compvertical = 0, comphorizantal = 0, compleft = 0, compright = 0; // ���⼺
			int activity = 0, direct = 0;
			int arc = 0; // �迭�� ����
			int temparc[9];
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++) // 3*3 mask�� �����Ƿ�, mask�� �̿��� convolution�� �����Ѵ�.
				{
					if (!(i == 0 || i == A - 1 || j == 0 || j == B - 1)) // mask�� �̹����� ������ ����� �ʴ´ٸ� �Ϲ����� ����� ����Ѵ�.
					{
						result += Y[i - 1 + l][j - 1 + k] * vertical[l][k]; // Ȱ������ ���ϱ� ���� �� result. vertical�� horizantal�� �տ� ���� ���� mapping�Ѵ�.
						result += Y[i - 1 + l][j - 1 + k] * horizantal[l][k];
						compvertical += Y[i - 1 + l][j - 1 + k] * vertical[l][k];
						comphorizantal += Y[i - 1 + l][j - 1 + k] * horizantal[l][k];
						compleft += Y[i - 1 + l][j - 1 + k] * left[l][k];
						compright += Y[i - 1 + l][j - 1 + k] * right[l][k];
						// ���͸� 4�� ��ĵ�ؼ� ���⼺�� ���Ѵ�.
						temparc[arc++] = Y[i - 1 + l][j - 1 + k];
						// filter�� � �������� �����Ѵ�.
					}
					else // mask�� �̹����� ������ ����ٸ� ��� �̹����� ������ ���� �ȼ��� ������ ���Ͻ�Ų��.
					{
						result += Y[i][j] * vertical[l][k]; // Ȱ������ ���ϱ� ���� �� result. vertical�� horizantal�� �տ� ���� ���� mapping�Ѵ�.
						result += Y[i][j] * horizantal[l][k];
						compvertical += Y[i][j] * vertical[l][k];
						comphorizantal += Y[i][j] * horizantal[l][k];
						compleft += Y[i][j] * left[l][k];
						compright += Y[i][j] * right[l][k];
						// ���͸� 4�� ��ĵ�ؼ� ���⼺�� ���Ѵ�.
						temparc[arc++] = Y[i][j];
						// filter�� � �������� �����Ѵ�.
					}
				}
			// max is 1020. 0,1,2�� ����ġ�� �ξ� �����ϰ� �м���ų �� �ֵ��� �ߵ�.
			if (abs(result) < 50)
				activity = 0;
			else if (abs(result) < 100)
				activity = 1;
			else if (abs(result) < 170)
				activity = 2;
			else if (abs(result) < 270)
				activity = 3;
			else
				activity = 4;

			//��� ������� ������ ���� �ֱ� ������ ���� ������ �Ѵ�.
			compvertical = abs(compvertical);
			comphorizantal = abs(comphorizantal);
			compleft = abs(compleft);
			compright = abs(compright);



			// ������ ���⼺�� ���Ѵ�. ���� ū ���ڰ� �ش��ϴ� ���⼺�� �ȴ�. ū ���� �������� ��쿡��, Ư�� ������ ����ؼ� ���⼺�� ���Ѵ�.
			if ((compvertical > comphorizantal && compvertical > compleft && compvertical > compright) || (compvertical == compright && compright == compleft && compleft > comphorizantal) || (((compright == compvertical) || ((compright == comphorizantal)) && compvertical > comphorizantal && compvertical > compleft))) // 90���� ��
				direct = 3;
			else if ((comphorizantal > compvertical && comphorizantal > compleft && comphorizantal > compright) || (comphorizantal == compleft && compleft == compright && compright > compvertical) || (((compleft == compvertical) || (compleft == comphorizantal)) && compvertical > comphorizantal && compvertical > compright)) // 0���� ��
				direct = 1;
			else if ((compright > comphorizantal && compright > compvertical && compright > compleft) || (compright == compvertical && compvertical == comphorizantal && compvertical > compleft)) // 45���� ��
				direct = 2;
			else if ((compleft > comphorizantal && compleft > compvertical && compleft > compright) || (compleft == compvertical && compleft == comphorizantal && compleft > compright)) // 135���� ��
				direct = 4;
			else // �������϶�
				direct = 0;

			// classify�� class�� ã�´�.
			result = activity * 5 + direct;


			// ���� pixel�� classify number�� �����Ѵ�.
			pixelvalue[i][j] = result;

			for (int k = 0; k < 9; k++)
			{
				// mask�� scan�� ������ ���Ҹ� ���� matrix�� �����Ѵ�.
				matrix[result].at<double>(k, resultcount[result]) = temparc[k];
			}
			// mask�� scan�ؼ� ���;� �� ��ǥ���� matrixR�� �����Ѵ�
			matrixR[result].at<double>(resultcount[result], 0) = Y1[i][j];


			// ����� ����Ǿ��ٸ�, ����� �迭�� ���� �ϳ� �ø���.
			resultcount[result] += 1;

			// psnr�� ���ϱ� ���� �̹����� Y�� ��ȯ��Ų��.
			image1.at<Vec3b>(i, j)[0] = Y1[i][j];
			image1.at<Vec3b>(i, j)[1] = Y1[i][j];
			image1.at<Vec3b>(i, j)[2] = Y1[i][j];
			image.at<Vec3b>(i, j)[0] = Y[i][j];
			image.at<Vec3b>(i, j)[1] = Y[i][j];
			image.at<Vec3b>(i, j)[2] = Y[i][j];

		}

	}
}

// �� ��° loop�ν�, ���� ���͸� ����� �̹����� ���͸��ؼ� ���� ���Ѵ�.
void secondloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR, Mat*filterbank)
{
	// 2��° loop��, ���������͸� ����� �̹����� ���͸��ϱ����� loop�� �ȴ�.
	for (int i = 0; i < A; i++)
	{
		for (int j = 0; j < B; j++)
		{
			double imagesum = 0; // ���������� ����Ұ�갪
			{
				int result = 0;

				// ������ pixel���� �ҷ��´�.
				result = pixelvalue[i][j];

				Mat comp = Mat::zeros(3, 3, CV_64FC1); // mask�� scan�� ������ �ӽ÷� ������ ����


				// mask�� scan�� 3*3������ �����Ѵ�.
				for (int k = 0; k < 3; k++)
					for (int l = 0; l < 3; l++)
						if (!(i == 0 || i == A - 1 || j == 0 || j == B - 1))
							comp.at<double>(k, l) = Y[i - 1 + l][j - 1 + k];
						else
							comp.at<double>(k, l) = Y[i][j];

				// ������ ���� filter�� �̿��ؼ� convolution�� �����ϰ�, ��� ���Ұ��� ���Ѵ�.
				for (int k = 0; k < 3; k++)
					for (int l = 0; l < 3; l++)
						imagesum += (comp.at<double>(k, l)) * (filterbank[result].at<double>(k * 3 + l, 0));

				// ���Ұ��� ���� ���� �� pixel�� ���� �ǹ��Ѵ�. pixel�� 0�� 255���̷� �����Ѵ�.
				if (imagesum > 255)
					imagesum = 255;
				else if (imagesum < 0)
					imagesum = 0;

				// ���� ���� �̹����� �����Ѵ�. 
				image.at<Vec3b>(i, j)[0] = floor(imagesum);
				image.at<Vec3b>(i, j)[1] = floor(imagesum);
				image.at<Vec3b>(i, j)[2] = floor(imagesum);
			}


		}
	}
}
//filter�� ����� ��������, �ּ��ڽ¹��� ����Ѵ�.
void makefilter(Mat*matrix, Mat*matrixR, Mat*filterbank)
{

	for (int k = 0; k < 25; k++) // ������ classify�� level�� ���� �������� ���͸� �����Ѵ�.
	{
		if (resultcount[k] == 0) // ���ο� ���Ұ� �ϳ��� ���ٸ�
		{
			filterbank[k].at<double>(1, 1) = 1; // 0,0,0 0,1,0 0,0,0���� �������Ѽ� ���͸� ����ϵ��� �Ѵ�.
		}
		else
		{
			//�ʱ� ������ 500������ �迭�� ������, ������ �迭�� ����� ������ �̺��� �ξ� ���� �� �ִ�.
			// ����, ���ʿ��� ����� ���ʿ��� ���� ����� �����ϱ� ���� ����Ѹ�ŭ�� �迭�� �缱���ϰ�, ���� �������ش�.
			Mat resultmat = Mat::zeros(9, resultcount[k], CV_64FC1);
			Mat resultmat1 = Mat::zeros(1, resultcount[k], CV_64FC1);
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < resultcount[k]; j++)
					resultmat.at<double>(i, j) = matrix[k].at<double>(i, j);

			for (int i = 0; i < resultcount[k]; i++)
				resultmat1.at<double>(0, i) = matrixR[k].at<double>(i, 0);

			// �ּ��ڽ¹��� �̿�, ���͸� ���ϰ� �����Ѵ�.
			filterbank[k] = ((resultmat * resultmat.t()).inv()) * (resultmat * resultmat1.t());
		}
	}
}
// �̹����� ����ϴ� ��������, 0�� Y���� �ִ� ����, 1�� RGB�� ��ȯ�� �̹����� �ȴ�.
void printimage(Mat&image, int num)
{
	if (num == 0)
		// �̹��� ����. Y ����
		imwrite("2014722024_gray.png", image);
	else
		// �̹��� ����. RGB ����
		imwrite("2014722024_color.png", image);
}
// psnr�� �˻��Ѵ�. �˻��ϰ� ���� ����Ѵ�.
double checkpsnr(Mat&image, Mat&image1)
{
	double checkdouble = PSNR(image, image1, 255); // psnr�� �����Ѵ�.
	return checkdouble;
}