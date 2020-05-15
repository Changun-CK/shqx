#include "_public.h"
#include<iostream>
struct st_rain
{
    int minr;   //分钟雨量，单位：0.1mm，由本小时累计雨量计算出来的，用当前分钟的小时雨量减本小时上一分钟的小时雨量，如果本小时上一分钟的数据缺失
    int r10m;
};

int main(void)
{
   struct st_rain m_rain;
   memset(&m_rain,0,sizeof(struct st_rain));
   m_rain.minr=0;
   m_rain.r10m=1;
   printf("m_rain.minr=%d",m_rain.minr);
   printf("m_rain.r10m=%d",m_rain.r10m);
   m_rain.minr+=m_rain.r10m;
   printf("m_rain.minr=%d",m_rain.minr);
  return 0;
} 
