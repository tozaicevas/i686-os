#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <multiboot2.h>
#include <stdarg.h>

#define TAB_LENGTH 8

namespace terminal { 
	enum VGA_MODE { TEXT, GRAPHICS, UNKNOWN };

	struct canvas_t {
		uint32_t width;
		uint32_t height;
		uint32_t bytes_per_pixel;
		uint32_t bytes_per_line;
		uint8_t *framebuffer_addr = nullptr;
	};

	extern uint16_t chars_x;
	extern uint16_t chars_y;
	extern canvas_t screen_canvas;

	void init(multiboot_framebuffer framebuffer);
	void kprintf(const char *s, ...) __attribute__ ((format (printf, 1, 2)));

	VGA_MODE get_vga_mode();
	canvas_t get_screen_canvas();

}

#endif
