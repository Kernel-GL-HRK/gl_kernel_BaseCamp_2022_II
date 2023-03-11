#ifndef GPIO_H
#define GPIO_H

#define TIMEOUT 1000

unsigned int gpio_get_status(void);

void gpio_blink_on(void);

void gpio_blink_off(void);

int gpio_init(unsigned int gpio);

void gpio_exit(void);

#endif //! GPIO_H
