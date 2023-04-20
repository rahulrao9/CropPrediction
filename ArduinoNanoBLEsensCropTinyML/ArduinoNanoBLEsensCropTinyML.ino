#include <ArduinoBLE.h>

#include <Arduino_HTS221.h> // Arduino module for Humidity Temperature sensor

#include "CropClassifierModel.h"

#define BLE_UUID_TEST_SERVICE               "9A48ECBA-2E92-082F-C079-9E75AAE428B1"
#define BLE_UUID_COUNTER                    "1A3AC130-31EE-758A-BC50-54A61958EF81"
#define BLE_UUID_STRING                    "1A3AC131-31EF-758B-BC51-54A61958EF82"
#define BLE_UUID_BUFFER                    "1A3AC132-31ED-758C-BC52-54A61958EF82"
#define BLE_UUID_RESET_COUNTER              "FE4E19FF-B132-0099-5E94-3FFB2CF07940"

BLEService testService( BLE_UUID_TEST_SERVICE );
BLEUnsignedLongCharacteristic counterCharacteristic( BLE_UUID_COUNTER, BLERead | BLENotify );
BLEBoolCharacteristic resetCounterCharacteristic( BLE_UUID_RESET_COUNTER, BLEWriteWithoutResponse );
BLECharacteristic stringCharacteristic( BLE_UUID_STRING, BLERead | BLENotify,"TEST123456789012" );
BLECharacteristic buffCharacteristic( BLE_UUID_BUFFER, BLERead | BLENotify,20,(1==1) );

float old_temp = 0;
float old_hum = 0;
char buf[20];

void setup() {

  Serial.begin(9600);
  while (!Serial);

    if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);

  }

  setupBleMode();

}


void loop() {

  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();

//  float temperature=20;
// float humidity=20;

  static unsigned long counter = 0;
  static long previousMillis = 0;
  uint8_t x;
  String aS ="TEst1String";
  BLEDevice central = BLE.central();

  // check if the range values in temperature are bigger than 0,5 ºC
  // and if the range values in humidity are bigger than 1%

  if (abs(old_temp - temperature) >= 0.5 || abs(old_hum - humidity) >= 1 ){

    old_temp = temperature;
    old_hum = humidity;
    // print each of the sensor values
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity    = ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.println();

  }

  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");

  // print an empty line
  Serial.println();
  
  String results[22]={"rice", "maize", "chickpea", "kidneybeans", "pigeonpeas",
       "mothbeans", "mungbean", "blackgram", "lentil", "pomegranate",
       "banana", "mango", "grapes", "watermelon", "muskmelon", "apple",
       "orange", "papaya", "coconut", "cotton", "jute", "coffee"};
  
  float input[2]={temperature,humidity};
  
  Serial.print("Prediction: ");
  Serial.println(cropClassifier.predict(input));
  Serial.print("Recommended Crop: ");
  Serial.println(results[cropClassifier.predict(input)]);
  
  if ( central )
  {

    Serial.print( "Connected to central: " );
    Serial.println( central.address() );

    while ( central.connected() )
    {

      if( resetCounterCharacteristic.written() )
      {

        counter = 0;
        Serial.println( "Reset" );

      }

        stringCharacteristic.writeValue(results[cropClassifier.predict(input)].c_str());
        aS.toCharArray(buf,20);
        buffCharacteristic.writeValue( buf,20,true);

      long interval = 5;
      unsigned long currentMillis = millis();
      if( currentMillis - previousMillis > interval ){

        previousMillis = currentMillis;

        if(x++==0){
          Serial.print( "Central RSSI: " );
          Serial.println( central.rssi() );

        }

        if( central.rssi() != 0 ){

          counterCharacteristic.writeValue( counter++ );
          counter+=0x1000;

        }

      } // intervall

    } // while connected

    Serial.print( F( "Disconnected from central: " ) );
    Serial.println( central.address() );

  }

  // wait 1 second to print again
  delay(1000);

}


void setupBleMode()
{

  uint8_t i=0;
  BLE.begin();

  // set advertised local name and service UUID:
  BLE.setDeviceName( "Arduino Nano 33 BLE" );
  BLE.setLocalName( "Arduino Nano 33 BLE" );
  BLE.setAdvertisedService( testService );

  // BLE add characteristic
  testService.addCharacteristic( stringCharacteristic );

  // add service
  BLE.addService( testService );
  for(i=0;i<20;i++) buf[i]=i+1;

  // set the initial value for the characeristic:
  counterCharacteristic.writeValue( 0 );
  buf[10]=0;
  buffCharacteristic.writeValue( buf, 20, true );

  //  buffCharacteristic.
  //  start advertising.
  BLE.advertise();

}
