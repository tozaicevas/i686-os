#include <font.h>

#define PSF_FONT_MAGIC 0x864ab572

extern char _binary_assets_Tamsyn10x20r_psf_start;

struct PSF_font_t {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t num_glyph;      /* number of glyphs */
    uint32_t bytes_per_glyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};

namespace font {

    uint8_t *get_bitmap(char c) {
        PSF_font_t *font = (PSF_font_t *) &_binary_assets_Tamsyn10x20r_psf_start;	
        return (uint8_t *) font + font->headersize + c * font->bytes_per_glyph;
    }

    uint8_t get_width() {
        PSF_font_t *font = (PSF_font_t *) &_binary_assets_Tamsyn10x20r_psf_start;	
        return font->width;
    }

    uint8_t get_height() {
        PSF_font_t *font = (PSF_font_t *) &_binary_assets_Tamsyn10x20r_psf_start;	
        return font->height;
    }
}
