#include <Adafruit_NeoPixel.h>
#define NEO_PIN   10      // 네오픽셀 DI핀과 연결된 핀번호 설정
#define NUM_LEDS  12 
int laser = 2;

int sound = A0;  // 소리센서 연결 핀
int soundValue;   // 소리센서 저장 변수

Adafruit_NeoPixel Neo = Adafruit_NeoPixel(NUM_LEDS, NEO_PIN, NEO_GRB + NEO_KHZ800);

int ledBright = 50;   //네오픽셀 밝기(최대 255)
//네오픽셀 색상 저장
uint32_t colorValue[13] = {
   Neo.Color(255, 0, 0),
   Neo.Color(255, 94, 0),
   Neo.Color(255, 187, 0),
   Neo.Color(255, 228, 0),
   Neo.Color(171, 242, 0),
   Neo.Color(29, 219, 22),
   Neo.Color(0, 216, 255),
   Neo.Color(0, 84, 255),
   Neo.Color(1, 0, 255),
   Neo.Color(95, 0, 255),
   Neo.Color(255, 0, 221),
   Neo.Color(255, 0, 127),
   Neo.Color(0, 0, 0)
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Neo.setBrightness(ledBright);
  Neo.begin();
  Neo.show();

  pinMode(sound, INPUT);   // 소리센서 값 받기
  pinMode(laser, OUTPUT);   // 레이저 빛 출력
   
}

void loop() {
  // put your main code here, to run repeatedly:
  soundValue = analogRead(sound);   // 소리센서값 저장
  //읽은 값 시리얼 모니터에 출력
  Serial.print("sound value=");
  Serial.print(soundValue);
  
  int lightValue = map(soundValue, 0, 600, 0, 18);  //소리센서 범위를 0~11구간으로 나눔
  //읽은 값 시리얼 모니터에 출력
  Serial.print("      light value=");
  Serial.print(lightValue); 

  for (int i=0; i<lightValue; i++) {
    Neo.setPixelColor(i, colorValue[i]);
    Neo.show();
    delay(50);
  }
  for (int i=lightValue; i>-1; i--) {
    Neo.setPixelColor(i, colorValue[12]);
    Neo.show();
    delay(40);
  }
  delay(500);
}
