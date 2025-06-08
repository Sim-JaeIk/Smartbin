#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define TRIG 23
#define ECHO 24
#define SERVO 18
#define LED_R 5
#define LED_G 6
#define LED_B 13
#define BUZZER 25

volatile int lid_open = 0;

int angle_to_pulse(int angle) {
    return 500 + (angle * 2000 / 180);
}

float get_distance() {
    gpioWrite(TRIG, 0);
    usleep(2000);
    gpioWrite(TRIG, 1);
    usleep(10);
    gpioWrite(TRIG, 0);

    while (gpioRead(ECHO) == 0);
    uint32_t start = gpioTick();
    while (gpioRead(ECHO) == 1);
    uint32_t end = gpioTick();

    return (end - start) / 58.0;
}

void set_rgb(int r, int g, int b) {
    gpioPWM(LED_R, r);
    gpioPWM(LED_G, g);
    gpioPWM(LED_B, b);
}

void rainbow_cycle(int step) {
    switch (step % 6) {
        case 0: set_rgb(255, 0, 0); break;     // Red
        case 1: set_rgb(255, 127, 0); break;   // Orange
        case 2: set_rgb(255, 255, 0); break;   // Yellow
        case 3: set_rgb(0, 255, 0); break;     // Green
        case 4: set_rgb(0, 0, 255); break;     // Blue
        case 5: set_rgb(139, 0, 255); break;   // Violet
    }
}

void* melody_thread(void* arg) {
    int notes[] = {262, 294, 330, 392};
    int duration = 300000;

    while (lid_open) {
        for (int i = 0; i < 4; i++) {
            if (!lid_open) break;
            gpioSetPWMfrequency(BUZZER, notes[i]);
            gpioPWM(BUZZER, 128);
            usleep(duration);
            gpioPWM(BUZZER, 0);
            usleep(50000);
        }
    }

    gpioPWM(BUZZER, 0);
    return NULL;
}

int main() {
    if (gpioInitialise() < 0) {
        printf("pigpio 초기화 실패\n");
        return 1;
    }

    gpioSetMode(TRIG, PI_OUTPUT);
    gpioSetMode(ECHO, PI_INPUT);
    gpioSetMode(SERVO, PI_OUTPUT);
    gpioSetMode(LED_R, PI_OUTPUT);
    gpioSetMode(LED_G, PI_OUTPUT);
    gpioSetMode(LED_B, PI_OUTPUT);
    gpioSetMode(BUZZER, PI_OUTPUT);

    gpioWrite(TRIG, 0);
    set_rgb(0, 0, 0);
    gpioPWM(BUZZER, 0);
    gpioServo(SERVO, angle_to_pulse(0));

    printf("🚮 스마트 쓰레기통 시작됨\n");

    while (1) {
        float dist = get_distance();
        printf("거리: %.2f cm\n", dist);

        if (dist < 30.0 && !lid_open) {
            printf("손 감지됨 → 뚜껑 열림 + 초록불 + 동요 시작\n");

            lid_open = 1;
            gpioServo(SERVO, angle_to_pulse(90));
            set_rgb(0, 255, 0);  // 초록불
            sleep(1); // 1초 정도 초록불 유지

            pthread_t melody_tid;
            pthread_create(&melody_tid, NULL, melody_thread, NULL);

            time_t start = time(NULL);
            int rainbow_step = 0;

            while (1) {
                rainbow_cycle(rainbow_step++);
                usleep(200000); // 색 전환 0.2초마다

                float d = get_distance();
                if (d < 30.0) {
                    start = time(NULL); // 타이머 리셋
                    printf("손 다시 감지됨 → 타이머 리셋\n");
                }

                if (difftime(time(NULL), start) > 6.0) break;
            }

            lid_open = 0;
            pthread_join(melody_tid, NULL); // 부저 멈춤

            printf("뚜껑 닫힘 + 빨간불\n");
            gpioServo(SERVO, angle_to_pulse(0));
            set_rgb(255, 0, 0);
            sleep(2);
            set_rgb(0, 0, 0); // LED OFF
        }

        sleep(1);
    }

    gpioTerminate();
    return 0;
}
