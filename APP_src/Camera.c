/*
 * Camera.c
 *
 *  Created on: 2016. 7. 7.
 *      Author: a0795
 */


#include "Camera.h"
#include "images.h"
#include "Test_Pwm.h"
#include "glcd.h"
#include <string.h>


void CameraScan(void){
   CameraFinished=0;
   ClkCount1=0;
   GTM_TOM1_CH6_IRQ_EN.B.CCU0TC_IRQ_EN=1;
   while(ClkCount1<128);
   CameraShow();
}

void CameraShow(void){
   uint8 i;
   uint8 tempvalue;
   memset(blank_camera,0xffff,sizeof(blank_camera));
   memset(blank_camera1,0xffff,sizeof(blank_camera1));
   Delay(10);
   for(i=0;i<128;i++){
      tempvalue=(uint8)((uint32)CameraResult[0][i] * 120 / 4096);
      blank_camera[tempvalue][i]=0x0000;
   }
   for(i=0;i<128;i++){
            tempvalue=(uint8)((uint32)CameraResult[1][i] * 120 / 4096);
            blank_camera1[tempvalue][i]=0x0000;
   }
   GLCD_bitmap(0,0,CAMERA_WIDTH,CAMERA_HEIGHT,blank_camera);
   GLCD_bitmap(0,120,CAMERA_WIDTH,CAMERA_HEIGHT,blank_camera1);
}

void CameraAdj(void){ // binary search
   uint16 delta=32768;
   const int resolution = 15;
   int i;
   int result;
    char text[32];
    uint16 temp=32768;
   while(1){
      GTM_TOM1_TGC0_GLB_CTRL.U=0x10100000;//disable, update, enable
      GTM_TOM1_CH2_SR0.B.SR0=temp-(temp%resolution);
      GTM_TOM1_CH6_SR0.B.SR0=temp-(temp%resolution);
      GTM_TOM1_TGC0_GLB_CTRL.U=0x20200000;
      result=0;

      CameraScan();
      CameraScan();
      for(i=0;i<128;i++){
         if(CameraResult[0][i]>3895) result++;
      }

      if(result>80) temp-=delta;
      delta/=2;
      temp+=delta;

      usr_sprintf(text,"%5d",GTM_TOM1_CH2_SR0.B.SR0);
      GLCD_displayStringLnOffset(LINE0,text,8);
      if(delta<=resolution) break;
      if (temp<1200) break;
   }
#if 0
   temp=32768;
   delta=32768;
   while(1){//upper bound searching : at least one point < 200
      GTM_TOM1_TGC0_GLB_CTRL.U=0x10100000;
      GTM_TOM1_CH2_SR0.B.SR0=temp-(temp%resolution);
      GTM_TOM1_CH6_SR0.B.SR0=temp-(temp%resolution);
      GTM_TOM1_TGC0_GLB_CTRL.U=0x20200000;
      result=0;

      CameraScan();
      for(i=0;i<128;i++){
         if(CameraResult[0][i]<500) result++;
      }

      if(result>10) temp+=delta;
      delta/=2;
      temp-=delta;

      usr_sprintf(text,"%5d",GTM_TOM1_CH2_SR0.B.SR0);
      GLCD_displayStringLnOffset(LINE0,text,8);
      if(delta<=resolution) break;
   }
#endif
}
