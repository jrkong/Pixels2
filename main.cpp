// GPU621 Project
// Alex Kong, Dmytro Sych, Yuriy Kartuzov
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
#include "loadpng.h"
#include <algorithm>
#include <iterator>

// Prototypes
std::vector<int> MakeBW(const std::vector<unsigned char> image, int width, int height, int newWidth, int newHeight);
std::vector<int> CropImage(const std::vector<int> bwImage,int w, int h, int nW, int nH);
std::vector<int> ImageReduce(const std::vector<int> image, int newImageWidth, int newImageHeight, int numOfElem);
std::vector<char> Map(const std::vector<int> input);
void WriteToFile(std::vector<char> image, int imageWidth, const char* filename);


int main(int argc, const char * argv[]) {
    
    //Filename
    const char* filename = "r2d2";
    char str[80];
    strcpy(str, "images/");
    strcat(str, filename);
    strcat(str, ".png");
    
   
    
    // 1. Original image capture
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, str);
    if(error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << "\n\n";
        return 0;
    }
    
	int newWidth = (4 * (int)(width / 4));
	int newHeight = (7 * (int)(height / 7));
	int newNumElem = (newWidth / 4) * (newHeight / 7);
    // 2. Getting Black and White Image
    std::vector<int> croppedImage = MakeBW(image, width, height, newWidth, newHeight);
    
    
    //TODO: add dynamic ratio scaling
#define Xratio 4
#define Yratio 7
    
    // 3. Cropping Image
    
    //std::vector<int> croppedImage = CropImage(bwImage, width, height, newWidth, newHeight);
    
    
    // 4. Reducing an image to multiple of 4:7
    std::vector<int> reducedImage = ImageReduce(croppedImage, newWidth, newHeight, newNumElem);

    
    // 5. Mapping happens here
    std::vector<char> output = Map(reducedImage);

    
    // 6. Drawing in a file - Main output
    WriteToFile(output, newWidth / 4, filename);
    
    // Reporting
    printf(" INPUT file: %s.png\n OUTPUT file: %s.txt\n\n", filename, filename);
    printf(" ORIGINAL file width: %d, height %d, pixels: %d\n", width, height, (int)image.size() / 4);
    //printf(" BLACK & WHITE file width: %d, height %d, pixels: %d\n", width, height, (int)bwImage.size() );
    printf(" CROPPED file width: %d, height %d, pixels: %d\n", newWidth, newHeight, (int)croppedImage.size() );
    printf(" REDUCED file width: %d, height %d, pixels: %d\n", newWidth / 4, newHeight / 7, (int)reducedImage.size() );
    printf(" MAPPED to ASCII vector<char> size is %d\n", (int)output.size() );
    return 0;
}




// Getting a single Luminocity out of RGBA set. Using standard formula (0.2126*R + 0.7152*G + 0.0722*B)
std::vector<int> MakeBW(std::vector<unsigned char> image, int width, int height, int newWidth, int newHeight){
    std::vector<int> bwImage;
	int diff = width - newWidth;
    
	int iter = 0;
    for(int i=0; i<newHeight; i++){
		for (int j = 0; j < newWidth; j++) {
			bwImage.push_back(
				(int)(0.2126 * (int)image.at(iter)) + (0.7152 * (int)image.at(iter + 1)) + (0.0722 * (int)image.at(iter + 2))
			);
			iter += 4;
		}
		iter += diff;
    }
    return bwImage;
}



// Cropping an Image so that width is multiple of 4 and height of 7
std::vector<int> CropImage(const std::vector<int> bwImage, int width, int height, int newWidth, int newHeight){
    std::vector<int> croppedImage;
    std::vector<int>::const_iterator current = bwImage.begin();
    
    for(int i = 0 ; i < height; i++){
        std::copy(current, current + newWidth, std::back_inserter(croppedImage));
        current += width;
    }

    return croppedImage;
}



// Reducing Image
std::vector<int> ImageReduce(const std::vector<int> image, int width, int height, int numElem){
    
    std::vector<int> reducedImage;
    int xP = 0; // an unfortunate name for a variable, what's the better name for it??
    int sum = 0;
    
    // Row block treversal
    for(int y = 0; xP < image.size() - (7 * width); y++){
        
        // Colomn block treversal
        for(int x = 0; x < (width / 4); x++){
            
            // Rows traversal
            for(int i = 0; i < 7; i++){
                
                // Colomn treversal
                for(int j = 0; j < 4; j++){
                    sum += image.at(xP + j);
                }
                
                xP += width;
            }
            
            // recording 4 * 7 pixels average
            int average = (int)sum / (4 * 7);
            reducedImage.push_back( average );
            
            // reposition & reset
            sum = 0;
            xP = xP - ( 7 * width) + 4; // ход конем
        }
        

        xP += (7 * width);
    }
    return reducedImage;
}


// Mapping to ASCII
std::vector<char> Map(const std::vector<int> image){
    
    // Original 70 char $@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.
    // Original 10 char {'@', '%', '#', '*', '+', '=', '-', ':', '.', ' ',' ' };

    // Special blend of 14 chars, mee-favourite :)
    char map[] {'W', '#', '@', '0', '&', '%', '*', 'z', '=', '+', '_', ',', '.', ' ' };
    
    std::vector<char> output;
    
    for(int i=0; i< image.size(); i++){
        int pixel = image.at(i);
        char character = map[int(floor(pixel / (255 / (sizeof(map) - 1))))];
        output.push_back(character);
    }
    return output;
}



// Drawing in a file - Main output
void WriteToFile(std::vector<char> image, int imageWidth, const char* filename){
    char str[80];
    strcpy(str, "images/");
    strcat(str, filename);
    strcat(str, ".txt");
    
    std::ofstream myfile (str);
    
    if(myfile.is_open())
    {
        for(int i=1;i<image.size(); i++){
            myfile << image.at(i);
            if (i % (imageWidth) == 0)
                myfile << "\n";
        }
    }else
        std::cout << "Write to file faled\n";
}
