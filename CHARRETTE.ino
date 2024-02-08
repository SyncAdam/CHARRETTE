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

const float margin = 2.8f;

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

  Serial.begin(9600);
}

void loop() {

  // Wait for button to start
  while(!(digitalRead(CENTER_BUTTON) || launched));
  Serial.println("Starting");
  launched = true;

  //calibrate();
  //goForwardXMillis(100);
  //goStraight();

  float distanceLeft = getDistance();
  float distanceRight = getDistance2();

  while(true)
  {
    if(distanceLeft - margin > distanceRight) slightLeft();
    else if(distanceRight - margin > distanceLeft) slightRight();
    else
    {
      goForwardXMillis(5000);
    }
    distanceLeft = getDistance();
    distanceRight = getDistance2();
    printDistances();

    toggle_led();
  }
}

void toggle_led() {
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

float getDistance()
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  return 0.017 * pulseIn(ECHO, HIGH);  
}

float getDistance2()
{
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG2, LOW);

  return 0.017 * pulseIn(ECHO2, HIGH);  
}

void printDistances()
{
  Serial.println("Distance 1 : ");
  Serial.println(getDistance());
  delay(10);
  Serial.println("Distance 2 : ");
  Serial.println(getDistance2());
}

void calibrate()
{
  while(true)
  {
    digitalWrite(LEFTMOTOR, HIGH);
    digitalWrite(RIGHTMOTOR, HIGH);
    delayMicroseconds(1500);
    digitalWrite(LEFTMOTOR, LOW);
    digitalWrite(RIGHTMOTOR, LOW);
    delayMicroseconds(20000);
  }
}

void goForwardXMillis(int interval)
{
  Serial.println("Going forward!");
  unsigned long time = micros();

  unsigned long leftTime = time;
  unsigned long rightTime = time;

  digitalWrite(LEFTMOTOR, HIGH);
  digitalWrite(RIGHTMOTOR, HIGH);

  bool leftPulsed = true;
  bool rightPulsed = true;

  while(time + interval*1000 > micros())
  {
    unsigned long realTime = micros();

    if(leftTime + 1300 < realTime && leftPulsed)
    {
      leftTime = realTime;
      digitalWrite(LEFTMOTOR, LOW);
      leftPulsed = false;
    }
    else if(leftTime + 20000 < realTime && !leftPulsed)
    {
      leftTime = realTime;
      digitalWrite(LEFTMOTOR, HIGH);
      leftPulsed = true;
    }

    if(rightTime + 1700 < realTime && rightPulsed)
    {
      rightTime = realTime;
      digitalWrite(RIGHTMOTOR, LOW);
      rightPulsed = false;
    }
    else if(rightTime + 20000 < realTime && !rightPulsed)
    {
      rightTime = realTime;
      digitalWrite(RIGHTMOTOR, HIGH);
      rightPulsed = true;
    }
  }
}

void slightLeft()
{
  Serial.println("I want to go left");
  unsigned long time = micros();

  unsigned long leftTime = time;
  unsigned long rightTime = time;

  digitalWrite(LEFTMOTOR, HIGH);
  digitalWrite(RIGHTMOTOR, HIGH);

  bool leftPulsed = true;
  bool rightPulsed = true;

  while(time + 400000 > micros())
  {
    unsigned long realTime = micros();

/*
    if(leftTime + 1300 < realTime && leftPulsed)
    {
      leftTime = realTime;
      digitalWrite(LEFTMOTOR, LOW);
      leftPulsed = false;
    }
    else if(leftTime + 20000 < realTime && !leftPulsed)
    {
      leftTime = realTime;
      digitalWrite(LEFTMOTOR, HIGH);
      leftPulsed = true;
    }
*/
    if(rightTime + 1700 < realTime && rightPulsed)
    {
      rightTime = realTime;
      digitalWrite(RIGHTMOTOR, LOW);
      rightPulsed = false;
    }
    else if(rightTime + 20000 < realTime && !rightPulsed)
    {
      rightTime = realTime;
      digitalWrite(RIGHTMOTOR, HIGH);
      rightPulsed = true;
    }
  }
}

void slightRight()
{
  Serial.println("I want ot go right");
  unsigned long time = micros();

  unsigned long leftTime = time;
  unsigned long rightTime = time;

  digitalWrite(LEFTMOTOR, HIGH);
  digitalWrite(RIGHTMOTOR, HIGH);

  bool leftPulsed = true;
  bool rightPulsed = true;

  while(time + 400000 > micros())
  {
    unsigned long realTime = micros();


    if(leftTime + 1300 < realTime && leftPulsed)
    {
      leftTime = realTime;
      digitalWrite(LEFTMOTOR, LOW);
      leftPulsed = false;
    }
    else if(leftTime + 20000 < realTime && !leftPulsed)
    {
      leftTime = realTime;
      digitalWrite(LEFTMOTOR, HIGH);
      leftPulsed = true;
    }
/*
    if(rightTime + 1700 < realTime && rightPulsed)
    {
      rightTime = realTime;
      digitalWrite(RIGHTMOTOR, LOW);
      rightPulsed = false;
    }
    else if(rightTime + 20000 < realTime && !rightPulsed)
    {
      rightTime = realTime;
      digitalWrite(RIGHTMOTOR, HIGH);
      rightPulsed = true;
    }
*/
  }

}

void goStraight()
{
  float distanceLeft = getDistance();
  float distanceRight = getDistance2();

  while(true)
  {
    if(distanceLeft - margin > distanceRight) slightLeft();
    else if(distanceRight - margin > distanceLeft) slightRight();
    else
    {
      goForwardXMillis(100);
    }
    distanceLeft = getDistance();
    distanceRight = getDistance2();
    printDistances();
  }
}
