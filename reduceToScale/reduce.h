#pragma once

#include <vector>

void imageToTextScaledNaive(const unsigned char *image, int size, unsigned imageWidth, unsigned int scaleX, unsigned int scaleY, unsigned char *output, int pixelSize, const cv::Mat characters[], int numOfChars);
void getScalingFactors(unsigned imageWidth, unsigned imageHeight, unsigned &scaleX, unsigned &scaleY);
