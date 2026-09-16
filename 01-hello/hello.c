//#include <stdio.h>
//#include <unistd.h>
//#include <string.h>
#include <stdlib.h>
#include <stdint.h>


//make some regiter write macros eventually

#define VIF0 0x10008000

#define D0_CHCR ((volatile uint32_t *)0x10008000)
#define D0_MADR (*(volatile uint32_t *)0x10008010)
#define D0_QWC  (*(volatile uint32_t *)0x10008020)

int main(int argc, char *argv[])
{


    volatile int num = 69;
    //int* num;
    volatile int num2 = 4;
    volatile uint32_t *p = (volatile uint32_t *)0x00100000;

    *p = 0x12345678;

    volatile uint32_t data[4];

    data[0] = 0x11111111;
    data[1] = 0x22222222;
    data[2] = 0x33333333;
    data[3] = 0x44444444;
    
    volatile uint32_t arr[16] = {0};
    volatile uint32_t* ptr = arr;

    for(int i = 0; i < 16; i++) {
      ptr[i] = 0x10000000 + i;
    }


    //DMA stuff

    volatile uint32_t* kernsrc = (volatile uint32_t*)0xA0200000;

    uint32_t tranlation = 0x0FFFFFFF;

    //set a proper formed packet in kernsrc
    //VIF_STCYCL
/*
    kernsrc[0] = 0x01000101;
    kernsrc[1] = 0x22222222;
    kernsrc[2] = 0x33333333;
    kernsrc[3] = 0x44444444;
*/

    //pull statu reg
    // bitwise or to set bits 3 and 4
    uint32_t kernmask = 0x00000018;
    //uint32_t dma_addr = 0x00200000;
    uint32_t QWC = 1;

    uint32_t Dn_CHCR = 0x00000100;
    D0_MADR = 0x00200000;
   /* D0_QWC = 1;
    //*D0_CHCR = 0x00000100;
        *D0_CHCR =
      (1 << 0) |   // DIR
      (0 << 2) |   // MOD
      (0 << 4) |   // ASP
      (0 << 6) |   // TTE
      (0 << 7) |   // TIE
      (1 << 8);     // STR
    //volatile uint32_t* CHCR_addr= *D0_CHCR | Dn_CHCR;
    while (*D0_CHCR & 0x100) {
      data[0]++;
    }*/
    volatile int x = 0x12345;   // breakpoint here

  uint32_t vif_packet[5] = {
    0x6C010000,
    0xDEADBEEF,
    0x12345678,
    0xCAFEBABE,
    0x0BADF00D
};

    kernsrc[0] = vif_packet[0];
    kernsrc[1] = vif_packet[1];
    kernsrc[2] = vif_packet[2];
    kernsrc[3] = vif_packet[3];
    kernsrc[4] = vif_packet[4];
    kernsrc[5] = 0;
    kernsrc[6] = 0;
    kernsrc[7] = 0;

    D0_QWC = 2;
    // Start DMA.
    *D0_CHCR = 0x101;
      while (*D0_CHCR & 0x100) {
      data[0]++;
    }
  while(1)
 {
    num++;
    if(num2 < num) {
        num2++;
    }
    x++;
 }  

 // unpack cmd 6C01 0000
  //sleep(30);


 return 0;
}
