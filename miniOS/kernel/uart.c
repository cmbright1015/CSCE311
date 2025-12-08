#include <stdint.h>
#include <stdarg.h>
#include "uart.h"
#include "common.h"

/* the memory mapped UART will be at this base address, THR is the register that writes to send bytes while the LSR reads the bits
LSR_THRE means THR is empty and it can write new char*/
#define UART_BASE   0x10000000u
#define UART_THR    0u
#define UART_LSR    5u
#define UART_LSR_THRE (1u << 5)


/* this function returns a pointer to the MMIO register, this tells us the base address + offset to help with the rest of our code
It is volatile because reads need to get the newest hardware value and writes cannot be reordered or cached. */
static inline volatile uint8_t *uart_reg(unsigned offset)
{
    return (volatile uint8_t *)(UART_BASE + offset);
}

/* UART defaults aren't guaranteed so we just set them, FIFO is used the prevent characters being lost through buffering inside UART.*/
void uart_init(void)
{
    /* Minimal init: 8N1, enable FIFO.
       On QEMU virt this is enough to talk over serial. */
    volatile uint8_t *lcr = uart_reg(3);
    volatile uint8_t *fcr = uart_reg(2);

    *lcr = 0x03;   /* 8 data bits, 1 stop, no parity */
    *fcr = 0x01;   /* enable FIFO */
}

/* Sends single characters to UART, THR must be empty before writing the next char 
the while loop reads LSR until THRE is empty and ready for more bytes.*/
void uart_putc(char c)
{
    volatile uint8_t *thr = uart_reg(UART_THR);
    volatile uint8_t *lsr = uart_reg(UART_LSR);

    /* Wait until THR empty */
    while ((*lsr & UART_LSR_THRE) == 0)
        ;

    *thr = (uint8_t)c;
}

/* this sends a string over the UART for logs and other messages where we would need strings.*/
void uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

/* this converts unsigned integers to ASCII and prints it using putc, it divides by base, converts the remainders into characters*/
static void uart_print_uint(unsigned value, unsigned base)
{
    char buf[16];
    unsigned i = 0;

    if (value == 0) {
        uart_putc('0');
        return;
    }

    while (value && i < sizeof(buf)) {
        unsigned digit = value % base;
        value /= base;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
    }

    while (i--)
        uart_putc(buf[i]);
}

/* this provides formatting to the printing, it goes through char by char and whenever it sees a % it processes that specifier*/
void uart_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            uart_putc(*fmt++);
            continue;
        }

        fmt++;
        if (!*fmt)
            break;

        switch (*fmt) {
        case 's': {
            const char *s = va_arg(ap, const char *);
            if (!s) s = "(null)";
            uart_puts(s);
        } break;
        case 'd': {
            int val = va_arg(ap, int);
            if (val < 0) {
                uart_putc('-');
                val = -val;
            }
            uart_print_uint((unsigned)val, 10);
        } break;
        case 'x': {
            unsigned val = va_arg(ap, unsigned);
            uart_print_uint(val, 16);
        } break;
        case 'c': {
            int c = va_arg(ap, int);
            uart_putc((char)c);
        } break;
        case '%':
            uart_putc('%');
            break;
        default:
            uart_putc('%');
            uart_putc(*fmt);
            break;
        }

        fmt++;
    }

    va_end(ap);
}

