#include <LPC17xx.h>
#include <RTL.h>
#include <string.h>
#include "UART0.h"
#include "Lcd.h"

#define LED_PIN       (1 << 18)      /* P1.18 */
#define MAX_ATTEMPTS  3
#define PASSWORD      "1234"

volatile unsigned char attempts = 0;
char pass_input[5];
unsigned char pass_idx = 0;
unsigned char forgot_idx = 0;

static void show_prompt(void)
{
    clr_disp();
    lcd_puts((unsigned char *)"ENTER PASSWORD");
    temp1 = 0xC0;
    lcd_com();
}

static void reset_password_state(void)
{
    pass_idx = 0;
    forgot_idx = 0;
    show_prompt();
}

__task void LED_Task(void)
{
    while (1)
    {
        if (attempts >= MAX_ATTEMPTS)
        {
            LPC_GPIO1->FIOSET = LED_PIN;
            os_dly_wait(50);
            LPC_GPIO1->FIOCLR = LED_PIN;
            os_dly_wait(50);
        }
        else
        {
            LPC_GPIO1->FIOCLR = LED_PIN;
            os_dly_wait(1);
        }
    }
}

__task void DoorLock_Task(void)
{
    const char forgot_word[] = "forgot";
    unsigned char ch;

    show_prompt();

    while (1)
    {
        if (attempts >= MAX_ATTEMPTS)
        {
            clr_disp();
            lcd_puts((unsigned char *)"SYSTEM LOCKED");
            while (1)
            {
                os_dly_wait(1);
            }
        }

        if (!rx0_flag)
        {
            os_dly_wait(1);
            continue;
        }

        ch = recv_data;
        rx0_flag = 0;

        if (ch >= 'A' && ch <= 'Z')
            ch += 32;    /* convert to lowercase */

        /* forgot reset */
        if (ch == forgot_word[forgot_idx])
        {
            forgot_idx++;
            if (forgot_idx == 6)
            {
                attempts = 0;
                forgot_idx = 0;
                clr_disp();
                lcd_puts((unsigned char *)"RESET PASSWORD");
                os_dly_wait(200);
                reset_password_state();
            }
            continue;
        }
        forgot_idx = 0;

        /* accept only digits */
        if (ch < '0' || ch > '9')
            continue;

        pass_input[pass_idx++] = ch;
        temp1 = '*';
        lcd_data();

        if (pass_idx == 4)
        {
            pass_input[4] = '\0';
            clr_disp();

            if (strcmp(pass_input, PASSWORD) == 0)
            {
                lcd_puts((unsigned char *)"ACCESS GRANTED");
                LPC_GPIO1->FIOSET = LED_PIN;
                while (1)
                {
                    os_dly_wait(1);
                }
            }

            lcd_puts((unsigned char *)"WRONG PASSWORD");
            attempts++;
            os_dly_wait(150);
            reset_password_state();
        }
    }
}

int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();

    lcd_init();
    UART0_Init();

    LPC_GPIO1->FIODIR |= LED_PIN;
    LPC_GPIO1->FIOCLR = LED_PIN;

    os_tsk_create(LED_Task, 1);
    os_tsk_create(DoorLock_Task, 2);
    os_sys_init_user();

    while (1)
    {
    }
}
