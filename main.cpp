#if _TEST_
#pragma comment(lib, "C:\\ProgramFolders\\gtest\\lib\\gtest.lib")
#include <gtest/gtest.h>
#endif

#include <iostream>
#include "util.h"
#include "cameraCalibration.h"


void printMenu()
{
	std::cout << "help" << std::endl;
	std::cout << "get_camera_images" << std::endl;
	std::cout << "calib_int" << std::endl;
	std::cout << "save_param" << std::endl;
	std::cout << "load_param" << std::endl;
	std::cout << "undistort" << std::endl;
	std::cout << "exit" << std::endl;
}


bool StringYes(const std::string& ans)
{
	return (ans == "y" || ans == "yes" || ans == "Y" || ans == "YES");
}


int main(int argc, char * argv[])
{
#if _TEST_
	::testing::InitGoogleTest(&argc, argv);

	RUN_ALL_TESTS();
	return 0;
#endif

	cameraCalibration camera_calibration;
	bool exitflag1 = false;

	while (!exitflag1){
		std::string opt = AskQuestionGetString("command: ");
		//std::string img_file = "../Data/CardImg/006.jpg";

		if (opt == "exit" || opt == "quit" || opt == "q" || 
			opt == "EXIT" || opt == "QUIT" || opt == "Q"){
			exitflag1 = true;
		}
		else if (opt == "help" || opt == "h"){
			printMenu();
		}
		else if (opt == "get_camera_images"){
			int img_num = AskQuestionGetInt("number of images: ");
			int interv = AskQuestionGetInt("frame interval: ");
			std::string dir_name = AskQuestionGetString("save directory: ");
			captureImages(img_num, interv, dir_name);
		}
		else if (opt == "calib_int"){
			int row = AskQuestionGetInt("Rows of checker corners: ");
			int col = AskQuestionGetInt("Cols of checker corners: ");
			double chess_size = AskQuestionGetDouble("Distance between corners (m): ");
			std::string fisheye_mode = AskQuestionGetString("Fisheye? (y/n): ");
			std::string folder_name = AskQuestionGetString("Path of image folder: ");
			std::string s = AskQuestionGetString("Start calibration? (y/n): ");
			if (StringYes(s)){
				std::vector<std::string> image_files;
				if (!ReadImageFilesInDirectory(folder_name, image_files))
					std::cout << "Fail to files in directory " << folder_name;
				else
					camera_calibration.doCalibration(image_files, cv::Size(col, row), chess_size, StringYes(fisheye_mode));
			}
		}
		else if (opt == "undistort"){
			std::string filename = AskQuestionGetString("Distort Image: ");
			std::string savefile = AskQuestionGetString("Undistort Image to save: ");
			camera_calibration.undistortImage(filename, savefile);
		}
		else if (opt == "save_param"){
			std::string filename = AskQuestionGetString("Filename to save: ");
			camera_calibration.saveIntCameraParameters(filename);
		}
		else if (opt == "load_param"){
			std::string filename = AskQuestionGetString("Filename to load: ");
			camera_calibration.loadIntCameraParameters(filename);
		}
		else{
			std::cout << "Error: Wrong Command\n" << std::endl;
		}
	}
	return 0;
}