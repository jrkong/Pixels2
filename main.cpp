// Yuriy Kartuzov GPU621 Project

#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>

#include "loadpng.h"
#include "Pixel2\util.h"
#include "Pixel2\advisor-annotate.h"

const char* filename = "tiger.png";

using namespace std::chrono;
using namespace std;

int calcLum(const unsigned char* pixels) {
	return (0.2126 * (int)pixels[0]) + (0.7152 * (int)pixels[1]) + (0.0722 * (int)pixels[2]);
}



void imageToTextNaive(const vector<unsigned char>& image, unsigned width, char* output, int size, int pixelSize) {
	// Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
	// Mapping luminecense
	int c = 0;
	output[0] = getLumCharacterFancyPants(calcLum(&image[0]));
	for (int i = pixelSize; i < image.size(); i += pixelSize) {
		c++;
		output[i / pixelSize] = getLumCharacterFancyPants(calcLum(&image[i]));
	}

	for (int i = width - 1; i < size; i += width) {
		output[i] = '\n';
	}

	cout << c << endl;
}


int imageToTextSPMD(const vector<unsigned char>& image, unsigned width, char* output, int size, int pixelSize) {
	int nt = 0;

#pragma omp parallel
	{

		int ct = omp_get_thread_num();
		int total = omp_get_num_threads();
		if (ct == 0) nt = total;

		for (int i = pixelSize * ct; i < image.size(); i += total * pixelSize) {
			output[i / pixelSize] = getLumCharacterFancyPants(calcLum(&image[i]));
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
	output[0] = getLumCharacterFancyPants(calcLum(&image[0]));

#pragma omp parallel for
	for (int i = pixelSize; i < image.size(); i += pixelSize) {
		output[i / pixelSize] = getLumCharacterFancyPants(calcLum(&image[i]));
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

// scales[0] = width, scales[1] = height
void getScalingFactors(unsigned imageWidth, unsigned imageHeight, unsigned desiredHeight, unsigned desiredWidth, unsigned* scaleX, unsigned* scaleY) {

	if (imageWidth < desiredWidth) {
		*scaleX = 1;
		*scaleY = 1;
	}
	else {
		desiredHeight = (imageWidth / imageHeight) * desiredWidth;
		*scaleX = ceil(imageWidth / (float)desiredWidth);
		*scaleY = ceil(imageHeight / (float)desiredHeight);
	}
}
double roundNum(double d)
{
	return floor(d + 0.5);
}


void imageToTextScaledNaive(const vector<unsigned char>& image, unsigned imageWidth, unsigned int scaleX, unsigned int scaleY, char* output, int size, int pixelSize) {
	int singleRow = imageWidth * pixelSize;
	// offset by the number of rows * scaleY (the number of shranked columns) and look back to calculate average luminosity.
	// the step size the same as the offset.
	// TODO: fix last row iteration
	for (int currentRow = singleRow * scaleY; currentRow < image.size(); currentRow += singleRow * scaleY) {
		// iterate over the entire row vertically collecting row and column data
		for (int partialWidth = 0; partialWidth < imageWidth; partialWidth += scaleX) {
			int sum = 0;
			// traverse the row from left to right
			for (int x = 0; x < scaleX; x++) {
				// traverse column from top to bottom
				for (int y = 0; y < scaleY; y++) {
					// get the starting point based on the step
					int base = (currentRow - singleRow * scaleY);
					// calculate coordinates of the pixel
					int coordinates = (partialWidth + x + y * imageWidth) * pixelSize;
					sum += calcLum(&image[base + coordinates]);
				}
			}
			// average out the sum and get the corresponding charater
			int index = (currentRow / (singleRow * scaleY) - 1) * (imageWidth / (scaleX)) + partialWidth / scaleX;
			output[index] = getLumCharacter(sum / (scaleX * scaleY));
		}
	}
	for (int i = (imageWidth / scaleX) - 1; i < size; i += imageWidth / scaleX) {
		output[i] = '\n';
	}

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
	unsigned int scaleX, scaleY;
	int desiredWidth = 300, desiredHeight = 400;
	getScalingFactors(width, height, desiredHeight, desiredWidth, &scaleX, &scaleY);

	// allocate resulting array of size image
	int sizeOfoutput = (width / scaleX) * (height / scaleY);
	char * output = new char[sizeOfoutput];
	int pixelSize = 4;

	//ts = steady_clock::now();
	//imageToTextNaive(image, width, output, sizeOfoutput, pixelSize);
	//te = steady_clock::now();
	//reportTime("Serial: ", te - ts);


	ts = steady_clock::now();
	imageToTextScaledNaive(image, width, scaleX, scaleY, output, sizeOfoutput, pixelSize);
	te = steady_clock::now();
	reportTime("Serial Scaled: ", te - ts);

	//ts = steady_clock::now();
	//int numOfThreads = imageToTextSPMD(image, width, output, sizeOfoutput, pixelSize);
	//te = steady_clock::now();
	//reportTime("SPMD: ", te - ts);

	//ts = steady_clock::now();
	//imageToTextWorkSharing(image, width, output, sizeOfoutput, pixelSize);
	//te = steady_clock::now();
	//reportTime("Work sharing with vectorization: ", te - ts);

	// Outputting to a file
	std::ofstream myfile("output.txt");
	if (!myfile.is_open()) {
		std::cout << "DID NOT WORK\n";
		return 1;
	}
	else {
		myfile.write(output, sizeOfoutput);
		myfile.close();
		std::cout << "Done." << std::endl;
	}
	delete[] output;
	return 0;
}
