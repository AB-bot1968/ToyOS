/*
 * PNG2RAW — конвертер PNG -> SPLASH.RAW для Toy OS.
 *
 * Назначение:
 *   - обычная hosted Win32-программа, собираемая GCC из W64DevKit;
 *   - не использует графические API Windows и скриптовые среды;
 *   - результат имеет фиксированный формат Toy OS:
 *         320 x 200 пикселей, 8 бит на пиксель, 64000 байт;
 *   - каждый байт RAW является индексом 256-цветной палитры VGADRV.
 *
 * Поддерживаемые PNG:
 *   bit depth = 8;
 *   color type = 0 (grayscale), 2 (RGB), 3 (indexed),
 *                4 (grayscale+alpha), 6 (RGBA);
 *   interlace = 0.
 *
 * Конвертер не использует внешние библиотеки PNG/zlib.
 * DEFLATE, CRC32 и распаковка zlib-потока реализованы непосредственно
 * в этом исходнике. Поэтому утилита собирается обычным GCC/W64DevKit
 * без zlib.h, -lz и без других сторонних зависимостей.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define OUT_WIDTH       320u
#define OUT_HEIGHT      200u
#define OUT_BYTES       (OUT_WIDTH * OUT_HEIGHT)
#define MAX_IMAGE_SIDE  8192u

struct rgba_image {
    unsigned width;
    unsigned height;
    unsigned char *rgba;
};

struct png_reader {
    const unsigned char *data;
    size_t size;
    size_t pos;
};

static uint32_t read_be32(const unsigned char *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           (uint32_t)p[3];
}

static uint16_t read_be16(const unsigned char *p) {
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static int reader_take(struct png_reader *r, size_t n, const unsigned char **out) {
    if (n > r->size - r->pos) {
        return 0;
    }
    *out = r->data + r->pos;
    r->pos += n;
    return 1;
}

static void free_rgba(struct rgba_image *img) {
    if (img != NULL) {
        free(img->rgba);
        img->rgba = NULL;
        img->width = 0u;
        img->height = 0u;
    }
}

static int append_bytes(unsigned char **buf, size_t *len, size_t *cap,
                        const unsigned char *src, size_t src_len) {
    size_t need;
    size_t new_cap;
    unsigned char *tmp;

    if (src_len > SIZE_MAX - *len) {
        return 0;
    }
    need = *len + src_len;
    if (need <= *cap) {
        memcpy(*buf + *len, src, src_len);
        *len = need;
        return 1;
    }

    new_cap = (*cap == 0u) ? 4096u : *cap;
    while (new_cap < need) {
        if (new_cap > SIZE_MAX / 2u) {
            new_cap = need;
            break;
        }
        new_cap *= 2u;
    }

    tmp = (unsigned char *)realloc(*buf, new_cap);
    if (tmp == NULL) {
        return 0;
    }
    *buf = tmp;
    *cap = new_cap;
    memcpy(*buf + *len, src, src_len);
    *len = need;
    return 1;
}

static unsigned paeth(unsigned a, unsigned b, unsigned c) {
    int p = (int)a + (int)b - (int)c;
    int pa = abs(p - (int)a);
    int pb = abs(p - (int)b);
    int pc = abs(p - (int)c);

    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

static int unfilter_rows(const unsigned char *raw, size_t raw_len,
                        unsigned width, unsigned height,
                        unsigned bpp, size_t rowbytes,
                        unsigned char **pixels_out) {
    size_t expected;
    size_t pixels_len;
    unsigned char *pixels;
    unsigned y;
    size_t src = 0u;

    if (rowbytes > SIZE_MAX / (size_t)height) {
        return 0;
    }
    pixels_len = rowbytes * (size_t)height;
    if ((rowbytes + 1u) > SIZE_MAX / (size_t)height) {
        return 0;
    }
    expected = (rowbytes + 1u) * (size_t)height;
    if (raw_len != expected) {
        return 0;
    }

    pixels = (unsigned char *)malloc(pixels_len ? pixels_len : 1u);
    if (pixels == NULL) {
        return 0;
    }

    (void)width;
    for (y = 0u; y < height; ++y) {
        unsigned filter = raw[src++];
        size_t row = (size_t)y * rowbytes;
        size_t prev = (y == 0u) ? 0u : (size_t)(y - 1u) * rowbytes;
        size_t x;

        if (filter > 4u) {
            free(pixels);
            return 0;
        }

        for (x = 0u; x < rowbytes; ++x) {
            unsigned raw_byte = raw[src++];
            unsigned left = (x >= bpp) ? pixels[row + x - bpp] : 0u;
            unsigned up = (y != 0u) ? pixels[prev + x] : 0u;
            unsigned up_left = (y != 0u && x >= bpp) ? pixels[prev + x - bpp] : 0u;
            unsigned value;

            switch (filter) {
                case 0u:
                    value = raw_byte;
                    break;
                case 1u:
                    value = (raw_byte + left) & 0xffu;
                    break;
                case 2u:
                    value = (raw_byte + up) & 0xffu;
                    break;
                case 3u:
                    value = (raw_byte + ((left + up) >> 1)) & 0xffu;
                    break;
                default:
                    value = (raw_byte + paeth(left, up, up_left)) & 0xffu;
                    break;
            }
            pixels[row + x] = (unsigned char)value;
        }
    }

    *pixels_out = pixels;
    return 1;
}

/*
 * Минимальный декодер zlib/DEFLATE.
 *
 * PNG хранит IDAT как zlib-поток. Для исключения зависимости от libz
 * реализуем здесь только то, что необходимо PNG:
 *   - zlib header/Adler-32;
 *   - DEFLATE stored blocks;
 *   - fixed Huffman blocks;
 *   - dynamic Huffman blocks;
 *   - LZ77-копирование по length/distance.
 */
struct bit_reader {
    const unsigned char *data;
    size_t size;
    size_t pos;
    uint32_t bits;
    unsigned count;
};

struct huffman_tree {
    uint16_t left[640u];
    uint16_t right[640u];
    int16_t symbol[640u];
    uint16_t nodes;
};

static int br_get_bits(struct bit_reader *br, unsigned count, unsigned *value) {
    uint32_t v;

    if (count > 16u || count == 0u) {
        return 0;
    }
    while (br->count < count) {
        if (br->pos >= br->size) {
            return 0;
        }
        br->bits |= (uint32_t)br->data[br->pos++] << br->count;
        br->count += 8u;
    }
    v = br->bits & ((1u << count) - 1u);
    br->bits >>= count;
    br->count -= count;
    *value = (unsigned)v;
    return 1;
}

static void br_align_byte(struct bit_reader *br) {
    unsigned drop = br->count & 7u;
    if (drop != 0u) {
        br->bits >>= drop;
        br->count -= drop;
    }
}

static uint32_t crc32_update_byte(uint32_t crc, unsigned char byte) {
    unsigned i;
    crc ^= (uint32_t)byte;
    for (i = 0u; i < 8u; ++i) {
        uint32_t mask = 0u - (crc & 1u);
        crc = (crc >> 1) ^ (0xedb88320u & mask);
    }
    return crc;
}

static uint32_t crc32_update(uint32_t crc, const unsigned char *data, size_t len) {
    size_t i;
    for (i = 0u; i < len; ++i) {
        crc = crc32_update_byte(crc, data[i]);
    }
    return crc;
}

static uint32_t adler32_update(uint32_t adler, unsigned char byte) {
    uint32_t a = adler & 0xffffu;
    uint32_t b = adler >> 16;
    a += byte;
    if (a >= 65521u) a -= 65521u;
    b += a;
    if (b >= 65521u) b -= 65521u;
    return (b << 16) | a;
}

static unsigned reverse_bits(unsigned value, unsigned count) {
    unsigned result = 0u;
    unsigned i;
    for (i = 0u; i < count; ++i) {
        result = (result << 1) | (value & 1u);
        value >>= 1;
    }
    return result;
}

static void huffman_init(struct huffman_tree *tree) {
    unsigned i;
    tree->nodes = 1u;
    for (i = 0u; i < 640u; ++i) {
        tree->symbol[i] = -1;
        tree->left[i] = 0u;
        tree->right[i] = 0u;
    }
}

static int huffman_add(struct huffman_tree *tree, unsigned code, unsigned length, int symbol) {
    uint16_t node = 0u;
    unsigned i;

    if (length == 0u || length > 15u) {
        return 0;
    }
    for (i = 0u; i < length; ++i) {
        uint16_t *child = ((code >> i) & 1u) ? &tree->right[node] : &tree->left[node];
        if (*child == 0u) {
            if (tree->nodes >= 640u) {
                return 0;
            }
            *child = tree->nodes++;
        }
        node = *child;
    }
    if (tree->symbol[node] != -1) {
        return 0;
    }
    tree->symbol[node] = (int16_t)symbol;
    return 1;
}

static int huffman_build(struct huffman_tree *tree,
                         const unsigned char *lengths, unsigned count) {
    unsigned counts[16];
    unsigned next[16];
    unsigned left;
    unsigned code = 0u;
    unsigned bits;
    unsigned sym;

    memset(counts, 0, sizeof(counts));
    memset(next, 0, sizeof(next));
    for (sym = 0u; sym < count; ++sym) {
        if (lengths[sym] > 15u) {
            return 0;
        }
        if (lengths[sym] != 0u) {
            ++counts[lengths[sym]];
        }
    }

    /* Проверяем, что canonical code space не переполнен. */
    left = 1u;
    for (bits = 1u; bits <= 15u; ++bits) {
        left <<= 1;
        if (counts[bits] > left) {
            return 0;
        }
        left -= counts[bits];
    }

    for (bits = 1u; bits <= 15u; ++bits) {
        code = (code + counts[bits - 1u]) << 1;
        next[bits] = code;
    }

    huffman_init(tree);
    for (sym = 0u; sym < count; ++sym) {
        unsigned length = lengths[sym];
        if (length != 0u) {
            unsigned rev = reverse_bits(next[length], length);
            if (!huffman_add(tree, rev, length, (int)sym)) {
                return 0;
            }
            ++next[length];
        }
    }
    return 1;
}

static int huffman_decode(struct bit_reader *br,
                          const struct huffman_tree *tree, unsigned *symbol) {
    uint16_t node = 0u;
    unsigned i;
    for (i = 0u; i < 15u; ++i) {
        unsigned bit;
        if (!br_get_bits(br, 1u, &bit)) {
            return 0;
        }
        node = bit ? tree->right[node] : tree->left[node];
        if (node == 0u) {
            return 0;
        }
        if (tree->symbol[node] >= 0) {
            *symbol = (unsigned)tree->symbol[node];
            return 1;
        }
    }
    return 0;
}

static int inflate_zlib(const unsigned char *src, size_t src_len,
                        unsigned char **out, size_t out_len) {
    static const unsigned length_base[29] = {
        3u,4u,5u,6u,7u,8u,9u,10u,11u,13u,15u,17u,19u,23u,27u,31u,35u,43u,51u,59u,67u,83u,99u,115u,131u,163u,195u,227u,258u
    };
    static const unsigned length_extra[29] = {
        0u,0u,0u,0u,0u,0u,0u,0u,1u,1u,1u,1u,2u,2u,2u,2u,3u,3u,3u,3u,4u,4u,4u,4u,5u,5u,5u,5u,0u
    };
    static const unsigned dist_base[30] = {
        1u,2u,3u,4u,5u,7u,9u,13u,17u,25u,33u,49u,65u,97u,129u,193u,257u,385u,513u,769u,1025u,1537u,2049u,3073u,4097u,6145u,8193u,12289u,16385u,24577u
    };
    static const unsigned dist_extra[30] = {
        0u,0u,0u,0u,1u,1u,2u,2u,3u,3u,4u,4u,5u,5u,6u,6u,7u,7u,8u,8u,9u,9u,10u,10u,11u,11u,12u,12u,13u,13u
    };
    static const unsigned cl_order[19] = {
        16u,17u,18u,0u,8u,7u,9u,6u,10u,5u,11u,4u,12u,3u,13u,2u,14u,1u,15u
    };
    unsigned char fixed_ll_lengths[288];
    unsigned char fixed_d_lengths[32];
    struct huffman_tree fixed_ll;
    struct huffman_tree fixed_d;
    unsigned char *dst;
    struct bit_reader br;
    uint32_t adler = 1u;
    int final = 0;

    if (src_len < 6u) {
        return 0;
    }
    if (src[0] != 0x78u || ((unsigned)src[0] * 256u + src[1]) % 31u != 0u || (src[1] & 0x20u) != 0u) {
        return 0;
    }

    dst = (unsigned char *)malloc(out_len ? out_len : 1u);
    if (dst == NULL) {
        return 0;
    }
    br.data = src + 2u;
    br.size = src_len - 2u;
    br.pos = 0u;
    br.bits = 0u;
    br.count = 0u;

    memset(fixed_ll_lengths, 0, sizeof(fixed_ll_lengths));
    memset(fixed_d_lengths, 0, sizeof(fixed_d_lengths));
    {
        unsigned i;
        for (i = 0u; i <= 143u; ++i) fixed_ll_lengths[i] = 8u;
        for (i = 144u; i <= 255u; ++i) fixed_ll_lengths[i] = 9u;
        for (i = 256u; i <= 279u; ++i) fixed_ll_lengths[i] = 7u;
        for (i = 280u; i <= 287u; ++i) fixed_ll_lengths[i] = 8u;
        for (i = 0u; i < 32u; ++i) fixed_d_lengths[i] = 5u;
    }
    if (!huffman_build(&fixed_ll, fixed_ll_lengths, 288u) ||
        !huffman_build(&fixed_d, fixed_d_lengths, 32u)) {
        free(dst);
        return 0;
    }

    {
        size_t produced = 0u;
        while (!final) {
            unsigned bfinal, btype;
            if (!br_get_bits(&br, 1u, &bfinal) || !br_get_bits(&br, 2u, &btype)) {
                free(dst);
                return 0;
            }
            final = (bfinal != 0u);

            if (btype == 0u) {
                unsigned len, nlen;
                br_align_byte(&br);
                if (!br_get_bits(&br, 16u, &len) || !br_get_bits(&br, 16u, &nlen) || ((len ^ 0xffffu) != nlen)) {
                    free(dst);
                    return 0;
                }
                if ((size_t)len > out_len - produced) {
                    free(dst);
                    return 0;
                }
                while (len != 0u) {
                    unsigned v;
                    if (!br_get_bits(&br, 8u, &v)) {
                        free(dst);
                        return 0;
                    }
                    dst[produced++] = (unsigned char)v;
                    adler = adler32_update(adler, (unsigned char)v);
                    --len;
                }
            } else if (btype == 1u || btype == 2u) {
                struct huffman_tree dyn_ll;
                struct huffman_tree dyn_d;
                const struct huffman_tree *ll = &fixed_ll;
                const struct huffman_tree *dd = &fixed_d;
                unsigned char ll_lengths[288];
                unsigned char d_lengths[32];

                if (btype == 2u) {
                    unsigned hlit, hdist, hclen;
                    unsigned char cl_lengths[19];
                    unsigned char all_lengths[320];
                    struct huffman_tree cl_tree;
                    unsigned i;
                    unsigned total;
                    unsigned at = 0u;

                    memset(cl_lengths, 0, sizeof(cl_lengths));
                    if (!br_get_bits(&br, 5u, &hlit) || !br_get_bits(&br, 5u, &hdist) || !br_get_bits(&br, 4u, &hclen)) {
                        free(dst);
                        return 0;
                    }
                    hlit += 257u;
                    hdist += 1u;
                    hclen += 4u;
                    if (hlit > 288u || hdist > 32u) {
                        free(dst);
                        return 0;
                    }
                    for (i = 0u; i < hclen; ++i) {
                        unsigned v;
                        if (!br_get_bits(&br, 3u, &v)) {
                            free(dst);
                            return 0;
                        }
                        cl_lengths[cl_order[i]] = (unsigned char)v;
                    }
                    if (!huffman_build(&cl_tree, cl_lengths, 19u)) {
                        free(dst);
                        return 0;
                    }
                    total = hlit + hdist;
                    memset(all_lengths, 0, sizeof(all_lengths));
                    while (at < total) {
                        unsigned sym;
                        if (!huffman_decode(&br, &cl_tree, &sym)) {
                            free(dst);
                            return 0;
                        }
                        if (sym <= 15u) {
                            all_lengths[at++] = (unsigned char)sym;
                        } else if (sym == 16u) {
                            unsigned repeat, prev;
                            if (at == 0u || !br_get_bits(&br, 2u, &repeat)) {
                                free(dst);
                                return 0;
                            }
                            repeat += 3u;
                            prev = all_lengths[at - 1u];
                            if (repeat > total - at) {
                                free(dst);
                                return 0;
                            }
                            while (repeat-- != 0u) all_lengths[at++] = (unsigned char)prev;
                        } else if (sym == 17u) {
                            unsigned repeat;
                            if (!br_get_bits(&br, 3u, &repeat)) {
                                free(dst);
                                return 0;
                            }
                            repeat += 3u;
                            if (repeat > total - at) {
                                free(dst);
                                return 0;
                            }
                            while (repeat-- != 0u) all_lengths[at++] = 0u;
                        } else if (sym == 18u) {
                            unsigned repeat;
                            if (!br_get_bits(&br, 7u, &repeat)) {
                                free(dst);
                                return 0;
                            }
                            repeat += 11u;
                            if (repeat > total - at) {
                                free(dst);
                                return 0;
                            }
                            while (repeat-- != 0u) all_lengths[at++] = 0u;
                        } else {
                            free(dst);
                            return 0;
                        }
                    }
                    memcpy(ll_lengths, all_lengths, hlit);
                    memcpy(d_lengths, all_lengths + hlit, hdist);
                    if (!huffman_build(&dyn_ll, ll_lengths, hlit) ||
                        !huffman_build(&dyn_d, d_lengths, hdist)) {
                        free(dst);
                        return 0;
                    }
                    ll = &dyn_ll;
                    dd = &dyn_d;
                }

                for (;;) {
                    unsigned sym;
                    if (!huffman_decode(&br, ll, &sym)) {
                        free(dst);
                        return 0;
                    }
                    if (sym < 256u) {
                        unsigned char v;
                        if (produced >= out_len) {
                            free(dst);
                            return 0;
                        }
                        v = (unsigned char)sym;
                        dst[produced++] = v;
                        adler = adler32_update(adler, v);
                    } else if (sym == 256u) {
                        break;
                    } else if (sym >= 257u && sym <= 285u) {
                        unsigned len_index = sym - 257u;
                        unsigned extra = length_extra[len_index];
                        unsigned extra_value = 0u;
                        unsigned length = length_base[len_index];
                        unsigned dsym, d_extra, d_extra_value = 0u, distance;
                        size_t copy_from;

                        if (extra != 0u && !br_get_bits(&br, extra, &extra_value)) {
                            free(dst);
                            return 0;
                        }
                        length += extra_value;
                        if (!huffman_decode(&br, dd, &dsym) || dsym >= 30u) {
                            free(dst);
                            return 0;
                        }
                        d_extra = dist_extra[dsym];
                        if (d_extra != 0u && !br_get_bits(&br, d_extra, &d_extra_value)) {
                            free(dst);
                            return 0;
                        }
                        distance = dist_base[dsym] + d_extra_value;
                        if (distance == 0u || (size_t)distance > produced ||
                            (size_t)length > out_len - produced) {
                            free(dst);
                            return 0;
                        }
                        copy_from = produced - distance;
                        while (length-- != 0u) {
                            unsigned char v = dst[copy_from++];
                            dst[produced++] = v;
                            adler = adler32_update(adler, v);
                        }
                    } else {
                        free(dst);
                        return 0;
                    }
                }
            } else {
                free(dst);
                return 0;
            }
        }

        if (produced != out_len) {
            free(dst);
            return 0;
        }
    }

    br_align_byte(&br);
    if (br.pos + 4u > br.size) {
        free(dst);
        return 0;
    }
    if ((((uint32_t)br.data[br.pos] << 24) |
         ((uint32_t)br.data[br.pos + 1u] << 16) |
         ((uint32_t)br.data[br.pos + 2u] << 8) |
         (uint32_t)br.data[br.pos + 3u]) != adler) {
        free(dst);
        return 0;
    }

    *out = dst;
    return 1;
}

static int read_png_rgba(const unsigned char *data, size_t size,
                         struct rgba_image *image) {
    static const unsigned char signature[8] =
        { 137u, 80u, 78u, 71u, 13u, 10u, 26u, 10u };
    struct png_reader rd;
    unsigned width = 0u;
    unsigned height = 0u;
    unsigned bit_depth = 0u;
    unsigned color_type = 0u;
    int seen_ihdr = 0;
    int seen_iend = 0;
    unsigned char palette[256u * 3u];
    unsigned palette_entries = 0u;
    unsigned char trns[256u];
    unsigned trns_len = 0u;
    unsigned trns_gray = 0u;
    unsigned trns_r = 0u;
    unsigned trns_g = 0u;
    unsigned trns_b = 0u;
    int has_trns_gray = 0;
    int has_trns_rgb = 0;
    unsigned char *idat = NULL;
    size_t idat_len = 0u;
    size_t idat_cap = 0u;
    unsigned char *raw = NULL;
    unsigned char *filtered = NULL;
    unsigned char *rgba = NULL;
    size_t rowbytes;
    unsigned channels;
    size_t raw_len;
    size_t rgba_len;
    unsigned y;

    memset(image, 0, sizeof(*image));
    if (size < sizeof(signature) || memcmp(data, signature, sizeof(signature)) != 0) {
        fprintf(stderr, "invalid PNG signature\n");
        return 0;
    }

    memset(palette, 0, sizeof(palette));
    memset(trns, 0, sizeof(trns));
    rd.data = data;
    rd.size = size;
    rd.pos = sizeof(signature);

    while (rd.pos < rd.size) {
        const unsigned char *len_ptr;
        const unsigned char *type_ptr;
        const unsigned char *chunk;
        const unsigned char *crc_ptr;
        uint32_t length;
        uint32_t wanted_crc;
        uint32_t actual_crc;
        char type[5];

        if (!reader_take(&rd, 4u, &len_ptr) ||
            !reader_take(&rd, 4u, &type_ptr)) {
            fprintf(stderr, "truncated PNG chunk header\n");
            goto fail;
        }
        length = read_be32(len_ptr);
        if ((size_t)length > rd.size - rd.pos) {
            fprintf(stderr, "truncated PNG chunk\n");
            goto fail;
        }
        if (!reader_take(&rd, (size_t)length, &chunk) ||
            !reader_take(&rd, 4u, &crc_ptr)) {
            fprintf(stderr, "truncated PNG chunk payload\n");
            goto fail;
        }

        memcpy(type, type_ptr, 4u);
        type[4] = '\0';
        wanted_crc = read_be32(crc_ptr);
        actual_crc = crc32_update(0xffffffffu, type_ptr, 4u);
        actual_crc = crc32_update(actual_crc, chunk, (size_t)length);
        actual_crc = ~actual_crc;
        if (actual_crc != wanted_crc) {
            fprintf(stderr, "PNG CRC error in %s\n", type);
            goto fail;
        }

        if (strcmp(type, "IHDR") == 0) {
            if (seen_ihdr || length != 13u) {
                fprintf(stderr, "invalid IHDR\n");
                goto fail;
            }
            width = read_be32(chunk + 0u);
            height = read_be32(chunk + 4u);
            bit_depth = chunk[8];
            color_type = chunk[9];
            if (width == 0u || height == 0u || width > MAX_IMAGE_SIDE || height > MAX_IMAGE_SIDE) {
                fprintf(stderr, "unsupported PNG dimensions: %ux%u\n", width, height);
                goto fail;
            }
            if (bit_depth != 8u || !(color_type == 0u || color_type == 2u ||
                                     color_type == 3u || color_type == 4u ||
                                     color_type == 6u)) {
                fprintf(stderr, "unsupported PNG format: bit_depth=%u color_type=%u\n",
                        bit_depth, color_type);
                goto fail;
            }
            if (chunk[10] != 0u || chunk[11] != 0u || chunk[12] != 0u) {
                fprintf(stderr, "only non-interlaced standard PNG is supported\n");
                goto fail;
            }
            seen_ihdr = 1;
        } else if (strcmp(type, "PLTE") == 0) {
            if (length == 0u || (length % 3u) != 0u || length > sizeof(palette)) {
                fprintf(stderr, "invalid PLTE\n");
                goto fail;
            }
            memcpy(palette, chunk, (size_t)length);
            palette_entries = length / 3u;
        } else if (strcmp(type, "tRNS") == 0) {
            if (color_type == 0u) {
                if (length != 2u) {
                    fprintf(stderr, "invalid grayscale tRNS\n");
                    goto fail;
                }
                trns_gray = read_be16(chunk);
                has_trns_gray = 1;
            } else if (color_type == 2u) {
                if (length != 6u) {
                    fprintf(stderr, "invalid RGB tRNS\n");
                    goto fail;
                }
                trns_r = read_be16(chunk + 0u);
                trns_g = read_be16(chunk + 2u);
                trns_b = read_be16(chunk + 4u);
                has_trns_rgb = 1;
            } else if (color_type == 3u) {
                if (length > sizeof(trns)) {
                    fprintf(stderr, "invalid indexed tRNS\n");
                    goto fail;
                }
                memcpy(trns, chunk, (size_t)length);
                trns_len = length;
            } else {
                fprintf(stderr, "tRNS is not valid for this PNG color type\n");
                goto fail;
            }
        } else if (strcmp(type, "IDAT") == 0) {
            if (!append_bytes(&idat, &idat_len, &idat_cap, chunk, (size_t)length)) {
                fprintf(stderr, "out of memory while accumulating IDAT\n");
                goto fail;
            }
        } else if (strcmp(type, "IEND") == 0) {
            if (length != 0u) {
                fprintf(stderr, "invalid IEND\n");
                goto fail;
            }
            seen_iend = 1;
            break;
        }
    }

    if (!seen_ihdr || !seen_iend || idat_len == 0u) {
        fprintf(stderr, "PNG is missing IHDR/IEND/IDAT\n");
        goto fail;
    }
    if (color_type == 3u && palette_entries == 0u) {
        fprintf(stderr, "indexed PNG requires PLTE\n");
        goto fail;
    }

    channels = (color_type == 0u) ? 1u :
               (color_type == 2u) ? 3u :
               (color_type == 3u) ? 1u :
               (color_type == 4u) ? 2u : 4u;

    if ((size_t)width > SIZE_MAX / channels) {
        fprintf(stderr, "PNG row size overflow\n");
        goto fail;
    }
    rowbytes = (size_t)width * channels;
    if ((rowbytes + 1u) > SIZE_MAX / (size_t)height) {
        fprintf(stderr, "PNG decompressed size overflow\n");
        goto fail;
    }
    raw_len = (rowbytes + 1u) * (size_t)height;
    if ((size_t)width > SIZE_MAX / (size_t)height ||
        (size_t)width * (size_t)height > SIZE_MAX / 4u) {
        fprintf(stderr, "PNG RGBA size overflow\n");
        goto fail;
    }
    rgba_len = (size_t)width * (size_t)height * 4u;

    if (!inflate_zlib(idat, idat_len, &raw, raw_len)) {
        fprintf(stderr, "unable to decompress PNG IDAT data\n");
        goto fail;
    }
    if (!unfilter_rows(raw, raw_len, width, height, channels, rowbytes, &filtered)) {
        fprintf(stderr, "invalid PNG filtered scanlines\n");
        goto fail;
    }

    rgba = (unsigned char *)malloc(rgba_len ? rgba_len : 1u);
    if (rgba == NULL) {
        fprintf(stderr, "out of memory for RGBA image\n");
        goto fail;
    }

    for (y = 0u; y < height; ++y) {
        unsigned x;
        size_t row = (size_t)y * rowbytes;
        for (x = 0u; x < width; ++x) {
            size_t src = row + (size_t)x * channels;
            size_t dst = ((size_t)y * width + x) * 4u;
            unsigned r = 0u, g = 0u, b = 0u, a = 255u;

            switch (color_type) {
                case 0u:
                    r = g = b = filtered[src];
                    if (has_trns_gray && r == trns_gray) a = 0u;
                    break;
                case 2u:
                    r = filtered[src + 0u];
                    g = filtered[src + 1u];
                    b = filtered[src + 2u];
                    if (has_trns_rgb && r == trns_r && g == trns_g && b == trns_b) a = 0u;
                    break;
                case 3u: {
                    unsigned idx = filtered[src];
                    if (idx >= palette_entries) {
                        fprintf(stderr, "indexed PNG palette index out of range\n");
                        goto fail;
                    }
                    r = palette[idx * 3u + 0u];
                    g = palette[idx * 3u + 1u];
                    b = palette[idx * 3u + 2u];
                    if (idx < trns_len) a = trns[idx];
                    break;
                }
                case 4u:
                    r = g = b = filtered[src + 0u];
                    a = filtered[src + 1u];
                    break;
                default:
                    r = filtered[src + 0u];
                    g = filtered[src + 1u];
                    b = filtered[src + 2u];
                    a = filtered[src + 3u];
                    break;
            }

            if (a < 255u) {
                /* Прозрачные пиксели смешиваем с чёрным фоном. */
                r = (r * a + 127u) / 255u;
                g = (g * a + 127u) / 255u;
                b = (b * a + 127u) / 255u;
            }

            rgba[dst + 0u] = (unsigned char)r;
            rgba[dst + 1u] = (unsigned char)g;
            rgba[dst + 2u] = (unsigned char)b;
            rgba[dst + 3u] = 255u;
        }
    }

    free(idat);
    free(raw);
    free(filtered);
    image->width = width;
    image->height = height;
    image->rgba = rgba;
    return 1;

fail:
    free(idat);
    free(raw);
    free(filtered);
    free(rgba);
    return 0;
}

static void build_vga_palette(unsigned char palette[256u][3u]) {
    unsigned r, g, b;
    unsigned idx = 0u;

    /* Первые 216 цветов: 6x6x6 cube. */
    for (r = 0u; r < 6u; ++r) {
        for (g = 0u; g < 6u; ++g) {
            for (b = 0u; b < 6u; ++b) {
                palette[idx][0] = (unsigned char)((r * 63u) / 5u);
                palette[idx][1] = (unsigned char)((g * 63u) / 5u);
                palette[idx][2] = (unsigned char)((b * 63u) / 5u);
                ++idx;
            }
        }
    }

    /* Последние 40 цветов: градации серого. */
    for (r = 0u; r < 40u; ++r) {
        unsigned v = (r * 63u) / 39u;
        palette[idx][0] = (unsigned char)v;
        palette[idx][1] = (unsigned char)v;
        palette[idx][2] = (unsigned char)v;
        ++idx;
    }
}

static unsigned to_dac6(unsigned value8) {
    return (value8 * 63u + 127u) / 255u;
}

static unsigned nearest_vga_index(unsigned r8, unsigned g8, unsigned b8,
                                  const unsigned char palette[256u][3u]) {
    unsigned r = to_dac6(r8);
    unsigned g = to_dac6(g8);
    unsigned b = to_dac6(b8);
    uint32_t best_distance = UINT32_MAX;
    unsigned best = 0u;
    unsigned i;

    for (i = 0u; i < 256u; ++i) {
        int dr = (int)r - palette[i][0];
        int dg = (int)g - palette[i][1];
        int db = (int)b - palette[i][2];
        uint32_t d = (uint32_t)(dr * dr + dg * dg + db * db);
        if (d < best_distance) {
            best_distance = d;
            best = i;
            if (d == 0u) break;
        }
    }
    return best;
}

static int convert_to_raw(const struct rgba_image *src, const char *output_path) {
    unsigned char palette[256u][3u];
    unsigned char raw[OUT_BYTES];
    unsigned y;
    FILE *f;

    build_vga_palette(palette);

    for (y = 0u; y < OUT_HEIGHT; ++y) {
        unsigned sy = (unsigned)(((uint64_t)y * src->height) / OUT_HEIGHT);
        unsigned x;
        if (sy >= src->height) sy = src->height - 1u;
        for (x = 0u; x < OUT_WIDTH; ++x) {
            unsigned sx = (unsigned)(((uint64_t)x * src->width) / OUT_WIDTH);
            size_t off;
            if (sx >= src->width) sx = src->width - 1u;
            off = ((size_t)sy * src->width + sx) * 4u;
            raw[(size_t)y * OUT_WIDTH + x] =
                (unsigned char)nearest_vga_index(src->rgba[off],
                                                  src->rgba[off + 1u],
                                                  src->rgba[off + 2u],
                                                  palette);
        }
    }

    f = fopen(output_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "unable to create output: %s\n", output_path);
        return 0;
    }
    if (fwrite(raw, 1u, sizeof(raw), f) != sizeof(raw)) {
        fclose(f);
        fprintf(stderr, "unable to write output: %s\n", output_path);
        return 0;
    }
    if (fclose(f) != 0) {
        fprintf(stderr, "unable to close output: %s\n", output_path);
        return 0;
    }
    return 1;
}

static void usage(const char *program) {
    printf("PNG2RAW — конвертер PNG -> Toy OS SPLASH.RAW\n");
    printf("Использование: %s INPUT.PNG [OUTPUT.RAW]\n", program);
    printf("Результат: 320x200, 256 цветов VGA, ровно 64000 байт.\n");
}

int main(int argc, char **argv) {
    const char *input_path;
    const char *output_path = "SPLASH.RAW";
    FILE *f;
    long file_size_long;
    size_t file_size;
    unsigned char *file_data;
    struct rgba_image image;

    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return 2;
    }

    input_path = argv[1];
    if (argc == 3) output_path = argv[2];
    if (strcmp(input_path, output_path) == 0) {
        fprintf(stderr, "PNG2RAW: ERROR: input and output must be different files.\n");
        return 1;
    }

    f = fopen(input_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "PNG2RAW: ERROR: input PNG not found: %s\n", input_path);
        return 1;
    }
    if (fseek(f, 0L, SEEK_END) != 0) {
        fclose(f);
        fprintf(stderr, "PNG2RAW: ERROR: cannot seek input.\n");
        return 1;
    }
    file_size_long = ftell(f);
    if (file_size_long < 8L) {
        fclose(f);
        fprintf(stderr, "PNG2RAW: ERROR: PNG file is too small.\n");
        return 1;
    }
    if (fseek(f, 0L, SEEK_SET) != 0) {
        fclose(f);
        fprintf(stderr, "PNG2RAW: ERROR: cannot rewind input.\n");
        return 1;
    }
    file_size = (size_t)file_size_long;
    file_data = (unsigned char *)malloc(file_size);
    if (file_data == NULL) {
        fclose(f);
        fprintf(stderr, "PNG2RAW: ERROR: out of memory for PNG.\n");
        return 1;
    }
    if (fread(file_data, 1u, file_size, f) != file_size) {
        free(file_data);
        fclose(f);
        fprintf(stderr, "PNG2RAW: ERROR: cannot read PNG.\n");
        return 1;
    }
    fclose(f);

    memset(&image, 0, sizeof(image));
    if (!read_png_rgba(file_data, file_size, &image)) {
        free(file_data);
        fprintf(stderr, "PNG2RAW: ERROR: invalid or unsupported PNG.\n");
        return 1;
    }
    free(file_data);

    if (!convert_to_raw(&image, output_path)) {
        free_rgba(&image);
        return 1;
    }

    printf("PNG2RAW: OK\n");
    printf("Input : %s\n", input_path);
    printf("Output: %s\n", output_path);
    printf("Source PNG: %ux%u\n", image.width, image.height);
    printf("Output RAW: 320x200, 256-color VGA, 64000 bytes\n");
    printf("Built with GCC/W64DevKit; no GUI runtime is required.\n");

    free_rgba(&image);
    return 0;
}
