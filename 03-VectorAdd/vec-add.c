#include <stdlib.h>
#include <stdint.h>

/*
  should be a little tech demo to demonstrate
  1. sending two vectors of data to VU memory
    via VIF0 DMA.
  2. once sent operate on that memory (vector add)
  3. inspect the result or DMA it back to EE mem?
*/




//make some regiter write macros eventually

#define VIF0 0x10008000

#define D0_CHCR ((volatile uint32_t *)0x10008000)
#define D1_CHCR ((volatile uint32_t *)0x10009000)
#define D0_MADR (*(volatile uint32_t *)0x10008010)
#define D1_MADR (*(volatile uint32_t *)0x10009010)

#define D0_QWC  (*(volatile uint32_t *)0x10008020)
#define D1_QWC  (*(volatile uint32_t *)0x10009020)


int main(int argc, char *argv[])
{

  // these two vectors will be initially allocated in EE memory 
  uint32_t vec1[4] = {0x3F800000, 0x40000000, 0x40400000, 0x40800000};
  uint32_t vec2[4] = {0x41200000, 0x41A00000, 0x41F00000, 0x42200000};
  

  // abitraritly chosen address, but it lies within the kernel
  // address space of which we can get the physically addresses
  // to be used in DMA operation
  volatile uint32_t* kernsrc = (volatile uint32_t*)0xA0200000;


  //packet of data containing the DMA command for VIF0
  //the command conatins the unpack comd here and 2 vecs
  uint32_t vif_packet[9] = {
    0x6C020000,
    vec1[0],
    vec1[1],
    vec1[2],
    vec1[3],
    vec2[0],
    vec2[1],
    vec2[2],
    vec2[3]
  };


    //will need 3 quadwords
    kernsrc[0] = vif_packet[0];
    kernsrc[1] = vif_packet[1];
    kernsrc[2] = vif_packet[2];
    kernsrc[3] = vif_packet[3];
    kernsrc[4] = vif_packet[4];
    kernsrc[5] = vif_packet[5];
    kernsrc[6] = vif_packet[6];
    kernsrc[7] = vif_packet[7];
    kernsrc[8] = vif_packet[8];
    /*padding for alignment*/
    kernsrc[9] = 0;
    kernsrc[10] = 0;
    kernsrc[11] = 0;
    // 4 * 9 bytes (36) neet t be transfered, only fits in 3 qw 
    //quad word count
    D0_QWC = 3;

    //physical address the DMAC must look at for data source
    D0_MADR = 0x00200000;


    volatile int x = 0x12345;   // breakpoint here


    // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }


    //by this point both vectors should be in the VU mem
    //time to add

    //using the VU mirco mem, need to load the vectors into regs 
    //then add them

    //need the encoding of the machines instructions, and those get shipped to vu
      uint32_t instrutions[10];
      //load qw from 0x11004000
      /*
      LQ - 0000000 (this is the opcode)
      destination - 0000 / 1111 (full vector load)
      ft - 00000 / 00011 vf3 determines which of the 32 fp registers we're storing at ()
      base - 00000 / 00001 vI1(identifies the int/vec registers used at the base adderss)
      offset - 00000000000
      */

      // NOP: 01011111111 -> 000002FF
      instrutions[0] = 0x01E10000;//corresponds to a load instructions
      //0000000 1111 00001 00000 00000000000
      uint64_t instruction = ((uint64_t)0x000002FF << 32) | 0x01E10000;

      //write intruction to the VU0 micro mem
      //VU0 code memory resides at 0x11000000
      //should use the MPG command

      //0x4A080000 (this should transfer 1 intruction to the base of the VIMicromem)
      //leave num 0 to transfer 2048 bytes (FF *8?)
      uint32_t vif_inst_packet[8] = {
        0x4A010000,
        0x01E10000,
        0x000002FF,
        0x00000000
      };
      kernsrc[12] = vif_inst_packet[0];
      kernsrc[13] = vif_inst_packet[1];
      kernsrc[14] = vif_inst_packet[2];
      kernsrc[15] = 0;
          D0_MADR = (0x00200000+(12*4));
              D0_QWC = 1;


// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }

      //0x4A080000 (this should transfer 1 intruction to the base of the VIMicromem)
      //leave num 0 to transfer 2048 bytes (FF *8?)
      uint32_t vif_inst_packet2[8] = {
        0x4A010001, //offset of 1 * 8 bytes for second instruction
        0x01E20001, //loads the mem offset also 1*8 bytes
        0x000002FF,
        0x00000000
      };
      kernsrc[16] = vif_inst_packet2[0];
      kernsrc[17] = vif_inst_packet2[1];
      kernsrc[18] = vif_inst_packet2[2];
      kernsrc[19] = 0;
          D0_MADR = (0x00200000+(16*4));
              D0_QWC = 1;

// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }


      //NOPS
//0x4A080000 (this should transfer 1 intruction to the base of the VIMicromem)
      //leave num 0 to transfer 2048 bytes (FF *8?)
      uint32_t vif_inst_packet22[8] = {
        0x4A010002, //offset of 1 * 8 bytes for third instruction
        0x000002FF, //loads the mem offset also 1*8 bytes
        0x000002FF,
        0x00000000
      };
      kernsrc[20] = vif_inst_packet22[0];
      kernsrc[21] = vif_inst_packet22[1];
      kernsrc[22] = vif_inst_packet22[2];
      kernsrc[23] = 0;
          D0_MADR = (0x00200000+(20*4));
              D0_QWC = 1;

// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }

      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      kernsrc[20] += 1;
      // Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }

      //vector add instruction here dma it over

      //0x4A080000 (this should transfer 1 intruction to the base of the VIMicromem)
      //leave num 0 to transfer 2048 bytes (FF *8?)
      uint32_t vif_inst_packet3[8] = {
        0x4A01000C, //offset of 128 bytes for fourth instruction
        0x000002FF,
        0x01E208E8, //0000000 1111 00001 00010 00011 101000
        0x00000000
      };
      kernsrc[24] = vif_inst_packet3[0];
      kernsrc[25] = vif_inst_packet3[1];
      kernsrc[26] = vif_inst_packet3[2];
      kernsrc[27] = 0;
          D0_MADR = (0x00200000+(24*4));
              D0_QWC = 1;

              

// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }


 //nops
      kernsrc[20] += 2;

        D0_MADR = (0x00200000+(20*4));
              D0_QWC = 1;

// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }

      kernsrc[20] += 1;

        D0_MADR = (0x00200000+(20*4));
              D0_QWC = 1;

// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
           kernsrc[20] += 1;

        D0_MADR = (0x00200000+(20*4));
              D0_QWC = 1;

// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }
      //try some store quad word instructions to wrtie back to VU data mem
      //then see if i can inspect from EE
      // 0000001 1111 00011 00000 0000001000
      // 0000001 1111 00011 00011 00000000010
      uint32_t vif_inst_packet4[8] = {
        0x4A010010, //offset of 128 bytes for F-th instruction
        0x03E31802,
        0x000002FF, 
        0x00000000
      };
      kernsrc[28] = vif_inst_packet4[0];
      kernsrc[29] = vif_inst_packet4[1];
      kernsrc[30] = vif_inst_packet4[2];
      kernsrc[31] = 0;
          D0_MADR = (0x00200000+(28*4));
              D0_QWC = 1;


// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }


      //use the MSCAL here to start the VU micro program
      uint32_t mscal_cmd = 0x14000000;
      kernsrc[32] = mscal_cmd;
      D0_MADR = (0x00200000+(32*4));
        D0_QWC = 1;


// Start DMA.
    *D0_CHCR = 0x101;
    while (*D0_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }

/*
      // copy back to EE mem from VU data mem via channel 1 (vif1?)
      *D1_CHCR = 0x0100;
      //physical address the DMAC must write to
      D1_MADR = 0x00200000 + (36*4);
      D1_QWC = 1;

        while (*D1_CHCR & 0x100)
      {
        //loop for synchronization, ends when the STR bit its unset in CHCR register
      }*/
      //can just retrived the memory since its mapped and made accessible to the EE 
      int* retval = (int*)0x00200090;
      *retval = *((int*)0x11004020);
    while(1)
  {
      x++;
  }  

 return 0;
}
