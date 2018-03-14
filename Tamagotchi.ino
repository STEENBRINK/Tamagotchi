//import libraries
#include <math.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
 #include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//define digital pins
#define NEO 2
#define BUA 3
#define BUB 4

//define analog pins
#define PTA 0
#define PTB 1
#define PTC 2
#define LCA 4
#define LCB 5



//register lcd panel
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, LCA, LCB, 6, 7, 3, POSITIVE);

//regitster Neopixel Ring6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEO, NEO_GRB + NEO_KHZ800);



//variables
int max_stats[] = {1000, 1000, 1000, 1000};
int stats[] = {1000, 500, 700, 500};
float percentages[] = {0.0, 0.0, 0.0, 0.0, 0.0};
String stat_names[] = {"HEALTH", "HAPPINESS", "HUNGER", "WEIGHT", "AGE"};
int change[] = {0, 1, 2, 0, 1}; //amount of stat changed after x amount of seconds
int potvars[3];

long max_age = 604800;
long age = 0;

int counter = 0;
int bar = 0;
int current_stat = 0;
int filler;



boolean sick = false;
boolean dead = true;
boolean debug = true;
boolean button_check = false;
boolean screens = true;
boolean update_round = false;

unsigned long current_time;
unsigned long last_time = 0;



void setup() {
  //debug
  if(debug){
    Serial.begin(9600);
  }

  //register pins
  pinMode(BUA, INPUT);
  
  //setup lcd screen
  lcd.begin(16,2); 
  lcd.backlight();

  //setup LED-Neopixel ring
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //set starting text to lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome");
  lcd.setCursor(0,1);
  lcd.print("Bitches");
}



void loop() {
  current_time = millis();
  if((current_time-last_time) > 500){
    update_round = true;
    last_time = current_time;
  }
  
  if(!dead){
    if(screens){
      getPercentages();
      potvars[0] = analogRead(PTA);
      //Serial.println(potvar);
      if(update_round){
        if(potvars[0] < 200){
            int stat = 0;
            colorWipe(strip.Color(255, 0, 0), 10, stat); // Red
            if(stat != current_stat){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(stat_names[stat]);
              current_stat = stat;
            }
            //Serial.println("RED");
        }else if(potvars[0] < 400){
            int stat = 1;
            colorWipe(strip.Color(0, 255, 0), 10, stat); // Green
            if(stat != current_stat){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(stat_names[stat]);
              current_stat = stat;
            }
            //Serial.println("GREEN");
        }else if(potvars[0] < 600){
            int stat = 2;
            colorWipe(strip.Color(0, 0, 255), 10, stat); // Blue
            if(stat != current_stat){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(stat_names[stat]);
              current_stat = stat;
            }
            //Serial.println("BLUE");
        }else if(potvars[0] < 800){
            int stat = 3;
            colorWipe(strip.Color(255, 255, 0), 10, stat); // Yellow
            if(stat != current_stat){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(stat_names[stat]);
              current_stat = stat;
            }
            //Serial.println("YELLOW");
        }else{
            int stat = 4;
            colorWipe(strip.Color(233, 0, 255), 10, stat); // Purple
            if(stat != current_stat){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(stat_names[stat]);
              current_stat = stat;
            }
            //Serial.println("PURPLE");
        }
      }
    }else{
      for(int i=0; i<12; i++){
        strip.setPixelColor(i,0,0,0);
      }
      strip.show();
    }
    if(update_round){
        age=age+1000;
        Serial.println(age);
    }
  }
  buttonPressed();
  update_round = false;
  if(!dead){
    if(age >= max_age){
      dead = true;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("GOOD JOB!");
      lcd.setCursor(0,1);
      lcd.print("MAX SCORE REACHED");
    }
  }
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, int wait, int type) {
  if (current_stat != type) {
    for(int i=0; i<12; i++){
      strip.setPixelColor(i,0,0,0);
      }
    strip.show();
  }
  int y = round(12 * percentages[type]);
  //Serial.println(y);
  for(int i=0; i<y; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}



//gets thet percentages of all the health bars
void getPercentages(){
  for(int i = 0; i < 4; i++){
    percentages[i]=(float)stats[i]/(float)max_stats[i];
    //Serial.println(percentages[i]);
  }
  if(age%1000 == 0){
    percentages[4]=(float)age/(float)max_age;
  }
}



void buttonPressed(){
    if(digitalRead(BUA)){
        Serial.println("Button A Pressed");
        for(int i = 0; i < 100; i++){ 
            delay(10); 
            if(!(digitalRead(BUA))){
                button_check = true;
                Serial.println("true");
                break;
            }
        }
        if(button_check){
          if(dead){
            dead = false;
          }else{
            Serial.println("Turning Screens Off/On");
            screens = !screens;
            Serial.println(screens);
          }
        }else{
            lcd.clear();
            lcd.print("KILLED");
            while(digitalRead(BUA)){filler=0;}
            dead = true;
        }
        if(screens){
          lcd.backlight();
          current_stat = 6;
        }else{
          lcd.clear();
          lcd.noBacklight();
        }
    }
    button_check = false;
}
