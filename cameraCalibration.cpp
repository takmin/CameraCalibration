#include "cameraCalibration.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;



cameraCalibration::cameraCalibration(void)
{
}

cameraCalibration::~cameraCalibration(void)
{
}


void cameraCalibration::ChessBoard3DCoord(const cv::Size& checker_size, double chess_size, std::vector<cv::Point3f>& coord3d)
{
	cv::Point3f obj;

	// 3次元空間座標の設定
	for (int j = 0; j < checker_size.height; j++) {
		obj.y = chess_size * j;
		for (int k = 0; k < checker_size.width; k++) {
			obj.x = chess_size * k;
			obj.z = 0.0;
			coord3d.push_back(obj);
		}
	}
}


bool cameraCalibration::findChessboardCornersSubpix(const cv::Mat& img, const cv::Size& pattern_size, std::vector<cv::Point2f>& corners)
{
	cv::Mat src_gray;
	cv::cvtColor(img, src_gray, cv::COLOR_BGR2GRAY);
	bool found = cv::findChessboardCorners(src_gray, pattern_size, corners);
	if (!found)
		return found;

	// コーナー位置をサブピクセル精度に修正
	cv::cornerSubPix(src_gray, corners, cv::Size(3, 3), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.03));

	return found;
}


void cameraCalibration::showResizedImage(const cv::Mat& img, int max_width, const std::string& win_name)
{
	cv::Size new_size = img.size();
	if (new_size.width > max_width) {
		new_size.width = max_width;
		new_size.height = img.rows * max_width / img.cols;
	}
	cv::Mat resize_img;
	cv::resize(img, resize_img, new_size);
	cv::imshow(win_name, resize_img);
	cv::waitKey(0);
}



bool cameraCalibration::doCalibration(const std::vector<std::string>& image_list, const cv::Size& pattern_size, double chess_size, bool fisheye)
{
	int image_num = image_list.size();
	if (image_num < 3) {
		cout << "The number of images are few!" << endl;
		return false;
	}

	// チェッカーボードの3次元空間座標の設定
	vector<cv::Point3f> objects;
	ChessBoard3DCoord(pattern_size, chess_size, objects);

	vector<vector<cv::Point2f>> image_points;
	vector<vector<cv::Point3f>> object_points;

	std::string winname = "Calibration";
	int max_window_width = 1024;

	cv::namedWindow(winname);
	cv::Size img_size;
	for (int i = 0; i < image_num; i++) {
		cout << i << ": " << image_list[i] << "...";

		// Read Image
		cv::Mat img = cv::imread(image_list[i]);
		if (img.empty()) {
			cout << endl << "Fail to read image file." << endl;
			return false;
		}

		if (i == 0) {
			img_size = img.size();
		}
		else {
			if (img_size.width != img.cols || img_size.height != img.rows) {
				cout << endl << "Size of image is not same as image 0" << endl;
				return false;
			}
		}

		// チェスボード（キャリブレーションパターン）のコーナー検出(サブピクセル精度)
		vector<cv::Point2f>	corners;
		bool found = findChessboardCornersSubpix(img, pattern_size, corners);
		cout << (found ? "ok" : "fail") << endl;
		if (found) {
			image_points.push_back(corners);
			object_points.push_back(objects);
		}

		// コーナー位置を描画
		cv::drawChessboardCorners(img, pattern_size, corners, found);
		showResizedImage(img, 1024, winname);
	}
	cv::destroyWindow(winname);

	if (image_points.size() < 3) {
		return false;
	}

	// 内部パラメータ，歪み係数, 外部パラメータの推定
	vector<cv::Mat> rotation;
	vector<cv::Mat> translation;

	this->fisheye = fisheye;
	this->image_size = img_size;
	double rms_err;
	if (fisheye) {
		rms_err = cv::fisheye::calibrate(object_points, image_points, img_size, camera_matrix, distortion,
			cv::noArray(), cv::noArray(), cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC | cv::fisheye::CALIB_CHECK_COND | cv::fisheye::CALIB_FIX_SKEW);
	}
	else {
		rms_err = calibrateCamera(object_points, image_points, img_size, camera_matrix, distortion, rotation, translation);
	}

	std::cout << "reprojection error is " << rms_err << " px" << std::endl;

	return true;
}


bool cameraCalibration::undistortImage(const std::string& in_file, const std::string& out_file) const
{
	cv::Mat in_img = cv::imread(in_file);
	if (in_img.empty()) {
		std::cout << "Fail to read image " << in_file << std::endl;
		return false;
	}
	cv::Mat out_img;
	bool ret = undistortImage(in_img, out_img);
	if (!ret)
		return false;

	if (!cv::imwrite(out_file, out_img)) {
		std::cout << "Fail to save image " << out_file << std::endl;
		return false;
	}
	return true;
}


bool cameraCalibration::undistortImage(const cv::Mat& in_img, cv::Mat& out_img) const
{
	if (camera_matrix.empty())
		return false;
	try {
		if (fisheye) {
			cv::fisheye::undistortImage(in_img, out_img, this->camera_matrix, this->distortion, this->camera_matrix, in_img.size());
		}
		else
			cv::undistort(in_img, out_img, this->camera_matrix, this->distortion);
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}


bool cameraCalibration::undistortPoints(const std::vector<cv::Point2f>& src_points, std::vector<cv::Point2f>& dst_points) const
{
	if (camera_matrix.empty())
		return false;
	try {
		if (fisheye) {
			cv::fisheye::undistortPoints(src_points, dst_points, camera_matrix, distortion, cv::noArray(), camera_matrix);
		}
		else {
			cv::undistortPoints(src_points, dst_points, camera_matrix, distortion, cv::noArray(), camera_matrix);
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}


bool cameraCalibration::saveIntCameraParameters(const string& filename) const
{
	FileStorage fs(filename, FileStorage::WRITE);
	if (!fs.isOpened()) {
		std::cout << "Fail to open " << filename << std::endl;
		return false;
	}
	return writeIntCameraParameters(fs, "IntParam");
}


bool cameraCalibration::writeIntCameraParameters(FileStorage& cvfs, const string& name) const
{
	try {
		if (!name.empty())
			cvfs << name;
		cvfs << "{";
		cvfs << "camera_matrix" << camera_matrix;
		cvfs << "distortion" << distortion;
		cvfs << "image_size" << image_size;
		cvfs << "fisheye" << (int)(fisheye ? 1 : 0);
		cvfs << "}";
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}


bool cameraCalibration::loadIntCameraParameters(const std::string& filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::cout << "Fail to open " << filename << std::endl;
		return false;
	}

	return readIntCameraParameters(fs["IntParam"]);
}


bool cameraCalibration::readIntCameraParameters(const cv::FileNode& cvfn)
{
	try {
		cvfn["camera_matrix"] >> camera_matrix;
		cvfn["distortion"] >> distortion;
		cvfn["image_size"] >> image_size;
		int f;
		cvfn["fisheye"] >> f;
		fisheye = f > 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}



void cameraCalibration::ChessBoard2DCoord(const cv::Size& checker_size, double chess_size, const cv::Point2d& marker_topleft, std::vector<cv::Point2f>& coord2d)
{
	cv::Point2f obj;

	// 3次元空間座標の設定
	for (int j = 0; j < checker_size.height; j++) {
		obj.y = chess_size * j + marker_topleft.y;
		for (int k = 0; k < checker_size.width; k++) {
			obj.x = chess_size * k + marker_topleft.x;
			coord2d.push_back(obj);
		}
	}
}


void cameraCalibration::Point3Dto2D(const std::vector<cv::Point3f>& points3d, std::vector<cv::Point2f>& points2d)
{
	for (int i = 0; i < points3d.size(); i++) {
		cv::Point2d pt(points3d[i].x, points3d[i].y);
		points2d.push_back(pt);
	}
}

/*
	void cameraCalibration::Point3DtoMat(const std::vector<cv::Point3f>& points3d, cv::Mat& dst_mat)
	{
		dst_mat.create(3, points3d.size(), CV_32FC1);
		for (int i = 0; i < points3d.size(); i++){
			dst_mat.at<float>(0, i) = points3d[i].x;
			dst_mat.at<float>(1, i) = points3d[i].y;
			dst_mat.at<float>(2, i) = points3d[i].z;
		}
	}
*/

void cameraCalibration::transformHomography(const cv::Point2f& src, cv::Point2f& dst, const cv::Mat& H)
{
	assert(H.type() == CV_64FC1);
	float div = H.at<double>(2, 0) * src.x + H.at<double>(2, 1) * src.y + H.at<double>(2, 2);
	dst.x = (H.at<double>(0, 0) * src.x + H.at<double>(0, 1) * src.y + H.at<double>(0, 2)) / div;
	dst.y = (H.at<double>(1, 0) * src.x + H.at<double>(1, 1) * src.y + H.at<double>(1, 2)) / div;
}


void cameraCalibration::transformHomography(const std::vector<cv::Point2f>& src, std::vector<cv::Point2f>& dst, const cv::Mat& H)
{
	for (int i = 0; i < src.size(); i++) {
		cv::Point2f dst_pt;
		transformHomography(src[i], dst_pt, H);
		dst.push_back(dst_pt);
	}
}


void cameraCalibration::projectPoints(const std::vector<cv::Point3f>& point3ds, std::vector<cv::Point2f>& prjectPoints) const {
	if (fisheye) {
		cv::fisheye::projectPoints(point3ds, prjectPoints, this->R, this->T, this->camera_matrix, this->distortion);
	}
	else {
		cv::projectPoints(point3ds, this->R, this->T, this->camera_matrix, this->distortion, prjectPoints);
	}
}

