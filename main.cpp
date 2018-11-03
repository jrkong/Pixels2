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
	int font = cv::FONT_HERSHEY_PLAIN;
	float fontScale = 0.2;
	int fontThickness = 1;
	auto fontColor = cv::Scalar(0, 0, 0);

	VideoCapture stream1(0); //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened())
	{ //check if video device has been initialised
		cout << "cannot open camera";
	}

	// stream1.set(CAP_PROP_FRAME_WIDTH, 1280);
	// stream1.set(CAP_PROP_FRAME_HEIGHT, 720);

	int width = stream1.get(CAP_PROP_FRAME_WIDTH);
	int height = stream1.get(CAP_PROP_FRAME_HEIGHT);
	int pixelSize = 3;
	int imageSize = height * width * pixelSize;

	unsigned char *whiteImage = new unsigned char[imageSize];

	unsigned scaleX, scaleY;
	getScalingFactors(width, height, scaleX, scaleY);
	int sizeOfOutput = (width / scaleX) * (height / scaleY);
	unsigned char *output = new unsigned char[sizeOfOutput];
	//unconditional loop
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
			cv::putText(cameraFrame,
						out,
						cv::Point(0, offset), // Coordinates
						font,				  // Font
						fontScale,			  // Scale. 2.0 = 2x bigger
						fontColor,			  // BGR Color
						fontThickness);		  // Anti-alias (Optional)
		}

		imshow("cam", cameraFrame);
		if (waitKey(10) >= 0)
			break;
	}

	delete[] output;
	delete[] whiteImage;

	return 0;
}
