# 🍀 스마트 자동 쓰레기통 (Smart Auto Trash Can)

**라즈베리파이 기반으로 구현된 초소형 스마트 쓰레기통**입니다.  
초음파 센서, 서보모터, RGB LED, 피에조 부저를 활용하여  
센서 감지 → 뚜껑 자동 개폐 → 무지개 LED 연출 → 동요 재생까지  
직관적이고 재미있는 반응형 쓰레기통을 만들 수 있습니다.

---

## 🔧 기능 요약

| 기능 | 설명 |
|------|------|
| 👋 **손 감지** | 초음파 센서로 30cm 이내 접근 감지 |
| 🟢 **뚜껑 열림** | 서보모터로 뚜껑 90도 열림 + 초록불 |
| 🌈 **LED 연출** | 무지개 색상 RGB LED 자동 순환 |
| 🎶 **동요 재생** | 피에조 부저로 동요 음계 반복 출력 |
| ⏱️ **타이머 리셋** | 열려 있는 중 다시 손 감지 시 타이머 초기화 |
| 🔴 **자동 닫힘** | 6초 후 자동으로 뚜껑 닫힘 + 빨간불 2초 |

---

## 🧰 사용 부품

- Raspberry Pi 4 (또는 3)
- 초음파 센서 (HC-SR04)
- 서보모터 (SG90 등)
- RGB LED (공통음극 타입)
- 피에조 부저 (Passive Buzzer 권장)
- 점퍼선, 브레드보드, 저항 등

---

## 🧾 핀 연결 (BCM 기준)

| 부품 | 핀 | Physical |
|------|----|----------|
| Trig (초음파) | GPIO 23 | Pin 16 |
| Echo (초음파) | GPIO 24 | Pin 18 |
| Servo | GPIO 18 | Pin 12 |
| LED R | GPIO 5 | Pin 29 |
| LED G | GPIO 6 | Pin 31 |
| LED B | GPIO 13 | Pin 33 |
| Buzzer | GPIO 25 | Pin 22 |
| GND | GND | Pin 6 등 |
| +5V | 5V | Pin 2 또는 4 |

---

## 🧑‍💻 실행 방법

1. pigpio 라이브러리 실행
    ```bash
    sudo pigpiod
    ```

2. 코드 컴파일
    ```bash
    gcc -o smartbin smartbin.c -lpigpio -lpthread -lrt
    ```

3. 실행
    ```bash
    sudo ./smartbin
    ```

---

## 🎼 재생 멜로디

- 도(262Hz), 레(294Hz), 미(330Hz), 솔(392Hz) 반복
- `melody_thread()` 내에서 반복 재생
- 동요는 열려 있는 동안 무한 반복되며 중간 감지 시 시간 연장됨

---

## 🗂️ 파일 구조 예시

```bash
├── smartbin.c           # 전체 C 소스 코드
├── README.md            # 설명 파일
