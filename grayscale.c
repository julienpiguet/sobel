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
unsigned int *grayscale_array_int;
int grayscale_width_int = 0;
int grayscale_width = 0;
int grayscape_height = 0;

void conv_grayscale(void *picture,
		            int width,
		            int height) {
	int x,y,pos,pxl,y0;
	unsigned int *pixels = (unsigned int *)picture;

	for (y = 0 ; y < grayscape_height ; y++) {
		y0 = y*grayscale_width_int;
		for (x = 0 ; x < grayscale_width_int ; x++) {
			pos = y0+x;

			pxl = pos<<1;

			grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			//IOWR_32DIRECT(grayscale_array_int, pos, ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]));
			//pxl = pixels[p2];
			//grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pxl&65535);
			//grayscale_array[pos+1] = ALT_CI_RGB2GRAY_0((pxl>>16));
			//grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
		}
	}
}

void conv_grayscale_init(int width, int height){
		grayscale_width = width;
		grayscale_width_int = width>>2;
		grayscape_height = height;
		if (grayscale_array != NULL)
			free(grayscale_array);
		grayscale_array = (unsigned char *) malloc(width* height);
		grayscale_array_int = (unsigned int *) grayscale_array;
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


