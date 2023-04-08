#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <string.h>

//
const int buzzer = 10;

// Change to 0.5 for a slower version of the song, 1.25 for a faster version
const float songSpeed = 1.0;

// Defining frequency of each music note
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988

// Music notes of the song, 0 is a rest/pulse
int notes[] = {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,

    NOTE_A4, NOTE_A4,
    //Repeat of first part
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,
    //End of Repeat

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4,

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4};

// Durations (in ms) of each music note of the song
// Quarter Note is 250 ms when songSpeed = 1.0
int durations[] = {
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,

    250, 125,
    //Rpeat of First Part
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,
    //End of Repeat

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500,

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500};
//


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ESP8266"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDzCFSr7XGTTzz8sbfatmcIaPqKA00tRTs"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://iot-project-c3f57-default-rtdb.asia-southeast1.firebasedatabase.app/"
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

#include "DHT.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
unsigned long previousMillis = 0; // variable to store the previous time
const long interval = 1000; // interval at which to update the LCD (in milliseconds)

#include <MQ2.h>  
int pin = A0;
MQ2 mq2(pin);
#define DHTPIN D7
#define DHTTYPE DHT22
#define GREEN D0
#define BLUE D3
#define RED D5
#define BUZZ D5
#define RELAY D8
DHT dht(DHTPIN, DHTTYPE);
void setup(){
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP().toString());
  Serial.println();
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  //Batas Firebase 
  dht.begin();
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  mq2.begin();
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(RELAY, OUTPUT);
}
void loop(){
  delay(100);
  float* values= mq2.read(true);
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  float c = mq2.readCO()*100;
  float s = mq2.readSmoke()*100;
  float g = mq2.readLPG()*100;

  unsigned long currentMillis = millis(); // get the current time

  Serial.print(F("\nHumidity : "));
  Serial.print(h);
  Serial.print(F("\nTemperature : "));
  Serial.print(t);
  Serial.print("\n");

  Firebase.RTDB.setInt(&fbdo,"Temperature", t);
  Firebase.RTDB.setInt(&fbdo,"Humidity", h);
  Firebase.RTDB.setInt(&fbdo,"CO", c);
  Firebase.RTDB.setInt(&fbdo,"Smoke", s);
  Firebase.RTDB.setInt(&fbdo,"LPG", g);

  //klasifikasi 

  if(t >= 18 && t <= 24 && h >= 30 && h <= 60 && s <= 100 && g <= 200){
    // ECO MOST IDEAL
    digitalWrite(GREEN, HIGH);
    Firebase.RTDB.setInt(&fbdo,"Level", 1);
  }
  else if(t >= 18 && t <= 24 &&  h < 30 && s <= 100 && g <= 200){
    // TERLALU RENDAH
    digitalWrite(GREEN, HIGH);
    Firebase.RTDB.setInt(&fbdo,"Level", 2);
  }
  else if(t >= 18 && t <= 24 &&  h > 60 && s <= 100 && g <= 200){
    // TERLALU TINGGI
    digitalWrite(GREEN, HIGH);
    Firebase.RTDB.setInt(&fbdo,"Level", 3);
  }
  else if(t >= 25 && t <= 30 && h >= 30 && h <= 60 && s <= 100 && g <= 200){
    // HANGAT
    digitalWrite(GREEN, HIGH);
    Firebase.RTDB.setInt(&fbdo,"Level", 4);
  }
  else if(t >= 25 && t <= 30 && h < 30 && s <= 100 && g <= 200){
    // HANGAT RENDAH
    digitalWrite(GREEN, HIGH);
    Firebase.RTDB.setInt(&fbdo,"Level", 5);
  }
  else if(t >= 25 && t <= 30 && h > 60 && s <= 100 && g <= 200){
    // HANGAT TINGGI
    digitalWrite(GREEN, HIGH);
    Firebase.RTDB.setInt(&fbdo,"Level", 6);
  }
  else if(t >= 31 && h >= 30 && h <= 60 && s <= 100 && g <= 200){
    // PANAS
    Firebase.RTDB.setInt(&fbdo,"Level", 7);
  }
  else if(t >= 31 && h < 30 && s <= 100 && g <= 200){
    // PANAS RENDAH
    Firebase.RTDB.setInt(&fbdo,"Level", 8);
  }
  else if(t >= 31 && h > 60 && s <= 100 && g <= 200){
    // PANAS TINGGI
    Firebase.RTDB.setInt(&fbdo,"Level", 9);
  }

  else if(t >= 18 && t <= 24 && h >= 30 && h <= 60 && s > 100 && s <= 500 && g <= 200){
    // ECO MOST IDEAL TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 10);
  }
  else if(t >= 18 && t <= 24 &&  h < 30 && s > 100 && s <= 500 && g <= 200){
    // TERLALU RENDAH TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 11);
  }
  else if(t >= 18 && t <= 24 &&  h > 60 && s > 100 && s <= 500 && g <= 200){
    // TERLALU TINGGI TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 12);
  }
  else if(t >= 25 && t <= 30 && h >= 30 && h <= 60 && s > 100 && s <= 500 && g <= 200){
    // HANGAT TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 13);
  }
  else if(t >= 25 && t <= 30 && h < 30 && s > 100 && s <= 500 && g <= 200){
    // HANGAT RENDAH TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 14);
  }
  else if(t >= 25 && t <= 30 && h > 60 && s > 100 && s <= 500 && g <= 200){
    // HANGAT TINGGI TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 15);
  }
  else if(t >= 31 && h >= 30 && h <= 60 && s > 100 && s <= 500 && g <= 200){
    // PANAS TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 16);
  }
  else if(t >= 31 && h < 30 && s > 100 && s <= 500 && g <= 200){
    // PANAS RENDAH TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 17);
  }
  else if(t >= 31 && h > 60 && s > 100 && s <= 500 && g <= 200){
    // PANAS TINGGI TAPI TERKONTAMINASI ASAP
    Firebase.RTDB.setInt(&fbdo,"Level", 18);
  }

  else if(t >= 18 && t <= 24 && h >= 30 && h <= 60 && s <= 100 && g > 200){
    // ECO MOST IDEAL TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 19);
  }
  else if(t >= 18 && t <= 24 &&  h < 30 && s <= 100 && g > 200){
    // TERLALU RENDAH TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 20);
  }
  else if(t >= 18 && t <= 24 &&  h > 60 && s <= 100 && g > 200){
    // TERLALU TINGGI TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 21);
  }
  else if(t >= 25 && t <= 30 && h >= 30 && h <= 60 && s <= 100 && g > 200){
    // HANGAT TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 22);
  }
  else if(t >= 25 && t <= 30 && h < 30 && s <= 100 && g > 200){
    // HANGAT RENDAH TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 23);
  }
  else if(t >= 25 && t <= 30 && h > 60 && s <= 100 && g > 200){
    // HANGAT TINGGI TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 24);
  }
  else if(t >= 31 && h >= 30 && h <= 60 && s <= 100 && g > 200){
    // PANAS TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 25);
  }
  else if(t >= 31 && h < 30 && s <= 100 && g > 200){
    // PANAS RENDAH TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 26);
  }
  else if(t >= 31 && h > 60 && s <= 100 && g > 200){
    // PANAS TINGGI TAPI TERKONTAMINASI GAS DAN BISA TERJADI KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 27);
  }
  else if(t >= 50 && h <= 30 && s > 500){
    // KEBAKARAN
    Firebase.RTDB.setInt(&fbdo,"Level", 28);
  }
  else{
    Serial.println("ERROR");
  }

  

  // for message
  if (Firebase.RTDB.get(&fbdo, "/Message")) {
    String switchValue = fbdo.stringData();

    if (strcmp(switchValue.c_str(), "true") == 0) {
      tone(BUZZ, 1000);
      Serial.println("Current Flowing");
      if (currentMillis - previousMillis >= interval) { // check if it's time to update the LCD
        previousMillis = currentMillis; // save the current time
        Firebase.RTDB.get(&fbdo, "/Text");
        String textValue = fbdo.stringData();
        Serial.println("Switch is ON");
        lcd.home();
        lcd.clear();
        lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
        lcd.print(textValue.substring(0,16));

        if (textValue.length() > 16) {         // check if the text is longer than 16 characters
          lcd.setCursor(0, 1);            // set the cursor position for row 2
          lcd.print(textValue.substring(16)); // display the remaining characters of the text on row 2
        }
      }
    } 
    else if (strcmp(switchValue.c_str(), "false") == 0) {
      analogWrite(BUZZ, 0);
      Serial.println("Current not Flowing");
      if (currentMillis - previousMillis >= interval) { // check if it's time to update the LCD
        previousMillis = currentMillis; // save the current time
        Serial.println("Switch is OFF");
        lcd.home();
        lcd.clear();
        lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
        lcd.print(F("Humidity   : "));
        lcd.print(h);
        lcd.print(F("%"));
        lcd.setCursor(0,1);   //Move cursor to character 0 on line 1
        lcd.print(F("Temperatur : "));
        lcd.print(t); 
        lcd.print(F("C"));
      }
    } 
    else {
      Serial.println("Unknown switch value");
      // Do something when the switch value is unknown
    }
  } 
  else {
    Serial.println("Failed to get switch value from Firebase");
  }


  // turn on fan
  if (Firebase.RTDB.get(&fbdo, "/Fan")) {
    String switchValue = fbdo.stringData();

    if (strcmp(switchValue.c_str(), "true") == 0) {
      const int totalNotes = sizeof(notes) / sizeof(int);
      // Loop through each note
      for (int i = 0; i < totalNotes; i++)
      {
        const int currentNote = notes[i];
        float wait = durations[i] / songSpeed;
        // Play tone if currentNote is not 0 frequency, otherwise pause (noTone)
        if (currentNote != 0)
        {
          tone(BUZZ, notes[i], wait); // tone(pin, frequency, duration)
        }
        else
        {
          noTone(BUZZ);
        }
        // delay is used to wait for tone to finish playing before moving to next loop
        delay(wait);
      }
      digitalWrite(RELAY, LOW);
      Serial.println("Current Fan Flowing");
      Serial.println("Switch is ON");
    } 
    else if (strcmp(switchValue.c_str(), "false") == 0) {
      digitalWrite(RELAY, HIGH);
      Serial.println("Current Fan not Flowing");
      Serial.println("Switch is OFF");
    }
    else {
      Serial.println("Unknown switch value");
      // Do something when the switch value is unknown
    }
  } 
  else {
    Serial.println("Failed to get switch value from Firebase");
  }
}

