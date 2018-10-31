#pragma once

#include <vector>

void imageToTextScaledNaive(const std::vector<unsigned char> &image, unsigned imageWidth, unsigned int scaleX, unsigned int scaleY, char *output, int size, int pixelSize);
void getScalingFactors(unsigned imageWidth, unsigned imageHeight, unsigned desiredHeight, unsigned desiredWidth, unsigned *scaleX, unsigned *scaleY);
