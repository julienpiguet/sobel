/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include "sobel.h"

#ifdef LOAD_SOBEL
#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include <system.h>
#include <sys/alt_cache.h>



unsigned char *sobel_result;
int sobel_width;
int sobel_height;

unsigned char *GetSobelResult() {
	return sobel_result;
}
#endif

#ifdef LOAD_SOBEL_OLD
const char gx_array[3][3] = {{-1,0,1},
                             {-2,0,2},
                             {-1,0,1}};
const char gy_array[3][3] = { {1, 2, 1},
                              {0, 0, 0},
                             {-1,-2,-1}};

short *sobel_x_result;
short *sobel_y_result;
unsigned short *sobel_rgb565;
#endif

#ifdef LOAD_SOBEL
#ifdef LOAD_SOBEL_OLD
void init_sobel_arrays(int width , int height) {
	int loop;
	sobel_width = width;
	sobel_height = height;
	printf("Sobel size: %d x %d = %d pixel\n", width, height, width*height);
	if (sobel_x_result != NULL)
		free(sobel_x_result);
	sobel_x_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_y_result != NULL)
		free(sobel_y_result);
	sobel_y_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)alt_uncached_malloc(width*height*sizeof(unsigned char));
	if (sobel_rgb565 != NULL)
		free(sobel_rgb565);
	sobel_rgb565 = (unsigned short *)malloc(width*height*sizeof(unsigned short));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_x_result[loop] = 0;
		sobel_y_result[loop] = 0;
		sobel_result[loop] = 0;
		sobel_rgb565[loop] = 0;
	}
}
#else
void init_sobel_arrays(int width , int height) {
	int loop;
	sobel_width = width;
	sobel_height = height;
	printf("Sobel size: %d x %d = %d pixel\n", width, height, width*height);
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)alt_uncached_malloc(width*height*sizeof(unsigned char));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_result[loop] = 0;
	}
}

#endif
#endif

#ifdef LOAD_ALL_SOBEL
short sobel_mac( unsigned char *pixels,
                 int x,
                 int y,
                 const char *filter,
                 unsigned int width ) {
	return		filter[0] * pixels[(y-1)*width+(x-1)] +
	         	filter[1] * pixels[(y-1)*width+(x)] +
				filter[2] * pixels[(y-1)*width+(x+1)] +

				filter[3] * pixels[(y)*width+(x-1)] +
			    filter[4] * pixels[(y)*width+(x)] +
				filter[5] * pixels[(y)*width+(x+1)] +

				filter[6] * pixels[(y+1)*width+(x-1)] +
			    filter[7] * pixels[(y+1)*width+(x)] +
				filter[8] * pixels[(y+1)*width+(x+1)];
}

void sobel_x( unsigned char *source ) {
   int x,y;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
         sobel_x_result[y*sobel_width+x] = sobel_mac(source,x,y,gx_array,sobel_width);
      }
   }
}

void sobel_x_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gx_array,sobel_width);
          sobel_x_result[y*sobel_width+x] = result;
          if (result < 0) {
        	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
          } else {
        	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
          }
      }
   }
}

void sobel_y( unsigned char *source ) {
   int x,y;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
         sobel_y_result[y*sobel_width+x] = sobel_mac(source,x,y,gy_array,sobel_width);
      }
   }
}

void sobel_y_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gy_array,sobel_width);
         sobel_y_result[y*sobel_width+x] = result;
         if (result < 0) {
       	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
         } else {
       	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
         }
      }
   }
}

void sobel_threshold(short threshold) {
	int x,y,arrayindex;
	short sum,value;
	for (y = 1 ; y < (sobel_height-1) ; y++) {
		for (x = 1 ; x < (sobel_width-1) ; x++) {
			arrayindex = (y*sobel_width)+x;
			value = sobel_x_result[arrayindex];
			sum = (value < 0) ? -value : value;
			value = sobel_y_result[arrayindex];
			sum += (value < 0) ? -value : value;
			sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
		}
	}
}



unsigned short *GetSobel_rgb() {
	return sobel_rgb565;
}

#endif

#if defined(LOAD_SOBEL_COMPLETE) && !defined(LOAD_SOBEL_COMPLETE_ONE_LOOP)
void sobel_complete(unsigned char *source, short threshold){
	int x,y;
	int a,b;
	int y1, y2, y0, yc;
	int p1, p2, p3, p4, p5, p6, p7, p8, p9;
	int width_len = sobel_width-1;
	for (y = 1 ; y < (sobel_height-1) ; y++) {
			y0 = (y-1)*sobel_width+1;
		    y1 = y*sobel_width+1;
		    y2 = (y+1)*sobel_width+1;

			p2 = source[y0-1];
			p5 = source[y1-1];
			p8 = source[y2-1];

			p3 = source[y0];
			p6 = source[y1];
			p9 = source[y2];

			for (x = 1 ; x < width_len ; x++) {
				p1 = p2;
				p4 = p5;
				p7 = p8;

				p2 = p3;
				p5 = p6;
				p8 = p9;

				yc = y1+x;
				p3 = source[y0+x];
				p6 = source[yc];
				p9 = source[y2+x];

				a = p1 - p9;
				b = p3 - p7;
				IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
						ALT_CI_ABSOLUTE_MULTIPLE_0(a + b + 2 * (p2 - p8),b - a + 2 * (p6 - p4))
						,threshold));
			}
		}
}
#endif

#if defined(LOAD_SOBEL_COMPLETE) && defined(LOAD_SOBEL_COMPLETE_ONE_LOOP)
void sobel_complete(unsigned char *source, short threshold){
		int x,y;
		int y1, y2, y0, yc;
		int p1, p2, p3, p4, p5, p6, p7, p8, p9;
		int width_len = sobel_width-1;
		for (y = 1 ; y < (sobel_height-1) ; y++) {
				y0 = (y-1)*sobel_width+1;
			    y1 = y*sobel_width+1;
			    y2 = (y+1)*sobel_width+1;

				p2 = *(source+y0-1);
				p5 = *(source+y1-1);
				p8 = *(source+y2-1);

				p3 = *(source+y0);
				p6 = *(source+y1);
				p9 = *(source+y2);

				for (x = 1 ; x < width_len ; x++) {
					p1 = p2;
					p4 = p5;
					p7 = p8;

					p2 = p3;
					p5 = p6;
					p8 = p9;

					yc = y1+x;
					p3 = *(source+y0+x);
					p6 = *(source+yc);
					p9 = *(source+y2+x);

					int a = p1 - p9;
					int b = p3 - p7;
					IOWR_8DIRECT(sobel_result,yc-1,ALT_CI_THRESHOLD_0(
							ALT_CI_ABSOLUTE_MULTIPLE_0(a + b + 2 * (p2 - p8),b - a + 2 * (p6 - p4))
							,threshold));
				}
			}
}
#endif

#ifdef LOAD_SOBEL_PARTED
void sobel_complete_parted(unsigned char *source, short threshold, int offset, int len){
	int x,y;
	int a,b;
	int y1, y2, y0;
	int p1, p2, p3, p4, p5, p6, p7, p8, p9;
	int width_len = sobel_width-1;
	for (y = offset ; y < (offset+len) ; y++) {
			y0 = (y-1)*sobel_width+1;
			y1 = y*sobel_width+1;
			y2 = (y+1)*sobel_width+1;

			p2 = source[y0-1];
			p5 = source[y1-1];
			p8 = source[y2-1];

			p3 = source[y0];
			p6 = source[y1];
			p9 = source[y2];

			for (x = 1 ; x < width_len ; x++) {
				p1 = p2;
				p4 = p5;
				p7 = p8;

				p2 = p3;
				p5 = p6;
				p8 = p9;

				p3 = source[y0+x];
				p6 = source[y1+x];
				p9 = source[y2+x];

				a = p1 - p9;
				b = p3 - p7;
				IOWR_8DIRECT(sobel_result,y1+x,ALT_CI_THRESHOLD_0(
						ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
						ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4))
						,threshold));
			}
		}
}
#endif

#ifdef LOAD_SOBEL_CHUNCK
void sobel_complete_chunck(unsigned char *source, short threshold, int offset_x, int offset_y, int width, int height){
	int x,y;
	int a,b;
	int y1, y2, y0;
	int p1, p2, p3, p4, p5, p6, p7, p8, p9;
	int width_len = sobel_width-1;
	for (y = offset ; y < (offset+len) ; y++) {
			y0 = (y-1)*sobel_width+1;
			y1 = y*sobel_width+1;
			y2 = (y+1)*sobel_width+1;

			p2 = source[y0-1];
			p5 = source[y1-1];
			p8 = source[y2-1];

			p3 = source[y0];
			p6 = source[y1];
			p9 = source[y2];

			for (x = 1 ; x < width_len ; x++) {
				p1 = p2;
				p4 = p5;
				p7 = p8;

				p2 = p3;
				p5 = p6;
				p8 = p9;

				p3 = source[y0+x];
				p6 = source[y1+x];
				p9 = source[y2+x];

				a = p1 - p9;
				b = p3 - p7;
				IOWR_8DIRECT(sobel_result,y1+x,ALT_CI_THRESHOLD_0(
						ALT_CI_ABSOLUTE_0(a + b + 2 * (p2 - p8)) +
						ALT_CI_ABSOLUTE_0(b - a + 2 * (p6 - p4))
						,threshold));
			}
		}
}
#endif
