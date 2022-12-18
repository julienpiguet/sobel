#include <stdio.h>
#include <system.h>
#include <stdlib.h>
#include <io.h>
#include "lcd_simple.h"
//#include "grayscale.h"
#include "i2c.h"
#include "camera.h"
#include "vga.h"
#include "dipswitch.h"
//#include "sobel.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"
#include "sobel_all.h"

//alt_u32 start_sobel = 0;
//alt_u32 end_sobel = 0;
//alt_u32 start_conv_grayscale = 0;
//alt_u32 end_conv_grayscale = 0;
alt_u32 start_all = 0;
alt_u32 end_all = 0;
alt_u32 start_global = 0;
alt_u32 end_global = 0;

int main()
{
  void *buffer1,*buffer2,*buffer3,*buffer4;
  unsigned short *image;
  unsigned char *grayscale;
  unsigned char current_mode;
  unsigned char mode;

  init_LCD();
  init_camera();
  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
  printf("Hello from Nios II!\n");
  cam_get_profiling();
  buffer1 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer2 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer3 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer4 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  cam_set_image_pointer(0,buffer1);
  cam_set_image_pointer(1,buffer2);
  cam_set_image_pointer(2,buffer3);
  cam_set_image_pointer(3,buffer4);
  enable_continues_mode();


  int pixels = (cam_get_xsize()>>1) * cam_get_ysize();

  //init_sobel_arrays(cam_get_xsize()>>1,cam_get_ysize());
  //conv_grayscale_init(cam_get_xsize()>>1, cam_get_ysize());
  sobel_all_init(cam_get_xsize()>>1, cam_get_ysize());

  do {
	  if (new_image_available() != 0) {
		  if (current_image_valid()!=0) {

			  alt_timestamp_start();

			  start_global = alt_timestamp();
			  current_mode = DIPSW_get_value();
			  mode = current_mode&(DIPSW_SW1_MASK|DIPSW_SW3_MASK|DIPSW_SW2_MASK);
			  image = (unsigned short*)current_image_pointer();
		      switch (mode) {
		      case 0 : transfer_LCD_with_dma(&image[16520],
		                	cam_get_xsize()>>1,
		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      default:
		    	  	   start_all = alt_timestamp();

		    	  	   /*
		    	  	   start_conv_grayscale = alt_timestamp();
		    	  	   conv_grayscale((void *)image,
	                                  cam_get_xsize()>>1,
	                                  cam_get_ysize());
		    	  	   end_conv_grayscale = alt_timestamp();

                       grayscale = get_grayscale_picture();

                       start_sobel = alt_timestamp();
                       sobel_complete(grayscale, 128);
                       end_sobel = alt_timestamp();
						*/


		    	  	   /*
		    	  	   conv_grayscale_partial((void *)image, 0, 60);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 0, 60);

		    	  	   conv_grayscale_partial((void *)image, 58, 60);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 58, 60);

		    	  	   conv_grayscale_partial((void *)image, 116, 60);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 116, 60);

		    	  	   conv_grayscale_partial((void *)image, 174, 60);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 174, 60);

		    	  	   conv_grayscale_partial((void *)image, 232, 60);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 232, 60);

		    	  	   conv_grayscale_partial((void *)image, 290, 60);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 290, 60);

		    	  	   conv_grayscale_partial((void *)image, 348, 36);
		    	  	   grayscale = get_grayscale_picture();
		    	  	   sobel_complete_sub(grayscale, 348, 36);
						*/


		    	  	   sobel_all_partial((void *)image);
		    	  	   //sobel_all_complete((void *)image);

                       end_all = alt_timestamp();

                       //grayscale=GetSobelResult();
                       grayscale=GetSobelAllResult();

		               transfer_LCD_with_dma(&grayscale[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		      	  		  vga_set_pointer(grayscale);
		      	  	   }
		      	  	   break;
		      }
		      end_global = alt_timestamp();
		      //printf("conv grayscale: %d\n",(int)(end_conv_grayscale-start_conv_grayscale));
		      //printf("sobel: %d\n",(int)(end_sobel-start_sobel));
		      printf("Total: %d Cycles and %d Cycles/Pixel\n",(int)(end_all-start_all), (int)(end_all-start_all)/pixels);
		      printf("FPS: %lfImg/s\n", 1.0L / ((double)(end_global- start_global) / ALT_CPU_CPU_FREQ));
		  }
	  }
  } while (1);
  return 0;
}
