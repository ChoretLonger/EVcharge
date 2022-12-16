/*!
    \file    main.c
    \brief   GPIO running LED demo

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
    \version 2021-12-31, V3.1.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f1x0.h"
#include "systick.h"

#define UART_OUT_TIME 50000

#define   IO595_oe   GPIO_PIN_6
#define   IO595_rst  GPIO_PIN_1
#define   IO595_clk  GPIO_PIN_4
#define   IO595_stb  GPIO_PIN_5
#define   IO595_dat  GPIO_PIN_7

#define   IO595_oe_H    GPIO_BOP(GPIOA)=IO595_oe 
#define   IO595_oe_L    GPIO_BC(GPIOA)=IO595_oe 
#define   IO595_rst_H   GPIO_BOP(GPIOA)=IO595_rst 
#define   IO595_rst_L   GPIO_BC(GPIOA)=IO595_rst 
#define   IO595_clk_H   GPIO_BOP(GPIOA)=IO595_clk 
#define   IO595_clk_L   GPIO_BC(GPIOA)=IO595_clk
#define   IO595_stb_H   GPIO_BOP(GPIOA)=IO595_stb 
#define   IO595_stb_L   GPIO_BC(GPIOA)=IO595_stb
#define   IO595_dat_H   GPIO_BOP(GPIOA)=IO595_dat 
#define   IO595_dat_L   GPIO_BC(GPIOA)=IO595_dat

#define   Prashort_H    GPIO_BOP(GPIOA)=GPIO_PIN_0 
#define   Prashort_L    GPIO_BC(GPIOA)=GPIO_PIN_0
#define   Nshort_H    GPIO_BOP(GPIOA)=GPIO_PIN_9 
#define   Nshort_L    GPIO_BC(GPIOA)=GPIO_PIN_9
#define   Pshort_H    GPIO_BOP(GPIOA)=GPIO_PIN_10 
#define   Pshort_L    GPIO_BC(GPIOA)=GPIO_PIN_10

unsigned int u_timeout ;
uint32_t EPR2set , ESR2set , EPC2set , pul2set ;
uint16_t plug_sta , plug_stashift ;
unsigned char datin_buf[30] ;
unsigned char buf_add ;
unsigned char board_id ;

unsigned char pullup_en , pulldown_en , matched ;

unsigned char unicnt , openloadcnt , shortloadcnt , shortgndcnt ;

void RCC_Configuration(void)
{
  //ErrStatus ErrStatusFlag;
  rcu_deinit();
  rcu_osci_on(RCU_IRC8M);
  rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
  //while(ErrStatusFlag != SUCCESS)
  //{
  //        ErrStatusFlag = rcu_osci_stab_wait(RCU_IRC8M);
  //        rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
  //        rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
  //        rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
  //        rcu_pll_config(RCU_PLLSRC_IRC8M_DIV2,RCU_PLL_MUL18);
  //        rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
  //        RCU_CTL0 |= RCU_CTL0_PLLEN;
  //}
}

void uart_init(void)
{
  nvic_irq_enable(USART0_IRQn, 0, 1);
  
  rcu_periph_clock_enable(RCU_GPIOA);

  /* enable USART clock */
  rcu_periph_clock_enable(RCU_USART0);

  /* connect port to USARTx_Tx */
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);

  /* connect port to USARTx_Rx */
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);

  /* configure USART Tx as alternate function push-pull */
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

  /* configure USART Rx as alternate function push-pull */
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

  /* USART configure */
  usart_deinit(USART0);
  usart_baudrate_set(USART0, 9600U);
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
  usart_receive_config(USART0, USART_RECEIVE_ENABLE);
  usart_enable(USART0);
  
  /* enable USART TBE interrupt */
  //usart_interrupt_enable(USART0, USART_INT_TBE);
  /* enable USART RBNE interrupt */
  usart_interrupt_enable(USART0, USART_INT_RBNE);
}

void USART0_IRQHandler(void)
{
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* receive data */
        //rx_buffer[rx_count++] = usart_data_receive(USART0);
        usart_data_transmit(USART0, usart_data_receive(USART0));
        //datin_buf[buf_add] = usart_data_receive(USART0) ;
        //buf_add ++ ;
        //u_timeout = 0 ;
        //if (rx_count >= rx_buffer_size){
        //    usart_interrupt_disable(USART0, USART_INT_RBNE);
        //}
    }

    //if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)){
    //    /* transmit data */
    //    usart_data_transmit(USART0, tx_buffer[tx_count++]);
    //    if (tx_count >= tx_buffer_size){
    //        usart_interrupt_disable(USART0, USART_INT_TBE);
    //    }
    //}
}

void IOpin_init(void)
{
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_deinit(GPIOA);
  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, IO595_oe|IO595_rst|IO595_clk|IO595_stb|IO595_dat);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, IO595_oe|IO595_rst|IO595_clk|IO595_stb|IO595_dat);
  /* configure IO GPIO port */
  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_0 | GPIO_PIN_9 | GPIO_PIN_10);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1 | GPIO_PIN_4);
  
  rcu_periph_clock_enable(RCU_GPIOF);
  gpio_deinit(GPIOF);
  gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
  
  rcu_periph_clock_enable(RCU_GPIOB);
  gpio_deinit(GPIOB);
  gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}

void set_IO(uint16_t out,unsigned char bitcnt)
{
  uint8_t bit595_cnt ;
  for(bit595_cnt = bitcnt;bit595_cnt;bit595_cnt --)
  {
    if(out & 0x8000) { 
      IO595_dat_L ; 
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_H ;
      IO595_clk_H ;
      IO595_clk_L ;
      IO595_dat_H ; 
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_H ;
      IO595_clk_H ;
      IO595_clk_L ;
    }
    else{
      IO595_dat_H ; 
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_H ;
      IO595_clk_H ;
      IO595_clk_L ;
      IO595_dat_L ; 
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_L ;
      IO595_clk_H ;
      IO595_clk_H ;
      IO595_clk_L ;
    }
    out = out << 1 ;
  }
  //IO595_stb_H ;
  //IO595_stb_H ;
  //IO595_stb_H ;
  //IO595_stb_H ;
  //IO595_stb_L ;
}

void core_state_update(uint32_t ESR_P_ohm,
                       uint32_t ESR_N_ohm,
                       uint32_t EPR_ohm,
                       uint32_t pullup_P_ohm,
                       uint32_t pulldown_N_ohm,
                       uint32_t EPC_pf)
{
  ESR_P_ohm      = ESR_P_ohm / 5 ;
  ESR_N_ohm      = ESR_N_ohm / 5 ;
  EPR_ohm        = EPR_ohm / 5 ;
  pullup_P_ohm   = pullup_P_ohm / 5 ;
  pulldown_N_ohm = pulldown_N_ohm / 5 ;
  EPC_pf         = EPC_pf / 10 ;
  
  EPR_ohm = EPR_ohm << 4 ;
  set_IO(EPR_ohm,12);
  
  ESR_P_ohm = ESR_P_ohm << 4 ;
  set_IO(ESR_P_ohm,12);
  
  ESR_N_ohm = ESR_N_ohm << 4 ;
  set_IO(ESR_N_ohm,12);
  
  EPC_pf = EPC_pf << 4 ;  // due to hardware , high 8 bit EPC
  set_IO(EPC_pf,8);
  
  pulldown_N_ohm = pulldown_N_ohm << 4 ;
  if(pulldown_en) pulldown_N_ohm |= 0x8000 ;
  else            pulldown_N_ohm &= 0x7fff ;
  set_IO(pulldown_N_ohm,12);
  
  pullup_P_ohm = pullup_P_ohm << 4 ;
  if(pullup_en) pullup_P_ohm |= 0x8000 ;
  else            pullup_P_ohm &= 0x7fff ;
  set_IO(pullup_P_ohm,12);
  
  EPC_pf = EPC_pf << 8 ;  // then low 4 bit
  set_IO(EPC_pf,4);
  
  IO595_stb_H ;
  IO595_stb_H ;
  IO595_stb_H ;
  IO595_stb_H ;
  IO595_stb_L ;
  
  IO595_oe_L ;
  delay_1ms(12);
  IO595_oe_H ;
}

void plug_state_update(uint16_t setplug)
{
  set_IO(setplug,12);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    
    RCC_Configuration();
      
    IOpin_init();

    gpio_bit_reset(GPIOA, GPIO_PIN_1 | GPIO_PIN_4);

    /* setup SysTick Timer for 1ms interrupts  */
    systick_config();
    
    uart_init();
    
    IO595_oe_L ;
    IO595_rst_H;
    IO595_clk_L;
    IO595_stb_L;
    IO595_dat_L;
    
    board_id = 0 ;
    if(SET ==  gpio_input_bit_get(GPIOF, GPIO_PIN_0)) board_id += 1 ;
    if(SET ==  gpio_input_bit_get(GPIOF, GPIO_PIN_1)) board_id += 2 ;
    if(SET ==  gpio_input_bit_get(GPIOB, GPIO_PIN_1)) board_id += 4 ;
    usart_data_transmit(USART0, board_id);
    
    if(board_id == 7){ // if is plug , set PB1 output
      gpio_deinit(GPIOB);
      gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
      gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    }
    
    pullup_en   = 0 ;
    pulldown_en = 0 ;
    
    delay_1ms(12);
    IO595_oe_H ;
    
    
    Prashort_L ;
    Pshort_L ;
    Nshort_L ;

    while(1){
    }
}
