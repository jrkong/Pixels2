#include <math.h>
#include <vector>
#include <iostream>
#include <string>

#include "loadpng.h"
#include "util.h"

using namespace std;

char getLumCharacterFancyPants(int lum)
{
    char map[]{'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};
    return map[int(floor(lum / (225 / (sizeof(map) - 1))))];
}

char getLumCharacter(int lum)
{
    char character;
    // $@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.  70 char set
    char map[]{'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};
    if (lum < 25)
        character = map[0];
    else if (lum >= 25 && lum < 50)
        character = map[1];
    else if (lum >= 50 && lum < 75)
        character = map[2];
    else if (lum >= 75 && lum < 100)
        character = map[3];
    else if (lum >= 100 && lum < 125)
        character = map[4];
    else if (lum >= 125 && lum < 150)
        character = map[5];
    else if (lum >= 150 && lum < 175)
        character = map[6];
    else if (lum >= 175 && lum < 200)
        character = map[7];
    else if (lum >= 200 && lum < 225)
        character = map[8];
    else if (lum >= 225)
        character = map[9];

    return character;
}

int calcLum(const unsigned char *pixels)
{
    return (0.2126 * (int)pixels[0]) + (0.7152 * (int)pixels[1]) + (0.0722 * (int)pixels[2]);
}

void getRGB(string filename, vector<unsigned char> &image, unsigned &width, unsigned &height)
{
    // Reading Image
    unsigned error = lodepng::decode(image, width, height, filename);

    if (error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
}

void loadCharacters(unsigned char chars[CHARACTER_COUNT][CHARACTER_SIZE])
{
    string base = "characters/";
    string files[] = {"@", "%", "#", "*", "+", "=", "-", ":", ".", "space"};
    string extension = ".png";

    int numOfFiles = sizeof(files) / sizeof(files[0]);

    for (int i = 0; i < numOfFiles; i++)
    {
        vector<unsigned char> out;
        unsigned width, height;
        getRGB(base + files[i] + extension, out, width, height);
        int size = out.size();
        for (int j = 0, k = 0; j < size; j += CHARACTER_PIXEL_SIZE + 1, k += CHARACTER_PIXEL_SIZE)
        {
            //this could be reversed, right now give RGB
            chars[i][k] = out[j];
            chars[i][k + 1] = out[j + 1];
            chars[i][k + 2] = out[j + 2];
        }
    }
}
