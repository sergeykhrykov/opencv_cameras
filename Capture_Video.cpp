#include <iostream>
#include <vector>

#include "cv.hpp"

using namespace std;
using namespace cv;

#define WIDTH 800
#define HEIGHT 600

//void stereoGrabberInitFrames();
//void stereoGrabberGrabFrames();
//void stereoGrabberStopCam();


void SCalibration(CvCapture *camera1, CvCapture *camera2)
{
	assert(camera1 && camera2);
	
	int nx = 10, ny = 7, boards_needed = 40, result1 = 0, result2 = 0; //Колличество углов по осям х и у, колличество циклов, результаты поиска углов
	int n = nx*ny; //Колличество углов на шахмтной доске
	int successes1 = 0, successes2 = 0;
	int frame = 0, corners_count1 = 0, corners_count2 = 0, step = 0;
	CvSize board_size = cvSize(nx, ny); //Размер доски
	vector<CvPoint2D32f> found_points_coords1(n);
	vector<CvPoint2D32f> found_points_coords2(n);//Массив координат точек найденных на шахматной доске
	/*stereoGrabberInitFrames();
	stereoGrabberGrabFrames();*/
	IplImage *frame1 = 0, *frame2 = 0;
	IplImage *gr_frame1 = 0, *gr_frame2 = 0;

	//Создание окон для отображения
	cvNamedWindow("capture1", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("capture2", CV_WINDOW_AUTOSIZE);

	//Выделение памяти под хранилища

	CvMat* image_points1 = cvCreateMat(boards_needed*n, 2, CV_32FC1);
	CvMat* image_points2 = cvCreateMat(boards_needed*n, 2, CV_32FC1);
	CvMat* object_points1 = cvCreateMat(boards_needed*n, 3, CV_32FC1);
	CvMat* object_points2 = cvCreateMat(boards_needed*n, 3, CV_32FC1);
	CvMat* point_counts1 = cvCreateMat(boards_needed, 1, CV_32SC1);
	CvMat* point_counts2 = cvCreateMat(boards_needed, 1, CV_32SC1);


	while ((successes1 < boards_needed) || (successes2 < boards_needed))
	{
		frame1 = cvQueryFrame(camera1);
		frame2 = cvQueryFrame(camera2);
		//Ожидание 20 кадров доски с разных ракурсов для захвата изображения
		if (frame++ % 20 == 0)
		{
			//Функция для поиска углов на доске(с точностью пикселя), со встроенной нормализацией и поиском черных квадратов для увеличения точности калибровки
			result1 = cvFindChessboardCorners(frame1,
				board_size,
				&found_points_coords1[0],
				nullptr,
				CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS);

			result2 = cvFindChessboardCorners(frame2,
				board_size,
				&found_points_coords2[0],
				nullptr,
				CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS);

			gr_frame1 = cvCreateImage(cvGetSize(frame1), IPL_DEPTH_8U, 1);
			gr_frame2 = cvCreateImage(cvGetSize(frame2), IPL_DEPTH_8U, 1);

			//Конвертация изображений в градации серого
			cvConvertImage(frame1, gr_frame1, CV_8S);
			cvConvertImage(frame2, gr_frame2, CV_8S);

			//Функция для поиска углов на доске(с точностью субпикселя), со встроенной нормализацией и поиском черных квадратов для увеличения точности калибровки
			cvFindCornerSubPix(gr_frame1,
				&found_points_coords1[0],
				corners_count1,
				cvSize(11, 11),
				cvSize(-1, -1),
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 0.01));

			cvFindCornerSubPix(gr_frame2,
				&found_points_coords2[0],
				corners_count2,
				cvSize(11, 11),
				cvSize(-1, -1),
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 0.01));

			//Отрисовка найденых углов
			cvDrawChessboardCorners(frame1,
				board_size,
				&found_points_coords1[0],
				corners_count1,
				result1);
			cvDrawChessboardCorners(frame2,
				board_size,
				&found_points_coords2[0],
				corners_count2,
				result2);

			////Отображение изображения
			cvShowImage("capture1", frame1);
			cvShowImage("capture2", frame2);


			//Добавление хорошего(у котрого задетектированы все углы) представления доски в коллекцию
			if (corners_count1 == n)
			{
				step = successes1*n;
				for (int i = step, j = 0; j < n; ++i, ++j)
				{
					CV_MAT_ELEM(*image_points1, float, i, 0) = found_points_coords1[j].x;
					CV_MAT_ELEM(*image_points1, float, i, 1) = found_points_coords1[j].y;
					CV_MAT_ELEM(*object_points1, float, i, 0) = j / nx;
					CV_MAT_ELEM(*object_points1, float, i, 1) = j%nx;
					CV_MAT_ELEM(*object_points1, float, i, 1) = 0.0f;
				}
			}
			if (corners_count2 == n)
			{
				step = successes2*n;
				for (int i = step, j = 0; j < n; ++i, ++j)
				{
					CV_MAT_ELEM(*image_points2, float, i, 0) = found_points_coords2[j].x;
					CV_MAT_ELEM(*image_points2, float, i, 1) = found_points_coords2[j].y;
					CV_MAT_ELEM(*object_points2, float, i, 0) = j / nx;
					CV_MAT_ELEM(*object_points2, float, i, 1) = j%nx;
					CV_MAT_ELEM(*object_points2, float, i, 1) = 0.0f;
				}
				CV_MAT_ELEM(*point_counts1, int, successes1, 0) = n;
				CV_MAT_ELEM(*point_counts2, int, successes2, 0) = n;
				successes1++;
				successes2++;
			}
		}

		//Съемка следущей фотографии
		/*stereoGrabberGrabFrames();*/
		//cvShowImage("camera 1", frame1);
		//cvShowImage("camera 1", frame2);

		//Ожидание нажатия клавишы для завершения
		if (cvWaitKey(15) == 27) break;
	}

	//Калибровка камеры
	//cvCalibrateCamera2(
	//	object_points1,
	//	image_points1,
	//	point_counts1,
	//	cvGetSize(frame1),

	//	);

	//Уничтожение окон
	cvDestroyAllWindows();
}



int main(int argc, char **argv)
{
	CvCapture *camera1, *camera2;
	
	// получаем любую подключённую камеру
	camera1 = cvCreateCameraCapture(-1); // или Ваш cvCaptureFromCAM( 0 );
	camera2 = cvCreateCameraCapture(-1); // или Ваш cvCaptureFromCAM( 0 );
	if (! (camera1&&camera2))
	{
		cout << "No camera detected" << endl;
	}
	// !!!
	SCalibration(camera1, camera1);

	
	//IplImage* frame2;

	//cvNamedWindow("capture1", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("capture2", CV_WINDOW_AUTOSIZE);
	while (true)
	{
	/*	 получаем кадр*/
		IplImage* frame1 = cvQueryFrame(camera1);
		IplImage* frame2 = cvQueryFrame(camera2);
		/* показываем*/
		cvShowImage("capture1", frame1);
		cvShowImage("capture2", frame2);
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
	}

	 //освобождаем ресурсы
	cvReleaseCapture(&camera1);
	
	cvReleaseCapture(&camera2);
	//cvDestroyWindow("capture1");

	return 0;
}


//void  stereoGrabberInitFrames()
//{
//	camera1 = cvCreateCameraCapture(3); 
//	cvWaitKey(100);
//	camera2 = cvCreateCameraCapture(4);
// // Inicializaciya kamer
//}

//void stereoGrabberGrabFrames()
//{
//	imageLeft = cvQueryFrame(camera1);
//	imageRight = cvQueryFrame(camera2);//Posledovatelnoe schitovanie kadrov c kameri 1 i 2
//}

//void stereoGrabberStopCam()
//{
//	cvReleaseCapture(&camera1);
//	cvReleaseCapture(&camera2);//Osvobowdenie pamyati
//}
