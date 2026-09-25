#include <Arduino.h>

#define NORTH_SENSOR_PIN 21   // Left button
#define EAST_SENSOR_PIN  20   // Right button

// North/South traffic light
#define NS_GREEN_PIN   13
#define NS_YELLOW_PIN  15
#define NS_RED_PIN      4

// East/West traffic light
#define EW_RED_PIN     27
#define EW_YELLOW_PIN  26
#define EW_GREEN_PIN   25

// FSM States

enum State {
  N_GREEN,
  N_YELLOW,
  ALL_RED_1,
  E_GREEN,
  E_YELLOW,
  ALL_RED_2
};

State state = N_GREEN;

// Turn all LEDs off
void allLightsOff()
{
  digitalWrite(NS_GREEN_PIN, LOW);
  digitalWrite(NS_YELLOW_PIN, LOW);
  digitalWrite(NS_RED_PIN, LOW);

  digitalWrite(EW_RED_PIN, LOW);
  digitalWrite(EW_YELLOW_PIN, LOW);
  digitalWrite(EW_GREEN_PIN, LOW);
}


// Set LEDs according to current state
void setLights(State currentState)
{
  // Turn everything off first
  allLightsOff();

  switch (currentState)
  {
    case N_GREEN:
      digitalWrite(NS_GREEN_PIN, HIGH);
      digitalWrite(EW_RED_PIN, HIGH);
      break;

    case N_YELLOW:
      digitalWrite(NS_YELLOW_PIN, HIGH);
      digitalWrite(EW_RED_PIN, HIGH);
      break;

    case ALL_RED_1:
      digitalWrite(NS_RED_PIN, HIGH);
      digitalWrite(EW_RED_PIN, HIGH);
      break;

    case E_GREEN:
      digitalWrite(NS_RED_PIN, HIGH);
      digitalWrite(EW_GREEN_PIN, HIGH);
      break;

    case E_YELLOW:
      digitalWrite(NS_RED_PIN, HIGH);
      digitalWrite(EW_YELLOW_PIN, HIGH);
      break;

    case ALL_RED_2:
      digitalWrite(NS_RED_PIN, HIGH);
      digitalWrite(EW_RED_PIN, HIGH);
      break;
  }
}


// Read the two buttons
// bit 1 = North
// bit 0 = East
uint8_t readInputs()
{
  uint8_t north = digitalRead(NORTH_SENSOR_PIN);
  uint8_t east  = digitalRead(EAST_SENSOR_PIN);

  return (north << 1) | east;
}



void setup()
{
  // Button inputs
  // ESP32's internal pull-down resistors
  pinMode(NORTH_SENSOR_PIN, INPUT_PULLDOWN);
  pinMode(EAST_SENSOR_PIN, INPUT_PULLDOWN);

  // LED outputs
  pinMode(NS_GREEN_PIN, OUTPUT);
  pinMode(NS_YELLOW_PIN, OUTPUT);
  pinMode(NS_RED_PIN, OUTPUT);

  pinMode(EW_RED_PIN, OUTPUT);
  pinMode(EW_YELLOW_PIN, OUTPUT);
  pinMode(EW_GREEN_PIN, OUTPUT);

  // Start with everything off
  allLightsOff();
}



void loop()
{

  // NORTH GREEN
  if (state == N_GREEN)
  {
    setLights(N_GREEN);

    delay(3000);

    // Read buttons
    uint8_t input = readInputs();

    // 00 = no cars
    // 01 = East has car
    // 10 = North has car
    // 11 = both have cars

    if (input == 0b00)
    {
      state = N_GREEN;
    }
    else
    {
      state = N_YELLOW;
    }
  }

  // NORTH YELLOW

  else if (state == N_YELLOW)
  {
    setLights(N_YELLOW);

    delay(1000);

    state = ALL_RED_1;
  }


  // ALL RED 1
  else if (state == ALL_RED_1)
  {
    setLights(ALL_RED_1);

    delay(500);

    uint8_t input = readInputs();

    if (input == 0b01)
    {
      // East car only
      state = E_GREEN;
    }
    else if (input == 0b11)
    {
      // Both have cars
      state = E_GREEN;
    }
    else
    {
      // North only or nobody
      state = N_GREEN;
    }
  }


  // EAST GREEN
  else if (state == E_GREEN)
  {
    setLights(E_GREEN);

    delay(3000);

    uint8_t input = readInputs();

    if (input == 0b00)
    {
      state = E_GREEN;
    }
    else
    {
      state = E_YELLOW;
    }
  }



  // EAST YELLOW
  else if (state == E_YELLOW)
  {
    setLights(E_YELLOW);

    delay(1000);

    state = ALL_RED_2;
  }


  // ALL RED 2
  else if (state == ALL_RED_2)
  {
    setLights(ALL_RED_2);

    delay(500);

    uint8_t input = readInputs();

    if (input == 0b10)
    {
      // North car only
      state = N_GREEN;
    }
    else if (input == 0b11)
    {
      // Both have cars
      state = N_GREEN;
    }
    else
    {
      // East only or nobody
      state = E_GREEN;
    }
  }
}