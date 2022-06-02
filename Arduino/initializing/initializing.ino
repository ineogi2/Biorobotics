#include <FlexCAN.h>
#include <EEPROM.h>
#include <HX711.h>
#include "Faulcan.h"

Faulcan mot1(5);// flexion
Faulcan mot2(4);// extension_mid
Faulcan mot3(3);// extension_proximal

// loadcell parameters
HX711 scale;
HX711 scale2;
HX711 scale3;

uint8_t dataPin = 15;
uint8_t clockPin = 16;

uint8_t dataPin2 = 17;//
uint8_t clockPin2 = 18;//

uint8_t dataPin3 = 19;//
uint8_t clockPin3 = 20;//

int pos;
int pos2;
int pos3;

int velM = 2000;
int velM2 = 2000;
int velM3 = 2000;


void setup() {
  Can0.begin(1000000);
//  ads.begin();
  Serial.begin(115200);

  //loadcell
  scale.begin(dataPin, clockPin);
  scale.set_scale(420.0983);
  scale.tare();
  scale2.begin(dataPin2, clockPin2);
  scale2.set_scale(420.0983);
  scale2.tare();
  scale3.begin(dataPin3, clockPin3);
  scale3.set_scale(420.0983);
  scale3.tare();

  // motor canbus initiating
  mot1.Faul_init();
  mot1.Faul_iniset();
  mot1.Faul_velset(velM);
  mot2.Faul_init();
  mot2.Faul_iniset();
  mot2.Faul_velset(velM2);
  mot3.Faul_init();
  mot3.Faul_iniset();
  mot3.Faul_velset(velM3);
  
  pos = mot1.Faul_abs_pos_r();
  pos2 = mot2.Faul_abs_pos_r();
  pos3 = mot2.Faul_abs_pos_r();

}

int val = 0;
int vvv = 0;
int eee = 0;
volatile float load_flx;
volatile float load_flx2;
volatile float load_ex2;

void loop() {
  
  load_flx = - scale.get_units();
  load_flx2 = - scale2.get_units();
  load_ex2 = - scale3.get_units();
  load_ex2 = load_ex2 / 2.28;
 
  Serial.print(pos);
  Serial.print(',');
  Serial.print(load_flx);
  Serial.print(',');
  Serial.print(pos2);
  Serial.print(',');
  Serial.print(load_flx2);
  Serial.print(',');
  Serial.print(pos3);
  Serial.print(',');
  Serial.println(load_ex2);
  
  if (Serial.available()){
    char input = Serial.read();
//    char enter = Serial.read(); // \n value
    if (input == '1'){
      val += 100;
    }
    else if (input == '2'){
      val += 500;
    }
    else if (input == '3'){
      val -= 500;
    }
    else if (input == '9'){
      val -= 100;
    }
    else if (input == '4'){
      vvv += 100;
    }
    else if (input == '5'){
      vvv += 500;
    }
    else if (input == '6'){
      vvv -= 500;
    }
    else if (input == '8'){
      vvv -= 100;
    }
    else if (input == 'q'){
     vvv = 0;
     val = 0;
     eee = 0;
    }
     else if (input == 'a'){
      eee += 500;
    }
    else if (input == 's'){
      eee += 100;
    }
    else if (input == 'd'){
      eee -= 500;
    }
    else if (input == 'f'){
      eee -= 100;
    }
  }

  mot1.Faul_velset(velM);
  pos = mot1.Faul_abs_pos_rw(vvv);
  mot2.Faul_velset(velM);
  pos2 = mot2.Faul_abs_pos_rw(val);
  mot3.Faul_velset(velM);
  pos3 = mot3.Faul_abs_pos_rw(eee);
}
