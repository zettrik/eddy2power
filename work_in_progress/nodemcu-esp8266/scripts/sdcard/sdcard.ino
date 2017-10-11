/*
 NodeMCU to MicroSD-Card Adapter Wiring:
    D5->SCK
    D6->MISO
    D7->MOSI
    D8->CS (chipSelect GPIO15)
    GND->GND
    3.3V->VCC
 */
#include <SPI.h>
#include <SD.h>

const int chipSelect = 15; // D8
const int led1 = 16;

Sd2Card card;
SdVolume volume;
SdFile root;

void setup()
{
  Serial.begin(115200);
  Serial.print("\nInitializing SD card...");
  pinMode(led1, OUTPUT);
  digitalWrite(led1, 1);

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  }
  else {
    Serial.println("Wiring is correct and a card is present.");
  }

  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  //ls_dir();
  read_file();
  write_data();
}

void ls_dir() {
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void read_file() {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

void write_data(){
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(read_analog());
    dataFile.close();
    // print to the serial port too:
    Serial.println("data written to datalog.txt");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

float read_analog() {
  int times = 5;
  int analog_in_sum = 0;
  for(int i = 0; i < times; i++) {
    delay(3);
    analog_in_sum += analogRead(A0); // one read lasts about 100µs
  }   
  analog_in_sum /= times;
  return analog_in_sum;
}

void loop(void) {
    delay(9000);
    digitalWrite(led1, 0);
    write_data();
    delay(1000);
    digitalWrite(led1, 1);
}
