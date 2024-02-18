#ifndef NEWCONTROLS_H
#define NEWCONTROLS_H

#include "controls.h"

#define motorForwardL 105
#define motorForwardR 75
#define motorMediumForwardL 92
#define motorMediumForwardR 88
#define motorSlowForwardL 93
#define motorSlowForwardR 88
#define motorStopL 90
#define motorStopR 90
#define motorReverseL 75
#define motorReverseR 105
#define motorGradientSmallStep 2
#define motorGradientBigStep 2

#define keepDistanceFront 3.0
#define keepDistanceR 5.1
#define margin 0.3
#define deltaMargin 0.25

#define LED 6

#define leftTriggerTimemillis 200

#define nMesureDistance 3

namespace control
{

enum motorAction
{
    STOPPED,
    FORWARD,
    SLIGHTRIGHT,
    SLIGHTLEFT,
    RIGHT,
    LEFT
};

void toggle_led(bool* ledState) {
  *ledState = !*ledState;
  digitalWrite(LED, *ledState);
}

float getDistanceR()
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

void turn90Left()
{

}

void changeDirection(int* action)
{
    if(*action == motorAction::SLIGHTLEFT)  *action = motorAction::SLIGHTRIGHT;
    else if(*action == motorAction::SLIGHTRIGHT)    *action = motorAction::SLIGHTLEFT;
}

void takeAction(int* action, int* lmotorSpeed, int* rmotorSpeed, float deltaD)
{
    long time = millis();

    int newlmotorSpeed = *lmotorSpeed;
    int newrmotorSpeed = *rmotorSpeed;
    /*

    Serial.print("Left motor speed = ");
    Serial.println(*lmotorSpeed);
    Serial.print("Right motor speed = ");
    Serial.println(*rmotorSpeed);
    Serial.print("DeltaD = ");
    Serial.println(deltaD);
    */

    int wantedLSpeed, wantedRSpeed;

    int motorGradientStep = (deltaD > deltaMargin || deltaD < -deltaMargin) ? motorGradientBigStep : motorGradientSmallStep;

    switch(*action)
    {
        case motorAction::FORWARD:
            wantedLSpeed = motorForwardL;
            wantedRSpeed = motorForwardR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;

            break;

        case motorAction::SLIGHTLEFT:
            wantedLSpeed = motorMediumForwardL;
            wantedRSpeed = motorForwardR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed - motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;

            break;

        case motorAction::SLIGHTRIGHT:
            wantedLSpeed = motorForwardL;
            wantedRSpeed = motorMediumForwardR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;
            else if(*rmotorSpeed < wantedRSpeed) newrmotorSpeed = *rmotorSpeed + motorGradientStep;

            break;

        case motorAction::LEFT:
            wantedLSpeed = motorSlowForwardL;
            wantedRSpeed = motorForwardR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;

            break;

        case motorAction::RIGHT:
            wantedLSpeed = motorForwardL;
            wantedRSpeed = motorSlowForwardR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;
            else if(*rmotorSpeed < wantedRSpeed) newrmotorSpeed = *rmotorSpeed + motorGradientStep;

            break;

        case motorAction::STOPPED:
            wantedLSpeed = motorStopL;
            wantedRSpeed = motorStopR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed - motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;
            else if(*rmotorSpeed < wantedRSpeed) newrmotorSpeed = *rmotorSpeed + motorGradientStep;

            break;

        default:
            wantedLSpeed = motorStopL;
            wantedRSpeed = motorStopR;

            if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
            else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed - motorGradientStep;
            if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;
            else if(*rmotorSpeed < wantedRSpeed) newrmotorSpeed = *rmotorSpeed + motorGradientStep;

            break;

    }

    if(newlmotorSpeed != *lmotorSpeed || newrmotorSpeed != *rmotorSpeed)
    {
        motorL.write(newlmotorSpeed);
        motorR.write(newrmotorSpeed);
        *lmotorSpeed = newlmotorSpeed;
        *rmotorSpeed = newrmotorSpeed;
    }

    

    /*
    Serial.print("it took ");
    Serial.print(millis() - time);
    Serial.println(" ms to take action\n");
    */

}

void escape(int interval)
{

    servoMotor.write(frontPosition);
    delay(500);

    bool ledState = false;

    float distanceFront = getDistanceG();
    float distanceRight = getDistanceD();

    float mesureR1;
    float mesureR2;
    //======================

    long time = millis();

    int action = motorAction::FORWARD;
    int prevAction = motorAction::STOPPED;

    int lmotorSpeed = motorStopL;
    int rmotorSpeed = motorStopR;
    bool leftTriggered = false;
    long leftTriggeredTimer;

    long ledTimer = millis();

    //loop time fairly consistent, no need to take in accound in the deltaD

    while(time + interval > millis())
    {
        long myTime = millis();
        distanceFront = getDistanceG();
        distanceRight = getDistanceR();

        //Serial.println(distanceRight);

        mesureR2 = getDistanceR();

        if(distanceRight < keepDistanceR - margin)
        {
            action = motorAction::SLIGHTLEFT;
            //Serial.println("\n Setting action to slightLeft\n");
        }
        else if(distanceRight > keepDistanceR + margin)
        {
            action = motorAction::SLIGHTRIGHT;
            //Serial.println("\n Setting action to slightRight\n");
        }
        else
        {
            action = motorAction::FORWARD;
            //Serial.println("\n Setting action to forward\n");
        }

        if(distanceFront < keepDistanceFront)
        {
            leftTriggered = true;
            leftTriggeredTimer = millis();
            
            //Serial.println("\n Setting action to Left\n");
        }

        if(leftTriggered)
        {
            action = motorAction::LEFT;
            if(leftTriggeredTimer + leftTriggerTimemillis > millis()) leftTriggered = false;
        }

        if(ledTimer + 500 < millis())
        {
            toggle_led(&ledState);
            ledTimer = millis();
        }

        /*

        Serial.print("\n inputing ");
        Serial.print(mesureR2);
        Serial.print(" - ");
        Serial.print(mesureR1);
        Serial.println(" into take action function");
        */
        takeAction(&action, &lmotorSpeed, &rmotorSpeed, (mesureR2 - mesureR1));

        mesureR1 = getDistanceR();

    }
    
    action = motorAction::STOPPED;

    servoMotor.write(leftPosition);

    while(lmotorSpeed != motorStopL || rmotorSpeed != motorStopR)
    {   
        takeAction(&action, &lmotorSpeed, &rmotorSpeed, 0);
    }
}

}

#endif