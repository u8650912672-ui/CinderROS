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
static int ext = 0;
char keyboard_getc(void) {
    if (!(inb(KBD_STATUS) & 1)) return 0; //this means no data ak fuck off
    uint8_t sc = inb(KBD_DATA);
    if (ext) { ext = 0; return 0; }
    if (sc == 0xE0) { ext = 1; return 0; }
    if (sc & 0x80) {
        uint8_t k = sc & 0x7F;
        if (k == 0x2A || k == 0x36) shift = 0;
        return 0;
    }
    if (sc == 0x2A || sc == 0x36) { shift = 1; return 0; }
    if (sc == 0x3A) { caps = !caps; return 0; }
    char c = normal_map[sc];
    if (shift)
        c = shift_map[sc];
    else if (caps && c >= 'a' && c <= 'z')
        c = (char)(c - ('a' - 'A')); //fixed the stupid bug i borrowed from cbos :/
    return c;
}
static void kbd_drain(void) {
    while (inb(KBD_STATUS) & 1) inb(KBD_DATA);
}

static int kbd_wait_data(int timeout) {
    while (timeout-- > 0)
        if (inb(KBD_STATUS) & 1) return 1;
    return 0;
}
static void kbd_send_dev(uint8_t b) {
    while (inb(KBD_STATUS) & 2) ; 
    outb(KBD_DATA, b);
}
void keyboard_init(void) { // i wounder if i could reuse the one i made in cbos
    outb(0x64, 0xAD); //sanity check (i disable the device to enable it later)
    kbd_drain(); //gets rid of stale shiit i dont want here
    kbd_send_dev(0xFF); //another reset just to mkae sure that i dont need to recode this part ever again
    for (int i = 0; i < 3; i++) 
        if (kbd_wait_data(100000)) inb(KBD_DATA);
    kbd_send_dev(0xF0); //asks for your scancodes and passwords and also hacks your wifi and install a trojan into your bios to spy on you when you use soemthing /this is a joke 
    if (kbd_wait_data(100000)) inb(KBD_DATA);
    kbd_send_dev(0x01); //I waant set 1 plsss :3
    if (kbd_wait_data(100000)) inb(KBD_DATA);
    outb(0x64, 0xAE); //and finally after 20 different sanity checks this will compile first try and work
    kbd_drain(); //i forgot this :sob:    
}