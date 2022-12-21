/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include "grayscale.h"

#ifdef LOAD_GRAYSCALE
#include <stdlib.h>
#include <io.h>
#include <system.h>
#include <sys/alt_cache.h>

unsigned char *grayscale_array;
unsigned int *grayscale_array_int;
int grayscale_width_int = 0;
int grayscale_width = 0;
int grayscape_height = 0;
int size_int = 0;

void conv_grayscale(void *picture) {
	register unsigned int * pos = (unsigned int *)(grayscale_array_int);
	register int pxl = 0;
	register unsigned int * end = (unsigned int *)(grayscale_array_int+size_int);
	register unsigned int *pixels = (unsigned int *)picture;

	while (pos < end) {

			*pos = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl++],pixels[pxl++]);
			pos++;
			//IOWR_32DIRECT(grayscale_array_int, pos, ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]));
	}
}

/*
INIT
     774:	d1e70b17 	ldw	r7,-25556(gp)
     778:	d0a70f17 	ldw	r2,-25540(gp)
     77c:	39cf883a 	add	r7,r7,r7
     780:	39cf883a 	add	r7,r7,r7
     784:	11cf883a 	add	r7,r2,r7
     788:	11c0092e 	bgeu	r2,r7,7b0 <conv_grayscale+0x3c>
     78c:	21400104 	addi	r5,r4,4

LOOP
     790:	20c00017 	ldw	r3,0(r4)
     794:	29800017 	ldw	r6,0(r5)
     798:	1987c0f2 	custom	3,r3,r3,r6
     79c:	10c00015 	stw	r3,0(r2)
     7a0:	10800104 	addi	r2,r2,4
     7a4:	21000204 	addi	r4,r4,8
     7a8:	29400204 	addi	r5,r5,8
     7ac:	11fff836 	bltu	r2,r7,790 <__alt_data_end+0xff000790>

RETURN
     7b0:	f800283a 	ret
 */

void conv_grayscale_init(int width, int height){
		grayscale_width = width;
		grayscale_width_int = width>>2;
		grayscape_height = height;
		if (grayscale_array != NULL)
			free(grayscale_array);
		grayscale_array = (unsigned char *) malloc(width* height);
		grayscale_array_int = (unsigned int *) grayscale_array;
		size_int = grayscape_height*grayscale_width_int;
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
#endif

#ifdef LOAD_GRAYSCALE_PARTED
void conv_grayscale_partial(void *picture, int offset, int len){
	int x,y,pos,pxl,y0;
	unsigned int *pixels = (unsigned int *)picture;
	for (y = offset ; y < offset+len ; y++) {
		y0 = y*grayscale_width_int;
				for (x = 0 ; x < grayscale_width_int ; x++) {
					pos = y0+x;
					pxl = pos<<1;
					grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
				}
	}
}
#endif

#ifdef LOAD_GRAYSCALE_CHUNCK
void conv_grayscale_chunck(unsigned char *source, int offset_x, int offset_y, int width, int height){
	int x,y,pos,pxl,y0;
		unsigned int *pixels = (unsigned int *)picture;
		for (y = offset_y ; y < offset_y+height ; y++) {
			y0 = y*grayscale_width_int;
					for (x = offset_x ; x < grayscale_width_int ; x++) {
						pos = y0+x;
						pxl = pos<<1;
						grayscale_array_int[pos] = ALT_CI_MULTIPLE_RGB2GRAY_0(pixels[pxl],pixels[pxl+1]);
					}
		}
	}
}
#endif
