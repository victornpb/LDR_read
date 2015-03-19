/*


  http://jsfiddle.net/Victornpb/tszmLyv2/4/
*/

 #define LIGHT true
 #define DARK false

//float array[256];

int maxValue, minValue;
float threshold;

const int ldrPin = A0;
 
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}


void loop(){
  int x;
  Serial.println(F("Type size of the buffer:"));
  while (!Serial.available()){}
  
  while (Serial.available()) {
    x = Serial.parseInt();
  }
  test(x);
}

void test(int x) {
    
    Serial.println(F("Waiting for preamble calibration..."));
  
    //LIGHT
    //DARK
    //LIGHT
    //DARK
    calibrateDigital();
    
    Serial.println(F("Calibration completed."));
    
    Serial.print(F("Light:"));
    Serial.print(maxValue);
    Serial.print(F(" Dark:"));
    Serial.print(minValue);
    Serial.print(F(" Threshold:"));
    Serial.print(threshold);
    Serial.print("\n");
    
    
    int i=0;
    while(true){
      
      //wait for falling edge
      waitFor(LIGHT); //30ms
      waitFor(DARK);  //100ms
      
      delay(250); //wait for level to settle after dark
      
      digitalWrite(13,HIGH);
      Serial.print(sample()); //takes 60ms
      Serial.print("\n");
      digitalWrite(13,LOW);
      
      if(++i>x) break;
    }
    Serial.println(F("end"));
}


float sample(){
  
   int sensorValue = 1024-analogRead(ldrPin); //invert values for pullup config
   
   float avg = sensorValue;
   
   for(int i=0; i<3; ++i){
     avg += 1024-analogRead(A0);
     avg/=2;
     
     delay(20);
   }
   
   return avg;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    Serial.print(sample());
    Serial.print("\n");
  }
}


void calibrateDigital(){
    int newread;
    
    //get LIGHT value
    maxValue = sample();    
    while(true){
      int newread = sample();
      if(newread > maxValue) maxValue = newread;
      delay(100);
      
      if(newread < maxValue*0.75) break;
    }
    
    //get LOW value
    minValue = maxValue;
    while(true){
      int newread = sample();
      if(newread < minValue) minValue = newread;
      delay(100);
      
      if(newread > maxValue*0.75) break;
    }
    
    threshold = minValue + ((maxValue-minValue)/2);
    
    //get LIGHT value
    maxValue = sample();    
    while(true){
      int newread = sample();
      if(newread > maxValue) maxValue = newread;
      delay(100);
      
      if(digitalValue()==DARK) break;
    }
    
    //get LOW value
    minValue = maxValue;
    while(true){
      int newread = sample();
      if(newread < minValue) minValue = newread;
      delay(100);
      
      if(digitalValue()==LIGHT) break;
    }
    
    threshold = minValue + ((maxValue-minValue)/2);
}

bool digitalValue(){
  return (1024-analogRead(A0)) > maxValue*0.9 ? LIGHT : DARK;
  //return (1024-analogRead(ldrPin)) > threshold ? LIGHT : DARK;
}

void waitFor(bool x){
    while(true){
      if(digitalValue()==x) break;
    }
}
