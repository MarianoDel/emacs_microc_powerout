# MicroCurrent version 1.0
micro STM32F103RCT6

Before begin check micro selection and configs.
-----------------------------------------------

* work area size y flash image
>stm32f1_flash.cfg

* work area size
>stm32f1_gdb.cfg

* work area size
>stm32f1_reset.cfg

* end of ram; stack size; memory lenght
>./startup_src/stm32_flash.ld

* check processor definition on line 68 -> but modify on Makefile
>./cmsis_boot/stm32f0xx.h
>.Makefile

