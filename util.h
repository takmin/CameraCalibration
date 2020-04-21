#ifndef __UTIL__
#define __UTIL__

#include <string>
#include <vector>
#include <opencv2/core.hpp>

std::string AskQuestionGetString(const std::string& question);
int AskQuestionGetInt(const std::string& question);
double AskQuestionGetDouble(const std::string& question);

bool ReadImageFilesInDirectory(const std::string& img_dir, std::vector<std::string>& image_lists,
	bool recursive = false, bool relative_path = false);

bool captureImages(int img_num, int interval, const std::string& dirctory_path);

/*!
\param[in] src1 ���͉摜1
\param[in] src2 ���͉摜2
\param[in] hol true�̎����ɕ��ׁAfalse�̎��c�ɕ��ׂ�
\return �����摜
*/
cv::Mat ConcatinateImage(const cv::Mat& src1, const cv::Mat& src2, bool hol);

cv::Mat Convert8UC3(const cv::Mat& src);

#endif