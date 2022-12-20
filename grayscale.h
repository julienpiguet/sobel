/*
 * grayscale.h
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#define LOAD_GRAYSCALE
//#define LOAD_GRAYSCALE_PARTED

#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_

#ifdef LOAD_GRAYSCALE
void conv_grayscale(void *picture);
void conv_grayscale_init(int width, int height);

int get_grayscale_width();
int get_grayscale_height();
unsigned char *get_grayscale_picture();
#endif

#ifdef LOAD_GRAYSCALE_PARTED
void conv_grayscale_partial(void *picture, int offset, int len);
#endif

#endif /* GRAYSCALE_H_ */
