#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

//functions to move pointer left, right, up, down
unsigned char moveLeft(unsigned char *p, int channels) {
    p += channels;
    //returns color
    return p+1;
}

unsigned char moveRight(unsigned char *p, int channels) {
    p -= channels;
    return p+1;
}

unsigned char moveUp(unsigned char *p, int channels, int width) {
    p -= width*channels;
    return p+1;
}

unsigned char moveDown(unsigned char *p, int channels, int width) {
    p += width*channels;
    return p+1;
}

//places p at the x, y position from p's starting point
void getPosition(unsigned char*p, int channels, int height, int width, int x, int y) {
    p = p + y*channels*width*channels + x*channels;
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

    int THRESHOLD = 10;
    int EYE1_X = 0;
    int EYE1_Y = 0;
    int EYE2_X = 0;
    int EYE2_Y = 0;
    int MOUTH_X = 0;
    int MOUTH_Y = 0;

    unsigned char *eye1p = malloc(gray_channels);
    unsigned char *eye2p = malloc(gray_channels);
    unsigned char *mouthp = malloc(gray_channels);
    
    int eye1size = 1;
    int eye2size = 1;
    int mouthsize = 1;

    //set initial position of eye1p, eye2p, and mouthp
    unsigned char* temp = img;
    getPosition(temp, gray_channels, height, width, EYE1_X, EYE1_Y);
    eye1p = temp;

    temp = img;
    getPosition(temp, gray_channels, height, width, EYE2_X, EYE2_Y);
    eye2p = temp;

    temp = img;
    getPosition(temp, gray_channels, height, width, MOUTH_X, MOUTH_Y);
    mouthp = temp;

    stbi_image_free(img);
    free(gray_img);


}
