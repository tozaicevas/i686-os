#include <scheduler.h>
#include <terminal.h>
#include <fs.h>
#include <multiboot2.h>
#include <debug.h>
#include <terminal.h>
#include <string.h>
#include <system.h>

const uint8_t MAX_PROCESS_COUNT = 10;

using namespace terminal;

namespace scheduler {
    uint8_t alive_process_count = 0;
    process_t processes[MAX_PROCESS_COUNT];
    uint8_t active_process_index = 0;
    bool is_shell_mode = true;

    void move_out_of_shell_mode();

    void init() {
        for (uint8_t i=0; i<MAX_PROCESS_COUNT; i++) {
            processes[i] = EMPTY_PROCESS;
        }
    }

    // corresponds to processes[id]
    uint8_t generate_process_id() {
        for (uint8_t i=0; i<MAX_PROCESS_COUNT; i++) {
            if (processes[i].is_ended)
                return i;
        }
        return 254; // if we get there, something went terribly wrong
    }

    uint8_t add_process(char *file_name) {
        multiboot_module *file = fs::get_file_by_name(file_name);
        if (!file)
            return 254;

        if (is_shell_mode) {
            move_out_of_shell_mode();
        }

        uint8_t id = generate_process_id();

        process_t new_process = {
            id,
            file_name,
            file->mod_start,
            false
        };

        processes[id] = new_process;
        alive_process_count++;
        return id;
    }

    void kill_process(uint8_t id) {
        processes[id].is_ended = true;
        alive_process_count--;
        if (alive_process_count == 0) {
            move_to_shell_mode();
        }
    }

    void on_keyboard_terminate_interrupt(uint8_t number) {
        kill_process(number);
    }

    static void execute_code(uint32_t addr) {
        typedef void (*call_t)(void);
        call_t execute = (call_t) addr;
        qemu_printf("executing");
        execute();
    }

    static process_t *get_next_process_to_execute() {
        uint8_t next_alive_index = active_process_index + 1;
        while (processes[next_alive_index % MAX_PROCESS_COUNT].is_ended) {
            next_alive_index++;
        }
        qemu_printf("get_next_process_to_execute active_process_index was ");
        qemu_printf(itoa(active_process_index));
        qemu_printf(", switched to ");
        qemu_printf(itoa(next_alive_index % MAX_PROCESS_COUNT));
        qemu_printf("\n");
        return &processes[next_alive_index % MAX_PROCESS_COUNT];
    }

    void do_switch() {
        if (is_shell_mode && alive_process_count > 0) {
            move_out_of_shell_mode();
        }
        else if (!is_shell_mode && alive_process_count == 0) {
            move_to_shell_mode();
            return;
        }
        if (alive_process_count == 0)
            return;
        process_t *next_process = get_next_process_to_execute();
        active_process_index = next_process->id;
        if (!next_process->has_started) {
            qemu_printf("hasn't started");
            kprintf("Process (id: %u) has been started\n", active_process_index);
            next_process->has_started = true;
            execute_code(next_process->entry_address);
        }
    }

    void move_to_shell_mode() {
        qemu_printf("moving to shell_mode");
        qemu_printf("\n");
        is_shell_mode = true;
        terminal::enable_user_shell();
    }

    void move_out_of_shell_mode() {
        qemu_printf("moving out of shell_mode");
        qemu_printf("\n");
        is_shell_mode = false;
        terminal::disable_user_shell();
    }

    void on_process_sys_exit() {
        kprintf("Process (id: %u) has successfully ended\n", active_process_index);
        kill_process(active_process_index);
    }

    bool get_is_shell_mode() {
        return is_shell_mode;
    }
}