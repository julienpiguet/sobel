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
unsigned int *grayscale_array_int;
int array_width = 0;
int array_height = 0;
int array_width_int = 0;
int size = 0;

alt_u32 start_sobel_a = 0;
alt_u32 end_sobel_a = 0;
alt_u32 start_conv_grayscale_a = 0;
alt_u32 end_conv_grayscale_a = 0;

void sobel_all_init(int width, int height){
	int loop = 0;
	array_width = width;
	array_height = height;
	size = width*height;
	array_width_int = width>>2;
	printf("Sobel size: %d x %d = %d pixel\n", width, height, size);
	if (grayscale_array != NULL)
		free(grayscale_array);
	grayscale_array = (unsigned char *) malloc(width* height);
	grayscale_array_int = (unsigned int *) grayscale_array;
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)alt_uncached_malloc(width*height*sizeof(unsigned char));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_result[loop] = 0;
	}
}

void sobel_all_complete(void *picture){
	int a,b;
	int x,y,pos,pxl;
	int y1, y2, y0, yc;
	int p1, p2, p3, p4, p5, p6, p7, p8, p9;
	int width_len = array_width-1;
	unsigned int *pixels = (unsigned int *)picture;


	start_conv_grayscale_a = alt_timestamp();
	for (y = 0 ; y < array_height ; y++) {
		y0 = y*array_width_int;
		for (x = 0 ; x < array_width_int ; x++) {
			pos = y0+x;
			pxl = pos<<1;
			grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
		}
	}
	end_conv_grayscale_a = alt_timestamp();

	start_sobel_a = alt_timestamp();
		for (y = 1 ; y < (array_height-1) ; y++) {
				y0 = (y-1)*array_width+1;
			    y1 = y*array_width+1;
			    y2 = (y+1)*array_width+1;
				p2 = grayscale_array[y0-1];
				p5 = grayscale_array[y1-1];
				p8 = grayscale_array[y2-1];
				p3 = grayscale_array[y0];
				p6 = grayscale_array[y1];
				p9 = grayscale_array[y2];
				for (x = 1 ; x < width_len ; x++) {
					p1 = p2;
					p4 = p5;
					p7 = p8;
					p2 = p3;
					p5 = p6;
					p8 = p9;
					yc = y1+x;
					p3 = grayscale_array[y0+x];
					p6 = grayscale_array[yc];
					p9 = grayscale_array[y2+x];

					a = p1 - p9;
					b = p3 - p7;
					IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
							ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
							ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4))
							,128));
		}
	}

	end_sobel_a = alt_timestamp();
	printf("conv grayscale: %d\n",(int)(end_conv_grayscale_a-start_conv_grayscale_a));
	printf("sobel: %d\n",(int)(end_sobel_a-start_sobel_a));
}

void sobel_all_partial(void *picture){
	    int a,b;
		int x,y,pos,pxl;
		int y1, y2, y0, yc;
		int p1, p2, p3, p4, p5, p6, p7, p8, p9;
		int width_len = array_width-1;
		unsigned int *pixels = (unsigned int *)picture;

		//**********************************************
		alt_remap_cached (grayscale_array, 8192);
		for (y = 0 ; y < 16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 1 ; y < 15 ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+14, 9216);
		for (y = 16 ; y < 16+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 15 ; y < (15+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+14, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+30, 9216);
		for (y = 32 ; y < 32+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 31 ; y < (31+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+30, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+46, 9216);
		for (y = 48 ; y < 48+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 47 ; y < (47+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+46, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+62, 9216);
		for (y = 64 ; y < 64+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 63 ; y < (63+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+62, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+78, 9216);
		for (y = 80 ; y < 80+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 79 ; y < (79+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+78, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+94, 9216);
		for (y = 96 ; y < 96+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 95 ; y < (95+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+94, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+110, 9216);
		for (y = 112 ; y < 112+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 111 ; y < (111+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+110, 8192);
		//**********************************************





		//**********************************************
		alt_remap_cached (grayscale_array+126, 9216);
		for (y = 128 ; y < 128+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 127 ; y < (127+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+126, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+142, 9216);
		for (y = 144 ; y < 144+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 143 ; y < (143+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+142, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+158, 9216);
		for (y = 160 ; y < 160+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 159 ; y < (159+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+158, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+174, 9216);
		for (y = 176 ; y < 176+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 175 ; y < (175+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+174, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+190, 9216);
		for (y = 192 ; y < 192+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 191 ; y < (191+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+190, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+206, 9216);
		for (y = 208 ; y < 208+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 207 ; y < (207+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+206, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+222, 9216);
		for (y = 224 ; y < 224+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 223 ; y < (223+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+222, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+238, 9216);
		for (y = 240 ; y < 240+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 239 ; y < (239+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+238, 8192);
		//**********************************************







		//**********************************************
		alt_remap_cached (grayscale_array+254, 9216);
		for (y = 256 ; y < 256+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 255 ; y < (255+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+254, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+270, 9216);
		for (y = 272 ; y < 272+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 271 ; y < (271+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+270, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+286, 9216);
		for (y = 288 ; y < 288+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 287 ; y < (287+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+286, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+302, 9216);
		for (y = 304 ; y < 304+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 303 ; y < (303+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+302, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+318, 9216);
		for (y = 320 ; y < 320+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 319 ; y < (319+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+318, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+334, 9216);
		for (y = 336 ; y < 336+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 335 ; y < (335+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+334, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+350, 9216);
		for (y = 352 ; y < 352+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 351 ; y < (351+16) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+350, 8192);
		//**********************************************

		//**********************************************
		alt_remap_cached (grayscale_array+366, 9216);
		for (y = 368 ; y < 368+16 ; y++) {
			y0 = y*array_width_int;
			for (x = 0 ; x < array_width_int ; x++) {
				pos = y0+x;
				pxl = pos<<1;
				grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
			}
		}
		for (y = 367 ; y < (367) ; y++) {
			y0 = (y-1)*array_width+1;
			y1 = y*array_width+1;
			y2 = (y+1)*array_width+1;
			p2 = grayscale_array[y0-1];
			p5 = grayscale_array[y1-1];
			p8 = grayscale_array[y2-1];
			p3 = grayscale_array[y0];
			p6 = grayscale_array[y1];
			p9 = grayscale_array[y2];
			for (x = 1 ; x < width_len ; x++) {
				p1 = p2; p4 = p5; p7 = p8;
				p2 = p3; p5 = p6; p8 = p9;
				yc = y1+x;
				p3 = grayscale_array[y0+x];
				p6 = grayscale_array[yc];
				p9 = grayscale_array[y2+x];
				a = p1 - p9; b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
					ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
					ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4)) ,128));
			}
		}
		alt_dcache_flush(grayscale_array+366, 9216);
		//**********************************************


}

void partial(void *picture, int fromgray, int fromsobel, int linesgray, int linessobel){
	int x,y,pos, f = fromgray*array_width, t = fromgray*array_width+linesgray*array_width;
	unsigned short *pixels = (unsigned short *)picture;

	//alt_dcache_flush_all();
	//alt_remap_cached(grayscale_array_a+from, size*array_width);
	for (pos = f ; pos < t ; pos++) {
		grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
	}


	int a,b,c,d;
	for (y = fromsobel+1 ; y < (fromsobel+linessobel-1) ; y++) {
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

void sobel_all_complete_fusion(void *picture){

	int x,y,pos;
	unsigned short *pixels = (unsigned short *)picture;


	for (pos = 0 ; pos < 3*array_width ; pos++) {
		grayscale_array[pos] = ALT_CI_RGB2GRAY_0(pixels[pos]);
	}


	int a,b,c,d;
	for (y = 1 ; y < (array_height-2) ; y++) {
		grayscale_array[(y+2)*array_width] = ALT_CI_RGB2GRAY_0(pixels[(y+2)*array_width]);
		for (x = 1 ; x < (array_width-1) ; x++) {
			grayscale_array[(y+2)*array_width+x] = ALT_CI_RGB2GRAY_0(pixels[(y+2)*array_width+x]);
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
		grayscale_array[(y+3)*array_width-1] = ALT_CI_RGB2GRAY_0(pixels[(y+3)*array_width-1]);
	}

	y = array_height-2;
	for (x = 1 ; x < (array_width-1) ; x++) {
				grayscale_array[y*array_width+x] = ALT_CI_RGB2GRAY_0(pixels[y*array_width]);
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
unsigned char *GetSobelAllResult() {
	return sobel_result;
}

