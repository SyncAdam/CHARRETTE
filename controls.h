#ifndef CONTROL_H
#define CONTROL_H

#define leftPosition 180
#define frontPosition 90
#define motorGradientDelayms 20

#include <Servo.h>

Servo motorL;
Servo motorR;
Servo servoMotor;

enum motorAction
{
    STOPPED,
    FORWARD,
    SLIGHTRIGHT,
    SLIGHTLEFT,
    RIGHT,
    LEFT
};

const float margin = 1.0f;
const int intmargin = 1;
const int slowCorrector = 2;
const int normalCorrector = 10;
const int fastCorrector = 90;

int motorAction = 0;


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

void turnSensor(bool *lookingLeft)
{
    if(*lookingLeft == 1) servoMotor.write(frontPosition); 
    else servoMotor.write(leftPosition);
    *lookingLeft = !*lookingLeft;
}


/**
  goForwardXMillis makes the robot advance forward for 'intervals' milliseconds
   @param int interval - the number of milliseconds the robot will advance for
*/
void goForwardXMillis(int interval)
{
  Serial.println("Going forward!");
  for(int i = 0; i < 15; i++)
  {
    motorL.write(90 + i);
    motorR.write(90 - i);
    delay(motorGradientDelayms);
  }

  motorL.write(180);
  motorR.write(0);

  delay(interval);

  for(int i = 0; i < 15; i++)
  {
    motorL.write(105 - i);
    motorR.write(75 + i);
    delay(motorGradientDelayms);
  }
  
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
    //printDistances(distanceRight, distanceLeft);

    //if theres more space to the left then to the right we turn slightly left and the other way around for the distance on the right
    //else go forwards
    if(distanceRight - margin > distanceLeft) slightRight();
    else if(distanceLeft - margin > distanceRight) slightLeft();
    else
    {
      goForwardXMillis(300);
    }

    //toggle_led();
  }
}


void advanceBlock()
{

  for(int i = 0; i < 15; i++)
  {
    motorL.write(90 + i);
    motorR.write(90 - i);
    delay(motorGradientDelayms);
  }

  motorL.write(180);
  motorR.write(0);

  delay(1750);

  for(int i = 0; i < 15; i++)
  {
    motorL.write(105 - i);
    motorR.write(75 + i);
    delay(motorGradientDelayms);
  }
  
  motorL.write(90);
  motorR.write(90);
}

void turnLeft()
{
  for(int i = 0; i < 15; i++)
  {
    motorL.write(90 - i);
    motorR.write(90 - i);
    delay(motorGradientDelayms);
  }

  motorL.write(0);
  motorR.write(0);

  delay(200);

  
  for(int i = 0; i < 15; i++)
  {
    motorL.write(75 + i);
    motorR.write(75 + i);
    delay(motorGradientDelayms);
  }
  
  motorL.write(90);
  motorR.write(90);
}

void turnRight()
{

  for(int i = 0; i < 15; i++)
  {
    motorL.write(90 + i);
    motorR.write(90 + i);
    delay(motorGradientDelayms);
  }

  motorL.write(180);
  motorR.write(180);

  delay(150);

  for(int i = 0; i < 15; i++)
  {
    motorL.write(105 - i);
    motorR.write(105 - i);
    delay(motorGradientDelayms);
  }

  motorL.write(90);
  motorR.write(90);
}

void followLeftWall(float interval)
{  
  bool lookingLeft = true;

  long time = millis();
  float distanceLeft = getDistanceG();

  turnSensor(&lookingLeft);
  delay(500);
  turnSensor(&lookingLeft);

  float distanceForward = getDistanceG();

  long lookForwardTime = (distanceForward / 19.0f)*1000;

  Serial.println(lookForwardTime);

  while(time + interval > millis())
  { 
    while(time + lookForwardTime*1000 > millis())
    {
        distanceLeft = getDistanceG();
        printDistances();

        if(distanceLeft < keepDistanceG)
        {
            slightRight();
        }
        else if(distanceLeft > keepDistanceG + intmargin)
        {
            slightLeft();
        }
        else
        {
            goForwardXMillis(0);
        }
    }

    lookForwardTime = (distanceForward / 19.0f)*1000; 

    if(distanceLeft > keepDistanceG)
    {
        turnSensor(&lookingLeft);
        goForwardXMillis(500);
        
        distanceForward = getDistanceG();
        turnSensor(&lookingLeft);

        if(distanceForward < keepDistanceG) turnRight();
    }

  }
}

void manageMovement(int * motorAction, int * prevMotorAction)
{
    long time = millis();
    switch(*motorAction)
    {
        case motorAction::FORWARD:
            if(*prevMotorAction == motorAction::SLIGHTRIGHT)
            {
                for(int i = 0; i < normalCorrector + 15; i++)
                {
                    motorL.write(90 + normalCorrector + i);
                    motorR.write(90 - slowCorrector - i);
                    delay(motorGradientDelayms);
                }
                motorL.write(180);
                motorR.write(0);
            }
            else if(*prevMotorAction == motorAction::SLIGHTLEFT)
            {
                for(int i = 0; i < normalCorrector + 15; i++)
                {
                    motorL.write(90 + slowCorrector + i);
                    motorR.write(90 - normalCorrector - i);
                    delay(motorGradientDelayms);
                }
                motorL.write(180);
                motorR.write(0);
            }
            else
            {
                Serial.println("Bro idk what to do.");
            }
            break;

        case motorAction::SLIGHTLEFT:
            if(*prevMotorAction == motorAction::FORWARD)
            {
                for(int i = 0; i < 15 - slowCorrector; i++)
                {
                    motorL.write(105 - i);     //goes to 90 + slowCorrect
                    delay(motorGradientDelayms);
                }
            }
            else if(*prevMotorAction == motorAction::SLIGHTRIGHT)
            {
                for(int i = 0; i < 15 - slowCorrector; i++)
                {
                    motorL.write(105 - i);    //goes to 90 + slowCorrect
                    motorR.write(90 - slowCorrector - i);    //goes to 75 maybe?
                    delay(motorGradientDelayms);
                }
            }
            else
            {
                Serial.println("Bro idk what to do.");
            }
            break;

        case motorAction::SLIGHTRIGHT:
            if(*prevMotorAction == motorAction::FORWARD)
            {
                for(int i = 0; i < 15 - slowCorrector; i++)
                {
                    motorR.write(75 + i);
                    delay(motorGradientDelayms);
                }
            }
            else if(*prevMotorAction == motorAction::SLIGHTLEFT)
            {
                for(int i = 0; i < 15 - slowCorrector; i++)
                {
                    motorL.write(90 + slowCorrector + i);
                    motorR.write(75 + i);
                    delay(motorGradientDelayms);
                }
            }
            else
            {
              Serial.println("Bro idk what to do.");
            }
            break;

        default:
            Serial.println("Bro idk what to do.");
            break;
    }
    Serial.print("It took ");
    Serial.print(millis() - time);
    Serial.print(" ms-s to manage the movement");
}

void followRightWall(float interval)
{
  long time = millis();

  servoMotor.write(frontPosition);
  delay(500);
  int motorAction = motorAction::STOPPED;
  int prevMotorAction = motorAction;
  
  prevMotorAction = motorAction::FORWARD;
  motorAction = motorAction::FORWARD;

  while(time + interval > millis())
  {
    float distanceRight = getDistanceD();
    float distanceFront = getDistanceG();
    printDistances();

    if(distanceFront < keepDistanceG) turnLeft();
    if(distanceRight < keepDistanceG)
    {
        motorAction = motorAction::SLIGHTRIGHT;
    }
    else if(distanceRight > keepDistanceG + intmargin)
    {
        motorAction = motorAction::SLIGHTLEFT;
    }
    else
    {
        motorAction = motorAction::FORWARD;
    }

    if(motorAction != prevMotorAction)
    {
      manageMovement(&motorAction, &prevMotorAction);
    }
    prevMotorAction = motorAction;
  }

  motorAction = motorAction::STOPPED;

  manageMovement(&motorAction, &prevMotorAction)

  servoMotor.write(leftPosition);
  delay(500);
}

#endif