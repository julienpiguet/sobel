/*
 * sobel.h
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */


//#define LOAD_SOBEL_OLD
#define LOAD_SOBEL_COMPLETE
//#define LOAD_SOBEL_PARTED

#if defined(LOAD_SOBEL_OLD) || defined(LOAD_SOBEL_COMPLETE) || defined(LOAD_SOBEL_PARTED)
	#define LOAD_SOBEL
#endif

#ifndef SOBEL_H_
#define SOBEL_H_



#ifdef LOAD_SOBEL
void init_sobel_arrays(int width , int height);
unsigned char *GetSobelResult();
#endif

#ifdef LOAD_SOBEL_OLD
void sobel_x( unsigned char *source );
void sobel_x_with_rgb( unsigned char *source );
void sobel_y( unsigned char *source );
void sobel_y_with_rgb( unsigned char *source );
void sobel_threshold(short threshold);

unsigned short *GetSobel_rgb();
#endif

#ifdef LOAD_SOBEL_COMPLETE
void sobel_complete(unsigned char *source, short threshold);
#endif

#ifdef LOAD_SOBEL_PARTED
void sobel_complete_parted(unsigned char *source, short threshold, int offset, int len);
#endif

#endif /* SOBEL_H_ */
