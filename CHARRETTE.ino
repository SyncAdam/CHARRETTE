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
#define keepDistanceG 4
#define keepDistanceD 6

#define leftPosition 180
#define frontPosition 90

#include <Servo.h>

Servo motorL;
Servo motorR;
Servo servoMotor;

const float margin = 1.0f;
const int intmargin = 1;
const int slowCorrector = 2;
const int normalCorrector = 10;
const int fastCorrector = 90;

const int limitNoWall = 13; // If a sensor detects something further than "limitNoWall" cm away, it means there's a door

long distanceReadUpdate;
long ledupdate;

bool launched = false;
bool ledState = false;

void setup() {

  //initialise pins for input or output
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

  //create motors and attach their respective pins
  motorL.attach(LEFTMOTOR);
  motorR.attach(RIGHTMOTOR);
  servoMotor.attach(SERVOMOTOR);

  //position/stop motors, wave ;)
  motorR.write(90);
  motorL.write(90);
  digitalWrite(LED, LOW);
  servoMotor.write(frontPosition);
  delay(500);
  servoMotor.write(leftPosition);

  //initialise serial communication with the pc
  Serial.begin(9600);

  //initialise timer variables to current time
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
  //take 'nMesureDistance' number of mesures and send their arithmetic mean
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
  //take 'nMesureDistance' number of mesures and send their arithmetic mean
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

/**
  goForwardXMillis makes the robot advance forward for 'intervals' milliseconds
   @param int interval - the number of milliseconds the robot will advance for
*/
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

/**
  slightLeft make the robot make a slight left turn while continuing straight for 200 milliseconds
*/
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

/**
  slightLeft make the robot make a slight right turn while continuing straight for 200 milliseconds
*/
void slightRight()
{  
  Serial.println("Turning right");
  motorL.write(90 + normalCorrector);
  motorR.write(90 - slowCorrector);
  delay(200);
  motorL.write(90);
  motorR.write(90);
}

/**
  goStraight makes the robot go straight while trying to keep an equal distance to the left and the right
*/
void goStraight(int interval)
{
  float distanceRight = getDistanceD();
  float distanceLeft = getDistanceG();

  long time = millis();

  while(time + interval > millis())
  {
    //read distances
    distanceRight = getDistanceD();
    distanceLeft = getDistanceG();
    printDistances(distanceRight, distanceLeft);

    //if theres more space to the left then to the right we turn slightly left and the other way around for the distance on the right
    //else go forwards
    if(distanceRight - margin > distanceLeft) slightRight();
    else if(distanceLeft - margin > distanceRight) slightLeft();
    else
    {
      goForwardXMillis(300);
    }

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
  if(incomingByte == 119) followRightWall(20000);
  if(incomingByte == 97) turnLeft();
  if(incomingByte == 100) turnRight();
}

void followLeftWall(float interval)
{
  long time = millis();

  while(time + interval > millis())
  {
    float distanceLeft = getDistanceG();
    printDistances();
    if(distanceLeft < keepDistanceG) {
      slightRight();
  } else if(distanceLeft > keepDistanceG + intmargin) {
      slightLeft();
    } else {
      goForwardXMillis(300);
    }
  }
}

void followRightWall(float interval)
{
  long time = millis();


  servoMotor.write(frontPosition);
  delay(500);

  while(time + interval > millis())
  {
    float distanceRight = getDistanceD();
    float distanceFront = getDistanceG();
    printDistances();
    if(distanceFront < keepDistanceD) turnLeft();
    if(distanceRight < keepDistanceD) {
      slightLeft();
} else if(distanceRight > keepDistanceD + intmargin) {
      slightRight();
    } else {
      goForwardXMillis(300);
    }
  }

  servoMotor.write(leftPosition);
  delay(500);
}

void escape()
{
  float distanceLeft = getDistanceG();
  float distanceRight = getDistanceD();
  //======================


}

void loop()
{
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