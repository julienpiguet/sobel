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
	int pos, size = width*height;
	unsigned short *pixels = (unsigned short *)picture;
	grayscale_width = width;
	grayscape_height = height;
	if (grayscale_array != NULL)
		free(grayscale_array);
	grayscale_array = (unsigned char *) malloc(width*height);
	for (pos = 0 ; pos < size ; pos++) {
		grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
		//IOWR_8DIRECT(grayscale_array,p,ALT_CI_RGB2GRAY_0(pixels[p]));
	}
}

void conv_grayscale_init(int width, int height){
		grayscale_width = width;
		grayscape_height = height;
		if (grayscale_array != NULL)
			free(grayscale_array);
		grayscale_array = (unsigned char *) malloc(width* height);
}

void conv_grayscale_partial(void *picture, int from, int size){
	int x,y,pos;
	unsigned short *pixels = (unsigned short *)picture;
	//alt_dcache_flush_all();
	//alt_remap_cached(grayscale_array+from, size*grayscale_width);
	for (y = from ; y < from+size ; y++) {
		for (x = 0 ; x < grayscale_width ; x++) {
			pos = y*grayscale_width+x;
			//IOWR_8DIRECT(grayscale_array,y*width+x,gray);
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


