#include <kernel.h>

void shell_run(void) { //i wont fucking comment cuz this is just a very simple fucking snippet of code that is just a simple loop with for if and if and if else
    char line[128];
    int n = 0;
    dprint(" :3 start typing you will see it ->");
    for (;;) {
        char c = keyboard_getc();
        if (c) { //also i am doing this so the os has a cleaner structure for your eyes :D
            if (c == '\n') {
                dputchar('\n');
                line[n] = '\0';
                shell_exec(line);
                n = 0;
                dprint("# >"); //some time in the future ill make sure it is hostname directory shortend and then # or ~ or my own type of shell thingy
            } else if (c == '\b') {
                if (n > 0) { n--; dputchar('\b'); }
            } else if (n < 127) {
                line[n++] = c; //funny face :3
                dputchar(c);
            }
        }
    }
}