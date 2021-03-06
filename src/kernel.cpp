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
#include <memory.h>
#include <paging.h>
#include <elf.h>
#include <fs.h>
#include <scheduler.h>

using namespace terminal;

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

bool cpu_has_MSR();
void enable_cache();

extern "C"
void kmain(uint64_t multiboot_addr) {
	multiboot_framebuffer *framebuffer = get_framebuffer(multiboot_addr);
	multiboot_basic_memory_information *meminfo = get_basic_meminfo(multiboot_addr);
	uint16_t modules_count = get_modules_count(multiboot_addr);

	if (framebuffer)
		terminal::init(*framebuffer);
	else {
		qemu_printf("multiboot_framebuffer not found");
		halt();
	}

	VGA_MODE vga_mode = terminal::get_vga_mode();

	if (vga_mode != VGA_MODE::GRAPHICS) {
		kprintf("Only graphics mode is supported by the OS.");
		halt();
	}

	canvas_t screen = terminal::get_screen_canvas();
	kprintf("Graphics initialized: %d x %d x %d\n", screen.width, screen.height, screen.bytes_per_pixel * 8);

	memory::init(multiboot_addr);
	kprintf("Memory manager initialized\n");

	gdt::init();
	kprintf("GDT initialized\n");

	idt::init();
	kprintf("IDT initialized\n");

	pic::init();
	kprintf("PIC initialized\n");

	keyboard::init();
	kprintf("PS/2 Keyboard initialized\n");

	kprintf("Available RAM: %uMB\n", memory::get_available_ram_mb());
	memory::print_kernel_memory();

	fs::init(modules_count);

	for (uint8_t i = 0; i<modules_count; i++) {
		multiboot_module *module = get_module(multiboot_addr, i);
		fs::add_file(module);
	}

	memory::update_placement_address(fs::get_furthest_file_address());

	fs::init_hdd();

	kprintf("File system initialized\n");

	scheduler::init();
	kprintf("Scheduler initialized\n");

	timer::init();
	kprintf("PIT ticks initialized\n");

	kprintf("MSR support: %s\n", cpu_has_MSR() ? "yes" : "no");

	enable_cache();
	kprintf("CPU Cache: enabled\n");

	terminal::init_user_shell();

	// kprintf("module by name: %s\n", fs::get_file_by_name("call_interrupt")->string);
	// typedef void (*call_module_t)(void);
    // kprintf("module: %s\n", get_module(multiboot_addr, 2)->string);
    // call_module_t start_program = (call_module_t) get_module(multiboot_addr, 2)->mod_start;
    // start_program();

	halt();

}

bool cpu_has_MSR() {
	const uint32_t CPUID_FLAG_MSR = 1 << 5;
 
	uint32_t ax, dx;
	cpuid(1, &ax, &dx);
	return dx & CPUID_FLAG_MSR;
}

void enable_cache() {
	uint32_t cr0 = read_cr0();
	uint32_t new_cr0 = cr0 & 0x9FFFFFFF; // clearing 29 and 30 bits
	write_cr0(new_cr0);
}
