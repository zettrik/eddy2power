const int ledPin1 = 13;
const int ledPin2 = 11;
const int APin0 = A0;  // 
const int APin1 = A1;  // voltage photoresistor
const int APin2 = A2;  // battery voltage
const int APin3 = A3;  // acs714 sensor out
const int APin4 = A4;  //
const int APin5 = A5;  //
long av;
int av0;
int av1;
int av2;
int av3;
int av4;
int av5;
int i;
int times;
float middle;
int start_time;
int stop_time;
int randv;

float multiple_read(int pin, int times) {
  av = 0;
  for(int i = 0; i < times; i++) {
    delay(3);
    av = av + analogRead(pin); // one read lasts about 100µs
    }
  middle = av / times;
  return middle;
}

void heartbeat() {
  for(i=255; i>=0; i--){
    analogWrite(ledPin2, i);
    delay(2);
    }
  delay(200);
  for(i=0; i<=255; i++){
    analogWrite(ledPin2, i);
    delay(2);
    }
  digitalWrite(ledPin1, HIGH);
  randv = random(0, 256);
  for(i=255; i>=randv; i--){
    analogWrite(ledPin2, i);
    delay(4);
    }
  for(i=randv; i<=255; i++){
    analogWrite(ledPin2, i);
    delay(2);
    }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.println("=== csv header === ");
  Serial.println("start, A0, A1, A2, A3, A4, A5, intVCC, end");
}

void loop() {
  //heartbeat()
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, LOW);
  delay(100);
  av0 = multiple_read(APin0, 30);
  delay(20);
  av1 = multiple_read(APin1, 30);
  delay(20);
  av2 = multiple_read(APin2, 30);
  delay(20);
  av3 = multiple_read(APin3, 30);
  delay(20);
  av4 = multiple_read(APin4, 30);
  delay(20);
  av5 = multiple_read(APin5, 30);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  // print sensor values
  Serial.print("A, ");
  Serial.print(av0);
  Serial.print(", ");
  Serial.print(av1);
  Serial.print(", ");
  Serial.print(av2);
  Serial.print(", ");
  Serial.print(av3);
  Serial.print(", ");
  Serial.print(av4);
  Serial.print(", ");
  Serial.print(av5);
  Serial.print(", ");
  Serial.print(readInternalVcc());
  Serial.println(", Z");
  delay(900);
}

double currentSensor(int RawADC) {
    int    Sensitivity    = 67; // mV/A
    long   InternalVcc    = readInternalVcc();
    double ZeroCurrentVcc = InternalVcc / 2;
    double SensedVoltage  = (RawADC * InternalVcc) / 1024;
    double Difference     = SensedVoltage - ZeroCurrentVcc;
    double SensedCurrent  = Difference / Sensitivity;
    return SensedCurrent;
}

long readInternalVcc() {
    long result;
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2); //wait for Vref to settle
    ADCSRA |= _BV(ADSC);                                        
    while (bit_is_set(ADCSRA,ADSC));
    result = ADCL;
    result |= ADCH<<8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    return result;
}
