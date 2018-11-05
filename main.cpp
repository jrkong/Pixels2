#include <iostream>
#include <fstream>
#include <chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <omp.h>

// #include "util/loadpng.h"
// #include "util/util.h"
// #include "util/advisor-annotate.h"
#include "reduceToScale/reduce.h"

#define CHARACTER_SIZE 256

// temporary variable to switch between capturing from webcam or from file
#define CAPTURE_VIDEO 1

const char *filename = "Pixel2/test2.png";

using namespace std::chrono;
using namespace std;
using namespace cv;

// report system time
//
void reportTime(const char *msg, steady_clock::duration span)
{
	auto ms = duration_cast<milliseconds>(span);
	std::cout << msg << " - took - " << ms.count() << " milliseconds" << std::endl;
}

int main(int argc, const char *argv[])
{
	const char *font = "Monospace";
	auto fontColor = cv::Scalar(0, 0, 0);
	int fontScale = 6;

	const char *windowName = "cam";

	VideoCapture stream1(0); //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened())
	{ //check if video device has been initialised
		cout << "cannot open camera";
		return -1;
	}

	// stream1.set(CAP_PROP_FRAME_WIDTH, 1280);
	// stream1.set(CAP_PROP_FRAME_HEIGHT, 720);

	int width = stream1.get(CAP_PROP_FRAME_WIDTH);
	int height = stream1.get(CAP_PROP_FRAME_HEIGHT);
	int pixelSize = 3;
	int imageSize = height * width * pixelSize;

	unsigned char *whiteImage = new unsigned char[imageSize];

	unsigned scaleX, scaleY;
	// getScalingFactors(width, height, scaleX, scaleY);

	// since we are using monspace font, there is no need for dynamic scaling
	scaleX = 4;
	scaleY = 5;

	int sizeOfOutput = (width / scaleX) * (height / scaleY);
	unsigned char *output = new unsigned char[sizeOfOutput];

	// create window
	cv::namedWindow(windowName);

	if (CAPTURE_VIDEO)
	{
		const char *readFile = "./MyVideo.mp4";
		const char *writeFile = "./test.mp4";
		VideoCapture stream2(readFile); //0 is the id of video device.0 if you have only one camera.

		if (!stream1.isOpened())
		{ //check if video device has been initialised
			cout << "cannot open camera";
			return -1;
		}

		width = stream2.get(CAP_PROP_FRAME_WIDTH);
		height = stream2.get(CAP_PROP_FRAME_HEIGHT);

		Size frame_size(width, height);
		int frames_per_second = stream2.get(CAP_PROP_FPS);

		//Create and initialize the VideoWriter object
		VideoWriter oVideoWriter(writeFile, VideoWriter::fourcc('M', 'P', '4', 'V'),
								 frames_per_second, frame_size, true);

		//If the VideoWriter object is not initialized successfully, exit the program
		if (oVideoWriter.isOpened() == false)
		{
			cout << "Cannot save the video to a file" << endl;
			return -1;
		}

		while (true)
		{
			Mat cameraFrame;
			bool isSuccess = stream2.read(cameraFrame); // read a new frame from the video camera

			//Breaking the while loop if frames cannot be read from the camera
			if (isSuccess == false)
			{
				cout << "Video camera is disconnected" << endl;
				break;
			}

			/*
        Make changes to the frame as necessary
        e.g.  
         1. Change brightness/contrast of the image
         2. Smooth/Blur image
         3. Crop the image
         4. Rotate the image
         5. Draw shapes on the image
        */
			// inverse image movements
			unsigned char *image = cameraFrame.data;
// initialize a white image used to draw text on
#pragma omp parallel for
			for (int i = 0; i < imageSize; i += 3)
			{
				whiteImage[i] = whiteImage[i + 1] = whiteImage[i + 2] = (unsigned char)255;
			}
			// substitute the frame with a white canvas to draw on
			cameraFrame.data = whiteImage;

			// convert for ascii
			imageToTextScaledNaive(image, width * height * pixelSize, width, scaleX, scaleY, output, pixelSize);

			// draw text line by line, '\n' is not supported, so we need to break it up by separately drawing each line
			for (int i = 0, offset = 0; i < sizeOfOutput; i += width / scaleX, offset += 5)
			{
				cv::String out((char *)output + i, width / scaleX);
				cv::addText(cameraFrame, out, cv::Point(0, offset), font, fontScale, fontColor, cv::QT_FONT_LIGHT);
			}

			//write the video frame to the file
			oVideoWriter.write(cameraFrame);

			//show the frame in the created window
			imshow(windowName, cameraFrame);

			//Wait for for 10 milliseconds until any key is pressed.
			//If the 'Esc' key is pressed, break the while loop.
			//If any other key is pressed, continue the loop
			//If any key is not pressed within 10 milliseconds, continue the loop
			if (waitKey(10) == 27)
			{
				cout << "Esc key is pressed by the user. Stopping the video" << endl;
				break;
			}
		}
	}
	else
	{
		while (true)
		{
			Mat cameraFrame;
			stream1.read(cameraFrame);
			// inverse image movements
			flip(cameraFrame, cameraFrame, 1);
			unsigned char *image = cameraFrame.data;
// initialize a white image used to draw text on
#pragma omp parallel for
			for (int i = 0; i < imageSize; i += 3)
			{
				whiteImage[i] = whiteImage[i + 1] = whiteImage[i + 2] = (unsigned char)255;
			}
			// substitute the frame with a white canvas to draw on
			cameraFrame.data = whiteImage;

			// convert for ascii
			imageToTextScaledNaive(image, width * height * pixelSize, width, scaleX, scaleY, output, pixelSize);

			// draw text line by line, '\n' is not supported, so we need to break it up by separately drawing each line
			for (int i = 0, offset = 0; i < sizeOfOutput; i += width / scaleX, offset += 5)
			{
				cv::String out((char *)output + i, width / scaleX);
				cv::addText(cameraFrame, out, cv::Point(0, offset), font, fontScale, fontColor, cv::QT_FONT_LIGHT);
			}

			imshow(windowName, cameraFrame);
			if (waitKey(10) >= 0)
				break;
		}
	}

	//unconditional loop

	delete[] output;
	delete[] whiteImage;

	return 0;
}
