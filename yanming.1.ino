#include "MeMCore.h"

//// Victory tune
MeBuzzer buzzer; // create the buzzer object

void celebrate() {
 // Each of the following "function calls" plays a single tone.
 // The numbers in the bracket specify the frequency and the duration (ms)
 buzzer.tone(392, 200);
 buzzer.tone(523, 200);
 buzzer.tone(659, 200);
 buzzer.tone(784, 200);
 buzzer.tone(659, 150);
 buzzer.tone(784, 400);
 buzzer.noTone();
}

//// Ultrasonic sensor
#define TURNING_TIME_MS 320 // The time duration (ms) for turning
MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
uint8_t motorSpeed = 255;
// Setting motor speed to an integer between 1 and 255
// The larger the number, the faster the speed

#define TIMEOUT 2000 // Max microseconds to wait; choose according to max distance of wall
#define SPEED_OF_SOUND 330 // Update according to your own experiment
#define ULTRASONIC 10
// If you are using Port 1 of mCore, the ultrasonic sensor uses digital pin 12
// If you are using Port 2 of mCore, the ultrasonic sensor uses digital pin 10

////Defining ports and pins
MeLineFollower lineFinder(PORT_1); // assigning lineFinder to RJ25 port 1

#define G_enable A4
#define A_input A0
#define B_input A1
#define IR_receiver A2
#define LDR_receiver A3

// Define time delay before the next RGB colour turns ON to allow LDR to stabilize
#define RGBWait 200 //in milliseconds
// Define time delay before tak`ing another LDR reading
#define LDRWait 10 //in milliseconds

//// Colour sensor
//placeholders for colour detected
int red = 0;
int green = 0;
int blue = 0;
double Hue = 0;
//floats to hold colour arrays
float colourArray[] = {0,0,0};
//float whiteArray[] = {990.00,1002.00,1001.00};
//float blackArray[] = {931.00,939.00,935.00};
float whiteArray[] = {945.00,985.00,981.00};
float blackArray[] = {805.00,881.00,862.00};
float greyDiff[] = {140.00,104.00,119.00};

char colourStr[3][5] = {"R = ", "G = ", "B = "};

//// movement functions
void move_forward()
{
leftMotor.run(-motorSpeed); // Negative: wheel turns anti-clockwise
rightMotor.run(motorSpeed-20); // Positive: wheel turns clockwise
}
void move_backward()
{
leftMotor.run(motorSpeed); // Negative: wheel turns clockwise
rightMotor.run(-motorSpeed); // Positive: wheel turns anti-clockwise 
}
void turn_left()
{
leftMotor.run(motorSpeed); // Positive: wheel turns clockwise
rightMotor.run(motorSpeed); // Positive: wheel turns clockwise
}
void turn_right()
{
leftMotor.run(-motorSpeed); // Positive: wheel turns anti-clockwise
rightMotor.run(-motorSpeed); // Positive: wheel turns anti-clockwise
}
void vehicle_stop()
{
leftMotor.stop(); // Stop left motor
rightMotor.stop(); // Stop right motor
}

//// bot state, 0 - debugging, 1 - moving, 2 - detect colour
int bot_state = 1;

//// decoder_state, controlling the 2 to 4 decoder
void decoder_state(int state) {
if (state == -1) {
  analogWrite(G_enable, 255);
}
else if (state == 0) {
  analogWrite(G_enable, 0);
  analogWrite(A_input, 0);
  analogWrite(B_input, 0);
}
else if (state == 1) {
  analogWrite(G_enable, 0);
  analogWrite(A_input, 255);
  analogWrite(B_input, 0);
}
else if (state == 2) {
  analogWrite(G_enable, 0);
  analogWrite(A_input, 0);
  analogWrite(B_input, 255);
}
else if (state == 3) {
  analogWrite(G_enable, 0);
  analogWrite(A_input, 255);
  analogWrite(B_input, 255);
}
}

void setup()
{
// Any setup code here runs only once:
pinMode(G_enable, OUTPUT);
pinMode(A_input, OUTPUT);
pinMode(B_input, OUTPUT);
pinMode(IR_receiver, INPUT);
pinMode(LDR_receiver, INPUT);
Serial.begin(9600); // to initialize the serial monitor
bot_state = 1;
//setBalance(); //calibration
}

void loop()
{
// The main code here will run repeatedly (i.e., looping):

  if (bot_state == 0) { // debugging code
  digitalWrite(A_input, HIGH);
  }
  if (bot_state == 1) { // movement mode
  decoder_state(3);

  //// line sensor
  int sensorState = lineFinder.readSensors(); // read the line sensor's state
  if (sensorState == S1_IN_S2_IN) //when black line is reached
  {
    vehicle_stop();
    bot_state = 2;
    delay(50);
  }

  //// ultrasonic sensor
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  long duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  float dist_cm = duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100;
  /*
  if (duration > 0) {
    Serial.print("distance(cm) = ");
    Serial.println(dist_cm);
  }
  else {
    Serial.print("out of range");
    Serial.println(dist_cm);
  }
  */
  float dx = dist_cm - 11.0;
  //Serial.println(dist_cm);

  else if (dist_cm == 0) {
    move_forward();
  }
  else if (dx <= 0) {
    leftMotor.run(-motorSpeed); // Negative: wheel turns anti-clockwise
    rightMotor.run(motorSpeed + dx*15 - 30); // Positive: wheel turns clockwise
  }
  else {
    leftMotor.run(-motorSpeed + dx*15); // Negative: wheel turns anti-clockwise
    rightMotor.run(motorSpeed - 30); // Positive: wheel turns clockwise
  }
  /*else if (dist_cm < 10) {
    leftMotor.run(-motorSpeed); // Negative: wheel turns anti-clockwise
    rightMotor.run(motorSpeed - 100); // Positive: wheel turns clockwise
  }
  else if (dist_cm > 15) {
    leftMotor.run(-motorSpeed + 100); // Negative: wheel turns anti-clockwise
    rightMotor.run(motorSpeed); // Positive: wheel turns clockwise
  }
  else {
    move_forward();
  }*/

  //// Infra-red sensor
  int val = analogRead(IR_receiver);  // read the input pin
  Serial.println(val);
  if (val < 700) {
    leftMotor.run(-motorSpeed+200);
    rightMotor.run(motorSpeed);
    delay(500);
  }
}
else if (bot_state == 2) { // colour detection mode
  for(int c = 0;c<=2;c++){    
    Serial.print(colourStr[c]);
    decoder_state(c); //turn ON the LED, red, green or blue, one colour at a time.
    delay(RGBWait);
    //get the average of 5 consecutive readings for the current colour and return an average 
    colourArray[c] = getAvgReading(5);
    //the average reading returned minus the lowest value divided by the maximum possible range, multiplied by 255 will give a value between 0-255, representing the value for the current reflectivity (i.e. the colour LDR is exposed to)
    colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;
    decoder_state(3);  //turn off the current LED colour
    delay(RGBWait);
    Serial.println(int(colourArray[c])); //show the value for the current colour LED, which corresponds to either the R, G or B of the RGB code
  }
  //Serial.println(0.299 * colourArray[0] + 0.587 * colourArray[1] + 0.114 * colourArray[2]);
  Hue = determine_colour(colourArray[0], colourArray[1], colourArray[2]);
  Serial.println(Hue);
    if (colourArray[0] > 200 && colourArray[1] > 220 && colourArray[2] > 220) //white
    {
      Serial.println("white");
      vehicle_stop();
      // Each of the following "function calls" plays a single tone.
      // The numbers in the bracket specify the frequency and the duration (ms)
      buzzer.tone(392, 200);
      buzzer.tone(523, 200);
      buzzer.tone(659, 200);
      buzzer.tone(784, 200);
      buzzer.tone(659, 150);
      buzzer.tone(784, 400);
      buzzer.noTone();
    }
    else if (colourArray[1] > colourArray[0] && colourArray[1] > colourArray[2]) //when G is max
    {
      
      turn_right();
      delay(TURNING_TIME_MS);
      vehicle_stop();
      delay(50);
      Serial.println("green");
      bot_state = 1;
    }
    else if(colourArray[0] > colourArray[1] && colourArray[0] > colourArray[2]) //when R is max
    {
      if (Hue < 10) { //red
        turn_left();
        delay(TURNING_TIME_MS);
        vehicle_stop();
        delay(50);
        
        Serial.println("red");
        bot_state = 1;
      
      }
      else { //orange
        turn_left();
        delay(2 * TURNING_TIME_MS);
        vehicle_stop();
        delay(50);
        
        Serial.println("orange");
        bot_state = 1;
      }
    }
    else if(colourArray[2] > colourArray[1] && colourArray[2] > colourArray[0]) //when blue is max
    {
      if (Hue > 205 || colourArray[1] < 190) 
      { //purple
        turn_left();
        delay(TURNING_TIME_MS);
        vehicle_stop();
        delay(50);
        move_forward();
        delay(750);
        vehicle_stop();
        delay(50);
        turn_left();
        delay(TURNING_TIME_MS);
        vehicle_stop();
        delay(50);
      
        Serial.println("purple");
        bot_state = 1;
      }
      else //when colour is blue
      { 
        turn_right();
        delay(TURNING_TIME_MS);
        vehicle_stop();
        delay(50);
        move_forward();
        delay(825);
        vehicle_stop();
        delay(50);
        turn_right();
        delay(TURNING_TIME_MS+40);
        //move_forward();
        //delay(750);
        vehicle_stop();
        delay(50);
        
      
        Serial.println("blue");
        bot_state = 1;
      }
    }
  }
}

//// calibration for white and black array
void setBalance()
{
  //set white balance
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);           //delay for five seconds for getting sample ready
  //scan the white sample.
  //go through one colour at a time, set the maximum reading for each colour -- red, green and blue to the white array
  for(int i = 0;i<=2;i++){
     decoder_state(i);
     delay(RGBWait);
     whiteArray[i] = getAvgReading(5);         //scan 5 times and return the average, 
     Serial.println(whiteArray[i]);
     decoder_state(3);
     delay(RGBWait);
  }
  //done scanning white, time for the black sample.
  //set black balance
  Serial.println("Put Black Sample For Calibration ...");
  delay(5000);     //delay for five seconds for getting sample ready 
  //go through one colour at a time, set the minimum reading for red, green and blue to the black array
  for(int i = 0;i<=2;i++){
     decoder_state(i);
     delay(RGBWait);
     blackArray[i] = getAvgReading(5);
     Serial.println(blackArray[i]);
     decoder_state(3);
     delay(RGBWait);
  //the differnce between the maximum and the minimum gives the range
     greyDiff[i] = whiteArray[i] - blackArray[i];
  }

  //delay another 5 seconds for getting ready colour objects
  Serial.println("Colour Sensor Is Ready.");
  delay(5000);
  }

//// getting colour array
int getAvgReading(int times){      
//find the average reading for the requested number of times of scanning LDR
  int reading;
  int total =0;
//take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
     reading = analogRead(LDR_receiver);
     total = reading + total;
     delay(LDRWait);
  }
//calculate the average and return it
  return total/times;
}

////calculating hue
int determine_colour(int R, int G, int B){
  double red = (double)R/255;
  double green = (double)G/255;
  double blue = (double)B/255;
  double newArray[3] = {red,green,blue};
  for(long i = 0; i < 2;i++){
    long flag = 0;
    for(long j = 0 ; j < 3 - (i + 1); j++){
      if (newArray[j+1] < newArray[j]){
          double temp = newArray[j + 1];
          newArray[j + 1] = newArray[j];
          newArray[j] = temp;
          flag += 1;
     }
      }
    if(flag == 0){
      break;
    }
  }
  
  double hue = 0;
  if(newArray[2] == red){
    hue = (green - blue)/(newArray[2] - newArray[0]);
  }
  else if(newArray[2] == green){
    hue = 2.0 + (blue - red)/(newArray[2] - newArray[0]);
  }
  else{
    hue = 4.0 + (red - green)/(newArray[2] - newArray[0]);
  }
  /*if(hue < 0){
    hue *= 60;
    return hue + 360;
  }*/
  return hue * 60;
}
