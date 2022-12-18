/*
 * sobel_all.c
 *
 *  Created on: 18 déc. 2022
 *      Author: Julien
 */


#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <system.h>
#include <sys/alt_cache.h>
#include "sys/alt_timestamp.h"

unsigned char *grayscale_array;
unsigned char *sobel_result;
int array_width = 0;
int array_height = 0;
int size = 0;

alt_u32 start_sobel = 0;
alt_u32 end_sobel = 0;
alt_u32 start_conv_grayscale = 0;
alt_u32 end_conv_grayscale = 0;

void sobel_all_init(int width, int height){
	int loop = 0;
	array_width = width;
	array_height = height;
	size = width*height;
	printf("Sobel size: %d x %d = %d pixel\n", width, height, size);
	if (grayscale_array != NULL)
		free(grayscale_array);
	grayscale_array = (unsigned char *) malloc(width* height);
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)malloc(width*height*sizeof(unsigned char));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_result[loop] = 0;
	}
}

void sobel_all_complete(void *picture){

	int x,y,pos;
	unsigned short *pixels = (unsigned short *)picture;
	start_conv_grayscale = alt_timestamp();
	for (pos = 0 ; pos < size ; pos++) {
		grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
		//IOWR_8DIRECT(grayscale_array,p,ALT_CI_RGB2GRAY_0(pixels[p]));
	}
	end_conv_grayscale = alt_timestamp();

	start_sobel = alt_timestamp();
	int a,b,c,d;
	for (y = 1 ; y < (array_height-1) ; y++) {
		for (x = 1 ; x < (array_width-1) ; x++) {
			a = grayscale_array[(y-1)*array_width+(x-1)];
			b = grayscale_array[(y-1)*array_width+(x+1)];
			c = grayscale_array[(y+1)*array_width+(x-1)];
			d = grayscale_array[(y+1)*array_width+(x+1)];
			IOWR_8DIRECT(sobel_result,(y*array_width)+x,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + 2 * grayscale_array[(y-1)*array_width+(x)] +
						b - c - 2 * grayscale_array[(y+1)*array_width+(x)] - d) +
					ALT_CI_ABSOLUTE_0(b - a - 2 * grayscale_array[(y)*array_width+(x-1)] +
						2 * grayscale_array[(y)*array_width+(x+1)] - c + d)
					,128));
		}
	}
	end_sobel = alt_timestamp();
	printf("conv grayscale: %d\n",(int)(end_conv_grayscale-start_conv_grayscale));
	printf("sobel: %d\n",(int)(end_sobel-start_sobel));
}

void sobel_all_partial(void *picture){
	int i, divider = 28, sub = array_height/divider;
	partial(picture, 0, sub,1);
	for (i = 1; i<divider; i++){
		partial(picture, (i * sub)-1, sub+1, 0);
	}
	partial(picture, divider * (sub-2), array_height%divider, 0);
}

void partial(void *picture, int from, int lines, int offsetfrom){
	int x,y,pos, f = from*array_width, t = from*array_width+lines*array_width;
	unsigned short *pixels = (unsigned short *)picture;

	//alt_dcache_flush_all();
	//alt_remap_cached(grayscale_array_a+from, size*array_width);
	for (pos = f ; pos < t ; pos++) {
		grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
	}


	int a,b,c,d;
	for (y = from+offsetfrom ; y < (from+lines-1) ; y++) {
		for (x = 1 ; x < (array_width-1) ; x++) {
					a = grayscale_array[(y-1)*array_width+(x-1)];
					b = grayscale_array[(y-1)*array_width+(x+1)];
					c = grayscale_array[(y+1)*array_width+(x-1)];
					d = grayscale_array[(y+1)*array_width+(x+1)];
					IOWR_8DIRECT(sobel_result,(y*array_width)+x,ALT_CI_THRESHOLD_0(
							ALT_CI_ABSOLUTE_0(a + 2 * grayscale_array[(y-1)*array_width+(x)] +
								b - c - 2 * grayscale_array[(y+1)*array_width+(x)] - d) +
							ALT_CI_ABSOLUTE_0(b - a - 2 * grayscale_array[(y)*array_width+(x-1)] +
								2 * grayscale_array[(y)*array_width+(x+1)] - c + d)
							,128));
		}
	}
}

unsigned char *GetSobelAllResult() {
	return sobel_result;
}

