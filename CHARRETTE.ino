#define LEFTMOTOR 11
#define RIGHTMOTOR 10
#define SERVOMOTOR 9

#define TRIG 4
#define ECHO 5
#define TRIG2 2
#define ECHO2 3

#define LED 6
#define LEFT_BUTTON 12
#define CENTER_BUTTON 7
#define RIGHT_BUTTON 8
#define UP_BUTTON 13
#define nMesureDistance 1

#define leftPosition 180
#define frontPosition 90

#include <Servo.h>

Servo motorL;
Servo motorR;
Servo servoMotor;

const float margin = 1.0f;
const int slowCorrector = 2;
const int normalCorrector = 10;
const int fastCorrector = 90;

long distanceReadUpdate;
long ledupdate;

bool launched = false;
bool ledState = false;

void setup() {

  pinMode(LEFTMOTOR, OUTPUT);
  pinMode(RIGHTMOTOR, OUTPUT);
  pinMode(SERVOMOTOR, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(LED, OUTPUT);
  
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(CENTER_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);

  motorL.attach(LEFTMOTOR);
  motorR.attach(RIGHTMOTOR);
  servoMotor.attach(SERVOMOTOR);

  servoMotor.write(leftPosition);
  motorR.write(90);
  motorL.write(90);
  digitalWrite(LED, LOW);

  Serial.begin(9600);

  distanceReadUpdate = millis();
  ledupdate = millis();

  // Wait for button to start
  while(!(digitalRead(CENTER_BUTTON) || launched));
  Serial.println("Starting");
  launched = true;
}


//Distance to the right
float getDistanceD()
{
  float result = 0;

  for(int i = 0; i < nMesureDistance; i++)
  {
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    result += 0.017 * pulseIn(ECHO, HIGH);

    delayMicroseconds(1000);
  }

  return result/nMesureDistance;

}

//distance to the left
float getDistanceG()
{
  float result = 0;

  for(int i = 0; i < nMesureDistance; i++)
  {
    digitalWrite(TRIG2, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG2, LOW);

    result += 0.017 * pulseIn(ECHO2, HIGH);

    delayMicroseconds(1000);
  }

  return result/nMesureDistance;
}

void printDistances(float distanceD, float distanceG)
{
  Serial.println("Distance D : ");
  Serial.println(distanceD);
  Serial.println("Distance G : ");
  Serial.println(distanceG);
}

void printDistances()
{
  Serial.println("Distance D : ");
  Serial.println(getDistanceD());
  Serial.println("Distance G : ");
  Serial.println(getDistanceG());
}

void calibrate()
{
  long time = 0;
}

void goForwardXMillis(int interval)
{
  Serial.println("Going forward!");
  motorL.write(180);
  motorR.write(0);

  delay(interval);

  motorL.write(90);
  motorR.write(90);
}

void slightLeft(float error)
{
  Serial.print("I want ot go left with error ");
  Serial.println(error);
  
  motorL.write(100 - error);
  motorR.write(0);

  delay(200);

  motorL.write(90);
  motorR.write(90);
}

void slightLeft()
{
  Serial.println("Turning left");
  motorL.write(90 + slowCorrector);
  motorR.write(90 - normalCorrector);
  delay(200);
  motorL.write(90);
  motorR.write(90);
}

void slightRight(float error)
{
  Serial.println("I want ot go right");
  
  motorL.write(180);
  motorR.write(80 + error);

  delay(200);

  motorL.write(90);
  motorR.write(90);
}

void slightRight()
{  
  Serial.println("Turning right");
  motorL.write(90 + normalCorrector);
  motorR.write(90 - slowCorrector);
  delay(200);
  motorL.write(90);
  motorR.write(90);
}

void turnLeftandRight()
{
  for(int i = 0; i < 180; i++)
  {
    Serial.println(i);
    motorL.write(i);
    motorR.write(180-i);
    delay(100);
  }
}

void goStraight()
{
  float distanceRight = getDistanceD();
  float distanceLeft = getDistanceG();

  long time = millis();

  while(time + 20000 > millis())
  {
    distanceRight = getDistanceD();
    distanceLeft = getDistanceG();
    printDistances(distanceRight, distanceLeft);
    //if(distanceRight - margin > distanceLeft) slightLeft(distanceRight - margin - distanceLeft);
    //else if(distanceLeft - margin > distanceRight) slightRight(distanceLeft - margin - distanceRight);
    if(distanceRight - margin > distanceLeft) slightRight();
    else if(distanceLeft - margin > distanceRight) slightLeft();
    else
    {
      goForwardXMillis(300);
    }
    //delay(2000);
    toggle_led();
  }
}

void toggle_led() {
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

void advanceBlock()
{
  motorL.write(180);
  motorR.write(0);

  delay(1750);

  motorL.write(90);
  motorR.write(90);
}

void turnLeft()
{
  motorL.write(0);
  motorR.write(0);

  delay(765);

  motorL.write(90);
  motorR.write(90);
}

void turnRight()
{
  motorL.write(180);
  motorR.write(180);

  delay(765);

  motorL.write(90);
  motorR.write(90);
}

void checkPhysicalInput()
{
  if(digitalRead(UP_BUTTON)) advanceBlock();
  if(digitalRead(LEFT_BUTTON)) turnLeft();
  if(digitalRead(RIGHT_BUTTON)) turnRight();
}

void checkConsoleInput()
{
  int incomingByte = 0; 
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }
  if(incomingByte == 10) return;
  if(incomingByte == 119) goStraight();
  if(incomingByte == 97) turnLeft();
  if(incomingByte == 100) turnRight();
}

void loop() { 

  if(distanceReadUpdate + 500 < millis())
  {
    distanceReadUpdate = millis();
    Serial.println("======= New mesures =======");
    printDistances();
    Serial.println();
  }
  if(ledupdate + 300 < millis())
  {
    ledupdate = millis();
    toggle_led();
  }

  checkPhysicalInput();
  checkConsoleInput();

}