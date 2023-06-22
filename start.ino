// Define the Arduino pin assignments for the relays and input signals
const int ENGINE_RELAY = 2;
const int STARTER_RELAY = 3;
const int AC_INPUT = 5;
const int ON_OFF_INPUT = 6;
const int LOADS = 10;

// Define the state for Actual
enum ActualState {
  ACTUAL_OFF,
  ACTUAL_STARTING,
  ACTUAL_STARTING_ERROR,
  ACTUAL_ON,
  ACTUAL_ON_ERROR,
};

ActualState actualstate = ACTUAL_STARTING;

int startAttempts = 0;
int offCounter = 0;

// millis counter
unsigned long previousMillis = 0;  // Variable to store the previous timestamp
const unsigned long interval = 10000;  // Interval of 10 seconds
bool isStartDelayed = false;  // Flag to indicate if start is delayed

void setup() {
  // Initialize the relays as OUTPUT
  pinMode(ENGINE_RELAY, OUTPUT);
  pinMode(STARTER_RELAY, OUTPUT);
  pinMode(LOADS, OUTPUT);

  // Initialize the input pins with internal pull-up resistors
  pinMode(AC_INPUT, INPUT_PULLUP);
  pinMode(ON_OFF_INPUT, INPUT_PULLUP);

  // Start the serial communication
  Serial.begin(9600);
  delay(500);
  Serial.println("Ready");
}

void loop() {
  // Handle STARTING error state
  if (digitalRead(ON_OFF_INPUT) == HIGH && actualstate == ACTUAL_STARTING_ERROR && offCounter == 0) {
    offCounter = 1;
    Serial.print("OFF Counter: ");
    Serial.println(offCounter);
  }

  if (digitalRead(ON_OFF_INPUT) == LOW && actualstate == ACTUAL_STARTING_ERROR && offCounter == 1) {
    offCounter = 0;
    actualstate = ACTUAL_STARTING;
    startAttempts = 0;
    Serial.print("OFF Counter: ");
    Serial.println(offCounter);
    Serial.println("STARTING_ERROR RESET");
  }

  // Handle ON error state
  if (digitalRead(ON_OFF_INPUT) == HIGH && actualstate == ACTUAL_ON_ERROR && (offCounter == 0 || offCounter == 1)) {
    offCounter += 1;
    Serial.print("OFF Counter: ");
    Serial.println(offCounter);
  }

  if (digitalRead(ON_OFF_INPUT) == LOW && actualstate == ACTUAL_ON_ERROR && offCounter == 2) {
    offCounter = 0;
    actualstate = ACTUAL_STARTING;
    startAttempts = 0;
    Serial.print("OFF Counter: ");
    Serial.println(offCounter);
    Serial.println("ON_ERROR RESET");
  }

  // Call functions
  if (digitalRead(ON_OFF_INPUT) == HIGH && actualstate != ACTUAL_OFF) {
    xshutdown();
  }

  if (digitalRead(ON_OFF_INPUT) == LOW && !isStartDelayed) {
    START();
  }

  // millis timer
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    isStartDelayed = false;
  }
}

void xshutdown() {
  Serial.println("shutdown procedure started");

  digitalWrite(LOADS, HIGH);  // Turn on LOADS
  unsigned long startTime = millis();  // Get the current time

  while (millis() - startTime < 30000) {  // Wait for 30 seconds
    if (digitalRead(ON_OFF_INPUT) == LOW) {
      // ON_OFF_INPUT is low, exit the function
      Serial.println("ON_OFF_INPUT is low, aborting shutdown");
      digitalWrite(LOADS, LOW);
      
      return;
    }
  }

 
  digitalWrite(ENGINE_RELAY, LOW);  // Turn off ENGINE_RELAY
  delay (5000);
  digitalWrite(LOADS, LOW);  // Turn off LOADS
  Serial.println("Setting actualstate to ACTUAL_OFF");
  actualstate = ACTUAL_OFF;
  startAttempts = 0;
  Serial.println("shutdown complete");
}




void START() {
  isStartDelayed = true;  // Set the flag to delay start
  previousMillis = millis();  // Store the current timestamp

  // Check if AC is present
  if ((digitalRead(AC_INPUT) == LOW || digitalRead(ON_OFF_INPUT) == HIGH) || startAttempts > 3 || actualstate == ACTUAL_STARTING_ERROR || actualstate == ACTUAL_ON || actualstate == ACTUAL_ON_ERROR) {
    if (digitalRead(AC_INPUT) == LOW) {
      if (actualstate != ACTUAL_ON) {
        actualstate = ACTUAL_ON;
       
        Serial.println("AC input present - changing status to ON");
      } else {
        Serial.println("AC input present - status is already ON");
      }
      return;
    }

    if (digitalRead(ON_OFF_INPUT) == HIGH) {
      Serial.println("Requesting signal turned off");
    }

    // Check start attempts
    if (startAttempts > 3) {
      Serial.println("Start attempts exceeded 3 - cancelling start");
      return;
    }

    // Check if error is present
    if (actualstate == ACTUAL_STARTING_ERROR) {
      Serial.println("starting_error state detected - cancelling start");
      return;
    }

    // Check if AC has been lost
    if (actualstate == ACTUAL_ON) {
      actualstate = ACTUAL_ON_ERROR;
      Serial.println("AC has previously been detected ( but not now ) - the generator could be running but not producing any power - attempting to start again could cause SERIOUS DAMAGE");
      Serial.println(" ");
      Serial.println("Physically check the generator:");
      Serial.println("1. Is not running");
      Serial.println("2. The breaker is not tripped");
      Serial.println("3. DIESEL level");
      Serial.println("3. Reset twice to clear this error");
      return;
    }
    return;
  }

  Serial.println("ENGINE RELAY ON");
  digitalWrite(ENGINE_RELAY, HIGH);
  actualstate = ACTUAL_STARTING;
  startAttempts += 1;
  Serial.print("START Attempt: ");
  Serial.println(startAttempts);
  digitalWrite(STARTER_RELAY, HIGH);

  for (int i = 0; i < 3; i++) {
    delay(1000);
    Serial.println("Checking AC signal");

    if (digitalRead(AC_INPUT) == LOW) {
      digitalWrite(STARTER_RELAY, LOW);
      Serial.println("AC signal present");
      Serial.println("starting sequence stopped");
      actualstate = ACTUAL_ON;
      startAttempts = 0;
      Serial.println("Status changed from STARTING to ON");
      return;
    }

    if (digitalRead(ON_OFF_INPUT) == HIGH) {
      digitalWrite(STARTER_RELAY, LOW);
      Serial.println("Requesting signal turned off");
      Serial.println("stopping starting sequence");
      return;
    }
  }

  digitalWrite(STARTER_RELAY, LOW);

  if (startAttempts == 3) {
    actualstate = ACTUAL_STARTING_ERROR;
    Serial.println("ERROR 3 UNSUCCESSFUL ATTEMPTS");
    Serial.println("ENGINE RELAY OFF");
    digitalWrite(ENGINE_RELAY, LOW);
  }

  Serial.println("NO AC detected after 3 seconds - exit start function");
  return;
}
