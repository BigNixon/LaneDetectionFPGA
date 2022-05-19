#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "bmp24_io.c"

#define RAD(A)  (M_PI*((double)(A))/180.0)
void main(argc,argv)
int argc;
char *argv[];
{
char  f_name[100];
int   x,y;
long  pixel_lt,pixel_ct,pixel_rt;
long  pixel_lc,         pixel_rc;
long  pixel_lb,pixel_cb,pixel_rb;
long  lum_lt,lum_ct,lum_rt;
long  lum_lc,     lum_rc;
long  lum_lb,lum_cb,lum_rb;
long  sum_x, sum_y;
long  sum_x_2, sum_y_2;
long  g_square_256;
long  g_square;
long  g_limit;
long  g_limit_13bit;
long  g_int;
long  lum_new;
long  **image_in;
int   y_size, x_size;
int A[128][72];

long  **image_out;
long  **image_out2;
long  **ht_image;

if (argc != 2)
  {
  printf("USAGE: %s <input file base>\n",argv[0]);
  exit(1);
  }

printf("Sobel-Filter\n");
printf("============\n\n");

sprintf(f_name ,"%s.bmp",argv[1]);
bmp24_open(f_name,&image_in,&x_size,&y_size);

bmp24_alloc(&image_out,400,1500);//theta=0-360 and rho=0-sqrt(w**2 + h**2)
bmp24_alloc(&image_out2,1280,720);
for(int i=0;i<400;i++){
  for(int j=0;j<1500;j++){
    bmp24_put(image_out,0,0,0,i,j,400,1500);
    
  }
}
for(int i=0;i<1280;i++){
  for(int j=0;j<720;j++){
    bmp24_put(image_out2,0,0,0,i,j,1280,720);
    
  }
}

for (y=0;y<y_size;y++){
  for (x=0;x<x_size;x++){
    pixel_lt = bmp24_get(image_in,x-1,y-1,x_size,y_size); /* left   top  */
    pixel_ct = bmp24_get(image_in,x  ,y-1,x_size,y_size); /* center top  */
    pixel_rt = bmp24_get(image_in,x+1,y-1,x_size,y_size); /* right  top  */
    pixel_lc = bmp24_get(image_in,x-1,y  ,x_size,y_size); /* left   center  */
    pixel_rc = bmp24_get(image_in,x+1,y  ,x_size,y_size); /* right  center  */
    pixel_lb = bmp24_get(image_in,x-1,y+1,x_size,y_size); /* left   bottom  */
    pixel_cb = bmp24_get(image_in,x  ,y+1,x_size,y_size); /* center bottom  */
    pixel_rb = bmp24_get(image_in,x+1,y+1,x_size,y_size); /* right  bottom  */

    lum_lt = 5*bmp24_r(pixel_lt) + 9*bmp24_g(pixel_lt) + 2*bmp24_b(pixel_lt);
    lum_ct = 5*bmp24_r(pixel_ct) + 9*bmp24_g(pixel_ct) + 2*bmp24_b(pixel_ct);
    lum_rt = 5*bmp24_r(pixel_rt) + 9*bmp24_g(pixel_rt) + 2*bmp24_b(pixel_rt);
    lum_lc = 5*bmp24_r(pixel_lc) + 9*bmp24_g(pixel_lc) + 2*bmp24_b(pixel_lc);
    lum_rc = 5*bmp24_r(pixel_rc) + 9*bmp24_g(pixel_rc) + 2*bmp24_b(pixel_rc);
    lum_lb = 5*bmp24_r(pixel_lb) + 9*bmp24_g(pixel_lb) + 2*bmp24_b(pixel_lb);
    lum_cb = 5*bmp24_r(pixel_cb) + 9*bmp24_g(pixel_cb) + 2*bmp24_b(pixel_cb);
    lum_rb = 5*bmp24_r(pixel_rb) + 9*bmp24_g(pixel_rb) + 2*bmp24_b(pixel_rb);

    sum_x = (lum_rt + 2*lum_rc + lum_rb) - (lum_lt + 2*lum_lc + lum_lb);
    sum_y = (lum_lt + 2*lum_ct + lum_rt) - (lum_lb + 2*lum_cb + lum_rb);

    sum_x_2 = (sum_x*sum_x);
    sum_y_2 = (sum_y*sum_y);

    g_square_256 = (sum_x_2 + sum_y_2);
    g_square = g_square_256/256;
    if (g_square > 262143)
        g_limit = 262143;
    else
        g_limit = g_square;
    g_limit_13bit = 32*(g_limit/32);
    g_int    = sqrt(g_limit_13bit)/2;

    lum_new = 255 - g_int;
    //================== hough transform ================

    if(x>160 && x<1120){
      if(y>480 && y<710){

        
        
        if(lum_new<120){ //if it is an edge
          int theta;
          for(theta = 0; theta < 360; theta++){
            double C = cos(RAD(theta));
            double S = sin(RAD(theta));
              
            double rho = 1.0*x*C + 1.0*y*S;
            int rho_int = (int)rho;
            if(rho_int >  0){
              int pixel_rho_theta = bmp24_get(image_out,theta,rho_int,400,1500); /* left   top  */
              bmp24_put(image_out,pixel_rho_theta+1,pixel_rho_theta+1,pixel_rho_theta+1,theta,rho_int,400,1500);
            }
          } 
        }

      }
    }
  }
}

int detected_r[10] = {0,0,0,0,0,0,0,0,0,0};
int detected_t[10] = {0,0,0,0,0,0,0,0,0,0};

int i=0;
for(int t=0;t<360;t++){
  for(int r=0;r<1500;r++){
    int acc = bmp24_get(image_out,t,r,400,1500);
    acc = (acc&255);
    if(acc>80){ 
      //detected line with equation:
      // r = xcos(theta)+ysin(theta)
      // y = (r - x*cos(theta))/sin(theta)
      for(int bx=-5;bx<=5;bx++){
        for(int by=-5;by<=5;by++){
          if(t+bx>=0 && r+by>=0){
            bmp24_put(image_out,0,255,0,t+bx,r+by,400,1500);
          }
        }
      }
      
      if(i<10){
        detected_r[i]=r;
        detected_t[i]=t;
        i+=1;
      }

    }else{
      int pixel_acc = bmp24_get(image_out,t,r,400,1500)*16;
      bmp24_put(image_out,pixel_acc,pixel_acc,pixel_acc,t,r,400,1500);
    }
  }
}

//once detected the lines I draw them into the image_out2
for (y=0;y<y_size;y++){
  for (x=0;x<x_size;x++){
    pixel_lt = bmp24_get(image_in,x-1,y-1,x_size,y_size); /* left   top  */
    pixel_ct = bmp24_get(image_in,x  ,y-1,x_size,y_size); /* center top  */
    pixel_rt = bmp24_get(image_in,x+1,y-1,x_size,y_size); /* right  top  */
    pixel_lc = bmp24_get(image_in,x-1,y  ,x_size,y_size); /* left   center  */
    pixel_rc = bmp24_get(image_in,x+1,y  ,x_size,y_size); /* right  center  */
    pixel_lb = bmp24_get(image_in,x-1,y+1,x_size,y_size); /* left   bottom  */
    pixel_cb = bmp24_get(image_in,x  ,y+1,x_size,y_size); /* center bottom  */
    pixel_rb = bmp24_get(image_in,x+1,y+1,x_size,y_size); /* right  bottom  */

    lum_lt = 5*bmp24_r(pixel_lt) + 9*bmp24_g(pixel_lt) + 2*bmp24_b(pixel_lt);
    lum_ct = 5*bmp24_r(pixel_ct) + 9*bmp24_g(pixel_ct) + 2*bmp24_b(pixel_ct);
    lum_rt = 5*bmp24_r(pixel_rt) + 9*bmp24_g(pixel_rt) + 2*bmp24_b(pixel_rt);
    lum_lc = 5*bmp24_r(pixel_lc) + 9*bmp24_g(pixel_lc) + 2*bmp24_b(pixel_lc);
    lum_rc = 5*bmp24_r(pixel_rc) + 9*bmp24_g(pixel_rc) + 2*bmp24_b(pixel_rc);
    lum_lb = 5*bmp24_r(pixel_lb) + 9*bmp24_g(pixel_lb) + 2*bmp24_b(pixel_lb);
    lum_cb = 5*bmp24_r(pixel_cb) + 9*bmp24_g(pixel_cb) + 2*bmp24_b(pixel_cb);
    lum_rb = 5*bmp24_r(pixel_rb) + 9*bmp24_g(pixel_rb) + 2*bmp24_b(pixel_rb);

    sum_x = (lum_rt + 2*lum_rc + lum_rb) - (lum_lt + 2*lum_lc + lum_lb);
    sum_y = (lum_lt + 2*lum_ct + lum_rt) - (lum_lb + 2*lum_cb + lum_rb);

    sum_x_2 = (sum_x*sum_x);
    sum_y_2 = (sum_y*sum_y);

    g_square_256 = (sum_x_2 + sum_y_2);
    g_square = g_square_256/256;
    if (g_square > 262143)
        g_limit = 262143;
    else
        g_limit = g_square;
    g_limit_13bit = 32*(g_limit/32);
    g_int    = sqrt(g_limit_13bit)/2;

    lum_new = 255 - g_int;  
    bmp24_put(image_out2,lum_new,lum_new,lum_new,x,y,1280,720);

    //verify if it (x,y) satisfies the rule of the detected lines
    for(int i=0;i<10;i++){
      if(detected_r[i]!=0 && detected_t[i]!=0){
        // y = (r - x*cos(theta))/sin(theta)
        double rule_y = (1.0*detected_r[i] - 1.0*x*cos(RAD(detected_t[i])))/sin(RAD(detected_t[i]));
        int int_rule_y = (int)rule_y;
        if( int_rule_y == y){
            bmp24_put(image_out2,255,0,0,x,y,1280,720);
        }
      }
    }
  }
}


sprintf(f_name ,"%s_ht1_fixed.bmp",argv[1]);
bmp24_close(f_name,image_out,400,1500);

sprintf(f_name ,"%s_lines_detected.bmp",argv[1]);
bmp24_close(f_name,image_out2,1280,720);

printf("OK ...\n");
}
