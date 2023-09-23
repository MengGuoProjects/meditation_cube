// Meditation cube firmware 
// by Meng Guo

#include "Encoder.h"
#include "Timer.h"

/* global variables */
enum MeditationState {
  INHALE,
  EXHALE,
  HOLD
};
MeditationState current_state = MeditationState::INHALE;
unsigned long state_start_time = 0;
unsigned long inhale_duration = 5000; 
unsigned long exhale_duration = 7000; 
const unsigned long hold_duration = 1000; 

const unsigned int LED_1_PIN = 11;
const unsigned int MAX_BRIGHTNESS_PERCENT = 100;
const unsigned int ENC_1_PIN = 1;
const unsigned int ENC_2_PIN = 2;

const unsigned int ENCODER_MAX = 10;
const unsigned int ENCODER_MIN = 1;
const unsigned int MS_PER_LEVEL = 1000;

unsigned int inhale_ms = 0;
//nsigned long encoder_new_position = 0;
unsigned long encoder_old_position = 0;
const unsigned long ENCODER_STEP_SIZE = 4;

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
  const unsigned int initial_level = 5;
  myEnc.write(initial_level * ENCODER_STEP_SIZE);
}

/* forward clarations */
int convPercentToADC(int input);
void breath(const unsigned int duration_to_breathin_ms, const bool is_exhale = false);
unsigned int convEncToLevel(long input);
unsigned long limitEncoderValue(const unsigned long encoder_position, const unsigned long min_value, const unsigned long max_value);

/* main function */
void loop() {
  unsigned long currentTime = millis();
  inhale_duration = convEncToLevel(myEnc.read()) * MS_PER_LEVEL;
  exhale_duration = inhale_duration * (7 / 5);
  switch (current_state) {
    case INHALE:
      // breath(inhale_duration);
      // if (currentTime - state_start_time >= inhale_duration) {
        current_state = MeditationState::EXHALE;
        state_start_time = currentTime;
      // }
      break;
    case EXHALE:
      // breath(exhale_duration, true);
      // if (currentTime - state_start_time >= exhale_duration) {
        current_state = MeditationState::HOLD;
        state_start_time = currentTime;
      // }
      break;
    case HOLD:
      // delay(hold_duration);
      // if (currentTime - state_start_time >= hold_duration) {
        current_state = MeditationState::INHALE;
        state_start_time = currentTime;
      // }
      break;
  }
}

/* helper functions */
void breath(const unsigned int duration_to_breathin_ms, const bool is_exhale = false) {
  for(int brightness_ramp_up_percent = 0;  brightness_ramp_up_percent <= MAX_BRIGHTNESS_PERCENT; brightness_ramp_up_percent++) {
    const unsigned int brightness_ramp_down_percent = 100 - brightness_ramp_up_percent;

    const unsigned int inverted_brightness_ramp_down_percent = 100 - brightness_ramp_down_percent; // invert because PIN high means led off and vice-versa
    const unsigned int inverted_brightness_ramp_up_percent = 100 - brightness_ramp_up_percent;

    if (is_exhale) {
      analogWrite(LED_1_PIN, convPercentToADC(inverted_brightness_ramp_down_percent));
    } else {
      analogWrite(LED_1_PIN, convPercentToADC(inverted_brightness_ramp_up_percent));
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

unsigned int convEncToLevel(long encoder_new_position) {
  encoder_new_position = limitEncoderValue(encoder_new_position, ENCODER_MIN, ENCODER_MAX);

  unsigned long encoder_position_single_step = encoder_old_position / ENCODER_STEP_SIZE;
    if ((encoder_new_position > encoder_old_position + 1) || (encoder_new_position < encoder_old_position - 1)) { //remove jitter from encoder value
      encoder_old_position = encoder_new_position;
      encoder_position_single_step = encoder_new_position / ENCODER_STEP_SIZE;
      Serial.print(encoder_position_single_step);
      Serial.println("%");
    }
  return static_cast<unsigned int>(encoder_position_single_step);
}

unsigned long limitEncoderValue(const unsigned long encoder_position, const unsigned long min_value, const unsigned long max_value) {
  if ((encoder_position / ENCODER_STEP_SIZE) > max_value) {
    myEnc.write(max_value * ENCODER_STEP_SIZE);
    return max_value * ENCODER_STEP_SIZE;
  }
  else if (encoder_position < ENCODER_STEP_SIZE * min_value) {
    myEnc.write(min_value * ENCODER_STEP_SIZE);
    return min_value * ENCODER_STEP_SIZE;
  } 
  return encoder_position;
}