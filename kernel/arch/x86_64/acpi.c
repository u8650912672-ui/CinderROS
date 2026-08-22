#include <kernel.h>
#include <io.h>

static uint8_t acpi_cksum(uint8_t *addr, int len) {
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) sum += addr[i];
    return sum == 0;
}
static uint32_t find_rsdp(void) {
    for (uint32_t p = 0x000E0000; p < 0x00100000; p += 16) {
        if (*(uint64_t *)p == 0x2052545020445352ULL && acpi_cksum((uint8_t *)p, 36))
            return p;
    }
    return 0;
}
static int parse_s5(uint32_t dsdt) {
    if (!dsdt) return 5;
    uint32_t len = *(uint32_t *)(dsdt + 4);
    uint8_t *data = (uint8_t *)(dsdt + 44);
    uint32_t max = len - 44;
    for (uint32_t i = 0; i + 6 < max; i++) {
        if (data[i] == 0x08 && data[i+1] == '_' && data[i+2] == 'S' &&
            data[i+3] == '5' && data[i+4] == '_' && data[i+5] == 0x12) {
            uint32_t j = i + 6;
            while (j < max && (data[j] & 0xC0) == 0x40) j++;
            j++;
            if (j < max && data[j] == 0x0A) {
                j++;
                if (j < max && (data[j] & 0xF0) == 0xA0)
                    return data[j] & 0x0F;
            }
            break;
        }
    }
    return 5;
}
int acpi_shutdown(void) { //im so smart i used 32 bit shutdown not 64 :D
    uint32_t rsdp = find_rsdp();
    if (!rsdp) return -1;
    uint64_t xsdt = *(uint64_t *)(rsdp+ 24);
    if (!xsdt) {
        uint32_t rsdt = *(uint32_t *)(rsdp + 16);
        if (!rsdt) return -1;
         uint32_t len = *(uint32_t *)(rsdt + 4);
        uint32_t entries = (len - 36) / 4;
        uint16_t pm1a = 0;
        uint32_t dsdt = 0;
        for (uint32_t i = 0; i < entries; i++) {
            uint32_t tbl = *(uint32_t *)(rsdt + 36 + i * 4);
            if (*(uint32_t *)tbl == 0x50434146) {
                pm1a = *(uint16_t *)(tbl + 0x18);
                dsdt = *(uint32_t *)(tbl + 40);
                break;
            }
        }
        if (!pm1a) return -1;
        int slp = parse_s5(dsdt);
        uint16_t val = (uint16_t)((slp << 10) | (1 << 13));
        outw(pm1a, val);
        return 0;
    }
    uint32_t xsdt_len = *(uint32_t *)((uint32_t)xsdt + 4);
    uint32_t entries = (xsdt_len - 44) / 8;
    uint16_t pm1a = 0;
    uint32_t dsdt = 0;
    for (uint32_t i = 0; i < entries; i++) {
        uint64_t tbl = *(uint64_t *)((uint32_t)xsdt + 44 + i * 8);
        if (*(uint32_t *)tbl == 0x50434146) {
            pm1a = *(uint16_t *)((uint32_t)tbl + 0x18);
            dsdt = *(uint32_t *)((uint32_t)tbl + 40);
            break;
        }
    }
    if (!pm1a) return -1;
    int slp = parse_s5(dsdt);
    uint16_t val = (uint16_t)((slp << 10) | (1 << 13));
    outw(pm1a, val);
    return 0;
}