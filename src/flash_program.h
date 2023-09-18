//------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### FLASH_PROGRAM.H ################################
//------------------------------------------------------
#ifndef __FLASH_PROGRAM_H_
#define __FLASH_PROGRAM_H_


//-- Configurations Defines --------------------
//--- Device ---------------
#define HIGH_DENSITY    //STM32F103RC
// #define MEDIUM_DENSITY    //STM32F103R8

//--- Backup Page ----------
#define FLASH_PAGE_FOR_BKP    PAGE126    // this page saves the configuration


//-- Internal Defines --------------------
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

// High Density from 256KB to 512KB, page size 2KB
// Medium Density from 64KB to 128KB, page size 1KB
#ifdef HIGH_DENSITY
#define FLASH_TOTAL_PAGES    128
#define FLASH_PAGE_SIZE    2048
#endif    //HIGH_DENSITY


#ifdef MEDIUM_DENSITY
#define FLASH_TOTAL_PAGES    64
#define FLASH_PAGE_SIZE    1024
#endif    //MEDIUM_DENSITY

// Most used Pages for Backup
#define PAGE62    (0x08000000 + FLASH_PAGE_SIZE * 62)
#define PAGE63    (0x08000000 + FLASH_PAGE_SIZE * 63)
#define PAGE126    (0x08000000 + FLASH_PAGE_SIZE * 126)
#define PAGE127    (0x08000000 + FLASH_PAGE_SIZE * 127)


//-- Sanity Checks -----------------------
#if (defined HIGH_DENSITY) && (defined MEDIUM_DENSITY)
#error "Select High or Medium Density Device"
#endif

#if (FLASH_PAGE_FOR_BKP > (0x08000000 + FLASH_PAGE_SIZE * (FLASH_TOTAL_PAGES - 1)))
#error "Flash Page selected too high"
#endif

#if (FLASH_PAGE_FOR_BKP <= (0x08000000))
#error "Flash Page selected it's wrong"
#endif


// Module Exported Functions ---------------------------------------------------
unsigned char Flash_WriteConfigurations (void);

#endif    /* __FLASH_PROGRAM_H_ */
