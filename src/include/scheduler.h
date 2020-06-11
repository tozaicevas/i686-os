#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

namespace scheduler {

    struct process_t {
        uint16_t id;
        char *name;
        uint32_t entry_address;
        bool is_ended;
    };

    static process_t EMPTY_PROCESS = {
        0, "", 0, true
    };

    void init();

    void do_switch();
    void move_to_shell_mode();

    uint8_t generate_process_id();

    void add_process(char *file_name);
    // void start(char *file_name);
    void kill_process(uint16_t id);

    // will fire when alt+number is pressed
    void on_keyboard_terminate_interrupt(uint8_t number);

    void on_process_sys_exit();

}

#endif
