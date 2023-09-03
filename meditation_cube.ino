// Meditation cube firmware 
// by Meng Guo

/* global variables */
const unsigned int led_1_pin = 11;

/* initializtion */
void setup() {
  pinMode(led_1_pin, OUTPUT); 
}

/* forward clarations */
void breath_in(const unsigned int duration_to_breathin_s);
int conv_percent_to_ADC(int input);

/* main function */
void loop() {
  const unsigned int time_breath_in_s = 5;
  breath_in(time_breath_in_s);
}

/* helper functions */
void breath_in(const unsigned int duration_to_breathin_s) {
  const unsigned int max_brightness_perecent = 100;
  for(int led_1_brightness_percent = 0;  led_1_brightness_percent <= max_brightness_perecent; led_1_brightness_percent++) {
    delay(duration_to_breathin_s * 1000 / max_brightness_perecent);
    analogWrite(led_1_pin, conv_percent_to_ADC(led_1_brightness_percent));
  }
}

int conv_percent_to_ADC(int input) {
  const unsigned int steps_of_ADC = 255;
  return input * steps_of_ADC / 100;
}