#ifndef __LIBS_H__
#define __LIBS_H__

#include <linux/kernel.h> 		// Для выполнения работы в ядре
#include <linux/module.h> 		// Для создания модуля. Содержит функции и определения для динамической загрузки модулей ядра
#include <linux/init.h>   		// Указывает на функции инициализации и очистки 
#include <linux/fs.h>     		// Содержит функции регистрации и удаления драйвера 
#include <linux/cdev.h>   		// Содержит необходимые функции для символьного драйвера 
#include <linux/gpio.h>   		// Для работы с пинами
#include <linux/interrupt.h> 	// Для работы с прерываниями
#include <linux/uaccess.h> 		// Предоставляет доступ к пространству пользователя 
#include <linux/kthread.h>		// Предоставляет функции для создания и управления ядром потоков
#include <linux/delay.h>		// Функции для задержки выполнения
#include <linux/kobject.h>		// Функции и макросы для работы с kobject
#include <linux/proc_fs.h>		// Для работы с procfs
#include <linux/device.h>		// Cтруктуры и функции, связанные с устройствами в ядре
#include <linux/kdev_t.h>		// Типы данных, связанные с идентификаторами устройств
#include <linux/err.h>			// Для обработки ошибок
#include <linux/ioctl.h>		// Для работы с ioctl

#endif
