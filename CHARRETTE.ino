#define LEFTMOTOR 11    //gauche
#define RIGHTMOTOR 10   //milieu
#define SERVOMOTOR 9    //droite

#define TRIG 4        //capteur de droite branché a gauche
#define ECHO 5
#define TRIG2 2       //capteur de gauche branché sur l'extremité
#define ECHO2 3

#define LED 6
#define LEFT_BUTTON 12
#define CENTER_BUTTON 7
#define RIGHT_BUTTON 8
#define UP_BUTTON 13
#define nMesureDistance 5
#define keepDistanceG 4
#define keepDistanceD 6

#include "newControls.h"

/**
 * Unimplemented idea:
 * We wanted to make the robot detect corners, and the idea was that the robot is "pre-programmable" such that detecting
 * a corner would make it either follow the left or the right wall, based on how it was programmed. This would in most 
 * scenarios work, if there are walls around all the time, but would possibly fail if the robot started in a box where
 * no wall are in the vicinity.  
*/

const int limitNoWall = 13; // If a sensor detects something further than "limitNoWall" cm away, it means there's a door

//const int directions[] = ''

//ew global variables
long distanceReadUpdate;
long ledupdate;

bool launched = false;
bool ledState = false;

enum MenuResult
{
  shortLeft,
  shortRight,
  shortForward,
  shortMid,
  longLeft,
  longRight,
  longForward,
  longMid
};

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

/**
 * This function toggles the LED on or off
*/
void toggle_led() {
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

/**
 * This function checks the inputs of the buttons situated on the robot
*/
void checkPhysicalInput()
{
  int position = leftPosition;
  if(digitalRead(UP_BUTTON)); //goStraight(30000);
  if(digitalRead(LEFT_BUTTON)); //control::followLeft(30000, &position);
  if(digitalRead(RIGHT_BUTTON)) escape();
}

/**
 * This function will likely make the robot escape the labyrinth, if the technology gods approve
*/
void escape()
{
  int movement = 0;
  int lastMovement = movement;

  const int firstStep = 2300;
  const int secondStep = 3500;
  const int thirdStep = 3000;
  const int forthStep = 10000;
  int servoPosition = leftPosition;

  int lmotorSpeed = motorStopL;
  int rmotorSpeed = motorStopR;

  long time = millis();  

  Serial.println("Going to follow Left 1");
  control::followLeft(firstStep, &servoPosition, &lmotorSpeed, &rmotorSpeed);
  Serial.println("Going to follow Right 1");
  control::followRight(secondStep, &servoPosition, &lmotorSpeed, &rmotorSpeed);
  Serial.println("Going to follow Left 2");
  control::followLeft(thirdStep, &servoPosition, &lmotorSpeed, &rmotorSpeed);
  Serial.println("Going to follow Right 2");
  control::followRight(forthStep, &servoPosition, &lmotorSpeed, &rmotorSpeed);
  Serial.println("Finished !");

  /*
  while(time + firstStep < millis())
  {
    control::followLeft(actionTimer, lastRightActions, lastActionsSize, &actionRightIndex);
  }
  time = millis();
  while(time + secondStep < millis())
  {
    control::followRight(actionTimer, lastRightActions, lastActionsSize, &actionRightIndex);
  }
  time = millis();
  while(time + thirdStep < millis())
  {
    control::followLeft(actionTimer, lastRightActions, lastActionsSize, &actionRightIndex);
  }
  time = millis();
  while(time + forthStep < millis())
  {
    control::followRight(actionTimer, lastRightActions, lastActionsSize, &actionRightIndex);
  }
  */
}


/*void waitMenuInput(int *result)
{
  while(false);
  
  //if multiple buttons are pressed, we acknowledge the first button pressed, we ignore the rest
  if(digitalRead(UP_BUTTON));
  if(digitalRead(LEFT_BUTTON)) followRight(20000);
  if(digitalRead(RIGHT_BUTTON)) followLeft(20000);
}

void menu()
{
  //stop wheels
  motorR.write(90);
  motorL.write(90); 
  int result;

  //long hold left or right button will follow the wall to the left or the right.
  waitMenuInput(&result);
  switch(result)
  {
    case MenuResult::longLeft:
      followLeftWall(300);
      break;
    case MenuResult::longRight:
      followRightWall(300);
      break;
  }
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
  if(incomingByte == 119) followLeftWall(20000);
  if(incomingByte == 97) turnLeft();
  if(incomingByte == 100) turnRight();
}*/

void loop()
{
  /*
  if(distanceReadUpdate + 500 < millis())
  {
    distanceReadUpdate = millis();

    Serial.println("======= New mesures =======");
    printDistances();
    Serial.println();
  }
  */

  if(ledupdate + 300 < millis())
  {
    ledupdate = millis();
    toggle_led();
  }

  checkPhysicalInput();
  //checkConsoleInput();

}