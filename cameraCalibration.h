#ifndef __CAMERA_CALIBRATION__
#define __CAMERA_CALIBRATION__

#include <vector>
#include <opencv2/core.hpp>


class cameraCalibration
{
public:
	cameraCalibration();
	~cameraCalibration(void);

public:
	// Intrinsic Parameters
	cv::Mat camera_matrix;
	cv::Mat distortion;
	bool fisheye;
	cv::Size image_size;

	// Extrinsic Parameters
	cv::Mat R;
	cv::Mat T;

	// Mapping Homography from Undistort Image to Floor
	cv::Mat H;

private:
	// marker info
	std::vector<cv::Point2f> _marker_undistort_corners;
	std::vector<cv::Point3f> _marker_corners_3D;
	cv::Size _marker_pattern;

public:
	static bool findChessboardCornersSubpix(const cv::Mat& img, const cv::Size& checker_size, std::vector<cv::Point2f>& corners);

	static void showResizedImage(const cv::Mat& img, int max_width, const std::string& win_name);

	static void ChessBoard3DCoord(const cv::Size& checker_size, double chess_size, std::vector<cv::Point3f>& coord3d);

	static void ChessBoard2DCoord(const cv::Size& checker_size, double chess_size, const cv::Point2d& marker_topleft, std::vector<cv::Point2f>& coord2d);

	static void Point3Dto2D(const std::vector<cv::Point3f>& points3d, std::vector<cv::Point2f>& points2d);

	//static void Point3DtoMat(const std::vector<cv::Point3f>& points3d, cv::Mat& dst_mat);

	static void transformHomography(const cv::Point2f& src, cv::Point2f& dst, const cv::Mat& H);

	static void transformHomography(const std::vector<cv::Point2f>& src, std::vector<cv::Point2f>& dst, const cv::Mat& H);

	////// Intrinsic Parameters ///////
	bool doCalibration(const std::vector<std::string>& image_list, const cv::Size& pattern, double chess_size, bool fisheye = false);

	bool saveIntCameraParameters(const std::string& filename) const;

	bool loadIntCameraParameters(const std::string& filename);

	bool writeIntCameraParameters(cv::FileStorage& cvfs, const std::string& name) const;

	bool readIntCameraParameters(const cv::FileNode& cvfn);

	bool undistortImage(const std::string& in_img, const std::string& out_img) const;

	bool undistortImage(const cv::Mat& in_img, cv::Mat& out_img) const;

	bool undistortPoints(const std::vector<cv::Point2f>& src_points, std::vector<cv::Point2f>& dst_points) const;

	cv::Size getImageSize() const { return image_size; }

	cv::Point2f getOpticCenter() const {
		return (camera_matrix.type() == CV_32FC1) ?
			cv::Point2f(camera_matrix.at<float>(0, 2), camera_matrix.at<float>(1, 2)) :
			cv::Point2f(camera_matrix.at<double>(0, 2), camera_matrix.at<double>(1, 2));
	}

	////// Others ///////
	void projectPoints(const std::vector<cv::Point3f>& point3ds, std::vector<cv::Point2f>& prjectPoints) const;
};

#endif