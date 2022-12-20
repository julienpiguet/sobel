#include <stdio.h>
#include <system.h>
#include <stdlib.h>
#include <io.h>
#include "lcd_simple.h"
#include "grayscale.h"
#include "i2c.h"
#include "camera.h"
#include "vga.h"
#include "dipswitch.h"
#include "sobel.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"
#include <sys/alt_cache.h>
//#include "sobel_all.h"

alt_u32 start_sobel = 0;
alt_u32 end_sobel = 0;
alt_u32 start_conv_grayscale = 0;
alt_u32 end_conv_grayscale = 0;
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

  init_sobel_arrays(cam_get_xsize()>>1,cam_get_ysize());
  conv_grayscale_init(cam_get_xsize()>>1, cam_get_ysize());
  //sobel_all_init(cam_get_xsize()>>1, cam_get_ysize());

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


		    	  	   start_conv_grayscale = alt_timestamp();
		    	  	   conv_grayscale((void *)image,
	                                  cam_get_xsize()>>1,
	                                  cam_get_ysize());
		    	  	   end_conv_grayscale = alt_timestamp();

                       grayscale = get_grayscale_picture();

                       start_sobel = alt_timestamp();
                       sobel_complete(grayscale, 128);
                       end_sobel = alt_timestamp();


                       //**************************************

		    	  	   /*
		    	  	   grayscale = get_grayscale_picture();

		    	  	   //alt_remap_cached (grayscale+0, 8192);
                       conv_grayscale_partial((void *)image, 0, 16);
                       sobel_complete_parted(grayscale, 128, 1, 14);

                       //alt_remap_cached (grayscale+14, 9216);
                       conv_grayscale_partial((void *)image, 16, 16);
                       sobel_complete_parted(grayscale, 128, 15, 16);

                       //alt_remap_cached (grayscale+30, 9216);
                       conv_grayscale_partial((void *)image, 32, 16);
                       sobel_complete_parted(grayscale, 128, 31, 16);

                       //alt_remap_cached (grayscale+46, 9216);
                       conv_grayscale_partial((void *)image, 48, 16);
                       sobel_complete_parted(grayscale, 128, 47, 16);

                       //alt_remap_cached (grayscale+62, 9216);
                       conv_grayscale_partial((void *)image, 64, 16);
                       sobel_complete_parted(grayscale, 128, 63, 16);

                       //alt_remap_cached (grayscale+78, 9216);
                       conv_grayscale_partial((void *)image, 80, 16);
                       sobel_complete_parted(grayscale, 128, 79, 16);

                       //alt_remap_cached (grayscale+94, 9216);
                       conv_grayscale_partial((void *)image, 96, 16);
                       sobel_complete_parted(grayscale, 128, 95, 16);

                       //alt_remap_cached (grayscale+110, 9216);
                       conv_grayscale_partial((void *)image, 112, 16);
                       sobel_complete_parted(grayscale, 128, 111, 16);



                       //alt_remap_cached (grayscale+126, 9216);
                       conv_grayscale_partial((void *)image, 128, 16);
                       sobel_complete_parted(grayscale, 128, 127, 16);

                       //alt_remap_cached (grayscale+142, 9216);
                       conv_grayscale_partial((void *)image, 144, 16);
                       sobel_complete_parted(grayscale, 128, 143, 16);

                       //alt_remap_cached (grayscale+158, 9216);
                       conv_grayscale_partial((void *)image, 160, 16);
                       sobel_complete_parted(grayscale, 128, 159, 16);

                       //alt_remap_cached (grayscale+174, 9216);
                       conv_grayscale_partial((void *)image, 176, 16);
                       sobel_complete_parted(grayscale, 128, 175, 16);

                       //alt_remap_cached (grayscale+190, 9216);
                       conv_grayscale_partial((void *)image, 192, 16);
                       sobel_complete_parted(grayscale, 128, 191, 16);

                       //alt_remap_cached (grayscale+206, 9216);
                       conv_grayscale_partial((void *)image, 208, 16);
                       sobel_complete_parted(grayscale, 128, 207, 16);

                       //alt_remap_cached (grayscale+222, 9216);
                       conv_grayscale_partial((void *)image, 224, 16);
                       sobel_complete_parted(grayscale, 128, 223, 16);

                       //alt_remap_cached (grayscale+238, 9216);
                       conv_grayscale_partial((void *)image, 240, 16);
                       sobel_complete_parted(grayscale, 128, 239, 16);



                       //alt_remap_cached (grayscale+254, 9216);
                       conv_grayscale_partial((void *)image, 256, 16);
                       sobel_complete_parted(grayscale, 128, 255, 16);

                       //alt_remap_cached (grayscale+270, 9216);
                       conv_grayscale_partial((void *)image, 272, 16);
                       sobel_complete_parted(grayscale, 128, 271, 16);

                       //alt_remap_cached (grayscale+286, 9216);
                       conv_grayscale_partial((void *)image, 288, 16);
                       sobel_complete_parted(grayscale, 128, 287, 16);

                       //alt_remap_cached (grayscale+302, 9216);
                       conv_grayscale_partial((void *)image, 304, 16);
                       sobel_complete_parted(grayscale, 128, 303, 16);

                       //alt_remap_cached (grayscale+318, 9216);
                       conv_grayscale_partial((void *)image, 320, 16);
                       sobel_complete_parted(grayscale, 128, 319, 16);

                       //alt_remap_cached (grayscale+334, 9216);
                       conv_grayscale_partial((void *)image, 336, 16);
                       sobel_complete_parted(grayscale, 128, 335, 16);

                       //alt_remap_cached (grayscale+350, 9216);
                       conv_grayscale_partial((void *)image, 352, 16);
                       sobel_complete_parted(grayscale, 128, 351, 16);

                       //alt_remap_cached (grayscale+366, 9216);
                       conv_grayscale_partial((void *)image, 368, 16);
                       sobel_complete_parted(grayscale, 128, 367, 16);
                       */

                       //**************************************



		    	  	   //sobel_all_partial((void *)image);
                       //sobel_all_complete((void *)image);
		    	  	   //sobel_all_complete_fusion((void *)image);

                       end_all = alt_timestamp();

                       grayscale=GetSobelResult();
                       //grayscale=GetSobelAllResult();

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
		      printf("conv grayscale: %d\n",(int)(end_conv_grayscale-start_conv_grayscale));
		      printf("sobel: %d\n",(int)(end_sobel-start_sobel));
		      printf("Total: %d Cycles and %d Cycles/Pixel\n",(int)(end_all-start_all), (int)(end_all-start_all)/pixels);
		      printf("FPS: %lfImg/s\n", 1.0L / ((double)(end_global- start_global) / ALT_CPU_CPU_FREQ));
		  }
	  }
  } while (1);
  return 0;
}
