#define NUM_READINGS 0
#define INDEX 1
#define TOTAL 2
#define AVEREAGE 3
#define MAX 4
#define PIN 5

int SNARE_PIN = A0;
int HIGH_HAT_PIN = A1;
int BASS_PIN = A2;
int CRASH_PIN = A3;

// Returns array of component data
int componentData(int num_readings, int index, int total, int average, int max) {
  int data[5];

  data[NUM_READINGS] = num_readings;
  data[INDEX] = index;
  data[TOTAL] = total;
  data[AVEREAGE] = average;
  data[MAX] = max;

  return data;
}

// Plays MIDI note 
void playComponent(int component_data[5], byte channel, byte note, byte velocity, long interval, int pin) { 
  
  // Delay between MIDI notes
  unsigned long time = millis();
  long previous_millis = 0;

  // Initialize readings
  int component_readings[component_data[NUM_READINGS]];
  for (int i = 0; i < component_data[NUM_READINGS]; i++)
  component_readings[i] = 0;

  int component_total = component_data[TOTAL];
  int component_index = component_data[INDEX];
  int component_average = component_data[AVEREAGE];

  // Read pin
  component_total = component_total - component_readings[component_index];
  component_readings[component_index] = analogRead(pin);
  component_total = component_total + component_readings[component_index];
  component_index++;

  // Trigger note once index reaches number of readings of component 
  if (component_index >= component_data[NUM_READINGS]) {
    component_index = 0;
    component_average = component_total / component_data[NUM_READINGS];
    
    // Play note if average accelerometer value surpasses max
    // and time since last millis is longer than preset interval
    if (component_average > component_data[MAX] && (time - previous_millis) > interval) {
      // Reset previous_millis
      previous_millis = time;
      byte midi = 0x90 + (channel - 1);

      // Write note to Serial
      Serial.write(midi);
      Serial.write(note);
      Serial.write(velocity);
    }
  }
}

// 9600 baud has worked best during testing
void setup() {
  Serial.begin(9600);
  pinMode(SNARE_PIN, INPUT);
  pinMode(HIGH_HAT_PIN, INPUT);
  pinMode(BASS_PIN, INPUT);
  pinMode(CRASH_PIN, INPUT);
  analogReference(EXTERNAL);
}

// Reads and executes notes of all connected components
void loop() {
  // Snare Drum
  int snare_data[5] = componentData(7, 0, 0, 0, 700);
  playComponent(snare_data, 1, 38, 127, 100, SNARE_PIN);

  // High Hat
  int high_hat_data[5] = componentData(2, 0, 0, 0, 550);
  playComponent(high_hat_data, 9, 46, 120, 100, HIGH_HAT_PIN);

  // Bass Drum
  int bass_data[5] = componentData(5, 0, 0, 0, 360);
  playComponent(bass_data, 9, 35, 120, 100, BASS_PIN);

  // Crash 
  int crash_data[5] = componentData(5, 0, 0, 0, -10);
  playComponent(crash_data, 9, 55, 120, 100, CRASH_PIN);
}
