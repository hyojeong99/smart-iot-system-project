#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <softPwm.h>

// I2C LCD 관련 설정
#define I2C_ADDR 0x27 // I2C 장치 주소
#define LCD_CHR 1     // Mode - Sending data
#define LCD_CMD 0     // Mode - Sending command
#define LINE1 0x80    // 1st line
#define LINE2 0xC0    // 2nd line
#define LCD_BACKLIGHT 0x08 // LCD 백라이트
#define ENABLE 0b00000100  // Enable bit

// 핀 설정
#define MAX_TIME 100
#define DHT11PIN 1  // DHT11 센서 핀 (WiringPi 핀 번호)
#define SERVO_PIN 0 // 서보모터 핀 (WiringPi 핀 번호)

// 조건 값
#define HUMIDITY_THRESHOLD 60  // 습도 기준값
#define TEMPERATURE_THRESHOLD 26  // 온도 기준값

// 전역 변수
int dht11_val[5] = {0, 0, 0, 0, 0};
int fd; // I2C LCD 핸들

// 함수 선언
void dht11_read_val();
void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);
void lcdLoc(int line);
void ClrLcd(void);
void typeln(const char *s);
void typeInt(int i);
void servoControl();

int main() {
    // WiringPi 초기화
    if (wiringPiSetup() == -1) {
        printf("WiringPi 초기화 실패!\n");
        return -1;
    }

    // I2C LCD 초기화
    fd = wiringPiI2CSetup(I2C_ADDR);
    lcd_init();

    // 서보모터 초기화
    softPwmCreate(SERVO_PIN, 0, 200); // 서보모터 PWM 초기화

    printf("DHT11 데이터를 읽습니다...\n");

    while (1) {
        // DHT11 데이터 읽기
        dht11_read_val();

        // LCD에 데이터 표시
        ClrLcd(); // LCD 화면 초기화
        lcdLoc(LINE1);
        typeln("Temp: ");
        typeInt(dht11_val[2]); // 온도
        typeln("C");

        lcdLoc(LINE2);
        typeln("Hum: ");
        typeInt(dht11_val[0]); // 습도
        typeln("%");

        // 조건에 따른 서보모터 동작
        if (dht11_val[0] > HUMIDITY_THRESHOLD || dht11_val[2] > TEMPERATURE_THRESHOLD) {
            printf("조건 충족 - 서보모터 동작 중...\n");
            servoControl();
        } else {
            printf("조건 미충족 - 서보모터 대기\n");
            softPwmWrite(SERVO_PIN, 0); // 서보모터 정지
        }

        sleep(1); // 1초 대기
    }

    return 0;
}

// DHT11 데이터 읽기 함수
void dht11_read_val() {
    uint8_t lststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;

    for (i = 0; i < 5; i++) dht11_val[i] = 0;

    pinMode(DHT11PIN, OUTPUT);
    digitalWrite(DHT11PIN, LOW);
    delay(18);
    digitalWrite(DHT11PIN, HIGH);
    delayMicroseconds(40);
    pinMode(DHT11PIN, INPUT);

    for (i = 0; i < MAX_TIME; i++) {
        counter = 0;
        while (digitalRead(DHT11PIN) == lststate) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) break;
        }
        lststate = digitalRead(DHT11PIN);

        if (counter == 255) break;

        if ((i >= 4) && (i % 2 == 0)) {
            dht11_val[j / 8] <<= 1;
            if (counter > 16) dht11_val[j / 8] |= 1;
            j++;
        }
    }

    if ((j >= 40) && (dht11_val[4] == ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF))) {
        printf("Humidity = %d.%d%%, Temperature = %d.%d°C\n",
               dht11_val[0], dht11_val[1], dht11_val[2], dht11_val[3]);
    } else {
        printf("Invalid Data Received\n");
    }
}

// 서보모터 제어 함수
void servoControl() {
    int position = 5; // 시작 위치
    int direction = 1; // 회전 방향

    while (1) {
        // 온습도 조건 확인
        dht11_read_val();
        if (dht11_val[0] <= HUMIDITY_THRESHOLD && dht11_val[2] <= TEMPERATURE_THRESHOLD) {
            printf("조건 미충족 - 서보모터 정지\n");
            softPwmWrite(SERVO_PIN, 0); // 서보모터 정지
            break; // 함수 탈출
        }

        // 서보모터 회전
        position += direction; // 위치 변경
        if (position > 25 || position < 5) {
            direction *= -1; // 방향 반전
        }
        softPwmWrite(SERVO_PIN, position);
        delay(20); // 움직임 속도 조절
    }
}

// I2C LCD 제어 함수
void lcd_init() {
    lcd_byte(0x33, LCD_CMD);
    lcd_byte(0x32, LCD_CMD);
    lcd_byte(0x06, LCD_CMD);
    lcd_byte(0x0C, LCD_CMD);
    lcd_byte(0x28, LCD_CMD);
    lcd_byte(0x01, LCD_CMD);
    delayMicroseconds(500);
}

void lcd_byte(int bits, int mode) {
    int bits_high, bits_low;
    bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

    wiringPiI2CReadReg8(fd, bits_high);
    lcd_toggle_enable(bits_high);
    wiringPiI2CReadReg8(fd, bits_low);
    lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits) {
    delayMicroseconds(500);
    wiringPiI2CReadReg8(fd, (bits | ENABLE));
    delayMicroseconds(500);
    wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
    delayMicroseconds(500);
}

void lcdLoc(int line) {
    lcd_byte(line, LCD_CMD);
}

void ClrLcd() {
    lcd_byte(0x01, LCD_CMD);
    lcd_byte(0x02, LCD_CMD);
}

void typeln(const char *s) {
    while (*s) lcd_byte(*(s++), LCD_CHR);
}

void typeInt(int i) {
    char buffer[20];
    sprintf(buffer, "%d", i);
    typeln(buffer);
}
