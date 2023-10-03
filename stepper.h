unsigned long START_DELAY_MS = 1000;
unsigned long MOTORRUN_MS = 20000 * 1;
unsigned long STEPS_PER_MS = 1;

#define ENABLE_PIN 13
#define PULSE_PIN 14
#define DIRECTION_PIN 12

uint32_t startTime = 0;

void setupStepper()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(DIRECTION_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIRECTION_PIN, LOW);

  while (millis() < START_DELAY_MS) // Wait for the start delay
  {
  }
  startTime = millis();
  digitalWrite(ENABLE_PIN, LOW);
}

void loopStepper(int MOTORRUN_MS1, int STEPS_PER_MS1)
{
  startTime = millis();
  MOTORRUN_MS = MOTORRUN_MS1;
  STEPS_PER_MS = STEPS_PER_MS1;
  while (millis() - startTime < MOTORRUN_MS)
  {
    digitalWrite(PULSE_PIN, HIGH);
    delayMicroseconds((uint32_t)(500 / STEPS_PER_MS));
    digitalWrite(PULSE_PIN, LOW);
    delayMicroseconds((uint32_t)(500 / STEPS_PER_MS));
  }
  {
    digitalWrite(ENABLE_PIN, HIGH);
  }
}
