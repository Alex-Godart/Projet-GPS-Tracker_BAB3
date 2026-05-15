const int ledPin = 13;

bool ledEtat = false;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  

}

void loop() {
  if (Serial.available()>0){
    String ledCommand = Serial.readString();

    if (ledCommand == "led 13"){
       if (ledEtat == false){
        digitalWrite(ledPin, HIGH);
        ledEtat= true;
        
        Serial.print("B0.bco=1024");
        Serial.write (0xFF);
        Serial.write (0xFF);
        Serial.write (0xFF);

        Serial.print("B0.txt=\"Led 13 ON\"");
        Serial.write (0xFF);
        Serial.write (0xFF);
        Serial.write (0xFF);
        }
        else{
            digitalWrite(ledPin, LOW);
            
            Serial.print("B0.bco=63488");
            Serial.write (0xFF);
            Serial.write (0xFF);
            Serial.write (0xFF);
    
            Serial.print("B0.txt=\"Led 13 OFF\"");
            Serial.write (0xFF);
            Serial.write (0xFF);
            Serial.write (0xFF); 
            
            ledEtat= false;
            }
       
  
  }
  delay (100);
}
}
