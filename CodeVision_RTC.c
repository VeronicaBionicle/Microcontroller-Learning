#include <mega328p.h>
#include <i2c.h>    // Функции I2C
#include <ds1307.h> // Функции DS1307
#include <stdio.h>  // Функции стандартного ввода-вывода
#include <delay.h>  // Функции ожидания

#define RS0 0
#define RS1 1 

/* Настройки для UART */
#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)

#define F_CPU 16000000UL  // Частота МК
#define BAUDRATE 9600L  // Скорость передачи UART
#define BAUDRATE_REG (F_CPU/(16*BAUDRATE)-1) 

// Инициализация UART
void init_UART()
{
	UCSR0A = 0;						                    // Асинхронный режим, 1 стоповый бит, без проверки четности
	UCSR0B = (1<<TXEN0);					            // Включаем передатчик
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);	// 8 бит данных
	UBRR0H = HI(BAUDRATE_REG);				            // Задаем скорость передачи
	UBRR0L = LO(BAUDRATE_REG);
}

// Дни недели
const char * weekdays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void main(void)
{
    // Параметры часов
    unsigned char rs = (0 << RS1)|(0 << RS0); 
    unsigned char sqwe = 1;
    unsigned char out = 0;
    
    // Дата и время
    unsigned char week_day, day, month, year; 
    unsigned char hour, minute, second;         

    // Делитель тактовой частоты: 1
    #pragma optsize-
    CLKPR=(1<<CLKPCE);
    CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
    #ifdef _OPTIMIZE_SIZE_
    #pragma optsize+
    #endif
    
    // Выключить аналоговый компаратор
    ACSR=(1<<ACD);
    // Выключить АЦП
    ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
    
    // Запустить UART
    init_UART();
    
    // Запустить I2C
    i2c_init();

    // Настройка часов
    rtc_init(rs, sqwe, out); 
                
    // Установка даты: день недели, день, месяц, год  
    rtc_set_date(6, 20, 12, 24);      
    
    // Установка времени: часы, минуты, секунды
    rtc_set_time(16, 1, 30);
    
    while (1) // смотрим время каждую секунду
    {
        rtc_get_date(&week_day,&day,&month,&year);    
        printf("%02u\\%02u\\%02u %p\r", day, month, year, weekdays[week_day-1]); 
        
        rtc_get_time(&hour,&minute,&second);
        printf("%02u:%02u:%02u\r", hour, minute, second);         
        
        delay_ms(1000);
    }
}
