#include <iostream>
#include <fstream>
#include <chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <omp.h>
#include <cstring>

// #include "util/loadpng.h"
#include "util/util.h"
// #include "util/advisor-annotate.h"
#include "reduceToScale/reduce.h"

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

	// stream.set(CAP_PROP_FRAME_WIDTH, 1280);
	// stream.set(CAP_PROP_FRAME_HEIGHT, 720);

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

	Mat *characters = new Mat[10];

	loadCharacters(characters);

	const char *font = "Monospace";
	auto fontColor = cv::Scalar(0, 0, 0);
	int fontScale = 6;

	const char *windowName = "cam";

	VideoCapture stream;
	VideoWriter outStream;
	int width, height, fps;
	int pixelSize = 3;

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

	unsigned scaleX, scaleY;
	// getScalingFactors(width, height, scaleX, scaleY);

	// since we are using monspace font, there is no need for dynamic scaling
	scaleX = 8;
	scaleY = 16;

	// int sizeOfOutput = (width / scaleX) * (height / scaleY) * pixelSize;
	int sizeOfOutput = width * height * pixelSize;

	unsigned char *output = new unsigned char[sizeOfOutput];
	int imageSize = height * width * pixelSize;

	// create window
	 cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
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
		auto n = cameraFrame.data;
		// convert for ascii
		imageToTextScaledNaive(n, width * height * pixelSize, width, scaleX, scaleY, output, pixelSize, characters);

		cameraFrame.data = output;
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

	return 0;
}
