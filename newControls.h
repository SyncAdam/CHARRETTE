#ifndef NEWCONTROLS_H
#define NEWCONTROLS_H

#define motorForwardL 105
#define motorForwardR 75
#define motorMediumForwardL 95
#define motorMediumForwardR 85
#define motorMediumBackwardL 84
#define motorMediumBackwardR 96
#define motorSlowForwardL 92
#define motorSlowBackwardL 87
#define motorSlowForwardR 88
#define motorSlowBackwardR 93
#define motorStopL 90
#define motorStopR 90
#define motorReverseL 75
#define motorReverseR 105
#define motorGradientSmallStep 1
#define motorGradientBigStep 2

#define keepDistanceFront 4.7
#define keepDistanceR 5.2
#define keepDistanceL 5.2
#define margin 0.3
#define deltaMargin 0.25
#define bigTurnMargin 4.0
#define mediumTurnMargin 2.0

#define LED 6

#define leftTriggerTimemillis 500
#define rightTriggerTimemillis 500

#define nMesureDistance 5

#define leftPosition 180
#define frontPosition 100
#define motorGradientDelayms 20


#include <Servo.h>

//----------------------------------

    Servo motorL;
    Servo motorR;
    Servo servoMotor;

namespace control
{ 
    enum motorAction
    {
        STOPPED,
        FORWARD,
        SLIGHTRIGHT,
        SLIGHTLEFT,
        MEDIUMRIGHT,
        MEDIUMLEFT,
        RIGHT,
        LEFT
    };

    void toggle_led(bool* ledState) {
        *ledState = !*ledState;
        digitalWrite(LED, *ledState);
    }

    //distance to the left
    float getDistanceL()
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

    //distance to the front
    float getDistanceF()
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

    /**
    * 
    */
    void takeAction(int* action, int* lmotorSpeed, int* rmotorSpeed, float deltaD)
    {
       
        int newlmotorSpeed = *lmotorSpeed;
        int newrmotorSpeed = *rmotorSpeed;

        int wantedLSpeed, wantedRSpeed;

        int motorGradientStep = (deltaD > deltaMargin || deltaD < -deltaMargin) ? motorGradientBigStep : motorGradientSmallStep;

        switch(*action)
        {
            case motorAction::FORWARD:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorForwardR;
                break;

            case motorAction::SLIGHTLEFT:
                wantedLSpeed = motorMediumForwardL;
                wantedRSpeed = motorForwardR;
                break;

            case motorAction::SLIGHTRIGHT:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorMediumForwardR;
                break;

            case motorAction::MEDIUMLEFT:
                wantedLSpeed = motorMediumForwardL;
                wantedRSpeed = motorForwardR;
                break;

            case motorAction::MEDIUMRIGHT:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorMediumForwardR;
                break;


            case motorAction::LEFT:
                wantedLSpeed = motorMediumBackwardL;
                wantedRSpeed = motorForwardR;
                break;

            case motorAction::RIGHT:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorMediumBackwardR;
                break;

            case motorAction::STOPPED:
                wantedLSpeed = motorStopL;
                wantedRSpeed = motorStopR;
                break;

            default:
                wantedLSpeed = motorStopL;
                wantedRSpeed = motorStopR;
                break;

        }

        if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
        else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed - motorGradientStep;
        if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;
        else if(*rmotorSpeed < wantedRSpeed) newrmotorSpeed = *rmotorSpeed + motorGradientStep;

        if(newlmotorSpeed != *lmotorSpeed || newrmotorSpeed != *rmotorSpeed)
        {
            motorL.write(newlmotorSpeed);
            motorR.write(newrmotorSpeed);
            *lmotorSpeed = newlmotorSpeed;
            *rmotorSpeed = newrmotorSpeed;
        }

    }

    //arrays are pointers
    void shiftArray(int array[], int arraySize)
    {
        for(int i = 1; i < arraySize; i++)
        {
            array[i - 1] = array[i];
        }
    }

    void followRight(int interval)
    {
        const int lastMesureSize = 40;
        int lastRightMesures[lastMesureSize];
        int mesureRightIndex = 0;

        servoMotor.write(frontPosition);
        delay(500);

        bool ledState = false;

        float distanceFront = getDistanceF();
        float distanceRight = getDistanceR();

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
            distanceFront = getDistanceF();
            distanceRight = getDistanceR();            

            //Serial.println(distanceRight);

            mesureR2 = getDistanceR();

            if(distanceRight < keepDistanceR - margin - bigTurnMargin)
            {
                action = motorAction::LEFT;
                //Serial.println("\n Setting action to left\n");
            }
            else if (distanceRight > keepDistanceR + margin + bigTurnMargin)
            {
                action = motorAction::RIGHT;
                //Serial.println("\n Setting action to right\n");
            }
            else if (distanceRight < keepDistanceR - margin - mediumTurnMargin)
            {
                action = motorAction::MEDIUMLEFT;
                // Serial.println("\n Setting action to mediumLeft\n");
            }
            else if (distanceRight > keepDistanceR + margin + mediumTurnMargin)
            {
                action = motorAction::MEDIUMRIGHT;
                //Serial.println("\n Setting action to mediumRight\n");
            }
            else if(distanceRight < keepDistanceR - margin)
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
                if(leftTriggeredTimer + leftTriggerTimemillis < millis()) leftTriggered = false;
            }

            if(ledTimer + 500 < millis())
            {
                toggle_led(&ledState);
                ledTimer = millis();
            }

            if(!(mesureRightIndex < lastMesureSize )

            if(mesureRightIndex < lastMesureSize)
            {
                lastRightMesures[mesureRightIndex] = action;
                mesureRightIndex++;
            }
            else
            {
                shiftArray(lastRightMesures, lastMesureSize);
                lastRightMesures[lastMesureSize - 1] = action;
            }

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

    //In progress
    void followLeft(int interval)
    {
        Serial.println("Following left");
        servoMotor.write(frontPosition);
        delay(500);

        bool ledState = false;
        bool needToCheckFront = true;
        bool lookingFront = false;

        float distanceFront = getDistanceF();
        servoMotor.write(leftPosition);
        delay(500);
        float distanceLeft = getDistanceL();

        float mesureR1;
        float mesureR2;
        //======================

        long time = millis();

        int action = motorAction::FORWARD;
        int prevAction = motorAction::STOPPED;

        int lmotorSpeed = motorStopL;
        int rmotorSpeed = motorStopR;
        bool rightTriggered = false;
        long rightTriggeredTimer;

        long ledTimer = millis();
        long frontTimer = millis();

        //loop time fairly consistent, no need to take in accound in the deltaD

        while(time + interval > millis())
        {
            long myTime = millis();
            if(needToCheckFront) {
                if(!lookingFront) {
                    servoMotor.write(frontPosition);
                    lookingFront = true;
                }
                
                if(millis() - frontTimer >= 500) {
                    distanceFront = getDistanceF();
                    servoMotor.write(leftPosition);
                } else if(millis() - frontTimer - 500 >= 500) {
                    lookingFront = false;
                }
                
                if(distanceFront )
            }

            if(!lookingFront) {
                distanceLeft = getDistanceL();
            }
            

            //Serial.println(distanceRight);

            mesureR2 = getDistanceR();

            if(distanceLeft < keepDistanceL - margin - bigTurnMargin)
            {
                action = motorAction::RIGHT;
                //Serial.println("\n Setting action to left\n");
            }
            else if (distanceLeft > keepDistanceL + margin + bigTurnMargin)
            {
                action = motorAction::LEFT;
                //Serial.println("\n Setting action to right\n");
            }
            else if (distanceLeft < keepDistanceL - margin - mediumTurnMargin)
            {
                action = motorAction::MEDIUMRIGHT;
            // Serial.println("\n Setting action to mediumLeft\n");

            }
            else if (distanceLeft > keepDistanceL + margin + mediumTurnMargin)
            {
                action = motorAction::MEDIUMLEFT;
                //Serial.println("\n Setting action to mediumRight\n");
            }
            else if(distanceLeft < keepDistanceL - margin)
            {
                action = motorAction::SLIGHTRIGHT;
                //Serial.println("\n Setting action to slightLeft\n");
            }
            else if(distanceLeft > keepDistanceL + margin)
            {
                action = motorAction::SLIGHTLEFT;
                //Serial.println("\n Setting action to slightRight\n");
            }
            else
            {
                action = motorAction::FORWARD;
                //Serial.println("\n Setting action to forward\n");
            }

            if(distanceFront < keepDistanceFront)
            {
                rightTriggered = true;
                rightTriggeredTimer = millis();
                
                //Serial.println("\n Setting action to Left\n");
            }

            if(rightTriggered)
            {
                action = motorAction::RIGHT;
                if(rightTriggeredTimer + rightTriggerTimemillis < millis()) rightTriggered = false;
            }

            if(ledTimer + 500 < millis())
            {
                toggle_led(&ledState);
                ledTimer = millis();
            }

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