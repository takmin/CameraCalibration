#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>
#include "util.h"


std::string AskQuestionGetString(const std::string& question) {
	std::cout << question;
	std::string ans;
	std::cin >> ans;
	return ans;
}


int AskQuestionGetInt(const std::string& question) {
	std::string ans = AskQuestionGetString(question);
	return atoi(ans.c_str());
}


double AskQuestionGetDouble(const std::string& question) {
	std::string ans = AskQuestionGetString(question);
	return atof(ans.c_str());
}

// intをStringへ変換
std::string Int2String(int x)
{
	std::stringstream strstr;
	strstr << x;
	return strstr.str();
}

bool hasImageExtention(const std::string& filename)
{
	std::string ext = boost::filesystem::path(filename).extension().string();

	return (ext == ".jpg" || ext == ".JPG" || ext == ".jpeg" || ext == ".JPEG" ||
		ext == ".bmp" || ext == ".BMP" || ext == ".png" || ext == ".PNG" ||
		ext == ".dib" || ext == ".DIB" || ext == ".pbm" || ext == ".PBM" ||
		ext == ".pgm" || ext == ".PGM" || ext == ".ppm" || ext == ".PPM" ||
		ext == ".sr" || ext == ".SR" || ext == ".ras" || ext == ".RAS");
}


// ディレクトリから画像ファイル名一覧を取得
bool ReadImageFilesInDirectory(const std::string& img_dir, std::vector<std::string>& image_lists,
	bool recursive, bool relative_path)
{
	using namespace boost::filesystem;

	path img_dir_path(img_dir);
	if (!is_directory(img_dir_path)) {
		return false;
	}

	directory_iterator end;
	for (directory_iterator p(img_dir_path); p != end; ++p) {
		std::string file_name = p->path().generic_string();
		if (hasImageExtention(file_name)) {
			if (relative_path && path(file_name).has_relative_path()) {
				file_name = path(file_name).relative_path().generic_string();
			}
			image_lists.push_back(file_name);
		}
		else if (recursive) {
			ReadImageFilesInDirectory(file_name, image_lists, recursive, relative_path);
		}
	}
	return true;
}


bool captureImages(int img_num, int interval, const std::string& directory_path)
{
	using namespace boost::filesystem;

	cv::VideoCapture	capture(0);

	if (!capture.isOpened()) {
		std::cout << "Failed to Open Camera" << std::endl;
		return false;
	}

	// create window
	const std::string  windowNameCam = "Camera Image";
	cv::namedWindow(windowNameCam, cv::WINDOW_AUTOSIZE);

	// for capture image
	cv::Mat	frame;

	// main loop
	std::cout << "Capture start with \"s\" button." << std::endl;
	do {
		// image capture from camera
		capture >> frame;
		// show caputred image
		cv::imshow(windowNameCam, frame);
	} while (cv::waitKey(1) != 's');

	std::cout << "Capture started!" << std::endl;
	std::cout << "Capture stop automatically or with \"q\" button." << std::endl;
	int counter = 0;
	int num_frames = 0;
	do {
		// カメラから画像をキャプチャ
		capture >> frame;
		// キャプチャした画像を表示
		cv::imshow(windowNameCam, frame);
		if (counter >= interval) {
			counter = 0;
			// 保存ファイル名
			path save_path = path(directory_path) / path(Int2String(num_frames) + ".png");
			if (!cv::imwrite(save_path.string(), frame)) {
				std::cout << "fail to save " << save_path.string() << std::endl;
				return false;
			}
			std::cout << "save " << save_path.string() << std::endl;
			num_frames++;
		}
		else {
			counter++;
		}
	} while (cv::waitKey(1) != 'q' && num_frames <= img_num);

	std::cout << " --- Finished." << std::endl;
	cv::destroyWindow(windowNameCam.c_str());
}


cv::Mat Convert8UC3(const cv::Mat& src)
{
	cv::Mat tmp, dst;
	if (src.depth() != CV_8U) {
		cv::normalize(src, tmp, 255, 0, cv::NORM_MINMAX, CV_8U);
	}
	else {
		tmp = src;
	}
	if (tmp.channels() == 1) {
		cv::cvtColor(tmp, dst, cv::COLOR_GRAY2RGB);
	}
	else {
		dst = tmp;
	}
	return dst;
}


/*!
\param[in] src1 入力画像1
\param[in] src2 入力画像2
\param[in] hol trueの時横に並べ、falseの時縦に並べる
\return 合成画像
*/
cv::Mat ConcatinateImage(const cv::Mat& src1, const cv::Mat& src2, bool hol)
{
	cv::Mat img1 = Convert8UC3(src1);
	cv::Mat img2 = Convert8UC3(src2);

	cv::Mat concatImg;
	cv::Size img_size;
	cv::Point pt;
	if (hol) {
		img_size.width = img1.cols + img2.cols;
		img_size.height = std::max(img1.rows, img2.rows);
		pt.x = img1.cols;
		pt.y = 0;
	}
	else {
		img_size.width = std::max(img1.cols, img2.cols);
		img_size.height = img1.rows + img2.rows;
		pt.x = 0;
		pt.y = img1.rows;
	}

	concatImg = cv::Mat::zeros(img_size, img1.type());
	img1.copyTo(concatImg(cv::Rect(0, 0, img1.cols, img1.rows)));
	img2.copyTo(concatImg(cv::Rect(pt.x, pt.y, img2.cols, img2.rows)));
	return concatImg;
}

