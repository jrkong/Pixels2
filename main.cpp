#include <iostream>
#include <fstream>
#include <chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// #include "util/loadpng.h"
// #include "util/util.h"
// #include "util/advisor-annotate.h"

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

	VideoCapture stream1(0); //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened())
	{ //check if video device has been initialised
		cout << "cannot open camera";
	}

	//unconditional loop
	while (true)
	{
		Mat cameraFrame;
		stream1.read(cameraFrame);
		imshow("cam", cameraFrame);
		if (waitKey(30) >= 0)
			break;
	}

	// // Welcome message
	// std::cout << "Pixels 2!\n";

	// // Filename
	// printf("file name is: %s\n", filename);

	// // Declarations
	// std::vector<unsigned char> image; //the raw pixels
	// unsigned width, height;

	// // Reading Image
	// unsigned error = lodepng::decode(image, width, height, filename);
	// if (error)
	// {
	// 	std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	// 	return 0;
	// }

	// // Printing Image details
	// printf("The image's width: %d\n", width);
	// printf("The image's height: %d\n", height);
	// printf("There are %d of RGBA struct\n", (int)(image.size() / 4));

	// // Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
	// steady_clock::time_point ts, te;
	// unsigned int scaleX, scaleY;
	// int desiredWidth = 600, desiredHeight = 400;
	// getScalingFactors(width, height, desiredHeight, desiredWidth, &scaleX, &scaleY);

	// //scaleX = scaleY = 1;

	// // allocate resulting array of size image
	// int sizeOfoutput = (width / scaleX) * (height / scaleY);
	// char *output = new char[sizeOfoutput];
	// char *out = new char[image.size() / 4];
	// int pixelSize = 4;

	// //ts = steady_clock::now();
	// //imageToTextNaive(image, width, out, sizeOfoutput, pixelSize);
	// //te = steady_clock::now();
	// //reportTime("Serial: ", te - ts);

	// image.push_back('\0');
	// ts = steady_clock::now();
	// imageToTextScaledNaive(image, width, scaleX, scaleY, output, sizeOfoutput, pixelSize);
	// te = steady_clock::now();
	// reportTime("Serial Scaled: ", te - ts);
	// image.pop_back();

	// //ts = steady_clock::now();
	// //int numOfThreads = imageToTextSPMD(image, width, output, sizeOfoutput, pixelSize);
	// //te = steady_clock::now();
	// //reportTime("SPMD: ", te - ts);

	// //ts = steady_clock::now();
	// //imageToTextWorkSharing(image, width, output, sizeOfoutput, pixelSize);
	// //te = steady_clock::now();
	// //reportTime("Work sharing with vectorization: ", te - ts);

	// // Outputting to a file
	// std::ofstream myfile("output.txt");
	// if (!myfile.is_open())
	// {
	// 	std::cout << "DID NOT WORK\n";
	// 	return 1;
	// }
	// else
	// {
	// 	myfile.write(output, sizeOfoutput);
	// 	myfile.close();
	// 	std::cout << "Done." << std::endl;
	// }
	// delete[] output;
	// delete[] out;
	return 0;
}
