/*
 * sobel_all.h
 *
 *  Created on: 18 déc. 2022
 *      Author: Julien
 */

//#define LOAD_SOBEL_ALL_COMPLETE
//#define LOAD_SOBEL_ALL_PARTED
//#define LOAD_SOBEL_ALL_FUSION
#if defined(LOAD_SOBEL_ALL_COMPLETE) || defined(LOAD_SOBEL_ALL_PARTED) || defined(LOAD_SOBEL_ALL_FUSION)
	#define LOAD_SOBEL_ALL
#endif

#ifndef SOBEL_ALL_H_
#define SOBEL_ALL_H_


#ifdef LOAD_SOBEL_ALL
void sobel_all_init(int width, int height);
unsigned char *GetSobelAllResult();
#endif

#ifdef LOAD_SOBEL_ALL_COMPLETE
void sobel_all_complete(void *picture);
#endif

#ifdef LOAD_SOBEL_ALL_PARTED
void sobel_all_partial(void *picture);
#endif

#ifdef LOAD_SOBEL_ALL_FUSION
void sobel_all_complete_fusion(void *picture);
#endif

#endif /* SOBEL_ALL_H_ */
