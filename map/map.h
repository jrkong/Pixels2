#pragma once
#include <vector>

// using namespace std;

void imageToTextNaive(const std::vector<unsigned char> &image, unsigned width, char *output, int size, int pixelSize);
int imageToTextSPMD(const std::vector<unsigned char> &image, unsigned width, char *output, int size, int pixelSize);
void imageToTextWorkSharing(const std::vector<unsigned char> &image, unsigned width, char *output, int size, int pixelSize);
