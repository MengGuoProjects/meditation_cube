// Meditation cube firmware 
// by Meng Guo

#include <Encoder.h>

/* global variables */
const unsigned int LED_1_PIN = 11;
const unsigned int MAX_BRIGHTNESS_PERCENT = 100;
const unsigned int ENC_1_PIN = 1;
const unsigned int ENC_2_PIN = 2;
const unsigned int MAX_INHALE_MS = 10000;

unsigned int inhale_ms = 0;
unsigned long encoder_new_position = 0;
unsigned long encoder_old_position = 0;
const unsigned long encoder_step_size = 4;

Encoder myEnc(ENC_1_PIN, ENC_2_PIN);


/* initializtion */
void setup() {
  pinMode(ENC_1_PIN, INPUT_PULLUP);
  pinMode(ENC_2_PIN, INPUT_PULLUP);
  pinMode(LED_1_PIN, OUTPUT); 
  // startupAnimation();
  digitalWrite(LED_1_PIN, LOW);
  Serial.begin(9600);
  Serial.println("Encoder Test:");
}

/* forward clarations */
int convPercentToADC(int input);
void breath(const unsigned int duration_to_breathin_ms, const bool is_exhale = false);
unsigned int convEncToMilliSec(long input);

/* main function */
void loop() {
  inhale_ms = convEncToMilliSec(myEnc.read());
  const unsigned int exhale_ms = inhale_ms * 7 / 5;
  const unsigned int hold_ms = 500;
  // breath(inhale_ms);
  // delay(hold_ms * 2);
  // breath(exhale_ms, true);
  // delay(hold_ms);
}

/* helper functions */
void breath(const unsigned int duration_to_breathin_ms, const bool is_exhale = false) {
  for(int brightness_ramp_up_percent = 0;  brightness_ramp_up_percent <= MAX_BRIGHTNESS_PERCENT; brightness_ramp_up_percent++) {
    const unsigned int brightness_ramp_down_percent = 100 - brightness_ramp_up_percent;

    if (is_exhale) {
      analogWrite(LED_1_PIN, convPercentToADC(brightness_ramp_down_percent));
    } else {
      analogWrite(LED_1_PIN, convPercentToADC(brightness_ramp_up_percent));
    }

    delay(duration_to_breathin_ms / MAX_BRIGHTNESS_PERCENT);
  }
}

int convPercentToADC(int input) {
  const unsigned int steps_of_ADC = 255;
  return input * steps_of_ADC / 100;
}

void startupAnimation() {
  for (int i = 0; i < 4; i++) {
    breath(i*1000);
  }
}

unsigned int convEncToMilliSec(long encoder_new_position) {
  unsigned long encoder_position_single_step = encoder_old_position / encoder_step_size;
    if ((encoder_new_position > encoder_old_position + 1) || (encoder_new_position < encoder_old_position - 1)) {
      encoder_old_position = encoder_new_position;
      if ((encoder_new_position / encoder_step_size) > MAX_INHALE_MS) {
        encoder_new_position = MAX_INHALE_MS * encoder_step_size;
      }
      encoder_position_single_step = encoder_new_position / encoder_step_size;
      Serial.print(encoder_position_single_step);
      Serial.println();
    }
  return static_cast<unsigned int>(encoder_position_single_step);
}