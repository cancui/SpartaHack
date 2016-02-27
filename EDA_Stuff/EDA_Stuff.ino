const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
const int TOL = 10;
int currRead = 0;

int inputPin = A0;

void setup() {
  Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  for (int i = 0; i < 5000; i++) {
    total = total - readings[readIndex];
    readings[readIndex] = analogRead(inputPin);
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
  
    if (readIndex >= numReadings) {
      readIndex = 0;
    }
  
    average = total / numReadings;
    Serial.println(average);
    delay(1);   
  }     
}

void loop() {
  /*total = total - readings[readIndex];
  readings[readIndex] = analogRead(inputPin);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  average = total / numReadings;
  Serial.println(average);
  delay(1);*/
  
  currRead = analogRead(inputPin);
  if (abs(currRead - average) > TOL) {
    //call function 
    Serial.println("HELLOWORLDHELLOWORLD");
  }
}

