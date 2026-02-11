#include <Arduino.h>
#include <SPI.h>
#include "mcp4921.h"

#define PIN_CS 1
#define PIN_LDAC 26
#define PIN_DEBUG 27

const float PERIOD_US = 50;

bool run = true;
float freq = 100.0;
float amp = 1.65;
float offset = 1.65;
float coefficient = 0.0;
int count = 0;
float value_f = 0.0;
uint16_t value = 0;

char uart_buf[50];

// 通信速度1Mbps、MSBファースト、モード0
SPISettings settings(4000000, MSBFIRST, SPI_MODE0);
MCP4921 dac(PIN_CS, PIN_LDAC);

struct repeating_timer st_timer;
bool timerFlag = false;

// タイマー割り込み処理
bool Timer(struct repeating_timer *t) {
  timerFlag = true;
  return true;
}

void setup() {
  Serial.begin(115200);

  // GPIOの初期化
  pinMode(PIN_DEBUG, OUTPUT);
  digitalWrite(PIN_DEBUG, LOW);

  // SPI通信の初期化
  SPI.setSCK(PIN_SPI0_SCK);
  SPI.setTX(PIN_SPI0_MOSI);
  SPI.setRX(PIN_SPI0_MISO);
  SPI.begin();

  // DACの初期化
  dac.begin(SPI, settings);

  // タイマーの初期化(割込み間隔はusで指定)
  add_repeating_timer_us(PERIOD_US, Timer, NULL, &st_timer);

  float dt = PERIOD_US / 1000000.0;
  coefficient = 2.0 * 3.141592 * freq * dt;
}

void loop() {
  if (timerFlag && run) {
    timerFlag = false;
    digitalWrite(PIN_DEBUG, HIGH);
    value_f = amp * sin(coefficient * (float)(count)) + offset;
    value = (uint16_t)(value_f * 1241.212121);  // 4096 / 3.3V = 1241.212121

    if (value > 4095) value = 4095;
    if (value < 0) value = 0;

    // SPI通信でDACにデータを送信
    dac.write(value);

    // カウントアップ
    count++;

    digitalWrite(PIN_DEBUG, LOW);
  }
}

void loop1(){
  // シリアルコマンドの処理
  if(Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("FREQ ")) {
      freq = command.substring(5).toFloat();
      float dt = PERIOD_US / 1000000.0;
      coefficient = 2.0 * 3.141592 * freq * dt;
      count = 0;
      Serial.print("Frequency set to: ");
      Serial.println(freq);
    } else if (command.startsWith("AMP ")) {
      amp = command.substring(4).toFloat();
      Serial.print("Amplitude set to: ");
      Serial.println(amp);
    } else if (command.startsWith("OFFSET ")) {
      offset = command.substring(7).toFloat();
      Serial.print("Offset set to: ");
      Serial.println(offset);
    } else if (command == "STOP") {
      run = false;
      Serial.println("Waveform generation stopped.");
    } else if (command == "START") {
      run = true;
      Serial.println("Waveform generation started.");
    } else {
      Serial.println("Unknown command.");
    }
  }
}