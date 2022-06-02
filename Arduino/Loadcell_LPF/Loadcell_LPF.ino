#include "HX711.h"

//last amp
#define DOUT_3 19
#define CLK_3 20
#define calibration_factor3 -890

HX711 scale3;

//  LPF
double pre_val = 0;
double pre_t = 0;
double tau = 1;

void setup() {
  Serial.begin(9600);
  scale3.begin(DOUT_3, CLK_3);
  scale3.set_scale();
  scale3.tare();
  Serial.println("cur_val,LPF");
}

void loop() {
  scale3.set_scale(calibration_factor3);
  double cur_val = scale3.get_units();

//  double cur_t = millis();
//  double dt = cur_t - pre_t;

  double val = 1/(tau+1)*pre_val + tau/(tau+1)*cur_val;
  Serial.print(cur_val);
  Serial.print(",");
  Serial.println(val);

  pre_val = val;
//  pre_t = cur_t;
}
