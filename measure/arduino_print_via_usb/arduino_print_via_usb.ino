// constants won't change. Used here to 
// set pin numbers:
const int ledPin1 =  13;      // the number of the LED pin
const int ledPin2 =  12;

// Variables will change:
int ledState = LOW; 

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println("foo");
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, LOW);
  delay(1000);        // delay in between reads for stability
  Serial.println("bar");
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin1, LOW);
  delay(1000);
}



