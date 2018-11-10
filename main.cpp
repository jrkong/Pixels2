#include <iostream>
#include <fstream>
#include <chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <omp.h>
#include <cstring>

// #include "util/loadpng.h"
// #include "util/util.h"
// #include "util/advisor-annotate.h"
#include "reduceToScale/reduce.h"

#define CHARACTER_SIZE 256

// temporary variable to switch between capturing from webcam or from file
#define CAMERA_OPTION 1
#define VIDEO_OPTION 2

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

bool getVideoFeed(VideoCapture &stream, const char *source, int &width, int &height, int &fps)
{
	// check if we got an index of camera or filename
	int index = atoi(source);
	if (!index && strlen(source) > 1)
	{
		stream.open(source);
	}
	else
	{
		stream.open(index);
	}

	if (!stream.isOpened())
	{
		return false;
	}

	width = stream.get(CAP_PROP_FRAME_WIDTH);
	height = stream.get(CAP_PROP_FRAME_HEIGHT);
	fps = stream.get(CAP_PROP_FPS);

	return true;
}

void displayUsage()
{
	int c = CAMERA_OPTION;
	int v = VIDEO_OPTION;
	cout << endl
		 << "Usage is: <mode> <sourceOfInput> [<destOfOutput>]" << endl
		 << '\t' << "<mode>: Camera=" << c << ", Video=" << v << endl
		 << '\t' << "<sourceOfInput>: " << endl
		 << "\t\t"
		 << "For Camera: 0 based index of the camera to be used as source e.g. 0 for built-in webcam" << endl
		 << "\t\t"
		 << "For Video: name of the source file to be read" << endl
		 << '\t' << "<destOfOutput>: optional name of the file where to store the result of conversion" << endl;
}

int main(int argc, const char *argv[])

{
	if (argc < 2)
	{
		cout << endl
			 << "Insufficient options. Please select <mode>. Terminating!" << endl;
		displayUsage();

		return -2;
	}
	else if (argc < 3)
	{
		cout << endl
			 << "Insufficient options. <sourceOfInput> is not specified. Terminating!" << endl;

		displayUsage();
		return -3;
	}

	int mode = atoi(argv[1]);

	const char *source = nullptr;
	const char *dest = nullptr;

	source = argv[2];
	// destination file was specified
	if (argc == 4)
	{
		dest = argv[3];
	}

	const char *font = "Monospace";
	auto fontColor = cv::Scalar(0, 0, 0);
	int fontScale = 6;

	const char *windowName = "cam";

	VideoCapture stream;
	VideoWriter outStream;
	int width, height, fps;
	int pixelSize = 3;

	// if (CAPTURE_VIDEO)
	// {
	// 	source = "./180905_02_01.mp4";
	// 	dest = "./test.mp4";
	// }
	// else
	// {
	// 	source = "0";
	// }

	if (!getVideoFeed(stream, source, width, height, fps))
	{ //check if video device has been initialised
		cout << "cannot open " << source << endl;
		return -1;
	}

	if (dest != nullptr)
	{
		outStream.open(dest,
					   VideoWriter::fourcc('m', 'p', '4', 'v'),
					   fps,
					   Size(width, height),
					   true);
		//If the VideoWriter object is not initialized successfully, exit the program
		if (outStream.isOpened() == false)
		{
			cout << "Cannot save the video to " << dest << endl;
			return -1;
		}
	}

	// stream1.set(CAP_PROP_FRAME_WIDTH, 1280);
	// stream1.set(CAP_PROP_FRAME_HEIGHT, 720);

	unsigned scaleX, scaleY;
	// getScalingFactors(width, height, scaleX, scaleY);

	// since we are using monspace font, there is no need for dynamic scaling
	scaleX = 10;
	scaleY = 10;

	int sizeOfOutput = (width / scaleX) * (height / scaleY);
	unsigned char *output = new unsigned char[sizeOfOutput];
	int imageSize = height * width * pixelSize;

	unsigned char *whiteImage = new unsigned char[imageSize];

	// create window
	cv::namedWindow(windowName);
	//unconditional loop
	while (true)
	{
		Mat cameraFrame;
		bool isSuccess = stream.read(cameraFrame); // read a new frame from the video camera

		//Breaking the while loop if frames cannot be read from the camera
		if (isSuccess == false)
		{
			cout << "Video camera is disconnected" << endl;
			break;
		}

		// flip the frame if we are reading from camera
		if (mode == CAMERA_OPTION)
		{
			flip(cameraFrame, cameraFrame, 1);
		}

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
		if (dest != nullptr)
		{
			outStream.write(cameraFrame);
		}

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

	if (dest != nullptr)
	{
		outStream.release();
	}

	delete[] output;
	delete[] whiteImage;

	return 0;
}
