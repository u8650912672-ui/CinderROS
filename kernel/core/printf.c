#include <stdarg.h>
#include <kernel.h>

static void sink(char c) { dputchar(c); ser_putc(c); }

static void print_uint(unsigned long v, int base, int upper) {
    char buf[20]; int n = 0;
    do {
        int d = v % base;
        buf[n++] = d < 10 ? (char)('0' + d) :
                            (char)((upper ? 'A' : 'a') + (d - 10));
        v /= base;
    } while (v);
    while (n--) sink(buf[n]); //thanks jonathan again for this wierd block that may work?
    }
void printf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { sink(*fmt); continue; }
        fmt++;
        switch (*fmt) {
            case '%': sink ('%'); break;
            case 'c': sink((char)va_arg(ap, int)); break;
            case 's': { const char *s = va_arg(ap, const char *); while (*s) sink(*s++); }
break;
            case 'd': case 'u': { int v = va_arg(ap, int);
                if (*fmt == 'd' && v < 0) { sink('-'); v = -v; }
                print_uint((unsigned)v, 10, 0); } break;
            case 'x': case 'X': print_uint(va_arg(ap, unsigned), 16, *fmt == 'X'); break;
            case 'p': sink ('0'); sink('x'); print_uint((unsigned long)va_arg(ap, void *), 16, 0); break;
            default: sink('%'); sink(*fmt); break;
        }
    }
    va_end(ap);
}