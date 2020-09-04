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


int resultcount[25] = { 0 }; // 각각의 원소 갯수가 몇 개인지를 측정하기 위함

void DynamicAllocation(int A, int B); // 동적할당
void DeleteArray(int A, int B); // 동적할당 해제로 메모리 누수 방지
void changeRGB(Mat &mat, int cols, int rows, int num); // RGB를 YCbCr로 변환
void changeYCbCr(Mat &mat, int cols, int rows); // YCbCr을 RGB로 변환
void firstloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR); // 첫 번째 loop로써, 최소자승법을 사용해서 filter를 만듦
void secondloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR, Mat*filterbank); // 두번째 루프로써, 만들어진 filter를 사용해서 이미지를 만듦.
void makefilter(Mat*matrix, Mat*matrixR, Mat*filterbank); // 필터를 만듦.
void printimage(Mat&image, int num); // 결과 이미지를 출력함
double checkpsnr(Mat&image, Mat&image1); // psnr을 체크하기 위한 함수

int main()
{
	Mat image;
	Mat image1;
	image = imread("0063_40.png", IMREAD_COLOR); // Read the file, 에러가 있는 이미지
	image1 = imread("0063.png", IMREAD_COLOR); // Read the file 에러가 없는 이미지

	//존재하지 않는 이미지라면 종료
	if (image.empty() || image1.empty())
		return 0;

	// 각각 이미지의 행과 열을 추출해낸다
	int A = image.rows;
	int B = image.cols;

	// 총 25개의 class가 있으며, 각각 9개의 원소가 존재하기 때문에 9개의 클래스를 담을 matrix를 선언한다.
	// 여기서 matrix는 9개의 원소를 직선형태로 저장하는 행렬이고, matrixR은 각각의 원소의 합의 목표치를 나타낸다. 즉 원본 이미지의 픽셀이 저장된다.
	Mat matrix[25];
	for (int i = 0; i < 25; i++)
		matrix[i] = Mat::zeros(9, 5000000, CV_64FC1);

	Mat matrixR[25];
	for (int i = 0; i < 25; i++)
		matrixR[i] = Mat::zeros(5000000, 1, CV_64FC1);

	// filterbank는 3*3*25의 크기를 가진다. 이를 0으로 초기화하고 선언한다.
	Mat *filterbank = new Mat[25];
	for (int i = 0; i < 25; i++)
		filterbank[i] = Mat::zeros(3, 3, CV_64FC1);

	// 필요한 배열을 동적할당
	DynamicAllocation(A, B);

	changeRGB(image, A, B, 0); // change rgb to ycbcr, Y는 수정전 이미지, Y1은 목표 이미지, 각각의 선언한 배열과 주어진 식을 사용해 rgb를 YCbCr로 변환
	changeRGB(image1, A, B, 1); // 0은 수정전 이미지, 1은 목표 이미지의 numbering.

	// 첫 번째 loop로써, filter를 만들기 위해서 각각의 결괏값을 저장하는 과정, 이 과정에서 이미지는 Y만 추출
	firstloop(image, image1, A, B, matrix, matrixR);

	// 각각 추출된 이미지의 psnr을 비교
	cout << "수정 전 이미지의 psnr은 : " << checkpsnr(image, image1) << endl;

	// 최소자승법을 이용, filter를 구함
	makefilter(matrix, matrixR, filterbank);

	// 두 번째 loop로써, 만들어진 filter를 사용해서 행렬을 scan하고, 그 값으로 이미지 pixel값을 결정
	secondloop(image, image1, A, B, matrix, matrixR, filterbank);

	// gray형태 이미지 추출
	printimage(image, 0);

	// filtering한 결괏값을 비교. Y만 변경되었기 때문에, Y의 값만 비교해서 psnr을 측정하고, 출력한다.
	cout << "수정 후 이미지의 psnr은 : " << checkpsnr(image, image1) << endl;

	// YCbCr의 값을  RGB로 변경
	changeYCbCr(image, A, B);

	//RGB로 변경된 이미지를 출력
	printimage(image, 1);

	//동적할당 해제
	DeleteArray(A, B);

	system("pause");

	return 0;
}


// rgb를 ycbcr로 변환
void changeYCbCr(Mat &image, int A, int B)
{
	//loop는 A*B만큼 회전, 주어진 공식을 사용해서 YCbCr에서 RGB로 변경한다.
	for (int i = 0; i < A; i++)
		for (int j = 0; j < B; j++)
		{
			int C = Y[i][j] - 16, D = Cb[i][j] - 128, E = Cr[i][j] - 128;
			Red[i][j] = ((298 * C + 409 * E + 128) >> 8);
			Green[i][j] = ((298 * C - 100 * D - 208 * E + 128) >> 8);
			Blue[i][j] = ((298 * C + 516 * D + 128) >> 8);

			//각각의 이미지 픽셀은 최대 255 최소 0에 맞춘다.
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

			// 구해진 값을 알맞은 영역에 매핑한다.
			image.at<Vec3b>(i, j)[0] = Blue[i][j];
			image.at<Vec3b>(i, j)[1] = Green[i][j];
			image.at<Vec3b>(i, j)[2] = Red[i][j];
		}
}
// rgb를 ycbcr로 변환
void changeRGB(Mat &mat, int cols, int rows, int num)
{
	//loop는 A*B만큼 회전, 주어진 공식을 사용해서 YCbCr에서 RGB로 변경한다.

	// 첫 번째로, 먼저 RGB값을 뽑아내야하기 때문에, 이미지를 먼저 scan해서 각각의 rgb값을 저장한다.
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			Blue[i][j] = (__int64)mat.at<Vec3b>(i, j)[0];
			Green[i][j] = (__int64)mat.at<Vec3b>(i, j)[1];
			Red[i][j] = (__int64)mat.at<Vec3b>(i, j)[2];
		}
	}
	// num = 0이면 노이즈가 있는 이미지, 1이면 노이즈가 없는 이미지가 된다. 각각의 상황에 맞게 YCbCr을 공식에 따라 뽑아낸다.
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
// 동적할당
void DynamicAllocation(int A, int B)
{

	//변수를 선언. Y,Cb,Cr은 에러가 있는 이미지, Y1,Cb1,Cr1은 에러가 없는 이미지, 2차원 동적할당을 한다.
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
// 동적할당 해제
void DeleteArray(int A, int B)
{
	// 모든 계산이 완료되었으니 동적할당을 해제한다.
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
// 첫 번째 loop. filter를 구하기 위해 각각의 class를 classify한다.
void firstloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR)
{
	// 수직을 감지하는 filter.  즉 90도
	int vertical[3][3] = { -1,0,1,
						   -2,0,2,
							-1,0,1 };


	//수평을 감지하는 filter. 즉 0도
	int horizantal[3][3] = { 1,2,1,
							0,0,0,
							-1,-2,-1 };

	// 좌측 대각선을 감지하는 filter. 즉 135도 
	int left[3][3] = { 0,1,2,
					-1,0,1,
					-2,-1,0 };


	// 우측 대각선을 감지하는 filter. 즉 45도
	int right[3][3] = { 2,1,0,
						1,0,-1,
						0,-1,-2 };



	// loop를 모든 pixel에 대해서 돈다.
	for (int i = 0; i < A; i++)
	{
		for (int j = 0; j < B; j++)
		{
			int result = 0; // 최종 class를 결정함
			int compvertical = 0, comphorizantal = 0, compleft = 0, compright = 0; // 방향성
			int activity = 0, direct = 0;
			int arc = 0; // 배열의 갯수
			int temparc[9];
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++) // 3*3 mask가 있으므로, mask를 이용해 convolution을 진행한다.
				{
					if (!(i == 0 || i == A - 1 || j == 0 || j == B - 1)) // mask가 이미지의 영역을 벗어나지 않는다면 일반적인 방법을 사용한다.
					{
						result += Y[i - 1 + l][j - 1 + k] * vertical[l][k]; // 활동성을 구하기 위한 값 result. vertical과 horizantal의 합에 따라 값을 mapping한다.
						result += Y[i - 1 + l][j - 1 + k] * horizantal[l][k];
						compvertical += Y[i - 1 + l][j - 1 + k] * vertical[l][k];
						comphorizantal += Y[i - 1 + l][j - 1 + k] * horizantal[l][k];
						compleft += Y[i - 1 + l][j - 1 + k] * left[l][k];
						compright += Y[i - 1 + l][j - 1 + k] * right[l][k];
						// 필터를 4번 스캔해서 방향성을 비교한다.
						temparc[arc++] = Y[i - 1 + l][j - 1 + k];
						// filter가 어떤 필터인지 저장한다.
					}
					else // mask가 이미지의 영역을 벗어난다면 벗어난 이미지의 영역은 현재 픽셀의 값으로 통일시킨다.
					{
						result += Y[i][j] * vertical[l][k]; // 활동성을 구하기 위한 값 result. vertical과 horizantal의 합에 따라 값을 mapping한다.
						result += Y[i][j] * horizantal[l][k];
						compvertical += Y[i][j] * vertical[l][k];
						comphorizantal += Y[i][j] * horizantal[l][k];
						compleft += Y[i][j] * left[l][k];
						compright += Y[i][j] * right[l][k];
						// 필터를 4번 스캔해서 방향성을 비교한다.
						temparc[arc++] = Y[i][j];
						// filter가 어떤 필터인지 저장한다.
					}
				}
			// max is 1020. 0,1,2에 가중치를 두어 세밀하게 분석시킬 수 있도록 했따.
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

			//모든 결과값은 음수일 수도 있기 때문에 절댓값 선언을 한다.
			compvertical = abs(compvertical);
			comphorizantal = abs(comphorizantal);
			compleft = abs(compleft);
			compright = abs(compright);



			// 각각의 방향성을 비교한다. 가장 큰 숫자가 해당하는 방향성이 된다. 큰 수가 복수개일 경우에는, 특정 기준을 사용해서 방향성을 정한다.
			if ((compvertical > comphorizantal && compvertical > compleft && compvertical > compright) || (compvertical == compright && compright == compleft && compleft > comphorizantal) || (((compright == compvertical) || ((compright == comphorizantal)) && compvertical > comphorizantal && compvertical > compleft))) // 90도일 때
				direct = 3;
			else if ((comphorizantal > compvertical && comphorizantal > compleft && comphorizantal > compright) || (comphorizantal == compleft && compleft == compright && compright > compvertical) || (((compleft == compvertical) || (compleft == comphorizantal)) && compvertical > comphorizantal && compvertical > compright)) // 0도일 때
				direct = 1;
			else if ((compright > comphorizantal && compright > compvertical && compright > compleft) || (compright == compvertical && compvertical == comphorizantal && compvertical > compleft)) // 45도일 때
				direct = 2;
			else if ((compleft > comphorizantal && compleft > compvertical && compleft > compright) || (compleft == compvertical && compleft == comphorizantal && compleft > compright)) // 135도일 때
				direct = 4;
			else // 무방향일때
				direct = 0;

			// classify된 class를 찾는다.
			result = activity * 5 + direct;


			// 현재 pixel의 classify number를 저장한다.
			pixelvalue[i][j] = result;

			for (int k = 0; k < 9; k++)
			{
				// mask가 scan한 영역의 원소를 전부 matrix에 저장한다.
				matrix[result].at<double>(k, resultcount[result]) = temparc[k];
			}
			// mask가 scan해서 나와야 할 목표값을 matrixR에 저장한다
			matrixR[result].at<double>(resultcount[result], 0) = Y1[i][j];


			// 결과가 저장되었다면, 저장된 배열의 값을 하나 올린다.
			resultcount[result] += 1;

			// psnr을 비교하기 위해 이미지를 Y로 변환시킨다.
			image1.at<Vec3b>(i, j)[0] = Y1[i][j];
			image1.at<Vec3b>(i, j)[1] = Y1[i][j];
			image1.at<Vec3b>(i, j)[2] = Y1[i][j];
			image.at<Vec3b>(i, j)[0] = Y[i][j];
			image.at<Vec3b>(i, j)[1] = Y[i][j];
			image.at<Vec3b>(i, j)[2] = Y[i][j];

		}

	}
}

// 두 번째 loop로써, 구한 필터를 사용해 이미지를 필터링해서 값을 구한다.
void secondloop(Mat &image, Mat&image1, int A, int B, Mat*matrix, Mat*matrixR, Mat*filterbank)
{
	// 2번째 loop로, 추출한필터를 사용해 이미지를 필터링하기위한 loop가 된다.
	for (int i = 0; i < A; i++)
	{
		for (int j = 0; j < B; j++)
		{
			double imagesum = 0; // 최종적으로 계산할계산값
			{
				int result = 0;

				// 각각의 pixel값을 불러온다.
				result = pixelvalue[i][j];

				Mat comp = Mat::zeros(3, 3, CV_64FC1); // mask가 scan할 영역을 임시로 복사할 공간


				// mask가 scan할 3*3영역을 복사한다.
				for (int k = 0; k < 3; k++)
					for (int l = 0; l < 3; l++)
						if (!(i == 0 || i == A - 1 || j == 0 || j == B - 1))
							comp.at<double>(k, l) = Y[i - 1 + l][j - 1 + k];
						else
							comp.at<double>(k, l) = Y[i][j];

				// 복사한 값을 filter를 이용해서 convolution을 실행하고, 모든 원소값을 합한다.
				for (int k = 0; k < 3; k++)
					for (int l = 0; l < 3; l++)
						imagesum += (comp.at<double>(k, l)) * (filterbank[result].at<double>(k * 3 + l, 0));

				// 원소값을 합한 값은 각 pixel의 값을 의미한다. pixel은 0과 255사이로 조정한다.
				if (imagesum > 255)
					imagesum = 255;
				else if (imagesum < 0)
					imagesum = 0;

				// 구한 값을 이미지에 저장한다. 
				image.at<Vec3b>(i, j)[0] = floor(imagesum);
				image.at<Vec3b>(i, j)[1] = floor(imagesum);
				image.at<Vec3b>(i, j)[2] = floor(imagesum);
			}


		}
	}
}
//filter를 만드는 과정으로, 최소자승법을 사용한다.
void makefilter(Mat*matrix, Mat*matrixR, Mat*filterbank)
{

	for (int k = 0; k < 25; k++) // 각각의 classify된 level에 따라서 최종적인 필터를 결정한다.
	{
		if (resultcount[k] == 0) // 내부에 원소가 하나도 없다면
		{
			filterbank[k].at<double>(1, 1) = 1; // 0,0,0 0,1,0 0,0,0으로 유지시켜서 필터를 통과하도록 한다.
		}
		else
		{
			//초기 선언은 500만개의 배열을 했지만, 실제로 배열을 사용한 갯수는 이보다 훨씬 적을 수 있다.
			// 따라서, 불필요한 연산과 불필요한 값의 계산을 방지하기 위해 사용한만큼의 배열을 재선언하고, 값을 복사해준다.
			Mat resultmat = Mat::zeros(9, resultcount[k], CV_64FC1);
			Mat resultmat1 = Mat::zeros(1, resultcount[k], CV_64FC1);
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < resultcount[k]; j++)
					resultmat.at<double>(i, j) = matrix[k].at<double>(i, j);

			for (int i = 0; i < resultcount[k]; i++)
				resultmat1.at<double>(0, i) = matrixR[k].at<double>(i, 0);

			// 최소자승법을 이용, 필터를 구하고 저장한다.
			filterbank[k] = ((resultmat * resultmat.t()).inv()) * (resultmat * resultmat1.t());
		}
	}
}
// 이미지를 출력하는 과정으로, 0은 Y만이 있는 버전, 1은 RGB로 변환한 이미지가 된다.
void printimage(Mat&image, int num)
{
	if (num == 0)
		// 이미지 저장. Y 버전
		imwrite("2014722024_gray.png", image);
	else
		// 이미지 저장. RGB 버전
		imwrite("2014722024_color.png", image);
}
// psnr을 검사한다. 검사하고 값을 출력한다.
double checkpsnr(Mat&image, Mat&image1)
{
	double checkdouble = PSNR(image, image1, 255); // psnr을 측정한다.
	return checkdouble;
}