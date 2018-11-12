#include <math.h>
#include <omp.h>

#include "../util/util.h"
#include "reduce.h"

using namespace std;

// scales[0] = width, scales[1] = height
void getScalingFactors(unsigned imageWidth, unsigned imageHeight, unsigned &scaleX, unsigned &scaleY)
{

    float originalRatio = imageWidth / imageHeight;
    if (originalRatio < 0)
    {
        scaleY = int(ceil(imageWidth / float(imageHeight / 16)));
        scaleX = int(scaleY / float(imageWidth / 8));
    }
    else
    {
        scaleX = int(ceil(imageHeight / float(imageWidth / 16)));
        scaleY = int(scaleX * float(imageHeight / 8));
    }
}

// image has to be 'image size + 1' or the last row won't be processed
void imageToTextScaledNaive(const unsigned char *image, int size, unsigned imageWidth, unsigned int scaleX, unsigned int scaleY, unsigned char *output, int pixelSize, const cv::Mat characters[])
{
    int singleRow = imageWidth * pixelSize;
    // offset by the number of rows * scaleY (the number of shrunken columns) and look back to calculate average luminosity.
    // the step size the same as the offset.
    // #pragma omp parallel for
    // #pragma omp parallel for
    for (int currentRow = singleRow * scaleY; currentRow < size; currentRow += singleRow * scaleY)
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
            int h = (currentRow - (singleRow * scaleY));
            const cv::Mat c = characters[getLumCharacter(sum / (scaleX * scaleY))];
            int rowSize = c.cols * pixelSize;
            int s = c.rows;
            for (int i = 0; i < s; i++)
            {
                int left = h + i * singleRow + (partialWidth / scaleX) * rowSize;
                memcpy(output + left, c.data + rowSize * i, rowSize);
            }
        }
    }
}
