// Meditation cube firmware 
// by Meng Guo

#include "Encoder.h"

/* global constants */
enum MeditationState {
  INHALE,
  EXHALE,
  HOLD
};
const unsigned int PIN_GREEN = 9;
const unsigned int PIN_RED = 10;
const unsigned int PIN_BLUE = 11;

const unsigned int ENC_1_PIN = 1;
const unsigned int ENC_2_PIN = 2;
const unsigned int MS_PER_LEVEL = 1250;
const unsigned int ENCODER_MIN = 1;
const unsigned int ENCODER_MAX = 10;
const unsigned long ENCODER_STEP_SIZE = 4;
Encoder myEnc(ENC_1_PIN, ENC_2_PIN);

const unsigned int MAX_BRIGHTNESS_PERCENT = 100;
const unsigned long HOLD_DURATION = 1500;

/* global variables */
enum MeditationState current_state = MeditationState::INHALE;
unsigned long encoder_old_position = 0;
unsigned long inhale_duration = 5000; 
unsigned long exhale_duration = 7000; 

/* initializtion */
void setup() {
  Serial.begin(9600);
  pinMode(ENC_1_PIN, INPUT_PULLUP);
  pinMode(ENC_2_PIN, INPUT_PULLUP);
  pinMode(PIN_BLUE, OUTPUT); 
  digitalWrite(PIN_BLUE, LOW);

  const unsigned int initial_level = 5;
  myEnc.write(initial_level * ENCODER_STEP_SIZE);

  startupAnimation();
}

/* forward declarations */
int convPercentToADC(int input);
unsigned long convEncToLevel(long input);
unsigned long limitEncoderValue(const unsigned long encoder_position, const unsigned long min_value, const unsigned long max_value);
void controlLedBreathing(const unsigned long inhale_duration, const unsigned long exhale_duration, const unsigned int hold_duration);

/* main function */
void loop() {
  inhale_duration = convEncToLevel(myEnc.read()) * MS_PER_LEVEL;
  exhale_duration = inhale_duration * (7 / 5);
  controlLedBreathing(inhale_duration, exhale_duration, HOLD_DURATION);
}

/* helper functions */
void controlLedBreathing(const unsigned long inhale_duration, const unsigned long exhale_duration, const unsigned int hold_duration) {
  static unsigned long start_time = 0;
  static int brightness_percent = 1;

  const unsigned long time = millis();

  switch (current_state) {
    case INHALE:
      if (time >= start_time + (inhale_duration / 100) && (brightness_percent < MAX_BRIGHTNESS_PERCENT)) {
        brightness_percent++;
        const unsigned int inverted_brightness_ramp_up_percent = 100 - brightness_percent; // invert because PIN high means led off and vice-versa
        analogWrite(PIN_BLUE, convPercentToADC(inverted_brightness_ramp_up_percent));
        start_time = time;
      }
      if (brightness_percent >= MAX_BRIGHTNESS_PERCENT) { // Transition to the next state
        current_state = MeditationState::EXHALE; 
        Serial.println("current_state: EXHALE");
      }
      break;
    case EXHALE:
      if (time >= start_time + (exhale_duration / 100) && (brightness_percent > 0)) {
        brightness_percent--;

        const unsigned int inverted_brightness_ramp_down_percent = 100 - brightness_percent; // invert because PIN high means led off and vice-versa
        analogWrite(PIN_BLUE, convPercentToADC(inverted_brightness_ramp_down_percent));
        start_time = time;
      }
      if (brightness_percent <= 0) {
        current_state = MeditationState::HOLD; 
        Serial.println("current_state: HOLD");
      }
      break;
    case HOLD:
      if (time >= start_time + hold_duration) {
        current_state = MeditationState::INHALE;       
        Serial.println("current_state: INHALE");
      }
      break;
    default:   
      Serial.println("unexpected default state");
      break;
  }
}

int convPercentToADC(int input) {
  const unsigned int steps_of_ADC = 255;
  return input * steps_of_ADC / 100;
}

void startupAnimation() {
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  analogWrite(PIN_RED,   156);
  analogWrite(PIN_GREEN, 108);
  analogWrite(PIN_BLUE,  198);
  
  delay(2000);

  // turn leds off (we control negative socket of LED, so inverted logic)
  digitalWrite(PIN_RED,   HIGH);
  digitalWrite(PIN_GREEN, HIGH);
  digitalWrite(PIN_BLUE,  HIGH);
}

unsigned long convEncToLevel(long encoder_new_position) {
  encoder_new_position = limitEncoderValue(encoder_new_position, ENCODER_MIN, ENCODER_MAX);

  unsigned long encoder_position_single_step = encoder_old_position / ENCODER_STEP_SIZE;
    if ((encoder_new_position > encoder_old_position + 1) || (encoder_new_position < encoder_old_position - 1)) { //remove jitter from encoder value
      encoder_old_position = encoder_new_position;
      encoder_position_single_step = encoder_new_position / ENCODER_STEP_SIZE;
      Serial.println(encoder_position_single_step);
    }
  return static_cast<unsigned long>(encoder_position_single_step);
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