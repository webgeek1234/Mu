#include <stdint.h>
#include <stdbool.h>

#include "pxa255_DMA.h"
#include "pxa255_DSP.h"
#include "pxa255_GPIO.h"
#include "pxa255_IC.h"
#include "pxa255_LCD.h"
#include "pxa255_PwrClk.h"
#include "pxa255_RTC.h"
#include "pxa255_TIMR.h"
#include "pxa255_UART.h"
#include "../armv5te/cpu.h"
#include "../armv5te/emu.h"
#include "../armv5te/mem.h"
#include "../armv5te/os/os.h"
#include "../armv5te/translate.h"
#include "../tungstenCBus.h"
#include "../emulator.h"


uint16_t*        pxa255Framebuffer;
static Pxa255ic  tungstenCIc;
static Pxa255lcd tungstenCLcd;

//PXA255 register accessors
static uint8_t pxa255_read_byte(uint32_t addr){

}
static uint16_t pxa255_read_half(uint32_t addr){

}
static uint32_t pxa255_read_word(uint32_t addr){
   uint32_t out;

   switch(addr >> 16){
      case PXA255_LCD_BASE >> 16:
         pxa255lcdPrvMemAccessF(&tungstenCLcd, addr, 4, false, &out);
         return out;

      default:
         return 0x00000000;
   }
}
static void pxa255_write_byte(uint32_t addr, uint8_t value){

}
static void pxa255_write_half(uint32_t addr, uint16_t value){

}
static void pxa255_write_word(uint32_t addr, uint32_t value){
   switch(addr >> 16){
      case PXA255_LCD_BASE >> 16:
         pxa255lcdPrvMemAccessF(&tungstenCLcd, addr, 4, true, &value);
         return;

      default:
         return;
   }
}

bool pxa255Init(uint8_t** returnRom, uint8_t** returnRam){
   uint32_t mem_offset = 0;
   uint8_t i;

   mem_and_flags = os_reserve(MEM_MAXSIZE * 2);
   if(!mem_and_flags)
      return false;

   //ROM
   mem_areas[0].base = PXA255_ROM_START_ADDRESS;
   mem_areas[0].size = TUNGSTEN_C_ROM_SIZE;
   mem_areas[0].ptr = mem_and_flags + mem_offset;
   mem_offset += TUNGSTEN_C_ROM_SIZE;

   //RAM
   mem_areas[1].base = PXA255_RAM_START_ADDRESS;
   mem_areas[1].size = TUNGSTEN_C_RAM_SIZE;
   mem_areas[1].ptr = mem_and_flags + mem_offset;
   mem_offset += TUNGSTEN_C_RAM_SIZE;

   //CPU registers
   mem_areas[2].base = PXA255_REG_START_ADDRESS;
   mem_areas[2].size = PXA255_REG_SIZE;
   mem_areas[2].ptr = NULL;

   for(i = 0; i < 64; i++){
       // will fallback to bad_* on non-memory addresses
       read_byte_map[i] = memory_read_byte;
       read_half_map[i] = memory_read_half;
       read_word_map[i] = memory_read_word;
       write_byte_map[i] = memory_write_byte;
       write_half_map[i] = memory_write_half;
       write_word_map[i] = memory_write_word;
   }

   read_byte_map[PXA255_START_BANK(PXA255_REG_START_ADDRESS)] = pxa255_read_byte;
   read_half_map[PXA255_START_BANK(PXA255_REG_START_ADDRESS)] = pxa255_read_half;
   read_word_map[PXA255_START_BANK(PXA255_REG_START_ADDRESS)] = pxa255_read_word;
   write_byte_map[PXA255_START_BANK(PXA255_REG_START_ADDRESS)] = pxa255_write_byte;
   write_half_map[PXA255_START_BANK(PXA255_REG_START_ADDRESS)] = pxa255_write_half;
   write_word_map[PXA255_START_BANK(PXA255_REG_START_ADDRESS)] = pxa255_write_word;

   //set up CPU hardware
   pxa255icInit(&tungstenCIc);
   pxa255lcdInit(&tungstenCLcd, &tungstenCIc);

   *returnRom = mem_areas[0].ptr;
   *returnRam = mem_areas[1].ptr;

   return true;
}

void pxa255Deinit(void){
   if(mem_and_flags){
       // translation_table uses absolute addresses
       flush_translations();
       memset(mem_areas, 0, sizeof(mem_areas));
       os_free(mem_and_flags, MEM_MAXSIZE * 2);
       mem_and_flags = NULL;
   }
}

void pxa255Reset(void){
   /*
   static void emu_reset()
   {
       memset(mem_areas[1].ptr, 0, mem_areas[1].size);

       memset(&arm, 0, sizeof arm);
       arm.control = 0x00050078;
       arm.cpsr_low28 = MODE_SVC | 0xC0;
       cpu_events &= EVENT_DEBUG_STEP;

       sched_reset();
       sched.items[SCHED_THROTTLE].clock = CLOCK_27M;
       sched.items[SCHED_THROTTLE].proc = throttle_interval_event;

       memory_reset();
   }
   */

   memset(&arm, 0, sizeof arm);
   arm.control = 0x00050078;
   arm.cpsr_low28 = MODE_SVC | 0xC0;
   cpu_events = 0;
   //cpu_events &= EVENT_DEBUG_STEP;

   //PC starts at 0x00000000, the first opcode for Palm OS 5 is a jump
}

void pxa255SetRtc(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds){
   //TODO: make this do something
}

uint32_t pxa255StateSize(void){
   uint32_t size = 0;

   return size;
}

void pxa255SaveState(uint8_t* data){
   uint32_t offset = 0;

}

void pxa255LoadState(uint8_t* data){
   uint32_t offset = 0;

}

void pxa255Execute(void){
   //TODO: need to set cycle_count_delta with the amout of opcodes to run
   cycle_count_delta = -500;//just a test value

   // clang segfaults with that, for an iOS build :(
#ifndef NO_SETJMP
   // Workaround for LLVM bug #18974
   while(__builtin_setjmp(restart_after_exception)){};
#endif

   exiting = false;//exiting is never set to true, maybe I should remove it?
    while (!exiting && cycle_count_delta < 0) {
         if (cpu_events & (EVENT_FIQ | EVENT_IRQ)) {
             // Align PC in case the interrupt occurred immediately after a jump
             if (arm.cpsr_low28 & 0x20)
                 arm.reg[15] &= ~1;
             else
                 arm.reg[15] &= ~3;

             if (cpu_events & EVENT_WAITING)
                 arm.reg[15] += 4; // Skip over wait instruction

             arm.reg[15] += 4;
             cpu_exception((cpu_events & EVENT_FIQ) ? EX_FIQ : EX_IRQ);
         }
         cpu_events &= ~EVENT_WAITING;//this might need to be move above?

         if (arm.cpsr_low28 & 0x20)
             cpu_thumb_loop();
         else
             cpu_arm_loop();
    }

    //render
    pxa255lcdFrame(&tungstenCLcd);
}
