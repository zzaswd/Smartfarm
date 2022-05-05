//===========================
// LCD declare
//===========================
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); // (주소,열,행)

int Clear_compare= 0; //Lcd Clear 적재적소에 할 수 있도록 기준 설정

//===========================
// DHT declare
//===========================
#include <DHT.h>
#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

int h;              // 측정된 Humidity 값
int t;              // 측정된 Temperature 값
int set_humi = 70;  // 기준 Humidity 값
#define plus_pin 4  // 기준 Humidity Plus Switch
#define minus_pin 5 // 기준 Humidity Minus Switch
#define Buzzer 10   // 기준 Humidity 초과시 알림

unsigned long push_time=0; // plus , minus 버튼 클릭 시 채터링 방지

//===========================
// Interrupt declare
//===========================
volatile byte state = HIGH;

const int duration = 1000;  // 채터링 방지
unsigned long pre_time = 0; // 채터링 방지
unsigned long cur_time = 0; // 채터링 방지

void isr(){
  Clear_compare = 0;
  cur_time = millis();
  if(cur_time - pre_time >= duration){
     Serial.println("인터럽트 발생");
     pre_time = cur_time;
     state = !state;
  }
}

void setup() {
//===========================
// LCD setup
//===========================
  lcd.init(); // 초기화 해주고
  lcd.backlight(); // 백라이트 켜주고

//===========================
// DHT setup
//===========================
  pinMode(plus_pin,INPUT);
  pinMode(minus_pin,INPUT);
  pinMode(Buzzer,OUTPUT);
  
//===========================
// Interrupt setup
//===========================
  attachInterrupt(0,isr,FALLING);
}


//===========================
// LCD Function
//===========================
void lcd_working_pan(void){
      digitalWrite(Buzzer,HIGH);
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("you need to do");
      lcd.setCursor(1,1);
      lcd.print("working pan!!");
      delay(1000);
      Clear_compare = 0;
}

void lcd_normal(void){
      digitalWrite(Buzzer,LOW);
      Check_clear();
      lcd.setCursor(1,0); lcd.print("Humi : "); lcd.print(h); lcd.print(" ");
      lcd.setCursor(1,1); lcd.print("Temp : "); lcd.print(t); lcd.print("'");
}

void lcd_setup(void){
      lcd.setCursor(1,0); 
      lcd.print("Set_Humi : ");
      lcd.print(set_humi);
}

void Check_clear(void){
      if(Clear_compare == 0){
        lcd.clear(); 
        Clear_compare = 1;
      }
}

//===========================
// DHT Function
//===========================
void Button_check(void){
      if(millis()-push_time>=300){
        if(digitalRead(plus_pin)==LOW){
          set_humi +=1;
        }
        else if(digitalRead(minus_pin)==LOW){
          set_humi -=1;
        }
        push_time = millis();
      }
}

void loop() {
  if(state ==HIGH){            // setup 모드냐 아니냐?
    h = dht.readHumidity();    
    t = dht.readTemperature();
    if(h >= set_humi)  lcd_working_pan();
    else               lcd_normal();
  }
  
  else{                         // setup 모드 시작
    digitalWrite(Buzzer,LOW);   // setup 모드로 들어오면 부저 끄기.
    Check_clear();              // clear 확인 후 lcd clear 여부 결정
    Button_check();             // set_humi 변경
    lcd_setup();                // set_humi 출력
  }
}
