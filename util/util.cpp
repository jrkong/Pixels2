#include <math.h>
#include <vector>
#include <iostream>
#include <string>

#include "util.h"

using namespace std;
using namespace cv;

int getLumCharacterFancyPants(int lum, int size)
{
    int rc = floor(lum / (225 / size));
    return rc > size ? size : rc;
}

int getLumCharacter(int lum)
{
    int character;
    // $@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.  70 char set
    char map[]{'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};
    if (lum < 25)
        character = 0;
    else if (lum >= 25 && lum < 50)
        character = 1;
    else if (lum >= 50 && lum < 75)
        character = 2;
    else if (lum >= 75 && lum < 100)
        character = 3;
    else if (lum >= 100 && lum < 125)
        character = 4;
    else if (lum >= 125 && lum < 150)
        character = 5;
    else if (lum >= 150 && lum < 175)
        character = 6;
    else if (lum >= 175 && lum < 200)
        character = 7;
    else if (lum >= 200 && lum < 225)
        character = 8;
    else if (lum >= 225)
        character = 9;

    return character;
}

int calcLum(const unsigned char *pixels)
{
    return (0.2126 * (int)pixels[2]) + (0.7152 * (int)pixels[1]) + (0.0722 * (int)pixels[0]);
}

int loadCharacters(Mat *characters)
{
    string base = "./characters/";
    string files[] = {"at", "percent", "hash", "star", "plus", "equals", "hyphen", "column", "dot", "space"};
    string extension = ".png";

    int numOfFiles = sizeof(files) / sizeof(files[0]);
    for (int i = 0; i < numOfFiles; i++)
    {
        characters[i] = imread(base + files[i] + extension);
        if (!characters[i].data)
        {
            return 0;
        }
    }
    return numOfFiles;
}
