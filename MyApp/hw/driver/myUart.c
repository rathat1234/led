#include "myUart.h"
#include <stdio.h>

#define RX_BUF_SIZE 128
uint8_t rx_data;

uint8_t rx_buf[RX_BUF_SIZE];
uint8_t send_buf[RX_BUF_SIZE];
uint8_t newline[] = "\r\n";
uint8_t entkey = '\r';
int check = 0;
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
uint8_t rx_buf_1[RX_BUF_SIZE];
uint8_t rx_buf_2[RX_BUF_SIZE];



#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  
  return ch;
}

void uartInit(void){
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf_2, RX_BUF_SIZE);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buf_1, RX_BUF_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        for (uint16_t i = 0; i < Size; i++)
        {
            uint8_t c = rx_buf_2[i];

            if (c == entkey)
            {
                if (check > 0)
                {
                    HAL_UART_Transmit(&huart1, send_buf, check, 100);
                }

                HAL_UART_Transmit(&huart1, newline, 2, 100);
                HAL_UART_Transmit(&huart2, newline, 2, 100);

                memset(send_buf, 0, RX_BUF_SIZE);
                check = 0;
            }
            else if (c == 0x08 || c == 0x7F)
            {
                if (check > 0)
                {
                    check--;

                    send_buf[check] = 0;

                    uint8_t backspace[] = "\b \b";
                    HAL_UART_Transmit(&huart2, backspace, 3, 100);
                }
            }
            else
            {
                if (check < RX_BUF_SIZE - 1)
                {
                    send_buf[check] = c;
                    check++;

                    HAL_UART_Transmit(&huart2, &c, 1, 10);
                }
            }
        }

        memset(rx_buf_2, 0, RX_BUF_SIZE);

        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart2,
            rx_buf_2,
            RX_BUF_SIZE
        );
    }

    else if (huart->Instance == USART1)
    {
        HAL_UART_Transmit(&huart2, rx_buf_1, Size, 100);

        memset(rx_buf_1, 0, RX_BUF_SIZE);

        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart1,
            rx_buf_1,
            RX_BUF_SIZE
        );
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart2,
            rx_buf_2,
            RX_BUF_SIZE
        );
    }
    else if (huart->Instance == USART1)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart1,
            rx_buf_1,
            RX_BUF_SIZE
        );
    }
}
