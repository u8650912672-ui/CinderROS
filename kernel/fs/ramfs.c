#include <kernel.h>

#define RAMFS_MAX 64
#define RAMFS_NAME 24
#define RAMFS_DATA 256

struct ramfs_inode {
    char name [RAMFS_NAME];
    uint8_t type; //this gives us 1 WHOLE FUCKING FILE WOOOO
    uint8_t data[RAMFS_DATA];
    uint16_t size;
    uint16_t parent;
}; //i wont comment alot since this is quite complex and some of the code i have stolen from tutorial and more aka some i dont even understand :3
//im just here to copy paste tihi :3

static struct ramfs_inode inodes[RAMFS_MAX];
static uint16_t cwd;
static uint16_t used;

static int fs_len(const char *s) { int n = 0; while (s[n]) n++; return n; }
static int fs_cmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}
static int fs_prefix(const char *s, const char *p) {
    while (*p) { if (*s++ != *p++) return 0; }
    return 1;
}
static void fs_cpy(char *dst, const char *src) {
    while (*src) *dst++ = *src++;
    *dst = 0;
}
static int fs_oct(const char *p, int n) {
    int v = 0;
    while (n-- > 0 && *p >= '0' && *p <= '7') v = v * 8 + (*p++ - '0');
    return v;
}
void ramfs_init(void) {
    used = 1;
    inodes[0].name[0] = '/'; inodes[0].name[1] = 0;
    inodes[0].type = 0; inodes[0].size = 0; inodes[0].parent = 0;
    cwd = 0;
}
static struct ramfs_inode *ramfs_find(uint16_t dir, const char *name) {
    for (uint16_t i = 0; i < used; i++)
        if (inodes[i].name[0] && inodes[i].parent == dir && fs_cmp(inodes[i].name, name) == 0)
            return &inodes[i];
        return 0;
}


int ramfs_mkdir(const char *name) {
    if (fs_len(name) >= RAMFS_NAME || ramfs_find(cwd, name)) return -1;
    for (uint16_t i = 0; i < RAMFS_MAX; i++) {
        if (!inodes[i].name[0]) {
            inodes[i].type = 0; inodes[i].size = 0; inodes[i].parent = cwd;
            fs_cpy(inodes[i].name, name);
            if (i >= used) used = i + 1;
            return i;
        }
    }
    return -1;
}

int ramfs_touch(const char *name) {
    if (fs_len(name) >= RAMFS_NAME || ramfs_find(cwd, name)) return -1;
    for (uint16_t i = 0; i < RAMFS_MAX; i++) {
        if (!inodes[i].name[0]) {
            inodes[i].type = 1; inodes[i].size = 0; inodes[i].parent = cwd;
            fs_cpy(inodes[i].name, name);
            if (i >= used) used = i + 1;
            return i;
        }
    }
    return -1;
}

void ramfs_ls(void) {
    for (uint16_t i = 0; i < used; i++) {
        if (!inodes[i].name[0] || inodes[i].parent != cwd) continue;
        dprint(inodes[i].name);
        dprint(inodes[i].type ? "\n" : "/\n");
    }
}
int ramfs_cat(const char *name) {
    struct ramfs_inode *f = ramfs_find(cwd, name);
    if (!f || f->type != 1) return -1;
    for (uint16_t i = 0; i < f->size; i++) dputchar(f->data[i]);
    dputchar('\n');
    return 0;
}
int ramfs_rm(const char *name) {
    for (uint16_t i = 1; i < used; i++) {
        if (inodes[i].parent == cwd && fs_cmp(inodes[i].name, name) == 0) {
            if (inodes[i].type == 0) return 0; //fuck folders lets make them fucking immune :)
            inodes[i].name[0] = 0; //send your files to the shadow realm >:3
            return 0;
        }
    }
    return -1; //nothing to kill get skill issued
}

int ramfs_write(const char *name, const char *data, int len) {

    struct ramfs_inode *f = ramfs_find(cwd, name);
    if (!f) {
        if (ramfs_touch(name) < 0) return -1;
        f = ramfs_find(cwd, name);
    }
    if (f->type != 1) return 0;
    if (len > RAMFS_DATA) len = RAMFS_DATA;
    for (int i = 0; i < len; i++) f->data[i] = data[i];
    f->size = (uint16_t)len;
    return 0;
}
static int ramfs_add(const char *name, uint8_t type, const char *data, int size) {
    if (used >= RAMFS_MAX || fs_len(name) >= RAMFS_NAME) return -1;
    inodes[used].type = type;
    inodes[used].size = (type == 1 && size <= RAMFS_DATA) ? (uint16_t)size : 0;
    inodes[used].parent = cwd;
    fs_cpy(inodes[used].name, name);
    for (int i = 0; i < inodes[used].size; i++) inodes[used].data[i] = data[i];
    return used++;
}
void ramfs_load_tar(uint64_t addr, uint64_t size) {
    uint64_t pos = addr, end = addr + size;
    while (pos + 512 <= end) {
        const char *h = (const char *)pos;
        if (h[0] == 0 || !fs_prefix(h + 257, "ustar")) break;
        int fsize = fs_oct(h + 124, 12);
        char type = h[156];
        const char *name = h;
        while (*name == '.' || *name == '/') name++;
        if (type == '5')  ramfs_mkdir(name);
        else if (type == '0') ramfs_add(name, 1, h + 512, fsize);
        pos += 512 + ((fsize + 511) & ~511);
    }
}
