#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

//functions to move pointer left, right, up, down
void moveLeft(unsigned char **p, int channels) {
    *p += channels;
}

void moveRight(unsigned char **p, int channels) {
    *p -= channels;
}

void moveUp(unsigned char **p, int channels, int width) {
    *p -= width*channels;
}

void moveDown(unsigned char **p, int channels, int width) {
    *p += width*channels;
}

//places p at the x, y position from p's starting point
void getPosition(unsigned char**p, int channels, int height, int width, int x, int y) {
    *p = *p + y*channels*width*channels + x*channels;
}

int getAverage(unsigned char*p, int channels, int width, int radius){
        int total = 0;
        int count = 0;

        //move to correct "ring" from center point
        for (int i = 0; i<radius; i++) {
            moveUp(&p, channels, width);
            moveLeft(&p, channels);
        }

        for (int i = 0; i<(2*radius); i++) {
            moveRight(&p, channels);
            total = total + *p;
            *p = 255;
            count ++;
        }

        for (int i = 0; i<(2*radius); i++) {
            moveDown(&p, channels, width);
            total = total + *p;
            *p = 255;
            count ++;
        }

        for (int i = 0; i<(2*radius); i++) {
            moveLeft(&p, channels);
            total = total + *p;
            *p = 255;
            count ++;
        }

        for (int i = 0; i<(2*radius); i++) {
            moveUp(&p, channels, width);
            total = total + *p;
            *p = 255;
            count ++;
        }

        return total/count;
    }

int main(int argc, char *argv[]) {

	// load image using the stb_image library 
	// accepts JPG/JPEG ONLY!!! simpler to ignore PNG alpha channel
    int width, height, channels;
    unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 0);
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    // convert the input image to gray
    size_t img_size = width * height * channels;
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;

    unsigned char *gray_img = malloc(gray_img_size);
    if(gray_img == NULL) {
        printf("Unable to allocate memory for the gray image.\n");
        exit(1);
    }

    for(unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        if(channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }

    stbi_write_jpg("grayed.jpg", width, height, gray_channels, gray_img, 100);

    img = gray_img;

    int THRESHOLD = 30;
    
    //masked person 3
    /*int EYE1_X = 4.2*width*gray_channels/10;
    int EYE1_Y = 5.4*width*gray_channels/10;
    int EYE2_X = 9*width*gray_channels/16;
    int EYE2_Y = 5.4*width*gray_channels/10;
    int MOUTH_X = width*gray_channels/2;
    int MOUTH_Y = 7*width*gray_channels/10;
    */

    //Becky
    /*
    int EYE1_X = 3.7*width*gray_channels/10;
    int EYE1_Y = 5*width*gray_channels/10;
    int EYE2_X = 5.9*width*gray_channels/10;
    int EYE2_Y = 4.9*width*gray_channels/10;
    int MOUTH_X = width*gray_channels/2;
    int MOUTH_Y = 7.3*width*gray_channels/10;
    */
    //Roberto
    
    THRESHOLD = 10;
    int EYE1_X = 4.7*width*gray_channels/10;
    int EYE1_Y = 4.85*width*gray_channels/10;
    int EYE2_X = 6.6*width*gray_channels/10;
    int EYE2_Y = 5.2*width*gray_channels/10;
    int MOUTH_X = 5.5*width*gray_channels/10;
    int MOUTH_Y = 7*width*gray_channels/10;
    
    
    unsigned char *eye1p = malloc(gray_channels);
    unsigned char *eye2p = malloc(gray_channels);
    unsigned char *mouthp = malloc(gray_channels);
    
    int eye1size = 1;
    int eye2size = 1;
    int mouthsize = 1;

    //set initial position of eye1p, eye2p, and mouthp
    unsigned char* temp = img;
    getPosition(&temp, gray_channels, height, width, EYE1_X, EYE1_Y);
    eye1p = temp;

    temp = img;
    getPosition(&temp, gray_channels, height, width, EYE2_X, EYE2_Y);
    eye2p = temp;

    temp = img;
    getPosition(&temp, gray_channels, height, width, MOUTH_X, MOUTH_Y);
    mouthp = temp;

    int eye1Average = getAverage(eye1p, gray_channels, width, 1);
    int eye2Average = getAverage(eye2p, gray_channels, width, 1);
    int mouthAverage = getAverage(mouthp, gray_channels, width, 1);


    //color centers white
    *eye1p = 255;
    *eye2p = 255;
    *mouthp = 255;


    //make clusters
    int ringaverage = eye1Average;
    int radius = 2;
    while ((abs(ringaverage-eye1Average)<=THRESHOLD)) {
        ringaverage = getAverage(eye1p, gray_channels, width, radius);
        radius ++;
        eye1size += 8*(radius-1);
    }

    ringaverage = eye2Average;
    radius = 2;
    while ((abs(ringaverage-eye2Average)<=THRESHOLD)) {
        ringaverage = getAverage(eye2p, gray_channels, width, radius);
        radius ++;
        eye2size += 8*(radius-1);
    }

    ringaverage = mouthAverage;
    radius = 2;
    while ((abs(ringaverage-mouthAverage)<=THRESHOLD)) {
        ringaverage = getAverage(mouthp, gray_channels, width, radius);
        radius ++;
        mouthsize += 8*(radius-1);
    }

    int eyeAverage = (eye1size + eye2size)/2;

    if (mouthsize/eyeAverage < 10){
        printf("Obie dangerous :( \n");
    }

    else
    {
        printf("Obie safe!\n");
    }
    

    stbi_write_jpg("clusters.jpg", width, height, gray_channels, gray_img, 100);
    
    printf("eyeaverage: %d mouthsize: %d\n", eyeAverage, mouthsize);

    //stbi_image_free(img);
    //stbi_image_free(gray_img);


}


