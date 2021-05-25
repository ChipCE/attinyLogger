void setup(){
    Serial.begin(115200);
    while(!Serial);
}


void loop(){
    Serial.print("{\"name\":\"temp\",\"value\":");
    Serial.print(rand() % 45);
    Serial.println("}");
    delay(1000);

    Serial.print("{\"name\":\"hum\",\"value\":");
    Serial.print(rand() % 100);
    Serial.println("}");
    delay(1000);
}