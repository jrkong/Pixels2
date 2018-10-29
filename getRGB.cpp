#include <vector>
#include <iostream>

#include "loadpng.h"

using namespace std;

bool getRGB(string filename, string outfile)
{
    // Filename
    cout << "file name is: " << filename;

    // Declarations
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;

    // Reading Image
    unsigned error = lodepng::decode(image, width, height, filename);

    if (error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    // Printing Image details
    printf("The image's width: %d\n", width);
    printf("The image's height: %d\n", height);
    printf("There are %d of RGBA struct\n", (int)(image.size() / 4));

    //Encode the image
    error = lodepng::encode(outfile, image, width, height);

    //if there's an error, display it
    if (error)
    {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    return true;
}
