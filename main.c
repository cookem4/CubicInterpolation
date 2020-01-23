#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int pixelCalculator(uint8_t fourPixels[4]){
    // We are ALWAYS calculating f(1/2) -> makes computation easy
    //Expansion from the ppt slide 9 gives very simple formula
    int newPixel = -0.0625*fourPixels[0] + 0.5625*fourPixels[1] + 0.5625*fourPixels[2] + -0.0625*fourPixels[3];
    if(newPixel < 0){
        return 0;
    }
    else if(newPixel > 255){
        return 255;
    }
    else{
        return newPixel;
    }
}
void shiftFour(uint8_t fourPixels[4], int nextIndex, uint8_t *image){
    fourPixels[0] = fourPixels[1];
    fourPixels[1] = fourPixels[2];
    fourPixels[2] = fourPixels[3];
    fourPixels[3] = image[nextIndex];
}
int main()
{
    int width, height, bpp;
    int NUM_CHANNELS = 1;
    uint8_t* image = stbi_load("owl.jpg", &width, &height, &bpp, NUM_CHANNELS);
    uint8_t* resizedImage = (uint8_t*) calloc(4*width*height, sizeof(uint8_t));


    //First go horizontally across the image interpolating cubically for a single point then shifting

    uint8_t fourPixels[4];
    int resizedImagePosition = 3; //insert first new calculated pixel in position 3
    //Every point we are calcualting is f(1/2)
    for(int k = 0; k < height; k++){
        resizedImagePosition = 3 + width*k*4;
        for(int i = 0; i < 4; i++){
            fourPixels[i] = image[i + k*width];
        }
        for(int i = 4; i <= width; i++){ // i is the index of the next pixel to shift in the fourPixels array
            //Perform calculation
            uint8_t interpolatedValue = pixelCalculator(fourPixels);
            resizedImage[resizedImagePosition] = interpolatedValue;
            resizedImagePosition+=2;
            shiftFour(fourPixels, i+k*width, image);
        }
    }

    //Populate reziedImage matrix with original image

    int origImgCounter = 0;
    long loopCounter = 0;
    for(int i = 0; i < 2*height; i++){
        for(int j = 0; j < 2*width; j++){
            if(j%2==0 && i%2==0){
                resizedImage[loopCounter] = image[origImgCounter];
                origImgCounter++;
            }
            loopCounter++;
        }
    }

    //Now go through horizontally, only dealing with the rezied image matrix
    //REMEMBER - skip 2nd column and second last column
    int newWidth = 2*width;
    int newHeight = 2*height;
    long colNum = 3;
    for(int k = 0; k < newWidth; k++){ //When k becomes 1441 a core gets dumped
        for(int i = 0; i < 4; i++){
            fourPixels[i] = resizedImage[i*2*newWidth+k];
        }
        colNum = 3;
        for(int i = 8; i < newHeight; i+=2){
            uint8_t interpolatedValue = pixelCalculator(fourPixels);
            resizedImage[colNum*newWidth+k] = interpolatedValue;
            colNum+=2;
            shiftFour(fourPixels, i*newWidth+k, resizedImage);
        }
    }

    //now fill in special cases. Use linear interpolation

    //The rows all at once:
    for(int i = 0; i < newWidth; i++){
        resizedImage[newWidth + i] = resizedImage[i]/2 + resizedImage[2*newWidth+i]/2;
        resizedImage[(newHeight-5)*newWidth + i] = resizedImage[(newHeight-6)*newWidth + i]/2 + resizedImage[(newHeight-4)*newWidth + i]/2;
        resizedImage[(newHeight-3)*newWidth + i] = resizedImage[(newHeight-4)*newWidth + i]/2 + resizedImage[(newHeight-2)*newWidth + i]/2;
    }

    //Now the columns all at once
    for(int i = 0; i<newHeight;i++){
        resizedImage[i*newWidth+1] = resizedImage[i*newWidth]/2 + resizedImage[i*newWidth+2]/2;
        resizedImage[i*newWidth+newWidth-3] = resizedImage[i*newWidth+newWidth-4]/2 + resizedImage[i*newWidth+newWidth-2]/2;
    }

    //Now finish final column on right side which MUST use nearest neighbour
    for(int i=0; i < newHeight; i++){
        resizedImage[newWidth*i + newWidth-1] = resizedImage[newWidth*i + newWidth-2];
    }

    //Now very bottom row
    for(int i=0; i < newWidth; i++){
        resizedImage[newWidth*(newHeight-1) + i] = resizedImage[newWidth*(newHeight-2) + i];
    }




    stbi_write_jpg("finalImage.jpg", 2*width, 2*height, NUM_CHANNELS, resizedImage, width*NUM_CHANNELS);

    stbi_image_free(image);
    free(resizedImage);
    return 0;
}

