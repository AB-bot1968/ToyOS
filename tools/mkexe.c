/*
 * mkexe.c - упаковщик связанного Ring-3 образа Toy OS в формат EXE1.
 *
 * Формат команды:
 *     mkexe RAW OUTPUT BSS_SIZE
 *
 * ВАЖНО:
 *   RAW    - уже связанный бинарный образ программы;
 *   OUTPUT - готовый файл EXE1;
 *   BSS_SIZE - размер BSS, который загрузчик должен обнулить.
 *
 * Утилита является host-программой и собирается обычным gcc W64DevKit.
 * Файлы открываются в бинарном режиме, поэтому утилита одинаково работает
 * в Windows и POSIX-окружении.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

struct exe_header {
    uint32_t magic;
    uint32_t entry;
    uint32_t image_size;
    uint32_t bss_size;
};

#define EXE_MAGIC 0x31455845u /* ASCII: "EXE1" в little-endian. */
#define EXE_ENTRY 0x00100000u

static int read_entire_file(const char *path, unsigned char **data, size_t *size)
{
    FILE *in;
    long end;
    unsigned char *buf;
    size_t want;
    size_t got;

    *data = NULL;
    *size = 0u;

    in = fopen(path, "rb");
    if (!in) {
        fprintf(stderr, "mkexe: cannot open input '%s': %s\n", path, strerror(errno));
        return 0;
    }

    if (fseek(in, 0L, SEEK_END) != 0) {
        fprintf(stderr, "mkexe: cannot seek input '%s'\n", path);
        fclose(in);
        return 0;
    }

    end = ftell(in);
    if (end < 1L) {
        fprintf(stderr, "mkexe: input '%s' is empty or too large\n", path);
        fclose(in);
        return 0;
    }

    if (fseek(in, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "mkexe: cannot rewind input '%s'\n", path);
        fclose(in);
        return 0;
    }

    want = (size_t)end;
    if ((long)want != end || want > (size_t)UINT32_MAX) {
        fprintf(stderr, "mkexe: input '%s' is too large for EXE1\n", path);
        fclose(in);
        return 0;
    }

    buf = (unsigned char *)malloc(want);
    if (!buf) {
        fprintf(stderr, "mkexe: out of memory while reading '%s'\n", path);
        fclose(in);
        return 0;
    }

    got = fread(buf, 1u, want, in);
    if (got != want) {
        fprintf(stderr, "mkexe: read failed for '%s'\n", path);
        free(buf);
        fclose(in);
        return 0;
    }

    if (fclose(in) != 0) {
        fprintf(stderr, "mkexe: close failed for '%s'\n", path);
        free(buf);
        return 0;
    }

    *data = buf;
    *size = want;
    return 1;
}

static int parse_bss(const char *text, uint32_t *value)
{
    char *end;
    unsigned long v;

    if (!text || !text[0]) {
        return 0;
    }
    errno = 0;
    end = NULL;
    v = strtoul(text, &end, 0);
    if (errno != 0 || end == text || *end != '\0' || v > (unsigned long)UINT32_MAX) {
        return 0;
    }
    *value = (uint32_t)v;
    return 1;
}

int main(int argc, char **argv)
{
    FILE *out;
    unsigned char *raw;
    size_t raw_size;
    uint32_t bss_size;
    struct exe_header h;

    if (argc != 4) {
        fprintf(stderr, "usage: mkexe RAW OUTPUT BSS_SIZE\n");
        return 2;
    }

    if (!parse_bss(argv[3], &bss_size)) {
        fprintf(stderr, "mkexe: invalid BSS_SIZE '%s'\n", argv[3]);
        return 2;
    }

    if (!read_entire_file(argv[1], &raw, &raw_size)) {
        return 1;
    }

    h.magic = EXE_MAGIC;
    h.entry = EXE_ENTRY;
    h.image_size = (uint32_t)raw_size;
    h.bss_size = bss_size;

    out = fopen(argv[2], "wb");
    if (!out) {
        fprintf(stderr, "mkexe: cannot create output '%s': %s\n", argv[2], strerror(errno));
        free(raw);
        return 1;
    }

    if (fwrite(&h, 1u, sizeof(h), out) != sizeof(h) ||
        fwrite(raw, 1u, raw_size, out) != raw_size) {
        fprintf(stderr, "mkexe: write failed for '%s'\n", argv[2]);
        fclose(out);
        free(raw);
        return 1;
    }

    if (fclose(out) != 0) {
        fprintf(stderr, "mkexe: close failed for '%s'\n", argv[2]);
        free(raw);
        return 1;
    }

    free(raw);
    printf("EXE1 created: %s, image=%lu bytes, bss=%lu bytes, entry=0x%08lx\n",
           argv[2], (unsigned long)raw_size, (unsigned long)bss_size,
           (unsigned long)EXE_ENTRY);
    return 0;
}
