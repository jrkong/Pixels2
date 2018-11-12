#pragma once
#include <opencv2/highgui/highgui.hpp>

void loadCharacters(cv::Mat *characters);
char getLumCharacterFancyPants(int lum);
int getLumCharacter(int lum);
int calcLum(const unsigned char *pixels);
