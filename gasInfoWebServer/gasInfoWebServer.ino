#include <ESP8266WiFi.h> // ESP 8266 와이파이 라이브러리
#include <ESP8266WebServer.h>  // ESP 8266 웹서버 라이브러리
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

#include <FS.h> //파일 시스템 라이브러리

ESP8266WebServer server(80); // 웹서버 오브젝트

#define RZERO 76.63
#define RLOAD 10.0
#define PARA 116.6020682
#define PARB 2.769034857

int analogIn = A0; // GPIO14
float PPM;
boolean stat = true;
int ledPin = 14; //위험시 작동하는 led
int ledState = LOW;

void setup()
{
  // 시리얼 세팅
  Serial.begin(115200);
  delay(10);
  Serial.println();

  // 와이파이 접속
  WiFi.begin("yourcoffee_2G", "yourcoffee1"); // 공유기 이름과 비밀번호

  SPIFFS.begin();

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) // 와이파이 접속하는 동안 "." 출력
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); // 접속된 와이파이 주소 출력

  server.on("/", handleRoot); // 루트에 접속 되었을때 실행할 함수 연결
  server.onNotFound(handleWebRequests); // 나머지 주소에 접속 되었을때 실행할 함수 연결

  server.begin(); // 웹서버 시작
  Serial.println("HTTP server started");

  // led 설정
  ledState = LOW;

  //Init LED output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
}

void handleRoot() { // 루트에 접속할때 실행할 내용
  if (stat) server.sendHeader("Location", "/main.html", true);
  else server.sendHeader("Location", "/processing.html", true);
  server.send(302, "text/html", "");
}

void loop() {
  server.handleClient();
  //가스센서 값 변수
  int air_quality = analogRead(analogIn); 
  air_quality = (1023./(float)air_quality) * 5. - 1.* RLOAD;
  
  float Resistance;
  Resistance = air_quality;

  PPM = PARA * pow((Resistance/RZERO), -PARB);

  Serial.println(PPM,1);
  delay(200);
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return; // SPIFFS에 요청한 파일 확인
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

bool loadFromSpiffs(String path) { // SPIFFS 에서 파일 확인
  String dataType = "text/plain";

  // 요청한 파일의 확장자에 따라 데이터 타입 정의
  if (SPIFFS.exists(path)) {
    // 요청한 주소에 따른 명령 처리
    if (path == "/processing.html"){
      stat = false;
      if(PPM<=450){
          path = "/happy.html";
          ledState == LOW;
          digitalWrite(ledPin, ledState);
      }
      else if(PPM<2000){
          path = "/soso.html";
          ledState == LOW;
          digitalWrite(ledPin, ledState);
      }
      else {
          path = "/die.html";
          ledState == HIGH;
          digitalWrite(ledPin, ledState);
      }
    }
    // 주소의 확장자에따라 데이터 타입 지정
    if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if (path.endsWith(".html")) dataType = "text/html";
    else if (path.endsWith(".htm")) dataType = "text/html";
    else if (path.endsWith(".css")) dataType = "text/css";
    else if (path.endsWith(".js")) dataType = "application/javascript";
    else if (path.endsWith(".png")) dataType = "image/png";
    else if (path.endsWith(".gif")) dataType = "image/gif";
    else if (path.endsWith(".jpg")) dataType = "image/jpeg";
    else if (path.endsWith(".ico")) dataType = "image/x-icon";
    else if (path.endsWith(".xml")) dataType = "text/xml";
    else if (path.endsWith(".pdf")) dataType = "application/pdf";
    else if (path.endsWith(".zip")) dataType = "application/zip";
    File dataFile = SPIFFS.open(path.c_str(), "r"); // SPIFFS 에서 파일 읽기
    if (server.hasArg("download")) dataType = "application/octet-stream";
    if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    }
    dataFile.close();
    return true;
  }
  else return false;
}
