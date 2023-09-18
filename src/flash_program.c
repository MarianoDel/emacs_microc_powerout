//------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### FLASH_PROGRAM.C ################################
//------------------------------------------------------
#include "flash_program.h"
#include "stm32f10x.h"
#include "parameters.h"


// Private Types Constants and Macros ------------------------------------------
typedef enum
{ 
  FLASH_BUSY = 1,
  FLASH_ERROR_PG,
  FLASH_ERROR_WRP,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
} FLASH_Status;


/* Flash Access Control Register bits */
#define ACR_LATENCY_Mask         ((uint32_t)0x00000038)
#define ACR_HLFCYA_Mask          ((uint32_t)0xFFFFFFF7)
#define ACR_PRFTBE_Mask          ((uint32_t)0xFFFFFFEF)

/* Flash Access Control Register bits */
#define ACR_PRFTBS_Mask          ((uint32_t)0x00000020) 

/* Flash Control Register bits */
#define CR_PG_Set                ((uint32_t)0x00000001)
#define CR_PG_Reset              ((uint32_t)0x00001FFE) 
#define CR_PER_Set               ((uint32_t)0x00000002)
#define CR_PER_Reset             ((uint32_t)0x00001FFD)
#define CR_MER_Set               ((uint32_t)0x00000004)
#define CR_MER_Reset             ((uint32_t)0x00001FFB)
#define CR_OPTPG_Set             ((uint32_t)0x00000010)
#define CR_OPTPG_Reset           ((uint32_t)0x00001FEF)
#define CR_OPTER_Set             ((uint32_t)0x00000020)
#define CR_OPTER_Reset           ((uint32_t)0x00001FDF)
#define CR_STRT_Set              ((uint32_t)0x00000040)
#define CR_LOCK_Set              ((uint32_t)0x00000080)

/* FLASH Mask */
#define RDPRT_Mask               ((uint32_t)0x00000002)
#define WRP0_Mask                ((uint32_t)0x000000FF)
#define WRP1_Mask                ((uint32_t)0x0000FF00)
#define WRP2_Mask                ((uint32_t)0x00FF0000)
#define WRP3_Mask                ((uint32_t)0xFF000000)
#define OB_USER_BFB2             ((uint16_t)0x0008)

/* FLASH Keys */
#define RDP_Key                  ((uint16_t)0x00A5)
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)

/* FLASH BANK address */
#define FLASH_BANK1_END_ADDRESS   ((uint32_t)0x807FFFF)

/* Delay definition */   
#define EraseTimeout          ((uint32_t)0x000B0000)
#define ProgramTimeout        ((uint32_t)0x00002000)

#define FLASH_FLAG_BSY    ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP    ((uint32_t)0x00000020)  /*!< FLASH End of Operation flag */
#define FLASH_FLAG_PGERR    ((uint32_t)0x00000004)  /*!< FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR    ((uint32_t)0x00000010)  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR    ((uint32_t)0x00000001)  /*!< FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY    FLASH_FLAG_BSY       /*!< FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP    FLASH_FLAG_EOP       /*!< FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_PGERR    FLASH_FLAG_PGERR     /*!< FLASH BANK1 Program error flag */
#define FLASH_FLAG_BANK1_WRPRTERR    FLASH_FLAG_WRPRTERR  /*!< FLASH BANK1 Write protected error flag */
 

// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;

// Globals ---------------------------------------------------------------------
uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t Data = 0x3210ABCD;
uint32_t NbrOfPage = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;

#ifdef STM32F10X_XL
volatile TestStatus MemoryProgramStatus2 = PASSED;
#endif /* STM32F10X_XL */


// Module Private Functions ----------------------------------------------------
void Flash_ErasePage_WithLock (uint32_t p_addr, unsigned char with_lock);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
unsigned char Flash_WriteFlash(unsigned int * p, uint32_t addr, unsigned char with_lock, unsigned short words_len);
void FLASH_UnlockBank1(void);
void FLASH_LockBank1(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FLASH_Status FLASH_GetBank1Status(void);

// Module Functions ------------------------------------------------------------
unsigned char Flash_WriteConfigurations (void)
{
    unsigned char result = FAILED;

    __disable_irq();
    Flash_ErasePage_WithLock (FLASH_PAGE_FOR_BKP,0);

    unsigned int * p_origin = (unsigned int *) &mem_conf;
    result = Flash_WriteFlash (p_origin, FLASH_PAGE_FOR_BKP, 1, (sizeof(parameters_typedef)) / 4);
    __enable_irq();

    return result;
}


void Flash_ErasePage_WithLock (uint32_t p_addr, unsigned char with_lock)
{
    FLASH_UnlockBank1 ();
    FLASH_ErasePage(p_addr);
    
    if (with_lock)
        FLASH_LockBank1 ();
}


FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
    FLASH_Status status = FLASH_COMPLETE;
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(EraseTimeout);
  
    if(status == FLASH_COMPLETE)
    { 
        /* if the previous operation is completed, proceed to erase the page */
        FLASH->CR|= CR_PER_Set;
        FLASH->AR = Page_Address; 
        FLASH->CR|= CR_STRT_Set;
    
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(EraseTimeout);
    
        /* Disable the PER Bit */
        FLASH->CR &= CR_PER_Reset;
    }

    /* Return the Erase Status */
    return status;
}



unsigned char Flash_WriteFlash(unsigned int * p, uint32_t addr, unsigned char with_lock, unsigned short words_len)
{
    unsigned short i;

    for (i = 0; i < words_len; i++)
    {
        FLASH_ProgramWord(addr, *(p + i));
        addr += 4;
    }

    if (with_lock)
        FLASH_LockBank1();

    // verify saved memory
    unsigned int * p_verif = (unsigned int *) addr;

    for (i = 0; i < words_len; i++)
    {
        if (*(p_verif + i) != *(p + i))
            return FAILED;
    }
    
    return PASSED;
}


/**
  * @brief  Unlocks the FLASH Bank1 Program Erase Controller.
  * @note   This function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices this function unlocks Bank1.
  *         - For all other devices it unlocks Bank1 and it is 
  *           equivalent to FLASH_Unlock function.
  * @param  None
  * @retval None
  */
void FLASH_UnlockBank1(void)
{
  /* Authorize the FPEC of Bank1 Access */
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

/**
  * @brief  Clears the FLASH's pending flags.
  * @note   This function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices, this function clears Bank1 or Bank2’s pending flags
  *         - For other devices, it clears Bank1’s pending flags.
  * @param  FLASH_FLAG: specifies the FLASH flags to clear.
  *   This parameter can be any combination of the following values:         
  *     @arg FLASH_FLAG_PGERR: FLASH Program error flag       
  *     @arg FLASH_FLAG_WRPRTERR: FLASH Write protected error flag      
  *     @arg FLASH_FLAG_EOP: FLASH End of Operation flag           
  * @retval None
  */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
#ifdef STM32F10X_XL
  /* Check the parameters */
  assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG)) ;

  if((FLASH_FLAG & 0x80000000) != 0x0)
  {
    /* Clear the flags */
    FLASH->SR2 = FLASH_FLAG;
  }
  else
  {
    /* Clear the flags */
    FLASH->SR = FLASH_FLAG;
  }  

#else
  /* Check the parameters */
  // assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG)) ;
  
  /* Clear the flags */
  FLASH->SR = FLASH_FLAG;
#endif /* STM32F10X_XL */
}

    
/**
  * @brief  Locks the FLASH Bank1 Program Erase Controller.
  * @note   this function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices this function Locks Bank1.
  *         - For all other devices it Locks Bank1 and it is equivalent 
  *           to FLASH_Lock function.
  * @param  None
  * @retval None
  */
void FLASH_LockBank1(void)
{
  /* Set the Lock Bit to lock the FPEC and the CR of  Bank1 */
  FLASH->CR |= CR_LOCK_Set;
}

/**
  * @brief  Programs a word at a specified address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT. 
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  // assert_param(IS_FLASH_ADDRESS(Address));

#ifdef STM32F10X_XL
  if(Address < FLASH_BANK1_END_ADDRESS - 2)
  { 
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank1Operation(ProgramTimeout); 
    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR |= CR_PG_Set;
  
      *(__IO uint16_t*)Address = (uint16_t)Data;
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(ProgramTimeout);
 
      if(status == FLASH_COMPLETE)
      {
        /* if the previous operation is completed, proceed to program the new second 
        half word */
        tmp = Address + 2;

        *(__IO uint16_t*) tmp = Data >> 16;
    
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        
        /* Disable the PG Bit */
        FLASH->CR &= CR_PG_Reset;
      }
      else
      {
        /* Disable the PG Bit */
        FLASH->CR &= CR_PG_Reset;
       }
    }
  }
  else if(Address == (FLASH_BANK1_END_ADDRESS - 1))
  {
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank1Operation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR |= CR_PG_Set;
  
      *(__IO uint16_t*)Address = (uint16_t)Data;

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank1Operation(ProgramTimeout);
      
	  /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank2Operation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new second 
      half word */
      FLASH->CR2 |= CR_PG_Set;
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank2Operation(ProgramTimeout);
        
      /* Disable the PG Bit */
      FLASH->CR2 &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR2 &= CR_PG_Reset;
    }
  }
  else
  {
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank2Operation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR2 |= CR_PG_Set;
  
      *(__IO uint16_t*)Address = (uint16_t)Data;
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank2Operation(ProgramTimeout);
 
      if(status == FLASH_COMPLETE)
      {
        /* if the previous operation is completed, proceed to program the new second 
        half word */
        tmp = Address + 2;

        *(__IO uint16_t*) tmp = Data >> 16;
    
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastBank2Operation(ProgramTimeout);
        
        /* Disable the PG Bit */
        FLASH->CR2 &= CR_PG_Reset;
      }
      else
      {
        /* Disable the PG Bit */
        FLASH->CR2 &= CR_PG_Reset;
      }
    }
  }
#else
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(ProgramTimeout);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new first 
    half word */
    FLASH->CR |= CR_PG_Set;
  
    *(__IO uint16_t*)Address = (uint16_t)Data;
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(ProgramTimeout);
 
    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new second 
      half word */
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(ProgramTimeout);
        
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
  }         
#endif /* STM32F10X_XL */
   
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Waits for a Flash operation to complete or a TIMEOUT to occur.
  * @note   This function can be used for all STM32F10x devices, 
  *         it is equivalent to FLASH_WaitForLastBank1Operation.
  *         - For STM32F10X_XL devices this function waits for a Bank1 Flash operation
  *           to complete or a TIMEOUT to occur.
  *         - For all other devices it waits for a Flash operation to complete 
  *           or a TIMEOUT to occur.
  * @param  Timeout: FLASH programming Timeout
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
  FLASH_Status status = FLASH_COMPLETE;
   
  /* Check for the Flash Status */
  status = FLASH_GetBank1Status();
  /* Wait for a Flash operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = FLASH_GetBank1Status();
    Timeout--;
  }
  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}

/**
  * @brief  Returns the FLASH Bank1 Status.
  * @note   This function can be used for all STM32F10x devices, it is equivalent
  *         to FLASH_GetStatus function.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP or FLASH_COMPLETE
  */
FLASH_Status FLASH_GetBank1Status(void)
{
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_FLAG_BANK1_BSY) == FLASH_FLAG_BSY) 
  {
    flashstatus = FLASH_BUSY;
  }
  else 
  {  
    if((FLASH->SR & FLASH_FLAG_BANK1_PGERR) != 0)
    { 
      flashstatus = FLASH_ERROR_PG;
    }
    else 
    {
      if((FLASH->SR & FLASH_FLAG_BANK1_WRPRTERR) != 0 )
      {
        flashstatus = FLASH_ERROR_WRP;
      }
      else
      {
        flashstatus = FLASH_COMPLETE;
      }
    }
  }
  /* Return the Flash Status */
  return flashstatus;
}
