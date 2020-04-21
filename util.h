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
\param[in] src1 “ü—Í‰æ‘œ1
\param[in] src2 “ü—Í‰æ‘œ2
\param[in] hol true‚Ì‰¡‚É•À‚×Afalse‚Ìc‚É•À‚×‚é
\return ‡¬‰æ‘œ
*/
cv::Mat ConcatinateImage(const cv::Mat& src1, const cv::Mat& src2, bool hol);

cv::Mat Convert8UC3(const cv::Mat& src);

#endif