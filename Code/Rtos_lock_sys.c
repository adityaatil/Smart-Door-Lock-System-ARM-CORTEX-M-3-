#include <string.h>
#include <LPC17xx.h>

/* LPC1768 pin assignments */
#define LCD_DATA    (0x0F << 20)       /* P1.20-P1.23 */
#define LCD_RS      (1 << 25)          /* P3.25 */
#define LCD_RW      (1 << 26)          /* P3.26 */
#define LCD_EN      (1 << 28)          /* P4.28 */
#define LED         (1 << 18)          /* P1.18 */

#define PASSWORD    "1234"
#define MAX_TRIES   3

static unsigned long data_word;
static char password[5];
static int password_index;
static int tries;

static void timer_init(void);
static void delay_ms(unsigned int ms);
static void uart_init(void);
static char uart_getchar(void);
static void lcd_init(void);
static void lcd_command(unsigned char command);
static void lcd_char(unsigned char character);
static void lcd_string(const char *text);
static void lcd_clear(void);
static void lcd_prompt(void);
static void lcd_write_nibble(unsigned long nibble, int character);
static void lcd_delay(unsigned int count);
static void led_blink(unsigned int count);

static void timer_init(void)
{
    LPC_SC->PCONP |= (1 << 1);
    LPC_TIM0->CTCR = 0;
    LPC_TIM0->PR = 25000 - 1;
    LPC_TIM0->TCR = 2;
}

static void delay_ms(unsigned int ms)
{
    LPC_TIM0->TCR = 2;
    LPC_TIM0->TCR = 1;
    while (LPC_TIM0->TC < ms) {
    }
    LPC_TIM0->TCR = 0;
}

static void uart_init(void)
{
    LPC_SC->PCONP |= (1 << 3);
    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));
    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6); /* P0.2 TXD0, P0.3 RXD0 */

    LPC_UART0->LCR = 0x83;              /* 8-bit, 1 stop, enable divisor */
    LPC_UART0->DLL = 78;                /* 9600 baud for the original clock */
    LPC_UART0->DLM = 0;
    LPC_UART0->LCR = 0x03;
}

static char uart_getchar(void)
{
    while (!(LPC_UART0->LSR & 1)) {
    }
    return LPC_UART0->RBR;
}

static void lcd_delay(unsigned int count)
{
    volatile unsigned int i;
    for (i = 0; i < count; i++) {
    }
}

static void lcd_write_nibble(unsigned long nibble, int character)
{
    LPC_GPIO1->FIOCLR = LCD_DATA;
    LPC_GPIO1->FIOSET = nibble;
    LPC_GPIO3->FIOCLR = LCD_RW;
    if (character) {
        LPC_GPIO3->FIOSET = LCD_RS;
    } else {
        LPC_GPIO3->FIOCLR = LCD_RS;
    }
    LPC_GPIO4->FIOSET = LCD_EN;
    lcd_delay(25);
    LPC_GPIO4->FIOCLR = LCD_EN;
}

static void lcd_command(unsigned char command)
{
    lcd_write_nibble((command & 0xF0) << 16, 0);
    lcd_write_nibble((command & 0x0F) << 20, 0);
    lcd_delay(1000);
}

static void lcd_char(unsigned char character)
{
    lcd_write_nibble((character & 0xF0) << 16, 1);
    lcd_write_nibble((character & 0x0F) << 20, 1);
    lcd_delay(1000);
}

static void lcd_string(const char *text)
{
    while (*text) {
        lcd_char((unsigned char)*text++);
    }
}

static void lcd_clear(void)
{
    lcd_command(0x01);
    delay_ms(5);
}

static void lcd_prompt(void)
{
    lcd_clear();
    lcd_string("ENTER PASSWORD");
    lcd_command(0xC0);
}

static void lcd_init(void)
{
    LPC_PINCON->PINSEL3 &= 0xFFFF00FF;
    LPC_PINCON->PINSEL7 &= 0xFFF3FFFF;
    LPC_PINCON->PINSEL9 &= 0xFCFFFFFF;

    LPC_GPIO1->FIODIR |= LCD_DATA;
    LPC_GPIO3->FIODIR |= LCD_RS | LCD_RW;
    LPC_GPIO4->FIODIR |= LCD_EN;

    LPC_GPIO1->FIOCLR = LCD_DATA;
    LPC_GPIO3->FIOCLR = LCD_RS | LCD_RW;
    LPC_GPIO4->FIOCLR = LCD_EN;
    lcd_delay(30000);

    lcd_write_nibble(0x30 << 16, 0);
    lcd_delay(30000);
    lcd_write_nibble(0x30 << 16, 0);
    lcd_delay(30000);
    lcd_write_nibble(0x20 << 16, 0);
    lcd_delay(30000);

    lcd_command(0x28);                 /* 4-bit, 2-line mode */
    lcd_command(0x0C);                 /* display ON, cursor OFF */
    lcd_command(0x06);                 /* increment cursor */
    lcd_clear();
}

static void led_blink(unsigned int count)
{
    while (count--) {
        LPC_GPIO1->FIOSET = LED;
        delay_ms(300);
        LPC_GPIO1->FIOCLR = LED;
        delay_ms(300);
    }
}

int main(void)
{
    char input;
    const char reset_word[] = "forgot";
    int reset_index = 0;

    SystemInit();
    SystemCoreClockUpdate();
    timer_init();
    lcd_init();
    uart_init();

    LPC_GPIO1->FIODIR |= LED;
    LPC_GPIO1->FIOCLR = LED;
    lcd_prompt();

    while (1) {
        if (tries >= MAX_TRIES) {
            lcd_clear();
            lcd_string("SYSTEM LOCKED");
            while (1) {
                LPC_GPIO1->FIOSET = LED;
                delay_ms(500);
                LPC_GPIO1->FIOCLR = LED;
                delay_ms(500);
            }
        }

        input = uart_getchar();
        if (input >= 'A' && input <= 'Z') {
            input += 'a' - 'A';
        }

        if (input == reset_word[reset_index]) {
            reset_index++;
            if (reset_index == 6) {
                tries = 0;
                password_index = 0;
                reset_index = 0;
                LPC_GPIO1->FIOCLR = LED;
                lcd_clear();
                lcd_string("RESET PASSWORD");
                delay_ms(2000);
                lcd_prompt();
            }
            continue;
        }
        reset_index = 0;

        if (input < '0' || input > '9') {
            continue;
        }

        password[password_index++] = input;
        lcd_char('*');

        if (password_index == 4) {
            password[4] = '\0';
            lcd_clear();

            if (strcmp(password, PASSWORD) == 0) {
                lcd_string("ACCESS GRANTED");
                led_blink(3);
                LPC_GPIO1->FIOSET = LED;
                while (1) {
                }
            }

            lcd_string("WRONG PASSWORD");
            LPC_GPIO1->FIOCLR = LED;
            tries++;
            delay_ms(1500);
            password_index = 0;
            lcd_prompt();
        }
    }
}
