#include "../Pixel2/util.h"

#include <omp.h>

using namespace std;

void imageToTextNaive(const vector<unsigned char> &image, unsigned width, char *output, int size, int pixelSize)
{
    // Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
    // Mapping luminecense
    int c = 0;
    output[0] = getLumCharacterFancyPants(calcLum(&image[0]));
    for (int i = pixelSize; i < image.size(); i += pixelSize)
    {
        c++;
        output[i / pixelSize] = getLumCharacterFancyPants(calcLum(&image[i]));
    }

    for (int i = width - 1; i < size; i += width)
    {
        output[i] = '\n';
    }
}

int imageToTextSPMD(const vector<unsigned char> &image, unsigned width, char *output, int size, int pixelSize)
{
    int nt = 0;

#pragma omp parallel
    {

        int ct = omp_get_thread_num();
        int total = omp_get_num_threads();
        if (ct == 0)
            nt = total;

        for (int i = pixelSize * ct; i < image.size(); i += total * pixelSize)
        {
            output[i / pixelSize] = getLumCharacterFancyPants(calcLum(&image[i]));
        }
    }

    for (int i = width - 1; i < size; i += width)
    {
        output[i] = '\n';
    }
    return nt;
}

void imageToTextWorkSharing(const vector<unsigned char> &image, unsigned width, char *output, int size, int pixelSize)
{
    // Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
    // Mapping luminecense
    output[0] = getLumCharacterFancyPants(calcLum(&image[0]));

#pragma omp parallel for
    for (int i = pixelSize; i < image.size(); i += pixelSize)
    {
        output[i / pixelSize] = getLumCharacterFancyPants(calcLum(&image[i]));
    }

    for (int i = width - 1; i < size; i += width)
    {
        output[i] = '\n';
    }
}
