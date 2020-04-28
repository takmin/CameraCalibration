# CameraCalibration

Internal camera parameter estimation with openCV.

1. Print pattern.png and paste it on a flat board. Measure length of each block in checker pattern with a ruler.
2. Start program.
3. At "command:" prompt, input "get_camera_images" and return.  If you alreay have enough images of checker patterns captured with the camera to calibrate, skip this step.
    3.1. At "number of images:" prompt, enter the number of images to capture.  More than 10 is recommended.
    3.2. At "frame interval:" prompt, enter the number of interval among image captures.
    3.3. At "save directory:" prompt, enter the directory to save captured images.  Then camera starts and its images are displayed on window.
    3.3. Set checker pattern board in front of camera.  If you are ready, push "s" button.
    3.4. Move checker board in front of camera until the number of captured images is over the target, or until "q" button is pushed. 
4. At "command:" prompt, input "calib_int" and return. 
    4.1. At "Rows of checker corners:" prompt, input the number of rows of checker corners.  "pattern.png" have 6.
    4.2. At "Cols of checker corners:" prompt, input the number of cols of checker corners.  "pattern.png" have 9.
    4.3. At "Distance between corners (m):" prompt, input the length measured at step 1.
    4.4. At "Fisheye? (y/n):" prompt, puress "y" if you want to calibrate fisheye camera, or press "n".
    4.5. At "Path of image folder:" prompt, enter the path of directory in which calibration patter images are stored.  If you have not skipped step 3, enter the same path of step 3.3.
    4.6. At "Start calibration? (y/n):" prompt, press "y" to start calibration or press "n" to quit.
    4.7. If you press "y", the captured images and its detected corners are displayed.  Hit any key to see the next.
5. At "command:" prompt, input "save_param:" and return.
    5.1. At "Filename to save:" prompt, enter file name (.txt, .yaml, .xml) to save internal camera paramters.
    5.1. Internal camera parameters are saved as yaml or xml file.
6. At "command:" prompt, input "undistort" and return if you want to see undistorted image by calibrated paramters.
    6.1. At "Distort Image:" prompt, enter image file path which is captured by the camera.
    6.2. At "Undistort Image to save:" prompt, enter image file path to save.

You can see the all commands by enter "h" at "command:" prompt. 

