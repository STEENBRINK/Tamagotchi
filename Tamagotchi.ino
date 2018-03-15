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
#define LCA 4
#define LCB 5



//register lcd panel
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, LCA, LCB, 6, 7, 3, POSITIVE);

//regitster Neopixel Ring6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEO, NEO_GRB + NEO_KHZ800);



//variables
int max_stats[] = {1000, 1000, 1000, 1000};
int stats[] = {1000, 500, 300, 500};
float percentages[] = {0.0, 0.0, 0.0, 0.0, 0.0};
String stat_names[] = {"HEALTH", "HAPPINESS", "FULLNESS", "WEIGHT", "AGE"};
int change[] = {0, -2, -2, -1, 10};

String activities[] = {"FOODS", "GAMES"};
String foods[] = {"BURGER", "CARROTS", "MEDICINE"};
int f_increase[][4] = {{0, 300, 500, 300}, {100, -100, 300, 50}, {50, -200, 50, 0}};
String games[] = {"SPORTS", "COMPUTER"};
int g_increase[][4] = {{100, 300, -250, -300}, {0, 300, 0, 300}};

long max_age = 604800;
long age = 0;

int potvars[3];
int counter = 0;
int sick_counter = 9;
int bar = 0;
int current_stat = 10;
int current_selected = 10;
int filler;
int update_timer = 0;
int selector;
int health_damage = 0;

boolean sick = false;
boolean dead = true;
boolean debug = true;
boolean button_check_a = false;
boolean button_check_b = false;
boolean screens = true;
boolean update_screens = false;
boolean update_stats = false;
boolean f_active = false;
boolean g_active = false;
boolean was_alive = false;
boolean bad[3] = {false, false, false};
boolean cure = false;
boolean update_lcd = false;

unsigned long current_time;
unsigned long last_time = 0;
unsigned long sick_cooldown = 0;



void setup() {
  //debug
  if (debug) {
    Serial.begin(9600);
  }

  //register pins
  pinMode(BUA, INPUT);

  //setup lcd screen
  lcd.begin(16, 2);
  lcd.backlight();

  //setup LED-Neopixel ring
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //set starting text to lcd
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO MY");
  lcd.setCursor(0, 1);
  lcd.print("TAMAGOTCHI");

  //set sick_cooldown first time
  sick_cooldown = millis();
}



void loop() {
  current_time = millis();
  if ((current_time - last_time) > 1000) {
    update_screens = true;
    update_timer++;
    if (update_timer == 10) {
      update_stats = true;
      update_timer = 0;
    }
    last_time = current_time;
  }

  if (!dead) {
    if (screens) {
      getPercentages();
      getPotVars();
      //Serial.println(potvar);
      if (update_screens) {
        if (potvars[0] < 200) {
          int stat = 0;
          colorWipe(strip.Color(255, 0, 0), 10, stat); // Red
          if (stat != current_stat || update_lcd) {
            current_selected = 4;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Stat: " + stat_names[stat]);
            current_stat = stat;
          }
          //Serial.println("RED");
        } else if (potvars[0] < 400) {
          int stat = 1;
          colorWipe(strip.Color(0, 255, 0), 10, stat); // Green
          if (stat != current_stat || update_lcd) {
            current_selected = 4;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Stat: " + stat_names[stat]);
            current_stat = stat;
          }
          //Serial.println("GREEN");
        } else if (potvars[0] < 600) {
          int stat = 2;
          colorWipe(strip.Color(0, 0, 255), 10, stat); // Blue
          if (stat != current_stat || update_lcd) {
            current_selected = 4;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Stat: " + stat_names[stat]);
            current_stat = stat;
          }
          //Serial.println("BLUE");
        } else if (potvars[0] < 800) {
          int stat = 3;
          colorWipe(strip.Color(255, 255, 0), 10, stat); // Yellow
          if (stat != current_stat || update_lcd) {
            current_selected = 4;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Stat: " + stat_names[stat]);
            current_stat = stat;
          }
          //Serial.println("YELLOW");
        } else {
          int stat = 4;
          colorWipe(strip.Color(233, 0, 255), 10, stat); // Purple
          if (stat != current_stat || update_lcd) {
            current_selected = 4;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(stat_names[stat]);
            current_stat = stat;
          }
          //Serial.println("PURPLE");
        }
        if (potvars[1] <= 500) {
          selector = 0;
        } else if (potvars[1] > 500) {
          selector = 1;
        }
        if ((selector == 0 && potvars[1] > 333) || (selector == 1 && potvars[1] < 666)) {
          if (sick) {
            cure = true;
          }
        }
        if (f_active) {
          if (cure) {
            lcd.setCursor(0, 1);
            lcd.print(foods[2]);
            current_selected = 2;
          } else if (selector != current_selected || update_lcd) {
            update_lcd = true;
            lcd.setCursor(0, 1);
            lcd.print(foods[selector]);
            current_selected = selector;
          }
          cure=false;
          //Serial.println("CARROTS");
        } else if (g_active) {
          if (selector != current_selected) {
            update_lcd = true;
            lcd.setCursor(0, 1);
            lcd.print(games[selector]);
            current_selected = selector;
          }
          //Serial.println("COMPUTER");
        } else {
          if (selector != current_selected) {
            update_lcd = true;
            lcd.setCursor(0, 1);
            lcd.print(activities[selector]);
            current_selected = selector;
          }
          //Serial.println("FOOD");
        }
        if (sick) {
          sick_counter++;
          if (sick_counter == 10) {
            uint32_t sick_color = strip.Color(14, 237, 0);
            if (current_stat == 0) {
              colorChase(sick_color, strip.Color(255, 0, 0), 0);
            }
            sick_counter = 0;
          }
        }
      }
    } else {
      for (int i = 0; i < 12; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      strip.show();
    }
    if (update_stats) {
      updateStats();
      checkSickness();
      update_stats = false;
    }
  } else {
    if (was_alive) {
      screens = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      if (age >= max_age) {
        lcd.print("MAXSCORE REACHED");
        lcd.setCursor(0, 1);
        lcd.print("SCORE: " + age);
      } else {
        lcd.print("GAME OVER!");
        lcd.setCursor(0, 1);
        lcd.print("SCORE: " + (String)age);
      }
      Serial.println("Health: " + (String)stats[0]);
      Serial.println("Happiness: " + (String)stats[1]);
      Serial.println("Hunger: " + (String)stats[2]);
      Serial.println("Weight: " + (String)stats[3]);
      Serial.println("Age: " + (String)age);
      Serial.println("Sick: " + (String)sick);
      Serial.println("");
    }
    was_alive = false;
  }
  buttonPressed();
  update_screens = false;
  finalChecks();
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, int wait, int type) {
  if (current_stat != type) {
    for (int i = 0; i < 12; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
  }
  int y = round(12 * percentages[type]);
  //Serial.println(y);
  for (int i = 0; i < y; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


void colorChase(uint32_t c1, uint32_t c2, int type) {
  int y = round(12 * percentages[type]);
  //Serial.println(y);
  for (int i = 0; i < 13; i++) {
    if (i == 0) {
      strip.setPixelColor(i, c1);
    } if (i == 12) {
      if (i <= y) {
        strip.setPixelColor((i - 1), c2);
      } else {
        strip.setPixelColor((i - 1), 0, 0, 0);
      }
    } else {
      strip.setPixelColor(i, c1);
      if (i <= y) {
        strip.setPixelColor((i - 1), c2);
      } else {
        strip.setPixelColor((i - 1), 0, 0, 0);
      }
    }
    strip.show();
    delay(20);
  }
}


//gets thet percentages of all the health bars
void getPercentages() {
  for (int i = 0; i < 4; i++) {
    percentages[i] = (float)stats[i] / (float)max_stats[i];
    //Serial.println(percentages[i]);
  }
  if (age % 1000 == 0) {
    percentages[4] = (float)age / (float)max_age;
  }
}



void getPotVars() {
  for (int i = 0; i < 3; i++) {
    potvars[i] = analogRead(i);
  }
}



void buttonPressed() {
  if (digitalRead(BUA)) {
    //Serial.println("Button A Pressed");
    for (int i = 0; i < 100; i++) {
      delay(10);
      if (!(digitalRead(BUA))) {
        button_check_a = true;
        //Serial.println("true");
        break;
      }
    }
    if (button_check_a) {
      if (dead) {
        dead = false;
        stats[0] = 800;
        stats[1] = 500;
        stats[2] = 300;
        stats[3] = 500;
        age = 0;
      } else {
        //Serial.println("Turning Screens Off/On");
        screens = !screens;
        //Serial.println(screens);
      }
    } else {
      lcd.clear();
      lcd.print("KILLED");
      while (digitalRead(BUA)) {
        filler = 0;
      }
      dead = true;
      was_alive = true;
    }
    if (screens) {
      lcd.backlight();
      current_stat = 6;
      current_selected = 6;
    } else {
      lcd.clear();
      lcd.noBacklight();
    }
  }
  if (screens) {
    if (digitalRead(BUB)) {
      //Serial.println("Button B Pressed");
      for (int i = 0; i < 100; i++) {
        delay(10);
        if (!(digitalRead(BUB))) {
          button_check_b = true;
          //Serial.println("true");
          break;
        }
      }
      if (button_check_b) {
        if (!dead) {
          if (f_active) {
            if (current_selected == 0) {
              stats[0] = stats[0] + f_increase[0][0];
              stats[1] += f_increase[0][1];
              stats[2] += f_increase[0][2];
              stats[3] += f_increase[0][3];
            }
            if (current_selected == 1) {
              stats[0] = stats[0] + f_increase[1][0];
              stats[1] += f_increase[1][1];
              stats[2] += f_increase[1][2];
              stats[3] += f_increase[1][3];
            }
            if (current_selected == 2) {
              stats[0] = stats[0] + f_increase[1][0];
              stats[1] += f_increase[1][1];
              stats[2] += f_increase[1][2];
              stats[3] += f_increase[1][3];
              sick = false;
            }
            f_active = false;
          } else if (g_active) {
            if (current_selected == 0) {
              stats[0] = stats[0] + g_increase[0][0];
              stats[1] += g_increase[0][1];
              stats[2] += g_increase[0][2];
              stats[3] += g_increase[0][3];
            }
            if (current_selected == 1) {
              stats[0] = stats[0] + g_increase[1][0];
              stats[1] += g_increase[1][1];
              stats[2] += g_increase[1][2];
              stats[3] += g_increase[1][3];
            }
            g_active = false;
          } else if (current_selected == 0) {
            f_active = true;
            current_selected = 10;
          } else if (current_selected == 1) {
            g_active = true;
            current_selected = 10;
          }
        }
      } else {
        while (digitalRead(BUB)) {
          filler = 0;
        }
        if (g_active) {
          g_active = false;
        }
        if (f_active) {
          f_active = false;
        }
      }
      if (screens) {
        lcd.backlight();
        current_stat = 6;
        current_selected = 6;
      } else {
        lcd.clear();
        lcd.noBacklight();
      }
    }
  }
  button_check_a = false;
  button_check_b = false;
}



void updateStats() {
  for (int i = 0; i < 4; i++) {
    if (sick) {
      stats[i] += (change[i] * 2);
    } else {
      stats[i] += change[i];
    }
  }
  age += change[4];
  for (int i = 0; i < 3; i++) {
    if (bad[i]) {
      stats[0] -= 2;
    }
  }
  if ((!bad[0] && !bad[1] && !bad[2]) && !sick) {
    stats[0] += 5;
  }
  if (debug) {
    Serial.println("Health: " + (String)stats[0]);
    Serial.println("Happiness: " + (String)stats[1]);
    Serial.println("Hunger: " + (String)stats[2]);
    Serial.println("Weight: " + (String)stats[3]);
    Serial.println("Age: " + (String)age);
    Serial.println("Sick: " + (String)sick);
    Serial.println("");
  }
}


void checkSickness() {
  unsigned long tmp = (current_time - sick_cooldown);
  long cd = 3600000;
  Serial.println(tmp);
  if (tmp > cd) {
    int chance = 10;
    if (!sick) {
      for (int i = 0; i < 3; i++) {
        if (bad[i]) {
          chance += 10;
        }
      }
      int random_chance = random(0, 100);
      if (random_chance < chance) {
        sick = true;
        Serial.println("GOTTEN SICK");
        sick_cooldown = millis();
      } else {
        Serial.println("NOT GOTTEN SICK");
      }
    }
  }
}



void finalChecks() {
  if (!dead) {
    for (int i = 1; i < 4; i++) {
      if (stats[i] > 1000) {
        stats[i] = 1000;
      }
      if (stats[i] < 0) {
        stats[i] = 0;
      }
    }
    if (age >= max_age) {
      dead = true;
      was_alive = true;
    }
    if (stats[0] <= 0) {
      dead = true;
      was_alive = true;
    }
    if (!dead) {
      if (stats[3] > 700 || stats[3] < 300) {
        if (stats[3] == 0) {
          dead = true;
          was_alive = true;
        } else {
          bad[0] = true;
        }
      } else {
        bad[0] = false;
      }
      if (stats[1] < 300) {
        if (stats[0] == 0) {
          dead = true;
          was_alive = true;
        } else {
          bad[1] = true;
        }
      } else {
        bad[1] = false;
      }
      if (stats[2] < 200) {
        if (stats[2] == 0) {
          dead = true;
          was_alive = true;
        } else {
          bad[2] = true;
        }
      } else {
        bad[2] = false;
      }
    }
  }
}
