; ////////////////////////////////////////////////////////////////////////////////
; /// @file     startup_mm32f327x_keil.s
; /// @author   AE TEAM
; /// @brief    THIS FILE PROVIDES ALL THE Device Startup File of MM32 Cortex-M
; ///           Core Device for ARM KEIL toolchain.
; ////////////////////////////////////////////////////////////////////////////////
; /// @attention
; ///
; /// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
; /// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
; /// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
; /// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
; /// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
; /// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
; ///
; /// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
; //////////////////////////////////////////////////////////////////////////////
;
; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
;Heap_Size       EQU     0x00000200
Heap_Size       EQU     0x00002000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                                            ;         Top of Stack
                DCD     Reset_Handler                                           ;         Reset Handler
                DCD     _Z11NMI_Handlerv                                             ; -14     NMI Handler
                DCD     _Z17HardFault_Handlerv                                       ; -13     Hard Fault Handler
                DCD     _Z17MemManage_Handlerv                                       ; -12     MPU Fault Handler
                DCD     _Z16BusFault_Handlerv                                        ; -11     Bus Fault Handler
                DCD     _Z18UsageFault_Handlerv                                      ; -10     Usage Fault Handler
__vector_table_0x1c
                DCD     0                                                       ; -9      Reserved
                DCD     0                                                       ; -8      Reserved
                DCD     0                                                       ; -7      Reserved
                DCD     0                                                       ; -6      Reserved
                DCD     _Z11SVC_Handlerv                                             ; -5      SVCall Handler
                DCD     _Z16DebugMon_Handlerv                                        ; -4      Debug Monitor Handler
                DCD     0                                                       ; -3      Reserved
                DCD     _Z14PendSV_Handlerv                                          ; -2      PendSV Handler
                DCD     _Z15SysTick_Handlerv                                         ; -1      SysTick Handler  ; External Interrupts     
                DCD     _Z15WWDG_IRQHandlerv                                         ; 0       Window Watchdog
                DCD     _Z14PVD_IRQHandlerv                                          ; 1       PVD through EXTI Line detect
                DCD     _Z17TAMPER_IRQHandlerv                                       ; 2       Tamper
                DCD     _Z14RTC_IRQHandlerv                                          ; 3       RTC
                DCD     _Z16FLASH_IRQHandlerv                                        ; 4       Flash
                DCD     _Z18RCC_CRS_IRQHandlerv                                      ; 5       RCC
                DCD     _Z16EXTI0_IRQHandlerv                                        ; 6       EXTI Line 0
                DCD     _Z16EXTI1_IRQHandlerv                                        ; 7       EXTI Line 1
                DCD     _Z16EXTI2_IRQHandlerv                                        ; 8       EXTI Line 2
                DCD     _Z16EXTI3_IRQHandlerv                                        ; 9       EXTI Line 3
                DCD     _Z16EXTI4_IRQHandlerv                                        ; 10      EXTI Line 4
                DCD     _Z24DMA1_Channel1_IRQHandlerv                                ; 11      DMA1 Channel 1
                DCD     _Z24DMA1_Channel2_IRQHandlerv                                ; 12      DMA1 Channel 2
                DCD     _Z24DMA1_Channel3_IRQHandlerv                                ; 13      DMA1 Channel 3
                DCD     _Z24DMA1_Channel4_IRQHandlerv                                ; 14      DMA1 Channel 4
                DCD     _Z24DMA1_Channel5_IRQHandlerv                                ; 15      DMA1 Channel 5
                DCD     _Z24DMA1_Channel6_IRQHandlerv                                ; 16      DMA1 Channel 6
                DCD     _Z24DMA1_Channel7_IRQHandlerv                                ; 17      DMA1 Channel 7
                DCD     _Z17ADC1_2_IRQHandlerv                                       ; 18      ADC1 and ADC2
                DCD     _Z21FlashCache_IRQHandlerv                                   ; 19      FlashCache outage
                DCD     0                                                       ; 20      Reserved
                DCD     _Z18CAN1_RX_IRQHandlerv                                      ; 21      CAN1_RX
                DCD     0                                                       ; 22      Reserved
                DCD     _Z18EXTI9_5_IRQHandlerv                                      ; 23      EXTI Line 9..5
                DCD     _Z19TIM1_BRK_IRQHandlerv                                     ; 24      TIM1 Break
                DCD     _Z18TIM1_UP_IRQHandlerv                                      ; 25      TIM1 Update
                DCD     _Z23TIM1_TRG_COM_IRQHandlerv                                 ; 26      TIM1 Trigger and Commutation
                DCD     _Z18TIM1_CC_IRQHandlerv                                      ; 27      TIM1 Capture Compare
                DCD     _Z15TIM2_IRQHandlerv                                         ; 28      TIM2
                DCD     _Z15TIM3_IRQHandlerv                                         ; 29      TIM3
                DCD     _Z15TIM4_IRQHandlerv                                         ; 30      TIM4
                DCD     _Z15I2C1_IRQHandlerv                                         ; 31      I2C1 Event
                DCD     0                                                       ; 32      Reserved
                DCD     _Z15I2C2_IRQHandlerv                                         ; 33      I2C2 Event
                DCD     0                                                       ; 34      Reserved
                DCD     _Z15SPI1_IRQHandlerv                                         ; 35      SPI1
                DCD     _Z15SPI2_IRQHandlerv                                         ; 36      SPI2
                DCD     _Z16UART1_IRQHandlerv                                        ; 37      UART1
                DCD     _Z16UART2_IRQHandlerv                                        ; 38      UART2
                DCD     _Z16UART3_IRQHandlerv                                        ; 39      UART3
                DCD     _Z20EXTI15_10_IRQHandlerv                                    ; 40      EXTI Line 15..10
                DCD     _Z19RTCAlarm_IRQHandlerv                                     ; 41      RTC Alarm through EXTI Line 17
                DCD     _Z22OTG_FS_WKUP_IRQHandlerv                                  ; 42      USB OTG FS Wakeup through EXTI line
                DCD     _Z19TIM8_BRK_IRQHandlerv                                     ; 43      TIM8 Break
                DCD     _Z18TIM8_UP_IRQHandlerv                                      ; 44      TIM8 Update
                DCD     _Z23TIM8_TRG_COM_IRQHandlerv                                 ; 45      TIM8 Trigger and Commutation
                DCD     _Z18TIM8_CC_IRQHandlerv                                      ; 46      TIM8 Capture Compare
                DCD     _Z15ADC3_IRQHandlerv                                         ; 47      ADC3
                DCD     0                                                       ; 48      Reserved
                DCD     _Z15SDIO_IRQHandlerv                                         ; 49      SDIO
                DCD     _Z15TIM5_IRQHandlerv                                         ; 50      TIM5
                DCD     _Z15SPI3_IRQHandlerv                                         ; 51      SPI3
                DCD     _Z16UART4_IRQHandlerv                                        ; 52      UART4
                DCD     _Z16UART5_IRQHandlerv                                        ; 53      UART5
                DCD     _Z15TIM6_IRQHandlerv                                         ; 54      TIM6
                DCD     _Z15TIM7_IRQHandlerv                                         ; 55      TIM7
                DCD     _Z24DMA2_Channel1_IRQHandlerv                                ; 56      DMA2 Channel 1
                DCD     _Z24DMA2_Channel2_IRQHandlerv                                ; 57      DMA2 Channel 2
                DCD     _Z24DMA2_Channel3_IRQHandlerv                                ; 58      DMA2 Channel 3
                DCD     _Z24DMA2_Channel4_IRQHandlerv                                ; 59      DMA2 Channel 4
                DCD     _Z24DMA2_Channel5_IRQHandlerv                                ; 60      DMA2 Channel 5
                DCD     _Z14ETH_IRQHandlerv                                          ; 61      Ethernet
                DCD     0                                                       ; 62      Reserved
                DCD     0                                                       ; 63      Reserved
                DCD     _Z18COMP1_2_IRQHandlerv                                      ; 64      COMP1,COMP2
                DCD     0                                                       ; 65      Reserved
                DCD     0                                                       ; 66      Reserved
                DCD     _Z17OTG_FS_IRQHandlerv                                       ; 67      USB OTG_FullSpeed
                DCD     0                                                       ; 68      Reserved
                DCD     0                                                       ; 69      Reserved
                DCD     0                                                       ; 70      Reserved
                DCD     _Z16UART6_IRQHandlerv                                        ; 71      UART6  
                DCD     0                                                       ; 72      Reserved
                DCD     0                                                       ; 73      Reserved
                DCD     0                                                       ; 74      Reserved
                DCD     0                                                       ; 75      Reserved
                DCD     0                                                       ; 76      Reserved
                DCD     0                                                       ; 77      Reserved
                DCD     0                                                       ; 78      Reserved
                DCD     0                                                       ; 79      Reserved
                DCD     0                                                       ; 80      Reserved
                DCD     0                                                       ; 81      Reserved
                DCD     _Z16UART7_IRQHandlerv                                        ; 82      UART7
                DCD     _Z16UART8_IRQHandlerv                                        ; 83      UART8
                                
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler              [WEAK]
                IMPORT  __main
                IMPORT  _Z10SystemInitv
                LDR     R0, =_Z10SystemInitv
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

_Z11NMI_Handlerv     PROC
                EXPORT  _Z11NMI_Handlerv                [WEAK]
                B       .
                ENDP
_Z17HardFault_Handlerv\
                PROC
                EXPORT  _Z17HardFault_Handlerv          [WEAK]
                B       .
                ENDP
_Z17MemManage_Handlerv\
                PROC
                EXPORT  _Z17MemManage_Handlerv          [WEAK]
                B       .
                ENDP
_Z16BusFault_Handlerv\
                PROC
                EXPORT  _Z16BusFault_Handlerv           [WEAK]
                B       .
                ENDP
_Z18UsageFault_Handlerv\
                PROC
                EXPORT  _Z18UsageFault_Handlerv         [WEAK]
                B       .
                ENDP
_Z11SVC_Handlerv     PROC
                EXPORT  _Z11SVC_Handlerv                [WEAK]
                B       .
                ENDP
_Z16DebugMon_Handlerv\
                PROC
                EXPORT  _Z16DebugMon_Handlerv           [WEAK]
                B       .
                ENDP
_Z14PendSV_Handlerv  PROC
                EXPORT  _Z14PendSV_Handlerv             [WEAK]
                B       .
                ENDP
_Z15SysTick_Handlerv PROC
                EXPORT  _Z15SysTick_Handlerv            [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  _Z15WWDG_IRQHandlerv            [WEAK]
                EXPORT  _Z14PVD_IRQHandlerv             [WEAK]
                EXPORT  _Z17TAMPER_IRQHandlerv          [WEAK]
                EXPORT  _Z14RTC_IRQHandlerv             [WEAK]
                EXPORT  _Z16FLASH_IRQHandlerv           [WEAK]
                EXPORT  _Z18RCC_CRS_IRQHandlerv         [WEAK]
                EXPORT  _Z16EXTI0_IRQHandlerv           [WEAK]
                EXPORT  _Z16EXTI1_IRQHandlerv           [WEAK]
                EXPORT  _Z16EXTI2_IRQHandlerv           [WEAK]
                EXPORT  _Z16EXTI3_IRQHandlerv           [WEAK]
                EXPORT  _Z16EXTI4_IRQHandlerv           [WEAK]
                EXPORT  _Z24DMA1_Channel1_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA1_Channel2_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA1_Channel3_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA1_Channel4_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA1_Channel5_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA1_Channel6_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA1_Channel7_IRQHandlerv   [WEAK]
                EXPORT  _Z17ADC1_2_IRQHandlerv          [WEAK]
                EXPORT  _Z21FlashCache_IRQHandlerv      [WEAK]
                EXPORT  _Z18CAN1_RX_IRQHandlerv         [WEAK]
                EXPORT  _Z18EXTI9_5_IRQHandlerv         [WEAK]
                EXPORT  _Z19TIM1_BRK_IRQHandlerv        [WEAK]
                EXPORT  _Z18TIM1_UP_IRQHandlerv         [WEAK]
                EXPORT  _Z23TIM1_TRG_COM_IRQHandlerv    [WEAK]
                EXPORT  _Z18TIM1_CC_IRQHandlerv         [WEAK]
                EXPORT  _Z15TIM2_IRQHandlerv            [WEAK]
                EXPORT  _Z15TIM3_IRQHandlerv            [WEAK]
                EXPORT  _Z15TIM4_IRQHandlerv            [WEAK]
                EXPORT  _Z15I2C1_IRQHandlerv            [WEAK]
                EXPORT  _Z15I2C2_IRQHandlerv            [WEAK]
                EXPORT  _Z15SPI1_IRQHandlerv            [WEAK]
                EXPORT  _Z15SPI2_IRQHandlerv            [WEAK]
                EXPORT  _Z16UART1_IRQHandlerv           [WEAK]
                EXPORT  _Z16UART2_IRQHandlerv           [WEAK]
                EXPORT  _Z16UART3_IRQHandlerv           [WEAK]
                EXPORT  _Z20EXTI15_10_IRQHandlerv       [WEAK]
                EXPORT  _Z19RTCAlarm_IRQHandlerv        [WEAK]
                EXPORT  _Z22OTG_FS_WKUP_IRQHandlerv     [WEAK]
                EXPORT  _Z19TIM8_BRK_IRQHandlerv        [WEAK]
                EXPORT  _Z18TIM8_UP_IRQHandlerv         [WEAK]
                EXPORT  _Z23TIM8_TRG_COM_IRQHandlerv    [WEAK]
                EXPORT  _Z18TIM8_CC_IRQHandlerv         [WEAK]
                EXPORT  _Z15ADC3_IRQHandlerv            [WEAK]
                EXPORT  _Z15SDIO_IRQHandlerv            [WEAK]
                EXPORT  _Z15TIM5_IRQHandlerv            [WEAK]
                EXPORT  _Z15SPI3_IRQHandlerv            [WEAK]
                EXPORT  _Z16UART4_IRQHandlerv           [WEAK]
                EXPORT  _Z16UART5_IRQHandlerv           [WEAK]
                EXPORT  _Z15TIM6_IRQHandlerv            [WEAK]
                EXPORT  _Z15TIM7_IRQHandlerv            [WEAK]
                EXPORT  _Z24DMA2_Channel1_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA2_Channel2_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA2_Channel3_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA2_Channel4_IRQHandlerv   [WEAK]
                EXPORT  _Z24DMA2_Channel5_IRQHandlerv   [WEAK]
                EXPORT  _Z14ETH_IRQHandlerv             [WEAK]
                EXPORT  _Z18COMP1_2_IRQHandlerv         [WEAK]
                EXPORT  _Z17OTG_FS_IRQHandlerv          [WEAK]
                EXPORT  _Z16UART6_IRQHandlerv           [WEAK]
                EXPORT  _Z16UART7_IRQHandlerv           [WEAK]
                EXPORT  _Z16UART8_IRQHandlerv           [WEAK]
                
_Z15WWDG_IRQHandlerv           
_Z14PVD_IRQHandlerv            
_Z17TAMPER_IRQHandlerv         
_Z14RTC_IRQHandlerv            
_Z16FLASH_IRQHandlerv          
_Z18RCC_CRS_IRQHandlerv        
_Z16EXTI0_IRQHandlerv          
_Z16EXTI1_IRQHandlerv          
_Z16EXTI2_IRQHandlerv          
_Z16EXTI3_IRQHandlerv          
_Z16EXTI4_IRQHandlerv          
_Z24DMA1_Channel1_IRQHandlerv  
_Z24DMA1_Channel2_IRQHandlerv  
_Z24DMA1_Channel3_IRQHandlerv  
_Z24DMA1_Channel4_IRQHandlerv  
_Z24DMA1_Channel5_IRQHandlerv  
_Z24DMA1_Channel6_IRQHandlerv  
_Z24DMA1_Channel7_IRQHandlerv  
_Z17ADC1_2_IRQHandlerv           
_Z21FlashCache_IRQHandlerv     
_Z18CAN1_RX_IRQHandlerv        
_Z18EXTI9_5_IRQHandlerv        
_Z19TIM1_BRK_IRQHandlerv       
_Z18TIM1_UP_IRQHandlerv        
_Z23TIM1_TRG_COM_IRQHandlerv   
_Z18TIM1_CC_IRQHandlerv        
_Z15TIM2_IRQHandlerv           
_Z15TIM3_IRQHandlerv           
_Z15TIM4_IRQHandlerv           
_Z15I2C1_IRQHandlerv           
_Z15I2C2_IRQHandlerv           
_Z15SPI1_IRQHandlerv           
_Z15SPI2_IRQHandlerv           
_Z16UART1_IRQHandlerv          
_Z16UART2_IRQHandlerv          
_Z16UART3_IRQHandlerv          
_Z20EXTI15_10_IRQHandlerv      
_Z19RTCAlarm_IRQHandlerv       
_Z22OTG_FS_WKUP_IRQHandlerv       
_Z19TIM8_BRK_IRQHandlerv       
_Z18TIM8_UP_IRQHandlerv        
_Z23TIM8_TRG_COM_IRQHandlerv   
_Z18TIM8_CC_IRQHandlerv        
_Z15ADC3_IRQHandlerv           
_Z15SDIO_IRQHandlerv           
_Z15TIM5_IRQHandlerv           
_Z15SPI3_IRQHandlerv           
_Z16UART4_IRQHandlerv          
_Z16UART5_IRQHandlerv          
_Z15TIM6_IRQHandlerv           
_Z15TIM7_IRQHandlerv           
_Z24DMA2_Channel1_IRQHandlerv  
_Z24DMA2_Channel2_IRQHandlerv  
_Z24DMA2_Channel3_IRQHandlerv  
_Z24DMA2_Channel4_IRQHandlerv  
_Z24DMA2_Channel5_IRQHandlerv  
_Z14ETH_IRQHandlerv     
_Z18COMP1_2_IRQHandlerv        
_Z17OTG_FS_IRQHandlerv     
_Z16UART6_IRQHandlerv          
_Z16UART7_IRQHandlerv          
_Z16UART8_IRQHandlerv          

                             
                B       .    
                             
                ENDP         
                             
                ALIGN        
                             
;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                IF      :DEF:__MICROLIB           
                            
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                            
                ELSE        
                            
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
                            
__user_initial_stackheap     
                             
                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, =  Stack_Mem
                BX      LR  
                            
                ALIGN       
                            
                ENDIF       
                            
                END         
                        