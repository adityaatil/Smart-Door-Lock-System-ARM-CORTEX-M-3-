#include <stdio.h>
#include <string.h>
#include <LPC17xx.h>

/* ================= LCD PIN DEFINITIONS ================= */
#define DT_CTRL   (0x0F << 20)   // P1.20–P1.23
#define RS_CTRL   (1 << 25)      // P3.25
#define RW_CTRL   (1 << 26)      // P3.26
#define EN_CTRL   (1 << 28)      // P4.28

/* ================= LED PIN ================= */
#define LED_PIN   (1 << 18)      // P1.18

/* ================= PASSWORD ================= */
#define PASSWORD "1234"
#define MAX_ATTEMPTS 3

/* ================= GLOBAL VARIABLES ================= */
unsigned long int temp1 = 0, temp2 = 0;
char input[5];          // EXACT 4 digits + null
int idx = 0;
int attempts = 0;

/* ================= FUNCTION PROTOTYPES ================= */
void initTimer0(void);
void delayMS(unsigned int);

void UART0_Init(void);
char UART0_GetChar(void);

void lcd_init(void);
void lcd_com(void);
void lcd_data(void);
void wr_cn(void);
void wr_dn(void);
void delay_lcd(unsigned int);
void clear_ports(void);
void lcd_puts(unsigned char *);

void LED_Blink3(void);

/* ================= MAIN ================= */
int main(void)
{
    char ch;
    char forgot_str[7] = "forgot";
    int forgot_idx = 0;

    SystemInit();
    SystemCoreClockUpdate();

    initTimer0();
    lcd_init();
    UART0_Init();

    // Configure P1.18 as GPIO
    LPC_GPIO1->FIODIR |= LED_PIN; // Output
    LPC_GPIO1->FIOCLR = LED_PIN;  // LED OFF initially

    temp1 = 0x01;
    lcd_com();
    delayMS(5);
    lcd_puts((unsigned char*)"ENTER PASSWORD");

    temp1 = 0xC0;
    lcd_com();
    delayMS(5);

    while (1)
    {
        // SYSTEM LOCKED CHECK
        if (attempts >= MAX_ATTEMPTS)
        {
            temp1 = 0x01;
            lcd_com();
            delayMS(5);
            lcd_puts((unsigned char*)"SYSTEM LOCKED");

            // Blink LED continuously
            while (1)
            {
                LPC_GPIO1->FIOSET = LED_PIN;  // LED ON
                delayMS(500);
                LPC_GPIO1->FIOCLR = LED_PIN;  // LED OFF
                delayMS(500);
            }
        }

        ch = UART0_GetChar();

        // Convert uppercase to lowercase for "forgot"
        if (ch >= 'A' && ch <= 'Z') ch += 32;

        // Check for "forgot"
        if (ch == forgot_str[forgot_idx])
        {
            forgot_idx++;
            if (forgot_idx == 6)
            {
                temp1 = 0x01; lcd_com(); delayMS(5);
                lcd_puts((unsigned char*)"RESET PASSWORD");
                forgot_idx = 0;
                idx = 0;
                attempts = 0;
                LPC_GPIO1->FIOCLR = LED_PIN; // LED OFF
                delayMS(2000);
                temp1 = 0x01; lcd_com(); delayMS(5);
                lcd_puts((unsigned char*)"ENTER PASSWORD");
                temp1 = 0xC0; lcd_com(); delayMS(5);
            }
            continue;
        }
        else
        {
            forgot_idx = 0;
        }

        // Accept only digits
        if (ch < '0' || ch > '9') continue;

        input[idx++] = ch;
        temp1 = '*';
        lcd_data(); // Password masking

        // Check after 4 digits
        if (idx == 4)
        {
            input[4] = '\0';

            temp1 = 0x01; lcd_com(); delayMS(5); // Clear LCD
            if (strcmp(input, PASSWORD) == 0)
            {
                lcd_puts((unsigned char*)"ACCESS GRANTED");
                delayMS(50); // ensure LCD updates

                // Blink LED 3 times for visual confirmation
                LED_Blink3();

                // Turn LED ON permanently after blink
                LPC_GPIO1->FIOSET = LED_PIN;

                while (1);  // stop system
            }
            else
            {
                lcd_puts((unsigned char*)"WRONG PASSWORD");
                LPC_GPIO1->FIOCLR = LED_PIN; // ensure LED OFF
                attempts++;
                delayMS(1500);
            }

            // Reset input
            idx = 0;
            temp1 = 0x01; lcd_com(); delayMS(5);
            lcd_puts((unsigned char*)"ENTER PASSWORD");
            temp1 = 0xC0; lcd_com(); delayMS(5);
        }
    }
}

void LED_Blink3(void)
{
    int i;  // Declare outside the for loop
    for(i = 0; i < 3; i++)
    {
        LPC_GPIO1->FIOSET = LED_PIN;
        delayMS(300);
        LPC_GPIO1->FIOCLR = LED_PIN;
        delayMS(300);
    }
}


/* ================= UART ================= */
void UART0_Init(void)
{
    LPC_SC->PCONP |= (1 << 3);
    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));
    LPC_PINCON->PINSEL0 |=  (1 << 4) | (1 << 6);   // P0.2 TXD0, P0.3 RXD0

    LPC_UART0->LCR = 0x83;
    LPC_UART0->DLL = 78;      // 9600 baud @ 12MHz
    LPC_UART0->DLM = 0;
    LPC_UART0->LCR = 0x03;
}

char UART0_GetChar(void)
{
    while (!(LPC_UART0->LSR & 0x01));
    return LPC_UART0->RBR;
}

/* ================= TIMER ================= */
void initTimer0(void)
{
    LPC_SC->PCONP |= (1 << 1);
    LPC_TIM0->CTCR = 0x00;
    LPC_TIM0->PR   = 25000 - 1;
    LPC_TIM0->TCR  = 0x02;
}

void delayMS(unsigned int ms)
{
    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->TCR = 0x01;
    while (LPC_TIM0->TC < ms);
    LPC_TIM0->TCR = 0x00;
}

/* ================= LCD ================= */
void lcd_init(void)
{
    LPC_PINCON->PINSEL3 &= 0xFFFF00FF;
    LPC_PINCON->PINSEL7 &= 0xFFF3FFFF;
    LPC_PINCON->PINSEL9 &= 0xFCFFFFFF;

    LPC_GPIO1->FIODIR |= DT_CTRL;
    LPC_GPIO3->FIODIR |= RS_CTRL | RW_CTRL;
    LPC_GPIO4->FIODIR |= EN_CTRL;

    clear_ports();
    delay_lcd(30000);

    temp2 = 0x30; wr_cn(); delay_lcd(30000);
    temp2 = 0x30; wr_cn(); delay_lcd(30000);
    temp2 = 0x20; wr_cn(); delay_lcd(30000);

    temp1 = 0x28; lcd_com();
    temp1 = 0x0C; lcd_com();
    temp1 = 0x06; lcd_com();
    temp1 = 0x01; lcd_com();
    delayMS(5);
}

void lcd_com(void)
{
    temp2 = (temp1 & 0xF0) << 16;
    wr_cn();
    temp2 = (temp1 & 0x0F) << 20;
    wr_cn();
    delay_lcd(1000);
}

void lcd_data(void)
{
    temp2 = (temp1 & 0xF0) << 16;
    wr_dn();
    temp2 = (temp1 & 0x0F) << 20;
    wr_dn();
    delay_lcd(1000);
}

void wr_cn(void)
{
    clear_ports();
    LPC_GPIO1->FIOPIN = temp2;
    LPC_GPIO3->FIOCLR = RS_CTRL | RW_CTRL;
    LPC_GPIO4->FIOSET = EN_CTRL;
    delay_lcd(25);
    LPC_GPIO4->FIOCLR = EN_CTRL;
}

void wr_dn(void)
{
    clear_ports();
    LPC_GPIO1->FIOPIN = temp2;
    LPC_GPIO3->FIOSET = RS_CTRL;
    LPC_GPIO3->FIOCLR = RW_CTRL;
    LPC_GPIO4->FIOSET = EN_CTRL;
    delay_lcd(25);
    LPC_GPIO4->FIOCLR = EN_CTRL;
}

void delay_lcd(unsigned int d)
{
    unsigned int i;
    for (i = 0; i < d; i++);
}

void clear_ports(void)
{
    LPC_GPIO1->FIOCLR = DT_CTRL;
    LPC_GPIO3->FIOCLR = RS_CTRL | RW_CTRL;
    LPC_GPIO4->FIOCLR = EN_CTRL;
}

void lcd_puts(unsigned char *str)
{
    while (*str)
    {
        temp1 = *str++;
        lcd_data();
    }
}