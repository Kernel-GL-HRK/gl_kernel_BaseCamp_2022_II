#include <stdint.h>

/*OUTPUT*/
#define BLUE_ON_BLACK	1
#define WHITE_ON_BLUE	2
#define RED_ON_YELLOW	3
#define MAX_BUFFER_OUT			256
/*ULTRASOUND*/
#define MAX_BUFFER_CLEAN_IMG	64
#define LINE_FOR_ULTRASONIC		"%3d -|- %s\\%s/%s\n"
#define UNTRACKED_CELL			'*'
#define TRACKED_CELL			' '
#define BARRIER_CELL			'-'
#define SENS_IMG				"/^^^^\\\n"
#define MIN_LINE_LEN			4
#define ONE_MSEC_IN_USEC		1000
#define OFFSET_FROM_COORDINATE	9
#define SERVO_IMG				"<-||%3d|%3d|%3d||->"
#define SERVO_IMG_SHIFT_RIGHT		"<-| |%3d|%3d|%3d|->"
#define SERVO_IMG_SHIFT_LEFT		"<-|%3d|%3d|%3d| |->"
#define SHIFT_SERVO_LEFT			0xffffffff
#define SHIFT_SERVO_RIGHT			0xeeeeeeee
#define AUTO_CONTROL				1
#define MANUAL_CONTROL				2
#define KEY_LEFT_FOR_SERVO			4
#define KEY_RIGHT_FOR_SERVO			5
#define ESC_EXIT					27
#define ROTATE_RIGHT				0
#define ROTATE_LEFT					1
#define MAX_DISTANCE				400
/*HEADER*/
#define NON	0xff
#define GND	0xEE
#define HEADER_FIRST_COL		"%2d [%s]\t"
#define HEADER_SECOND_COL		"[%s] %2d\n"
#define NUM_FIRST_COL	1
#define NUM_SECOND_COL	2
#define NUM_HEADER_ROWS	20
#define NUM_HEADER_COLS	2
/*MENU*/
enum {
	OUT_HEAD = 1,
	SCAN_ULTRASONIC,
	SHOW_SERVO_DESC,
	SHOW_US_DESC,
	EXIT,
};
void init_color_screen(void);
void output_header(void);
void output_distance(uint32_t max_distance, uint32_t control);
void menu(void);
void show_servo_description(void);
void show_ultrasonic_description(void);
