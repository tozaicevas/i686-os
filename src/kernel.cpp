#include <stdint.h>
#include <terminal.h>
#include <debug.h>
#include <multiboot2.h>
#include <system.h>
#include <keyboard.h>
#include <string.h>
#include <system.h>
#include <idt.h>
#include <keyboard.h>
#include <gdt.h>
#include <pic.h>
#include <timer.h>

using namespace terminal;

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

const uint32_t CPUID_FLAG_MSR = 1 << 5;
 
bool cpuHasMSR() {
   uint32_t a, d; // eax, edx
   cpuid(1, &a, &d);
   return d & CPUID_FLAG_MSR;
}

extern "C"
void kmain(uint64_t addr) {
	multiboot_framebuffer *framebuffer = get_framebuffer(addr);

	if (framebuffer)
		terminal::init(*framebuffer);
	else {
		qemu_printf("multiboot_framebuffer not found");
		return;
	}

	VGA_MODE vga_mode = terminal::get_vga_mode();

	if (vga_mode != VGA_MODE::GRAPHICS) {
		kprintf("Only graphics mode is supported by the OS.");
		return;
	}

	canvas_t screen = terminal::get_screen_canvas();
	kprintf("Graphics initialized: %d x %d x %d\n", screen.width, screen.height, screen.bytes_per_pixel * 8);

	gdt::init();
	kprintf("GDT initialized\n");

	idt::init();
	kprintf("IDT initialized\n");

	pic::init();
	kprintf("PIC initialized\n");

	keyboard::init();
	kprintf("PS/2 Keyboard initialized\n");

	timer::init();
	kprintf("PIT ticks initialized\n");


	halt();

}
