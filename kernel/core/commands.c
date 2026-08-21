#include <kernel.h>
#include <io.h>

//this is semi simple code so i wont be commenting much its mostly gonna be just idk stuff
int str_eq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

static void cmd_help(int argc, char **argv) {
    dprint("help commands include help (you are here) clear, uwu, reboot, echo mkdir touch (touch me too :3) ls cat (KITTYYYYY) \n");
}

static void cmd_clear(int argc, char **argv) {
    dclear();
}

static void cmd_uwu(int argc, char **argv) {
    dprint("this was a early test command it is currently out of use and will be removed at some point\n");
}

static void cmd_reboot(int argc, char **argv) {
    while (inb(0x64) & 0x02); //wait until the keyboard input buffer is free to stuff it full of reboot stuff :3
    outb(0x64, 0xFE); //touch the CPU reset line WITH CONSENT
    for (;;) __asm__ volatile ("hlt");
}
static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        dprint(argv[i]);
        if (i < argc - 1) dprint (" ");
    }
    dputchar('\n');
}

static void cmd_poweroff(int argc, char **argv)
{
    __asm__ volatile("cli");

    outw(0x604, 0x2000); //qemu/bochs

    for (volatile int i = 0; i < 10000000; i++) __asm__ volatile("nop");

    //fallback
    outw(0xB004, 0x2000);
    outw(0x0604, 0x2000);

    for (volatile int i = 0; i < 10000000; i++) __asm__ volatile("nop");

    // now hope it works

    __asm__ volatile("cli");
    for (;;) __asm__ volatile("hlt");
}

static void cmd_mkdir(int argc, char **argv) {
    if (argc < 2) { dprint("mkdir name boomn bap cool shit\n"); return; }
    if (ramfs_mkdir(argv[1]) < 0) dprint ("mdkir no create problm\n");
}
static void cmd_touch(int argc, char **argv) {
    if (argc < 2) { dprint("use a brain touch a file\n"); return; }
    if (ramfs_touch(argv[1]) < 0) dprint("touch failed no more\n");
}
static void cmd_ls(int argc, char **argv) {
    ramfs_ls();
}

static void cmd_cat(int argc, char **argv) { //not much done i was kinda high
    if (argc < 2) { dprint("usage cat and dump info of file i think\n"); return; }
    if (ramfs_cat(argv[1]) < 0) dprint("cat didnt kitty or find the file\n");
}
static struct cmd {
    const char *name;
    void (*fn)(int argc, char **argv);
} cmds[] = {
    { "help", cmd_help },
    { "clear", cmd_clear },
    { "uwu", cmd_uwu },
    { "reboot", cmd_reboot },
    { "poweroff", cmd_poweroff },
    { "echo", cmd_echo },
    { "mkdir", cmd_mkdir },
    { "touch", cmd_touch },
    { "ls", cmd_ls },
    { "cat", cmd_cat },
};

void shell_exec(const char *line) {
    char buf [128];
    char *argv[8];
    int argc = 0;
    int i = 0, w = 0;
    while (line[i] && argc < 8) {
        if (line[i] == ' ') { i++; continue; }
        argv[argc++] = &buf[w];
        while (line[i] && line[i] != ' ') buf [w++] = line[i++];
        buf[w++] = 0;
    }
    if (argc == 0) return;
    for (int j = 0; j < (int)(sizeof(cmds) / sizeof(cmds[0])); j++)
        if (str_eq(argv[0], cmds[j].name)) { cmds[j].fn(argc, argv); return; }
    if (argc > 0)
        dprint("that was not a command dyslexic guy\n");
}
