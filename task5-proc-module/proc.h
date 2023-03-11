#ifndef PROC_H
#define PROC_H

#define PROC_DIR "proc_module"
#define PROC_GPIO_STATUS "gpio_status"
#define PROC_TIMEOUT "timeout"

int proc_init(void);

void proc_exit(void);

#endif //! PROC_H
