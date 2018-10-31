#include <math.h>

#include "../util/util.h"
#include "reduce.h"

using namespace std;

// scales[0] = width, scales[1] = height
void getScalingFactors(unsigned imageWidth, unsigned imageHeight, unsigned desiredHeight, unsigned desiredWidth, unsigned *scaleX, unsigned *scaleY)
{

    float originalRatio = imageWidth / imageHeight;
    if (originalRatio < 0)
    {
        *scaleY = int(floor(imageWidth / 180));
        *scaleX = int(*scaleY / 2);
    }
    else
    {
        *scaleX = int(floor(imageHeight / 256));
        *scaleY = int(*scaleX * 2);
    }
}

// image has to be 'image size + 1' or the last row won't be processed
void imageToTextScaledNaive(const vector<unsigned char> &image, unsigned imageWidth, unsigned int scaleX, unsigned int scaleY, char *output, int size, int pixelSize)
{
    int singleRow = imageWidth * pixelSize;
    // offset by the number of rows * scaleY (the number of shrunken columns) and look back to calculate average luminosity.
    // the step size the same as the offset.
    // #pragma omp parallel for
    for (int currentRow = singleRow * scaleY; currentRow < image.size(); currentRow += singleRow * scaleY)
    {
        // iterate over the entire row vertically collecting row and column data
        for (int partialWidth = 0; partialWidth < imageWidth; partialWidth += scaleX)
        {
            int sum = 0;
            // traverse the row from left to right
            for (int x = 0; x < scaleX; x++)
            {
                // traverse column from top to bottom
                for (int y = 0; y < scaleY; y++)
                {
                    // get the starting point based on the step
                    int base = currentRow - singleRow * scaleY;
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
    for (int i = (imageWidth / scaleX) - 1; i < size; i += imageWidth / scaleX)
    {
        output[i] = '\n';
    }
}
