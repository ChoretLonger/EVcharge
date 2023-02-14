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


unsigned char rx_buffer[128] ;
unsigned char rx_count ;
unsigned char Tx_buffer[128] ;
unsigned char Tx_count ;

unsigned char dmadone ;

unsigned int overtime , maxmin ;

__IO uint16_t ad_value[256];

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
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

  /* connect port to USARTx_Rx */
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

  /* configure USART Tx as alternate function push-pull */
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

  /* configure USART Rx as alternate function push-pull */
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);

  /* USART configure */
  usart_deinit(USART0);
  usart_baudrate_set(USART0, 115200U);
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
        rx_buffer[rx_count++] = usart_data_receive(USART0);
        overtime = 0 ;
        //usart_data_transmit(USART0, usart_data_receive(USART0));
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
  //gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, IO595_oe|IO595_rst|IO595_clk|IO595_stb|IO595_dat);
  //gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, IO595_oe|IO595_rst|IO595_clk|IO595_stb|IO595_dat);
  /* configure IO GPIO port */
  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_4);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
  
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_0);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_1);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_2);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_3);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_4);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_5);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_6);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_7);
  
  //rcu_periph_clock_enable(RCU_GPIOF);
  //gpio_deinit(GPIOF);
  //gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1);
  //gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
  //
  //rcu_periph_clock_enable(RCU_GPIOB);
  //gpio_deinit(GPIOB);
  //gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
  //gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}

void AD_config(void)
{
  timer_oc_parameter_struct timer_ocintpara;
  timer_parameter_struct timer_initpara;

  dma_parameter_struct dma_init_struct;
  
  /* ADCCLK = PCLK2/6 */
  rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

  /* enable dma1 and gpioc clock */
  rcu_periph_clock_enable(RCU_DMA);

  /* enable adc1 clock */
  rcu_periph_clock_enable(RCU_ADC);

  /* enable timer1 clock */
  rcu_periph_clock_enable(RCU_TIMER1);

  /* ------------------------------- initialize DMA channel0 -------------------------------- */
  dma_deinit(DMA_CH0);
  dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_addr = (uint32_t)ad_value;
  dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
  dma_init_struct.number = 256;
  dma_init_struct.periph_addr = (uint32_t)&(ADC_RDATA);
  dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
  dma_init(DMA_CH0,&dma_init_struct);

  /* configure DMA mode */
  dma_circulation_enable(DMA_CH0);
  dma_memory_to_memory_disable(DMA_CH0);
  
  /* enable DMA channel0 interrupt */
  dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);
  nvic_irq_enable(DMA_Channel0_IRQn, 1, 1);
  /* enable DMA channel0 */
  dma_channel_enable(DMA_CH0);
  
  /*  ------------------------------- TIMER1 configuration  ------------------------------- */
  timer_deinit(TIMER1);
  timer_initpara.prescaler         = 500;
  timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;
  timer_initpara.period            = 199;
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER1,&timer_initpara);

  /* CH1 configuration in PWM mode1 */
  timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
  timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
  timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);

  timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
  timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
  timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

  /* auto-reload preload enable */
  timer_auto_reload_shadow_enable(TIMER1);
  
  /*  ------------------------------- ADC configuration  ------------------------------- */
  /* ADC channel length config */
  adc_channel_length_config(ADC_REGULAR_CHANNEL,8);

  /* ADC regular channel config */
  adc_regular_channel_config(0,ADC_CHANNEL_0,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(1,ADC_CHANNEL_1,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(2,ADC_CHANNEL_2,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(3,ADC_CHANNEL_3,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(4,ADC_CHANNEL_4,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(5,ADC_CHANNEL_5,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(6,ADC_CHANNEL_6,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(7,ADC_CHANNEL_7,ADC_SAMPLETIME_55POINT5);

  /* ADC external trigger enable */
  adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);
  /* ADC external trigger source config */
  adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_T1_CH1);
  /* ADC data alignment config */
  adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
  /* enable ADC interface */
  adc_enable();
  /* ADC calibration and reset calibration */
  adc_calibration_enable();
  /* ADC SCAN function enable */
  adc_special_function_config(ADC_SCAN_MODE,ENABLE);
  /* ADC DMA function enable */
  adc_dma_mode_enable();
  
  /* TIMER1 counter enable */
  //timer_enable(TIMER1);
}

void DMA_Channel0_IRQHandler(void)
{
  dmadone = 1 ;
  dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FTF);
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

    /* setup SysTick Timer for 1ms interrupts  */
    //systick_config();
    
    uart_init();
    
    usart_data_transmit(USART0, 0x00);
    
    overtime = 0 ;
    rx_count = 0 ;
    dmadone = 0 ;
    
    AD_config();
    
    while(1){
      //while(overtime < 1000) overtime ++ ;
      //usart_data_transmit(USART0, rx_count);
      //
      //if(rx_count > 10) IO595_clk_L ;
      //else IO595_clk_H ;
      //rx_count = 0 ;
      //while(overtime > 999) ;
      timer_enable(TIMER1);
      //while( !dma_flag_get(DMA_CH0,DMA_FLAG_FTF ));
      //
      ///* clear channel1 transfer complete flag */
      //dma_flag_clear(DMA_CH0,DMA_FLAG_FTF ); 
      while(dmadone == 0) ;
      dmadone = 0 ;
      timer_disable(TIMER1);
      for(rx_count = 0;rx_count < 255;rx_count ++)
      {
        usart_data_transmit(USART0, ad_value[rx_count]>>8);
        while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
        usart_data_transmit(USART0, ad_value[rx_count]&0xff);
        while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
      }
      usart_data_transmit(USART0, ad_value[255]>>8);
      while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
      usart_data_transmit(USART0, ad_value[255]&0xff);
      while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
    }
}
