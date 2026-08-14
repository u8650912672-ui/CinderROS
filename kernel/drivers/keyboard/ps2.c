#include <io.h>
#include <stdint.h>
#define KBD_DATA   0x60
#define KBD_STATUS 0x64
static int shift = 0;
static int caps = 0;
//more comments finally i am a lazy fuck tho also i think alreyd did thsi so il steal from a past project
static const char normal_map[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0
};
//shift version aka funny symbolic stuff and shit
static const char shift_map[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0
};
char keyboard_getc(void) {
    if (!(inb(KBD_STATUS) & 1)) return 0; //this means no data ak fuck off
    uint8_t sc = inb(KBD_DATA);
    if (sc & 0x80) {                        //key had be dismanteld :D
        uint8_t k = sc & 0x7F;
        if (k == 0x2A || k == 0x36) shift = 0;
        return 0;
    }
    if (sc == 0x2A || sc == 0x36) { shift = 1; return 0; }
    if (sc == 0x3A) { caps = !caps; return 0; }
    return (shift || caps) ? shift_map[sc] : normal_map[sc];
}