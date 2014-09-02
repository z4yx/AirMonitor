#include "mt7681.h"
#include "usart.h"
#include "calendar.h"

/*
Flash MT7681:
screen
AT#Reboot
AT#UpdateFW
Ctrl+A
:exec !! lsx -b -X /Users/zhang/Projects/AirMonitor/MT7681_Module/out/sta/MT7681_sta_header.bin
*/

enum{STATE_INIT, STATE_SYM1, STATE_RECV};

#define LINEBUF_LEN   512
static volatile char usart_line_buf[LINEBUF_LEN], output_str_buf[128];
static volatile int line_buf_ptr;
static volatile bool data_available;

static void MT7681_USART_SetInterrupt(void);

void MT7681_Init(void)
{
    line_buf_ptr = 0;
    usart_line_buf[0] = 0;
    USARTx_Config(MT7681_USART, 115200);
    MT7681_USART_SetInterrupt();
}

static void MT7681_USART_SetInterrupt()
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = MT7681_USART_IRQ; //指定中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 指定响应优先级别
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(MT7681_USART, USART_IT_RXNE, ENABLE);
}

static void encode_date_str(char *out, const char* in)
{
    for (; *in; ++in,++out)
    {
        *out = (*in == '-') ? '_' : *in;
    }
    *out = 0;
}

void MT7681_Yeelink_Send(const char *datetime, int air, float temp, float humi)
{
    char date_str[24];
    // AT#YeelinkPub -s2014_08_31T16:02:47 -t20.5 -a1000 -h43.2
    encode_date_str(date_str, datetime);
    sprintf(output_str_buf, "AT#YeelinkPub -s%s -t%.2f -a%d -h%.2f",
        date_str, temp, air, humi);
    USARTx_printf(MT7681_USART, "%s\r\n", output_str_buf);
    DBG_MSG("%s", output_str_buf);
}

void MT7681_ntp_query(const char *host)
{
    // AT#NtpQuery -h17.82.253.7
    USARTx_printf(MT7681_USART, "AT#NtpQuery -h%s\r\n", host);
}

void MT7681_USART_Interrupt(void)
{
    uint8_t ch;
    static uint8_t state = STATE_INIT;

    ch = USART_ReceiveData(MT7681_USART);
    USART_ClearFlag(MT7681_USART, USART_FLAG_RXNE);

    switch(state) {
        case STATE_INIT:
            line_buf_ptr = 0;
            if(ch == '@'){
                state = STATE_SYM1;
            }
            break;
        case STATE_SYM1:
            if(ch == '$'){
                state = STATE_RECV;
            }else{
                state = STATE_INIT;
            }
            break;
        case STATE_RECV:
            if(ch == '\r' || ch == '\n'){
                state = STATE_INIT;
                data_available = true;
                usart_line_buf[line_buf_ptr++] = '\0';
            }else if(line_buf_ptr < LINEBUF_LEN-1)
                usart_line_buf[line_buf_ptr++] = ch;
            break;
    }
}

void MT7681_Task(void)
{
    if(data_available){
        DBG_MSG("Recv:[%s]", usart_line_buf);
        if(strncmp(usart_line_buf,"T=",2) == 0){
            uint32_t high, low;
            sscanf(usart_line_buf, "T=%u,%u", &high, &low);
            Calendar_SetCurrentTime(((int64_t)high << 32) | low);
        }
        data_available = false;
    }
}