/*
 * sobel_all.h
 *
 *  Created on: 18 déc. 2022
 *      Author: Julien
 */

#ifndef SOBEL_ALL_H_
#define SOBEL_ALL_H_

void sobel_all_init(int width, int height);
void sobel_all_complete(void *picture);
void sobel_all_partial(void *picture);
void partial(void *picture, int fromgray, int fromsobel, int linesgray, int linessobel);
void sobel_all_complete_fusion(void *picture);
unsigned char *GetSobelAllResult();


#endif /* SOBEL_ALL_H_ */
