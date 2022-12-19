/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <io.h>
#include <system.h>
#include <sys/alt_cache.h>

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;

void conv_grayscale(void *picture,
		            int width,
		            int height) {
	int x,y,pos;
	unsigned short *pixels = (unsigned short *)picture;
	grayscale_width = width;
	grayscape_height = height;

	for (y = 0 ; y < height ; y++) {
		for (x = 0 ; x < width ; x++) {
			pos = y*width+x;
			grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
		}
	}
}

void conv_grayscale_init(int width, int height){
		grayscale_width = width;
		grayscape_height = height;
		if (grayscale_array != NULL)
			free(grayscale_array);
		grayscale_array = (unsigned char *) malloc(width* height);
}

void conv_grayscale_partial(void *picture, int offset, int len){
	int x,y,pos;
	unsigned short *pixels = (unsigned short *)picture;
	for (y = offset ; y < offset+len ; y++) {
		for (x = 0 ; x < grayscale_width ; x++) {
			pos = y*grayscale_width+x;
			grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
		}
	}
}


int get_grayscale_width() {
	return grayscale_width;
}

int get_grayscale_height() {
	return grayscape_height;
}

unsigned char *get_grayscale_picture() {
	return grayscale_array;
}


