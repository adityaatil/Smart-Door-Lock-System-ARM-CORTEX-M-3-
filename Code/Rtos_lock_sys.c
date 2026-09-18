#include <LPC17xx.h>
#include <RTL.h>
#include "UART0.h"
#include "Lcd.h"

#define LED_PIN       (1 << 18)       /* P1.18 */
#define MAX_ATTEMPTS  3
#define PASSWORD      "1234"

enum { LED_OFF, ACCESS_GRANTED, SYSTEM_LOCKED };
volatile unsigned char lock_state = LED_OFF;
volatile unsigned char attempts = 0;

static char entered[5];
static unsigned char index_pos = 0;
static unsigned char forgot_pos = 0;
static const char reset_word[] = "forgot";

static void led_delay(unsigned int ticks)
{
    os_dly_wait(ticks);
}

__task void LedTask(void)
{
    unsigned char i;

    while (1) {
        if (lock_state == SYSTEM_LOCKED) {
            LPC_GPIO1->FIOSET = LED_PIN;
            led_delay(50);
            LPC_GPIO1->FIOCLR = LED_PIN;
            led_delay(50);
        } else if (lock_state == ACCESS_GRANTED) {
            for (i = 0; i < 3 && lock_state == ACCESS_GRANTED; i++) {
                LPC_GPIO1->FIOSET = LED_PIN;
                led_delay(30);
                LPC_GPIO1->FIOCLR = LED_PIN;
                led_delay(30);
            }
            if (lock_state == ACCESS_GRANTED) {
                LPC_GPIO1->FIOSET = LED_PIN;
                os_tsk_delete_self();
            }
        } else {
            LPC_GPIO1->FIOCLR = LED_PIN;
            led_delay(1);
        }
    }
}

static void show_prompt(void)
{
    clr_disp();
    lcd_puts((unsigned char *)"ENTER PASSWORD");
    temp1 = 0xC0;
    lcd_com();
}

static void reset_input(void)
{
    index_pos = 0;
    forgot_pos = 0;
    show_prompt();
}

__task void PasswordTask(void)
{
    unsigned char ch;

    show_prompt();

    while (1) {
        if (attempts >= MAX_ATTEMPTS) {
            lock_state = SYSTEM_LOCKED;
            clr_disp();
            lcd_puts((unsigned char *)"SYSTEM LOCKED");
            os_tsk_delete_self();
        }

        if (!rx0_flag) {
            os_dly_wait(1);
            continue;
        }

        ch = recv_data;
        rx0_flag = 0;

        if (ch >= 'A' && ch <= 'Z') {
            ch += 'a' - 'A';
        }

        /* Type "forgot" to clear the failed-attempt counter. */
        if (ch == reset_word[forgot_pos]) {
            if (++forgot_pos == 6) {
                attempts = 0;
                lock_state = LED_OFF;
                clr_disp();
                lcd_puts((unsigned char *)"RESET PASSWORD");
                os_dly_wait(200);
                reset_input();
            }
            continue;
        }
        forgot_pos = 0;

        if (ch < '0' || ch > '9') {
            continue;
        }

        entered[index_pos++] = ch;
        temp1 = '*';
        lcd_data();

        if (index_pos == 4) {
            entered[4] = '\0';
            clr_disp();

            if (strcmp(entered, PASSWORD) == 0) {
                lcd_puts((unsigned char *)"ACCESS GRANTED");
                lock_state = ACCESS_GRANTED;
                os_tsk_delete_self();
            }

            lcd_puts((unsigned char *)"WRONG PASSWORD");
            attempts++;
            os_dly_wait(150);
            reset_input();
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

    os_tsk_create(LedTask, 1);
    os_tsk_create(PasswordTask, 1);
    os_sys_init(PasswordTask);

    while (1) {
    }
}
