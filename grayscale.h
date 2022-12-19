/*
 * grayscale.h
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_

void conv_grayscale(void *picture,
		            int width,
		            int height);
void conv_grayscale_init(int width, int height);
void conv_grayscale_partial(void *picture, int offset, int len);
int get_grayscale_width();
int get_grayscale_height();
unsigned char *get_grayscale_picture();

#endif /* GRAYSCALE_H_ */
