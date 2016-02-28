 
void setup()
{
  Serial.begin(9600); 

}

long averageBeats()
{
    int sensorValue = analogRead(A0);
    long start;
    long finish;
    long time1; 
    long start2;
    long finish2;
    long time2;
    long start3;
    long finish3;
    long time3;
    while (sensorValue <= 340);
    start  = millis();
    while (sensorValue >= 250);
    finish = millis();
    time1 = finish - start;

    while (sensorValue <= 340);
    start2  = millis();
    while (sensorValue >= 250);
    finish2 = millis();
    time2 = finish2 - start2;


    while (sensorValue <= 340);
    start3  = millis();
    while (sensorValue >= 250);
    finish3 = millis();
    time3 = finish3 - start3;


    return (time1+time2+time3)/3;
}
 long originalBeats = averageBeats();
void loop()
{
    
    delay (180000); //3 minutes

    if (abs(averageBeats()-originalBeats) > 300)
    {
      //buzz;
    }
    
      

    
}

