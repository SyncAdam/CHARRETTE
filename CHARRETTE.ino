#define LEFTMOTOR 9
#define RIGHTMOTOR 10
#define TRIG 5
#define ECHO 4
#define TRIG2 7
#define ECHO2 6

#define nMesureDistance 1

#include <Servo.h>

Servo motorL;
Servo motorR;

const float margin = 2.0f;

void setup() {

  pinMode(LEFTMOTOR, OUTPUT);
  pinMode(RIGHTMOTOR, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  motorL.attach(LEFTMOTOR);
  motorR.attach(RIGHTMOTOR);

  Serial.begin(9600);
}



float getDistance()
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

float getDistance2()
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

void printDistances()
{
  Serial.println("Distance 1 : ");
  Serial.println(getDistance());
  Serial.println("Distance 2 : ");
  Serial.println(getDistance2());
}

void calibrate()
{
  motorR.write(90);
  motorL.write(90);
  while(true)
  {
    delay(100);
  }
}

void goForwardXMillis(int interval)
{
  Serial.println("Going forward!");
  motorL.write(180);
  motorR.write(0);

  delay(200);

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

void slightRight(float error)
{
  Serial.println("I want ot go right");
  
  motorL.write(180);
  motorR.write(80 + error);

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
  float distanceLeft = getDistance();
  float distanceRight = getDistance2();

  while(true)
  {
    distanceLeft = getDistance();
    distanceRight = getDistance2();

    if(distanceLeft - margin > distanceRight) slightLeft(distanceLeft - margin - distanceRight);
    else if(distanceRight - margin > distanceLeft) slightRight(distanceRight - margin - distanceLeft);
    else
    {
      goForwardXMillis(100);
    }
    printDistances();
  }
}

void loop() {
  goStraight();
}