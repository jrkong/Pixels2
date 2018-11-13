#pragma once
#include <opencv2/highgui/highgui.hpp>

int loadCharacters(cv::Mat *characters);
int getLumCharacterFancyPants(int lum, int size);
int getLumCharacter(int lum);
int calcLum(const unsigned char *pixels);
