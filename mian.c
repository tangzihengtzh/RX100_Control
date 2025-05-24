#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128 // OLED显示屏宽度, 单位: 像素
#define SCREEN_HEIGHT 64 // OLED显示屏高度, 单位: 像素

#define OLED_RESET    -1 // 复位引脚（未使用） 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 按键引脚定义
#define BUTTON1 4
#define BUTTON2 5
#define BUTTON3 6
#define BUTTON4 7
#define test0 8
#define test1 9


#define focus 3
#define shut 2


unsigned long interval = 5000; // 拍摄间隔时间（单位：毫秒），默认5秒
unsigned int photoCount = 0;   // 拍摄计数
unsigned int totalPhotos = 0;  // 总共拍摄数量
bool shooting = false;         // 拍摄状态
unsigned long lastShootTime = 0; // 上次拍摄时间

bool button1Pressed = false;
bool button2Pressed = false;
bool button3Pressed = false;
bool button4Pressed = false;
//bool buttonPressed = false;

int con=0;
int con1=1;

void setup() {
  // 初始化串口

  digitalWrite(shut,LOW);
  digitalWrite(focus,LOW);

  // 初始化按键引脚
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(focus,OUTPUT);
  pinMode(shut,OUTPUT);
  pinMode(test0,INPUT);
  pinMode(test1,INPUT);

  // digitalWrite(test0,LOW);
  // digitalWrite(test0,LOW);

  // 初始化OLED显示屏
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.display();
  while(digitalRead(BUTTON1) == HIGH)
  {
      if(digitalRead(BUTTON1) == LOW)
      {
        break;
      }
  }
  while(digitalRead(BUTTON1) == LOW)
  {
    ;
  }
  //delay(2000);
  display.clearDisplay();
  updateDisplay(0); // 初始化显示，传入初始倒计时时间
}

void loop() {
  handleButtons();

  if(digitalRead(test0)==LOW && con == 0)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("NO CAMERA");
    display.setCursor(0,10);
    display.println("Press b1 to continue");
    display.display();
    //continue;
    while(digitalRead(test0)==LOW && con == 0)
    {
      if(digitalRead(BUTTON1) == LOW)
      {
        while(digitalRead(BUTTON1) == LOW)
        {
          //con = 1;
          ;
        }
        con = 1;
      }
    }
  }

  if (shooting) {
    unsigned long currentMillis = millis();
    unsigned long timeToNextShoot = interval - (currentMillis - lastShootTime);

    if (currentMillis - lastShootTime >= interval) {
      takePicture();
      photoCount++;
      totalPhotos++;
      lastShootTime = currentMillis;
    }

    updateDisplay(timeToNextShoot);
  } else {
    updateDisplay(0);
  }
}

void handleButtons() {
  // 按键1：减少间隔时间
  if (digitalRead(BUTTON1) == LOW && con==1) {
    if (!button1Pressed) {
      button1Pressed = true;
      if (interval > 1000) {
        interval -= 1000;
        updateDisplay(0);
      }
    }
  } else {
    button1Pressed = false;
  }

  // 按键2：增加间隔时间
  if (digitalRead(BUTTON2) == LOW) {
    if (!button2Pressed) {
      button2Pressed = true;
      interval += 1000;
      updateDisplay(0);
    }
  } else {
    button2Pressed = false;
  }

//按键3，暂停、继续
if (digitalRead(BUTTON3) == LOW) {
    if (!button3Pressed) {
      button3Pressed = true;
      shooting = !shooting; // 切换拍摄状态

      if (shooting) {
        photoCount = 0;
        lastShootTime = millis();
        updateDisplay(interval); // 开始拍摄，更新显示为拍摄间隔
      } else {
        updateDisplay(0); // 暂停拍摄，更新显示为0
      }
    }
  } else {
    button3Pressed = false;
  }

  // 按键3：开始/暂停拍摄
  // if (digitalRead(BUTTON3) == LOW)
  // {
  //   con1=-con1;
  // }

  // if (digitalRead(BUTTON3) == LOW) {
  //   if (!button3Pressed) {
  //     button3Pressed = true;
  //     shooting = true;
  //     photoCount = 0;
  //     lastShootTime = millis();
  //     updateDisplay(interval);
  //   }
  // } else {
  //   button3Pressed = false;
  // }

  // // 按键4：暂停拍摄
  // if (digitalRead(BUTTON4) == LOW) {
  //   if (!button3Pressed) {
  //     button3Pressed = true;
  //     shooting = false;
  //     updateDisplay(0);
  //   }
  // } else {
  //   button4Pressed = false;
  // }
}

void takePicture() {
  // 按下快门按钮
  digitalWrite(focus,HIGH);
  delay(400); 
  digitalWrite(shut,HIGH);
  delay(400);
  digitalWrite(shut,LOW);
  digitalWrite(focus,LOW);
  // 释放快门按钮
}

void updateDisplay(unsigned long timeToNextShoot) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Interval: ");
  display.print(interval / 1000);
  display.println(" sec");
  display.print("Photos: ");
  display.println(photoCount);
  display.print("Total: ");
  display.println(totalPhotos);
  if (!shooting) {
    display.println("Paused\nPress b3 to continue");
  } else {
    display.print("Next in: ");
    if((timeToNextShoot / 1000)>=255)
    {
      display.print(0);
    }else{
      display.print(timeToNextShoot / 1000);
    }
    display.println(" sec");

    // 绘制进度条
    display.drawRect(0, 50, SCREEN_WIDTH, 10, SSD1306_WHITE); // 边框
    int progressBarWidth = map(timeToNextShoot, 0, interval, 0, SCREEN_WIDTH);
    display.fillRect(0, 50, progressBarWidth, 10, SSD1306_WHITE); // 填充
  }
  display.display();
}
