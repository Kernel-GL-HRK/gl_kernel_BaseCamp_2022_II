#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define CLASS_NAME "rotary"
#define DEVICE_NAME "rotary_encoder" // имя файла устройства
#define CDEV_FILE "/dev/rotary1"

#define BUFFER_SIZE 16      // размер буффера для выводимой информации
#define MAX_BUFFER_SIZE 16
#define PERMS 0644          // права доступа
#define MINOR_COUNT 1       // минорный номер для создаваемого устройства
#define UPDATE_INTERVAL 1   // количество секунд между updates to the user-space process

// подключенные GPIO пины
#define GPIO_PIN_A 17       // dt
#define GPIO_PIN_B 18       // clk
#define GPIO_PIN_SW 27      // sw


// переменные положения энкодера
static int prevA = -1;
static int prevB = -1;
static int currA = -1;
static int currB = -1;
static int prevAB = -1;
static int currAB = -1;
static int currBTN = -1;
static int prevBTN = -1;
static int BTNprescount = 0;
static int irq_a, irq_b, irq_sw;
static volatile int encoder_pos = 0;
static struct task_struct *task;

static int is_open; // флаг открытия файла энкодера

#endif