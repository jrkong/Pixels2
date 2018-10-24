// Yuriy Kartuzov GPU621 Project

#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>

#include "loadpng.h"
#include "Pixel2\util.h"
#include "Pixel2\advisor-annotate.h"

const char* filename = "test1.png";

using namespace std::chrono;
using namespace std;

int calcLum(const unsigned char* pixels) {
	return (0.2126 * (int)pixels[0]) + (0.7152 * (int)pixels[1]) + (0.0722 * (int)pixels[2]);
}

void imageToTextNaive(const vector<unsigned char>& image, unsigned width, char* output, int size, int pixelSize) {
	// Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
	// Mapping luminecense
	output[0] = getLumCharacter(calcLum(&image[0]));
	for (int i = pixelSize; i < image.size(); i += pixelSize) {
		output[i / pixelSize] = getLumCharacter(calcLum(&image[i]));
	}

	for (int i = width - 1; i < size; i += width) {
		output[i] = '\n';
	}
}


int imageToTextSPMD(const vector<unsigned char>& image, unsigned width, char* output, int size, int pixelSize) {
	int nt = 0;

#pragma omp parallel
	{

		int ct = omp_get_thread_num();
		int total = omp_get_num_threads();
		if (ct == 0) nt = total;

		for (int i = pixelSize * ct; i < image.size(); i += total * pixelSize) {
			output[i / pixelSize] = getLumCharacter(calcLum(&image[i]));
		}
	}

	for (int i = width - 1; i < size; i += width) {
		output[i] = '\n';
	}
	return nt;
}

void imageToTextWorkSharing(const vector<unsigned char>& image, unsigned width, char* output, int size, int pixelSize) {
	// Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
	// Mapping luminecense
	output[0] = getLumCharacter(calcLum(&image[0]));

#pragma omp parallel for
	for (int i = pixelSize; i < image.size(); i += pixelSize) {
		output[i / pixelSize] = getLumCharacter(calcLum(&image[i]));
	}

	for (int i = width - 1; i < size; i += width) {
		output[i] = '\n';
	}
}

// report system time
//
void reportTime(const char* msg, steady_clock::duration span) {
	auto ms = duration_cast<milliseconds>(span);
	std::cout << msg << " - took - " <<
		ms.count() << " milliseconds" << std::endl;
}

int main(int argc, const char * argv[]) {
	// Welcome message
	std::cout << "Pixels 2!\n";

	// Filename
	printf("file name is: %s\n", filename);

	// Declarations
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	// Reading Image
	unsigned error = lodepng::decode(image, width, height, filename);
	if (error) {
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}

	// Printing Image details
	printf("The image's width: %d\n", width);
	printf("The image's height: %d\n", height);
	printf("There are %d of RGBA struct\n", (int)(image.size() / 4));


	// Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
	steady_clock::time_point ts, te;
	// allocate resulting array of size image
	int sizeOfoutput = image.size() / 4;
	char * output = new char[sizeOfoutput];
	int pixelSize = 4;

	ts = steady_clock::now();
	imageToTextNaive(image, width, output, sizeOfoutput, pixelSize);
	te = steady_clock::now();
	reportTime("Serial: ", te - ts);


	ts = steady_clock::now();
	int numOfThreads = imageToTextSPMD(image, width, output, sizeOfoutput, pixelSize);
	te = steady_clock::now();
	reportTime("SPMD: ", te - ts);

	ts = steady_clock::now();
	imageToTextWorkSharing(image, width, output, sizeOfoutput, pixelSize);
	te = steady_clock::now();
	reportTime("Work sharing with vectorization: ", te - ts);

	// Outputting to a file
	//std::ofstream myfile("output.txt");
	//if (!myfile.is_open()) {
	//	std::cout << "DID NOT WORK\n";
	//	return 1;
	//}
	//else {
	//	myfile.write(output, sizeOfoutput);
	//	myfile.close();
	//	std::cout << "Done." << std::endl;
	//}
	delete[] output;
	return 0;
}
