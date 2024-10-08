#include <sensors.hpp>

SensorVal Sensors[SENSORNUM]; //up to SENSORNUM sensors will be monitored


#ifdef _USEBME680
  BME680_Class BME680;  ///< Create an instance of the BME680 class
  int32_t temperature, humidity, pressure, gas;
  uint32_t last_BME680 =0;
#endif

#ifdef _USEBME680_BSEC
  Bsec iaqSensor;
  
#endif


#ifdef DHTTYPE
  DHT dht(DHTPIN,DHTTYPE,11); //third parameter is for faster cpu, 8266 suggested parameter is 11
#endif

#ifdef _USEAHT
  AHTxx aht21(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
#endif



#ifdef _USEBMP

  Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//  #define BMP_SCK  (13)
//  #define BMP_MISO (12)
//  #define BMP_MOSI (11)
// #define BMP_CS   (10)
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK); //software SPI
#endif

#ifdef _USEBME

  Adafruit_BME280 bme; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//  #define BMP_SCK  (13)
//  #define BMP_MISO (12)
//  #define BMP_MOSI (11)
// #define BMP_CS   (10)
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK); //software SPI
#endif

#ifdef  _USESSD1306

  SSD1306AsciiWire oled;
#endif



void setupSensors() {

#ifdef _CHECKHEAT
  HEATPIN=0;
  #endif

  for (byte i=0;i<SENSORNUM;i++) {
    Sensors[i].snsType=SENSORTYPES[i];

    Sensors[i].Flags = 0;
    if (bitRead(MONITORED_SNS,i)) bitWrite(Sensors[i].Flags,1,1);
    else bitWrite(Sensors[i].Flags,1,0);
    
    if (bitRead(OUTSIDE_SNS,i)) bitWrite(Sensors[i].Flags,2,1);
    else bitWrite(Sensors[i].Flags,2,0);

    switch (SENSORTYPES[i]) {
      case 1: //DHT temp
        #ifdef DHTTYPE
          Sensors[i].snsPin=DHTPIN;
          snprintf(Sensors[i].snsName,31,"%s_T", ARDNAME);
          if (bitRead(OUTSIDE_SNS,i)) {
            Sensors[i].limitUpper = 88;
            Sensors[i].limitLower = 20;
          }
          else {
            Sensors[i].limitUpper = 80;
            Sensors[i].limitLower = 60;
          }
          Sensors[i].PollingInt=120;
          Sensors[i].SendingInt=2*60;          
        #endif
        break;
      case 2: //DHT RH
        #ifdef DHTTYPE
          Sensors[i].snsPin=DHTPIN;
          snprintf(Sensors[i].snsName,31,"%s_RH",ARDNAME);
          if (bitRead(OUTSIDE_SNS,i)) {
            Sensors[i].limitUpper = 95;
            Sensors[i].limitLower = 10;
          }
          else {
            Sensors[i].limitUpper = 65;
            Sensors[i].limitLower = 25;
          }
          Sensors[i].PollingInt=2*60;
          Sensors[i].SendingInt=5*60;
        #endif
        break;
      case 3: //soil
        #ifdef _USESOILCAP
          Sensors[i].snsPin=SOILPIN;
          snprintf(Sensors[i].snsName,31,"%s_soil",ARDNAME);
          Sensors[i].limitUpper = 290;
          Sensors[i].limitLower = 25;
          Sensors[i].PollingInt=120;
          Sensors[i].SendingInt=600;
        #endif
        #ifdef _USESOILRES
          Sensors[i].snsPin=SOILPIN;
          snprintf(Sensors[i].snsName,31,"%s_soilR",ARDNAME);
          Sensors[i].limitUpper = SOILR_MAX;
          Sensors[i].limitLower = 0;
          Sensors[i].PollingInt=120;
          Sensors[i].SendingInt=60*60;
        #endif

        break;
      case 4: //AHT temp
        #ifdef _USEAHT
          Sensors[i].snsPin=0;
          snprintf(Sensors[i].snsName,31,"%s_AHT_T",ARDNAME);
          if (bitRead(OUTSIDE_SNS,i)) {
            Sensors[i].limitUpper = 88;
            Sensors[i].limitLower = 20;
          }
          else {
            Sensors[i].limitUpper = 80;
            Sensors[i].limitLower = 60;
          }
          Sensors[i].PollingInt=10*60;
          Sensors[i].SendingInt=10*60;
        #endif
        break;
      case 5:
        #ifdef _USEAHT
          Sensors[i].snsPin=0;
          snprintf(Sensors[i].snsName,31,"%s_AHT_RH",ARDNAME);
          if (bitRead(OUTSIDE_SNS,i)) {
            Sensors[i].limitUpper = 95;
            Sensors[i].limitLower = 10;
          }
          else {
            Sensors[i].limitUpper = 65;
            Sensors[i].limitLower = 25;
          }
          Sensors[i].PollingInt=10*60;
          Sensors[i].SendingInt=10*60;
        #endif
        break;
  

      case 7: //dist
        Sensors[i].snsPin=0; //not used
        snprintf(Sensors[i].snsName,31,"%s_Dist",ARDNAME);
        Sensors[i].limitUpper = 100;
        Sensors[i].limitLower = 10;
        Sensors[i].PollingInt=100;
        Sensors[i].SendingInt=100;
        break;
      case 9: //BMP pres
        Sensors[i].snsPin=0; //i2c
        snprintf(Sensors[i].snsName,31,"%s_hPa",ARDNAME);
        Sensors[i].limitUpper = 1022; //normal is 1013
        Sensors[i].limitLower = 1009;
        Sensors[i].PollingInt=30*60;
        Sensors[i].SendingInt=60*60;
        break;
      case 10: //BMP temp
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_BMP_t",ARDNAME);
          if (bitRead(OUTSIDE_SNS,i)) {
            Sensors[i].limitUpper = 88;
            Sensors[i].limitLower = 25;
          }
          else {
            Sensors[i].limitUpper = 80;
            Sensors[i].limitLower = 60;
          }
        Sensors[i].PollingInt=30*60;
        Sensors[i].SendingInt=60*60;
        break;
      case 11: //BMP alt
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_alt",ARDNAME);
        Sensors[i].limitUpper = 100;
        Sensors[i].limitLower = -5;
        Sensors[i].PollingInt=60000;
        Sensors[i].SendingInt=60000;
        break;
      case 12: //Bar prediction
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_Pred",ARDNAME);
        Sensors[i].limitUpper = 0;
        Sensors[i].limitLower = 0; //anything over 0 is an alarm
        Sensors[i].PollingInt=60*60;
        Sensors[i].SendingInt=60*60;
        bitWrite(Sensors[i].Flags,3,1); //calculated
        bitWrite(Sensors[i].Flags,4,1); //predictive
        break;
      case 13: //BME pres
        Sensors[i].snsPin=0; //i2c
        snprintf(Sensors[i].snsName,31,"%s_hPa",ARDNAME);
        Sensors[i].limitUpper = 1022; //normal is 1013
        Sensors[i].limitLower = 1009;
        Sensors[i].PollingInt=30*60;
        Sensors[i].SendingInt=60*60;
        break;
      case 14: //BMEtemp
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_BMEt",ARDNAME);
          if (bitRead(OUTSIDE_SNS,i)) {
            Sensors[i].limitUpper = 88;
            Sensors[i].limitLower = 25;
          }
          else {
            Sensors[i].limitUpper = 80;
            Sensors[i].limitLower = 60;
          }
        Sensors[i].PollingInt=120;
        Sensors[i].SendingInt=5*60;
        break;
      case 15: //bme rh
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_BMErh",ARDNAME);
        if (bitRead(OUTSIDE_SNS,i)) {
          Sensors[i].limitUpper = 95;
          Sensors[i].limitLower = 10;
        }
        else {
          Sensors[i].limitUpper = 65;
          Sensors[i].limitLower = 25;
        }
        Sensors[i].PollingInt=120;
        Sensors[i].SendingInt=5*60;
        break;
      case 16: //bme alt
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_alt",ARDNAME);
        Sensors[i].limitUpper = 100;
        Sensors[i].limitLower = -5;
        Sensors[i].PollingInt=15*60*60;
        Sensors[i].SendingInt=15*60*60;
        break;
      case 17: //bme680
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_T",ARDNAME);
        if (bitRead(OUTSIDE_SNS,i)) {
          Sensors[i].limitUpper = 88;
          Sensors[i].limitLower = 25;
        }
        else {
          Sensors[i].limitUpper = 80;
          Sensors[i].limitLower = 60;
        }
        Sensors[i].PollingInt=15*60;
        Sensors[i].SendingInt=15*60;
        break;
      case 18: //bme680
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_RH",ARDNAME);
        if (bitRead(OUTSIDE_SNS,i)) {
          Sensors[i].limitUpper = 95;
          Sensors[i].limitLower = 10;
        }
        else {
          Sensors[i].limitUpper = 65;
          Sensors[i].limitLower = 25;
        }
        Sensors[i].PollingInt=15*60;
        Sensors[i].SendingInt=15*60;
        break;
      case 19: //bme680
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_hPa",ARDNAME);
        Sensors[i].limitUpper = 1020;
        Sensors[i].limitLower = 1012;
        Sensors[i].PollingInt=60*60;
        Sensors[i].SendingInt=60*60;
        break;
      case 20: //bme680
        Sensors[i].snsPin=0;
        snprintf(Sensors[i].snsName,31,"%s_gas",ARDNAME);
        Sensors[i].limitUpper = 1000;
        Sensors[i].limitLower = 50;
        Sensors[i].PollingInt=1*60;
        Sensors[i].SendingInt=1*60;
        break;

    #ifdef _CHECKHEAT

      case 55: //heat
        Sensors[i].snsPin=DIOPINS[HEATPIN];
        snprintf(Sensors[i].snsName,31,"%s_%s",ARDNAME,HEATZONE[HEATPIN++]);
        pinMode(Sensors[i].snsPin, INPUT_PULLUP);
        Sensors[i].limitUpper = 5;
        Sensors[i].limitLower = LOW;
        Sensors[i].PollingInt=1*60;
        Sensors[i].SendingInt=10*60;
        break;
    #endif


    #ifdef _CHECKAIRCON
      case 56: //aircon compressor
        Sensors[i].snsPin=DIOPINS[0];
        pinMode(Sensors[i].snsPin, INPUT_PULLUP);
        snprintf(Sensors[i].snsName,31,"%s_comp",ARDNAME);
        Sensors[i].limitUpper = 5;
        Sensors[i].limitLower = LOW;
        Sensors[i].PollingInt=1*60;
        Sensors[i].SendingInt=10*60;
        break;
      case 57: //aircon fan
        Sensors[i].snsPin=DIOPINS[1];
        pinMode(Sensors[i].snsPin, INPUT_PULLUP);
        snprintf(Sensors[i].snsName,31,"%s_fan",ARDNAME);
        Sensors[i].limitUpper = 5;
        Sensors[i].limitLower = LOW;
        Sensors[i].PollingInt=1*60;
        Sensors[i].SendingInt=10*60;
        break;
    #endif


    }

    Sensors[i].snsID=find_sensor_count((String) ARDNAME,Sensors[i].snsType); 
        
    Sensors[i].snsValue=0;
    Sensors[i].LastReadTime=0;
    Sensors[i].LastSendTime=0;  

  }

  #ifdef _CHECKHEAT
    HEATPIN=0;
  #endif


}


bool ReadData(struct SensorVal *P) {
  double val;
  bitWrite(P->Flags,0,0);
  
  switch (P->snsType) {
    case 1: //DHT temp
      #ifdef DHTTYPE
        //DHT Temp
        P->snsValue =  (dht.readTemperature()*9/5+32);
      #endif
      break;
    case 2: //DHT RH
      #ifdef DHTTYPE
        //DHT Temp
        P->snsValue = dht.readHumidity();
      #endif
      break;
    case 3: //soil
      #ifdef _USESOILCAP
        //soil moisture v1.2
        val = analogRead(P->snsPin);
        //based on experimentation... this eq provides a scaled soil value where 0 to 100 corresponds to 450 to 800 range for soil saturation (higher is dryer. Note that water and air may be above 100 or below 0, respec
        val =  (int) ((-0.28571*val+228.5714)*100); //round value
        P->snsValue =  val/100;
      #endif

      #ifdef _USESOILRES
        //soil moisture by stainless steel wire (Resistance)
        digitalWrite(_USESOILRES, HIGH);
        val = analogRead(P->snsPin);
        digitalWrite(_USESOILRES, LOW);
        //voltage divider, calculate soil resistance: Vsoil = 3.3 *r_soil / ( r_soil + r_fixed)
        //so R_soil = R_fixed * (3.3/Vsoil -1)
      
        val = val * (3.3 / 1023); //it's 1023 because the value 1024 is overflow
        P->snsValue =  (int) ((double) SOILRESISTANCE * (3.3/val -1)); //round value
        
      #endif

      break;
    case 4: //AHT Temp
      #ifdef _USEAHT
        //AHT21 temperature
          val = aht21.readTemperature();
          if (val != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
          {
            P->snsValue = (100*(val*9/5+32))/100; 
            #ifdef _DEBUG
              Serial.print(F("AHT Temperature...: "));
              Serial.print(P->snsValue);
              Serial.println(F("F"));
            #endif
          }
          else
          {
            #ifdef _DEBUG
              Serial.print(F("AHT Temperature Error"));
            #endif
          }
      #endif
      break;
    case 5: //AHT RH
      //AHT21 humidity
        #ifdef _USEAHT
          val = aht21.readHumidity();
          if (val != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
          {
            P->snsValue = (val*100)/100; 
            #ifdef _DEBUG
              Serial.print(F("AHT HUmidity...: "));
              Serial.print(P->snsValue);
              Serial.println(F("%"));
            #endif
          }
          else
          {
            #ifdef _DEBUG
              Serial.print(F("AHT Humidity Error"));
            #endif
          }
      #endif
      break;

    case 7: //dist
      #ifdef _USEHCSR04
        #define USONIC_DIV 58   //conversion for ultrasonic distance time to cm
        digitalWrite(TRIGPIN, LOW);
        delayMicroseconds(2);
        //Now we'll activate the ultrasonic ability
        digitalWrite(TRIGPIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGPIN, LOW);

        //Now we'll get the time it took, IN MICROSECONDS, for the beam to bounce back
        long duration = pulseIn(ECHOPIN, HIGH);

        //Now use duration to get distance in units specd b USONIC_DIV.
        //We divide by 2 because it took half the time to get there, and the other half to bounce back.
        P->snsValue = (duration / USONIC_DIV); 
      #endif

      break;
    case 9: //BMP pres
      #ifdef _USEBMP
         P->snsValue = bmp.readPressure()/100; //in hPa
        #ifdef _USEBARPRED
          //adjust critical values based on history, if available
          if (P->snsValue<1009 && BAR_HX[0] < P->snsValue  && BAR_HX[0] > BAR_HX[2] ) {
            //pressure is low, but rising
            P->limitLower = 1000;
          } else {
            P->limitLower = 1009;
          }

          if (LAST_BAR_READ+60*60 < now()) {
            pushDoubleArray(BAR_HX,24,P->snsValue);
            LAST_BAR_READ = now();          
          }
        #endif

      #endif
      break;
    case 10: //BMP temp
      #ifdef _USEBMP
        P->snsValue = ( bmp.readTemperature()*9/5+32);
      #endif
      break;
    case 11: //BMP alt
      #ifdef _USEBMP
         P->snsValue = (bmp.readAltitude(1013.25)); //meters
      #endif
      break;
    case 12: //make a prediction about weather
      #ifdef _USEBARPRED
        /*rules: 
        3 rise of 10 in 3 hrs = gale
        2 rise of 6 in 3 hrs = strong winds
        1 rise of 1.1 and >1015 = poor weather
        -1 fall of 1.1 and <1009 = rain
        -2 fall of 4 and <1023 = rain
        -3 fall of 4 and <1009 = storm
        -4 fall of 6 and <1009 = strong storm
        -5 fall of 7 and <990 = very strong storm
        -6 fall of 10 and <1009 = gale
        -7 fall of 4 and fall of 8 past 12 hours and <1005 = severe tstorm
        -8 fall of 24 in past 24 hours = weather bomb
        //https://www.worldstormcentral.co/law%20of%20storms/secret%20law%20of%20storms.html
        */        
        //fall of >3 hPa in 3 hours and P<1009 = storm
        P->snsValue = 0;
        if (BAR_HX[2]>0) {
          if (BAR_HX[0]-BAR_HX[2] >= 1.1 && BAR_HX[0] >= 1015) {
            P->snsValue = 1;
            snprintf(WEATHER,22,"Poor Weather");
          }
          if (BAR_HX[0]-BAR_HX[2] >= 6) {
            P->snsValue = 2;
            snprintf(WEATHER,22,"Strong Winds");
          }
          if (BAR_HX[0]-BAR_HX[2] >= 10) {
            P->snsValue = 3;        
            snprintf(WEATHER,22,"Gale");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 1.1 && BAR_HX[0] <= 1009) {
            P->snsValue = -1;
            snprintf(WEATHER,22,"Rain");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 4 && BAR_HX[0] <= 1023) {
            P->snsValue = -2;
            snprintf(WEATHER,22,"Rain");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 4 && BAR_HX[0] <= 1009) {
            P->snsValue = -3;
            snprintf(WEATHER,22,"Storm");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 6 && BAR_HX[0] <= 1009) {
            P->snsValue = -4;
            snprintf(WEATHER,22,"Strong Storm");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 7 && BAR_HX[0] <= 990) {
            P->snsValue = -5;
            snprintf(WEATHER,22,"Very Strong Storm");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 10 && BAR_HX[0] <= 1009) {
            P->snsValue = -6;
            snprintf(WEATHER,22,"Gale");
          }
          if (BAR_HX[2]-BAR_HX[0] >= 4 && BAR_HX[11]-BAR_HX[0] >= 8 && BAR_HX[0] <= 1005) {
            P->snsValue = -7;
            snprintf(WEATHER,22,"TStorm");
          }
          if (BAR_HX[23]-BAR_HX[0] >= 24) {
            P->snsValue = -8;
            snprintf(WEATHER,22,"BOMB");
          }
        }
      #endif
      break;
    case 13: //BME pres
      #ifdef _USEBME
         P->snsValue = bme.readPressure(); //in Pa
        #ifdef _USEBARPRED
          //adjust critical values based on history, if available
          if (P->snsValue<1009 && BAR_HX[0] < P->snsValue  && BAR_HX[0] > BAR_HX[2] ) {
            //pressure is low, but rising
            P->limitLower = 1000;
          } else {
            P->limitLower = 1009;
          }

          if (LAST_BAR_READ+60*60 < now()) {
            pushDoubleArray(BAR_HX,24,P->snsValue);
            LAST_BAR_READ = now();          
          }
        #endif
      #endif
      break;
    case 14: //BMEtemp
      #ifdef _USEBME
        P->snsValue = (( bme.readTemperature()*9/5+32) );
      #endif
      break;
    case 15: //bme rh
      #ifdef _USEBME
      
        P->snsValue = ( bme.readHumidity() );
      #endif
      break;
    case 16: //BMEalt
      #ifdef _USEBME
         P->snsValue = (bme.readAltitude(1013.25)); //meters

      #endif
      break;
    #ifdef _USEBME680
    case 17: //bme680 temp
      read_BME680();
      P->snsValue = (double) (( ((double) temperature/100) *9/5)+32); //degrees F
      break;
    case 18: //bme680 humidity
      read_BME680();
      P->snsValue = ((double) humidity/1000); //RH%
      break;
    case 19: //bme680 air pressure
      read_BME680();
      P->snsValue = ((double) pressure/100); //hPa
      break;
    case 20: //bme680 gas
      read_BME680();
      P->snsValue = (gas); //milliohms
      break;
    #endif

      case 55: //heat
        if (digitalRead(P->snsPin)==LOW) {
          P->snsValue += P->PollingInt/60; //snsvalue is the number of minutes the heat was on
          bitWrite(P->Flags,0,1); //for heat, flag if on        
        } else{
          bitWrite(P->Flags,0,0); //no heat
        }
        break;

      case 56: //aircon compressor
        if (digitalRead(P->snsPin)==LOW) {
          P->snsValue += P->PollingInt/60; //snsvalue is the number of minutes the ac was on
          bitWrite(P->Flags,0,1); //for heat, flag if on        
        } else{
          bitWrite(P->Flags,0,0); //no heat
        }
        break;
      case 57: //aircon fan
        if (digitalRead(P->snsPin)==LOW) {
          P->snsValue += P->PollingInt/60; //snsvalue is the number of minutes the ac was on
          bitWrite(P->Flags,0,1); //for fan, flag if on        
        } else{
          bitWrite(P->Flags,0,0); //no fan
        }
        break;
    
  }

  if (P->snsType<55)  checkSensorValFlag(P); //sets isFlagged, but don't do this for heat or ac
  P->LastReadTime = now(); //localtime
  

#ifdef _DEBUG
      Serial.println(" ");
      Serial.println("*****************");
      Serial.println("Reading Sensor");
      Serial.print("Device: ");
          Serial.println(P->snsName);
      Serial.print("SnsType: ");
          Serial.println(P->snsType);
      Serial.print("SnsID: ");
          Serial.println(P->snsID);
      Serial.print("Value: ");
          Serial.println(P->snsValue);
      Serial.print("LastLogged: ");
          Serial.println(P->LastReadTime);
      Serial.print("isFlagged: ");
          Serial.println(bitRead(P->Flags,0));          
      Serial.println("*****************");
      Serial.println(" ");

      #endif


return true;
}

byte find_limit_sensortypes(String snsname, byte snsType,bool highest){
  //returns nidex to sensorval for the entry that is flagged and highest for sensor with name like snsname and type like snsType
byte cnt = find_sensor_count(snsname,snsType);
byte j = 255;
byte ind = 255;
double snsVal;
if (highest) snsVal=-99999;
else snsVal = 99999;
bool newind = false;
  for (byte i=1;i<=cnt;i++) {
    newind = false;
    j = find_sensor_name(snsname,snsType,i);
    if (j!=255) {
      if (highest) { 
        if (snsVal<Sensors[j].snsValue) newind = true;
      } else {
        if (snsVal>Sensors[j].snsValue) newind = true;
      }

      if (newind) {
        snsVal = Sensors[j].snsValue;
        ind=j;
      }
    }
  }

  return ind;

}


byte find_sensor_count(String snsname,byte snsType) {
//find the number of sensors associated with sensor havign snsname 
String temp;
byte cnt =0;
  for (byte j=0; j<SENSORNUM;j++) {
    temp = Sensors[j].snsName; 

    if (temp.indexOf(snsname)>-1 && Sensors[j].snsType == snsType) cnt++;

  }

  return cnt;
}

byte find_sensor_name(String snsname,byte snsType,byte snsID) {
  //return the first sensor that has fieldname within its name
  //set snsID to 255 to ignore this field (this is an optional field, if not specified then find first snstype for fieldname)
  //returns 255 if no such sensor found
  String temp;
  for (byte j=0; j<SENSORNUM;j++) {
    temp = Sensors[j].snsName; 

    if (snsID==255) {
      if (temp.indexOf(snsname)>-1 && Sensors[j].snsType == snsType) return j;
    } else {
      if (temp.indexOf(snsname)>-1 && Sensors[j].snsType == snsType && Sensors[j].snsID == snsID) return j;
    }
  }
  return 255;
}

bool checkSensorValFlag(struct SensorVal *P) {
  if (P->snsValue>P->limitUpper || P->snsValue<P->limitLower) {
    bitWrite(P->Flags,0,1);
    if (P->snsValue>P->limitUpper) bitWrite(P->Flags,5,1);
    else bitWrite(P->Flags,5,0);
  }       
      
  else {
    bitWrite(P->Flags,0,0);
    bitWrite(P->Flags,5,0);
  }
  
return bitRead(P->Flags,0);


    #ifdef _DEBUG
      Serial.print("Setup ended. Time is ");
      Serial.println(dateify(now(),"hh:nn:ss"));

    #endif


}

uint8_t findSensor(byte snsType, byte snsID) {
  for (byte j=0;j<SENSORNUM;j++)  {
    if (Sensors[j].snsID == snsID && Sensors[j].snsType == snsType) return j; 
  }
  return 255;  
}

uint16_t findOldestDev() {
  int oldestInd = 0;
  int  i=0;
  for (i=0;i<SENSORNUM;i++) {
    if (Sensors[oldestInd].LastReadTime == 0) oldestInd = i;
    else if (Sensors[i].LastReadTime< Sensors[oldestInd].LastReadTime && Sensors[i].LastReadTime >0) oldestInd = i;
  }
  if (Sensors[oldestInd].LastReadTime == 0) oldestInd = 30000;

  return oldestInd;
}

void initSensor(byte k) {
  sprintf(Sensors[k].snsName,"");
  Sensors[k].snsID = 0;
  Sensors[k].snsType = 0;
  Sensors[k].snsValue = 0;
  Sensors[k].PollingInt = 0;
  Sensors[k].SendingInt = 0;
  Sensors[k].LastReadTime = 0;
  Sensors[k].LastSendTime = 0;  
  Sensors[k].Flags =0; 
}


uint8_t countDev() {
  uint8_t c = 0;
  for (byte j=0;j<SENSORNUM;j++)  {
    if (Sensors[j].snsID > 0) c++; 
  }
  return c;
}

void pushDoubleArray(double arr[], byte N, double value) {
  for (byte i = N-1; i > 0 ; i--) {
    arr[i] = arr[i-1];
  }
  arr[0] = value;

  return ;

/*
  double arr2[N];
  arr2[0] = value;

  for (byte i = 0 ; i < N-1 ; i++)
    {
        arr2[i+1] = *(arr+i);
        *(arr+i) = arr2[i];
    }
    *(arr+N-1) = arr2[N-1];
*/
}


#ifdef _USEBME680
void read_BME680() {
  
  uint32_t m = millis();
  
  if (last_BME680>m || m-last_BME680>500)   BME680.getSensorData(temperature, humidity, pressure, gas);  // Get readings
  else return;
  last_BME680=m;
}
#endif

#ifdef _USESSD1306

void redrawOled() {

  byte j;

  oled.clear();
  oled.setCursor(0,0);
  oled.println(WiFi.localIP().toString());
  oled.print(hour());
  oled.print(":");
  oled.println(minute());

  if (_OLEDTYPE == &Adafruit128x64) {       
    for (byte j=0;j<3;j++) {
      oled.print(SERVERIP[j].IP.toString());
      oled.print(":");
      oled.print(SERVERIP[j].server_status);
      if (j!=2) oled.println(" ");    
    }    
  }

  for (j=0;j<SENSORNUM;j++) {
    if (bitRead(Sensors[j].Flags,0))   oled.print("!");
    else oled.print("O");
    oled.println(" ");
    #ifdef _USEBARPRED
      if (Sensors[j].snsType==12) {
        oled.println(WEATHER);
      } 
    #endif
  }

  return;    
}

#endif

