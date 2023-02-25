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

static const unsigned int crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

unsigned char recbuf[128] ;
unsigned char rx_count ;
unsigned char sendbuf[128] ;
unsigned char Tx_count ;

unsigned char dmadone , bufp ;

unsigned int overtime , maxmin ;

unsigned char flen , cr3c,cr2c,cr1c,cr0c ;

uint32_t crc32val ;

uint64_t time ;

unsigned char echo_req[4] = {0xd0,0xee,0x01,0x00} ;
unsigned char prepare_login[4] = {0xd0,0xee,0x02,0x00} ;
unsigned char login[4] = {0xd0,0xee,0x03,0x00} ;
unsigned char get_charging_info[4] = {0xd0,0xee,0x04,0x00} ;
unsigned char report_charging_info[4] = {0xd0,0xee,0x05,0x00} ;

unsigned char device_ID[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07} ;
unsigned char rannum[8] = {0x0f,0xf0,0x01,0x10,0x0e,0xe0,0x02,0x20} ;
unsigned char soft_ver[4] = {0x01,0x02,0x03,0x04} ;
unsigned char hard_ver[4] = {0x40,0x30,0x20,0x10} ;

unsigned char token[16] ;

__IO uint16_t ad_value[256];

#pragma pack(1)
struct status {
  uint8_t length;
  uint8_t type;
  uint16_t apitype;
  uint16_t apimethod;
  uint8_t token[16];
  uint64_t timestamp;
  uint64_t power;
  uint32_t status;
  uint32_t voltage;
  uint32_t current;
};


struct charging_respond {
  uint32_t crc;
  uint8_t length;
  uint8_t type;
  uint16_t apitype;
  uint16_t apimethod;
  uint32_t err_code;
  uint8_t token[16];
  //uint64_t timestamp;
  uint8_t charging;
};
#pragma pack()

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
        recbuf[rx_count++] = usart_data_receive(USART0);
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
  //gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_2);
  //gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_3);
  //gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_4);
  //gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_5);
  //gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_6);
  //gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_7);
  
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
  adc_channel_length_config(ADC_REGULAR_CHANNEL,2);

  /* ADC regular channel config */
  adc_regular_channel_config(0,ADC_CHANNEL_0,ADC_SAMPLETIME_55POINT5);
  adc_regular_channel_config(1,ADC_CHANNEL_1,ADC_SAMPLETIME_55POINT5);
  //adc_regular_channel_config(2,ADC_CHANNEL_2,ADC_SAMPLETIME_55POINT5);
  //adc_regular_channel_config(3,ADC_CHANNEL_3,ADC_SAMPLETIME_55POINT5);
  //adc_regular_channel_config(4,ADC_CHANNEL_4,ADC_SAMPLETIME_55POINT5);
  //adc_regular_channel_config(5,ADC_CHANNEL_5,ADC_SAMPLETIME_55POINT5);
  //adc_regular_channel_config(6,ADC_CHANNEL_6,ADC_SAMPLETIME_55POINT5);
  //adc_regular_channel_config(7,ADC_CHANNEL_7,ADC_SAMPLETIME_55POINT5);

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

void tx_crc(unsigned char crclen)
{
  unsigned char crcp ;
  crc32val = 0xffffffff ;
  for (crcp = 0;  crcp < (crclen+2);  crcp++) {
    crc32val = crc32_tab[(crc32val ^ sendbuf[crcp]) & 0xFF] ^ ((crc32val >> 8) & 0x00FFFFFF);
  }
    
  for(crcp = (crclen+6);crcp > 3;crcp--) sendbuf[crcp] = sendbuf[crcp-4] ;
  sendbuf[3] =~ ((crc32val >> 24) & 0xff) ;
  sendbuf[2] =~ ((crc32val >> 16) & 0xff) ;
  sendbuf[1] =~ ((crc32val >> 8) & 0xff) ;
  sendbuf[0] =~ (crc32val  & 0xff) ;
}

void echo_frame()
{
    unsigned char makep , f_length ;
    f_length = 4 ;
    for(makep = 2;makep < (f_length+2);makep++) sendbuf[makep] = echo_req[makep-2] ;
    sendbuf[0] = f_length ;
    sendbuf[1] = 2 ;
    tx_crc(f_length) ;
    flen = f_length ;
}

void prepare_login_frame()
{
    unsigned char makep , f_length ;
    f_length = 28 ;
    for(makep = 2;makep < 6;makep++) sendbuf[makep] = prepare_login[makep-2] ;
    for(makep = 6;makep < 14;makep++) sendbuf[makep] = device_ID[makep-6] ;
    for(makep = 14;makep < 18;makep++) sendbuf[makep] = hard_ver[makep-14] ;
    for(makep = 18;makep < 22;makep++) sendbuf[makep] = soft_ver[makep-18] ;
    for(makep = 22;makep < 30;makep++) sendbuf[makep] = rannum[makep-22] ;
    sendbuf[0] = f_length ;
    sendbuf[1] = 2 ;
    tx_crc(f_length) ;
    flen = f_length ;
}

void login_frame()
{
    unsigned char makep , f_length ;
    f_length = 20 ;
    for(makep = 2;makep < 6;makep++) sendbuf[makep] = login[makep-2] ;
    for(makep = 6;makep < 14;makep++) sendbuf[makep] = device_ID[makep-6] ;
    for(makep = 14;makep < 22;makep++) sendbuf[makep] = recbuf[makep+20] ;
    sendbuf[0] = f_length ;
    sendbuf[1] = 2 ;
    tx_crc(f_length) ;
    flen = f_length ;
}

void report_charging_info_frame()
{
    struct status *sta;
    
    sta = (void *)sendbuf;
    sta->apitype = 0xeed0 ;
    sta->apimethod = 0x0005 ;
    sta->timestamp = 222;
    sta->power = 111;
    sta->current = 333 ;
    for (bufp = 0;  bufp < 16;  bufp++) sta->token[bufp] = recbuf[bufp+14] ;
    sta->length = sizeof(struct status) - 2 ;
    sta->type   = 2 ;
    tx_crc(sizeof(struct status) - 2) ;
    flen = sizeof(struct status)-2 ;
}

void charging_handle()
{
    struct charging_respond *c_res;
    
    c_res = (void *)recbuf;
    if(c_res->charging) IO595_clk_L ;
    else               IO595_clk_H ;
}

void Rx(void)
{
  rx_count = 0 ;overtime = 1000 ;
  while(overtime > 999) ;
  while(overtime < 1000) overtime ++ ;
  //usart_data_transmit(USART0, rx_count);
  //if(rx_count > 10) IO595_clk_L ;
  //else IO595_clk_H ;
  //
  crc32val = 0xffffffff ;
  for (bufp = 4;  bufp < rx_count;  bufp++) {
    crc32val = crc32_tab[(crc32val ^ recbuf[bufp]) & 0xFF] ^ ((crc32val >> 8) & 0x00FFFFFF);
  }
  cr0c = ~(crc32val  & 0xff) ;
  cr1c = ~((crc32val >> 8) & 0xff) ;
  cr2c = ~((crc32val >> 16) & 0xff) ;
  cr3c = ~((crc32val >> 24) & 0xff) ;
  if((recbuf[3] != cr3c) |(recbuf[2] !=cr2c) |(recbuf[1] !=cr1c) |(recbuf[0] !=cr0c) ) ;
  else ;
}
void Tx(void)
{
  for(bufp = 0;bufp < (flen+6);bufp++)
  {
    usart_data_transmit(USART0, sendbuf[bufp]);
    while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
  }
}
void TRx(void)
{
  Tx();
  Rx();
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
    IO595_clk_H ;

    /* setup SysTick Timer for 1ms interrupts  */
    //systick_config();
    
    uart_init();
    
    //usart_data_transmit(USART0, 0x00);
    
    overtime = 0 ;
    rx_count = 0 ;
    dmadone = 0 ;
    
    AD_config();
    
    prepare_login_frame();
    TRx();
    
    login_frame();
    TRx();
    
    while(1){
      overtime = 0 ;
      report_charging_info_frame();
      TRx();

      charging_handle();
      while(overtime < 60000) overtime ++ ;
      //usart_data_transmit(USART0, rx_count);
      //
      //if(rx_count > 10) IO595_clk_L ;
      //else IO595_clk_H ;
      //rx_count = 0 ;
      //while(overtime > 999) ;
      //timer_enable(TIMER1);
      //while( !dma_flag_get(DMA_CH0,DMA_FLAG_FTF ));
      //
      ///* clear channel1 transfer complete flag */
      //dma_flag_clear(DMA_CH0,DMA_FLAG_FTF ); 
      //while(dmadone == 0) ;
      //dmadone = 0 ;
      //timer_disable(TIMER1);
      //for(rx_count = 0;rx_count < 255;rx_count ++)
      //{
      //  usart_data_transmit(USART0, ad_value[rx_count]>>8);
      //  while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
      //  usart_data_transmit(USART0, ad_value[rx_count]&0xff);
      //  while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
      //}
      //usart_data_transmit(USART0, ad_value[255]>>8);
      //while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
      //usart_data_transmit(USART0, ad_value[255]&0xff);
      //while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
    }
}
