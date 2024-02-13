#ifndef CONTROL_H
#define CONTROL_H

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
            goForwardXMillis(300);
        }
    }

    lookForwardTime = (distanceForward / 19.0f)*1000                                              ; 

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
    if(distanceRight < keepDistanceD) 
    {
      slightLeft();
    }
    else if(distanceRight > keepDistanceD + intmargin)
    {
      slightRight();
    }
    else
    {
      goForwardXMillis(300);
    }
  }

  servoMotor.write(leftPosition);
  delay(500);
}

#endif