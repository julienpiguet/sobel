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

alt_u32 start_sobel_x = 0;
alt_u32 end_sobel_x = 0;
alt_u32 start_sobel_y = 0;
alt_u32 end_sobel_y = 0;
alt_u32 start_conv_grayscale = 0;
alt_u32 end_conv_grayscale = 0;
alt_u32 start_sobel_threshold = 0;
alt_u32 end_sobel_threshold = 0;

short *sobel_x_result;
short *sobel_y_result;
int sobel_width;
int sobel_height;

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
  init_sobel_arrays(cam_get_xsize()>>1,cam_get_ysize());

  sobel_width = cam_get_xsize()>>1;
  sobel_height = cam_get_ysize();
  sobel_x_result = (short *)malloc(sobel_width*sobel_height*sizeof(short));
  sobel_y_result = (short *)malloc(sobel_width*sobel_height*sizeof(short));
  for (int loop = 0 ; loop < sobel_width*sobel_height ; loop++) {
  		sobel_x_result[loop] = 0;
  		sobel_y_result[loop] = 0;
  }

  do {
	  if (new_image_available() != 0) {
		  if (current_image_valid()!=0) {

			  alt_timestamp_start();
			  start_sobel_x = 0;
			  end_sobel_x = 0;
			  start_sobel_y = 0;
			  end_sobel_y = 0;
			  start_conv_grayscale = 0;
			  end_conv_grayscale = 0;
			  start_sobel_threshold = 0;
			  end_sobel_threshold = 0;

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
		      case 1 :
		    	  	   start_conv_grayscale = alt_timestamp();
		    	  	   conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		    	  	   end_conv_grayscale = alt_timestamp();

		               grayscale = get_grayscale_picture();
		               transfer_LCD_with_dma(&grayscale[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		      	  		  vga_set_pointer(grayscale);
		      	  	   }
		      	  	   break;
		      case 2 :
		    	  	   start_conv_grayscale = alt_timestamp();
		    	  	   conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		    	  	   end_conv_grayscale = alt_timestamp();

		               grayscale = get_grayscale_picture();

		               start_sobel_x = alt_timestamp();
		               sobel_x_with_rgb(grayscale);
		               end_sobel_x = alt_timestamp();

		               image = GetSobel_rgb();
		               transfer_LCD_with_dma(&image[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      case 3 :
		    	  	   start_conv_grayscale = alt_timestamp();
		    	       conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		    	       end_conv_grayscale = alt_timestamp();

		               grayscale = get_grayscale_picture();

		               start_sobel_x = alt_timestamp();
		               sobel_x(grayscale);
		               end_sobel_x = alt_timestamp();

		               start_sobel_y = alt_timestamp();
		               sobel_y_with_rgb(grayscale);
		               end_sobel_y = alt_timestamp();

		               image = GetSobel_rgb();
		               transfer_LCD_with_dma(&image[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      default:
		    	  	   start_conv_grayscale = alt_timestamp();
		    	  	   conv_grayscale((void *)image,
	                                  cam_get_xsize()>>1,
	                                  cam_get_ysize());
		    	  	   end_conv_grayscale = alt_timestamp();

                       grayscale = get_grayscale_picture();

                       int x,y;
                       start_sobel_x = alt_timestamp();
                       for (y = 1 ; y < (sobel_height-1) ; y++) {
                    	   for (x = 1 ; x < (sobel_width-1) ; x++) {
                    		   sobel_x_result[y*sobel_width+x] =
                    				   -1 * grayscale[(y-1)*sobel_width+(x-1)] +
									   0 * grayscale[(y-1)*sobel_width+(x)]   +
									   1 * grayscale[(y-1)*sobel_width+(x+1)] +

									   -2 * grayscale[(y)*sobel_width+(x-1)]   +
									   0 * grayscale[(y)*sobel_width+(x)]     +
									   2 * grayscale[(y)*sobel_width+(x+1)]   +

									   -1 * grayscale[(y+1)*sobel_width+(x-1)] +
									   0 * grayscale[(y+1)*sobel_width+(x)]   +
									   1 * grayscale[(y+1)*sobel_width+(x+1)];
                    	   }
                       }
                       end_sobel_x = alt_timestamp();

                       start_sobel_y = alt_timestamp();
                       for (y = 1 ; y < (sobel_height-1) ; y++) {
                    	   for (x = 1 ; x < (sobel_width-1) ; x++) {
                      		   sobel_y_result[y*sobel_width+x] =
                      				   1 * grayscale[(y-1)*sobel_width+(x-1)] +
									   2 * grayscale[(y-1)*sobel_width+(x)] +
									   1 * grayscale[(y-1)*sobel_width+(x+1)] +

									   0 * grayscale[(y)*sobel_width+(x-1)] +
									   0 * grayscale[(y)*sobel_width+(x)] +
									   0 * grayscale[(y)*sobel_width+(x+1)] +

									   -1 * grayscale[(y+1)*sobel_width+(x-1)] +
									   -2 * grayscale[(y+1)*sobel_width+(x)] +
									   -1 * grayscale[(y+1)*sobel_width+(x+1)];
                    	   }
                       }
                       end_sobel_y = alt_timestamp();

                       start_sobel_threshold = alt_timestamp();
                       sobel_threshold(128);
                       end_sobel_threshold = alt_timestamp();

                       grayscale=GetSobelResult();
		               transfer_LCD_with_dma(&grayscale[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		      	  		  vga_set_pointer(grayscale);
		      	  	   }
		      	  	   break;
		      }

		      printf("conv grayscale: %d\n",(end_conv_grayscale-start_conv_grayscale));
		      printf("sobel x: %d\n",(end_sobel_x-start_sobel_x));
		      printf("sobel y: %d\n",(end_sobel_y-start_sobel_y));
		      printf("sobel threshold: %d\n",(end_sobel_threshold-start_sobel_threshold));
		  }
	  }
  } while (1);
  return 0;
}
