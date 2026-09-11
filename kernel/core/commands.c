#include <kernel.h>
#include <io.h>

//this is semi simple code so i wont be commenting much its mostly gonna be just idk stuff
int str_eq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}
//no commands lol i need to fix stuff
static struct cmd { const char *name; void (*fn)(int argc, char **argv); } cmds[] = {};

void shell_exec(const char *line){
    (void)line; //shsuh gcc noone cares :D
    dprint("we dont have commands here cuz im stupid\n"); //do something soon bout this
}