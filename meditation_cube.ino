// Meditation cube firmware 
// by Meng Guo

/* global variables */
const unsigned int LED_1_PIN = 11;
const unsigned int MAX_BRIGHTNESS_PERCENT = 100;

/* initializtion */
void setup() {
  pinMode(LED_1_PIN, OUTPUT); 
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

/* forward clarations */
int conv_percent_to_ADC(int input);
void breath(const unsigned int duration_to_breathin_s, const bool is_exhale = false);

/* main function */
void loop() {
  const unsigned int inhale_s = 5;
  const unsigned int exhale_s = 7;
  const unsigned int hold_ms = 500;
  breath(inhale_s);
  delay(hold_ms * 2);
  breath(exhale_s, true);
  delay(hold_ms);
}

/* helper functions */
void breath(const unsigned int duration_to_breathin_s, const bool is_exhale = false) {
  for(int brightness_ramp_up_percent = 0;  brightness_ramp_up_percent <= MAX_BRIGHTNESS_PERCENT; brightness_ramp_up_percent++) {
    const unsigned int brightness_ramp_down_percent = 100 - brightness_ramp_up_percent;

    if (is_exhale) {
      analogWrite(LED_1_PIN, conv_percent_to_ADC(brightness_ramp_down_percent));
    } else {
      analogWrite(LED_1_PIN, conv_percent_to_ADC(brightness_ramp_up_percent));
    }

    delay(duration_to_breathin_s * 1000 / MAX_BRIGHTNESS_PERCENT);
  }
}

int conv_percent_to_ADC(int input) {
  const unsigned int steps_of_ADC = 255;
  return input * steps_of_ADC / 100;
}