#ifndef NEWCONTROLS_H
#define NEWCONTROLS_H

#define motorForwardL 105
#define motorForwardR 75
#define motorMediumForwardL 95
#define motorMediumForwardR 85
#define motorMediumBackwardL 84
#define motorMediumBackwardR 96
#define motorSlowForwardL 93
#define motorSlowBackwardL 87
#define motorSlowForwardR 88
#define motorSlowBackwardR 93
#define motorStopL 90
#define motorStopR 90
#define motorReverseL 75
#define motorReverseR 105
#define motorGradientSmallStep 1
#define motorGradientBigStep 2
#define servoStep 5

#define keepDistanceFront 4.7
#define keepDistanceR 5.2
#define keepDistanceL 5.2
#define margin 0.25
#define deltaMargin 0.25
#define bigTurnMargin 4.0
#define mediumTurnMargin 2.0

#define LED 6

#define leftTriggerTimemillis 500
#define rightTriggerTimemillis 500

#define nMesureDistance 3

#define leftPosition 180
#define frontPosition 100
#define motorGradientDelayms 20

#define directionForwardMargin 1.5
#define directionLRMargin 1.0


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
        LEFT,
        MEDIUMLEFT,
        SLIGHTLEFT,
        FORWARD,
        SLIGHTRIGHT,
        MEDIUMRIGHT,
        RIGHT
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
                wantedLSpeed = motorSlowForwardL;
                wantedRSpeed = motorForwardR;
                break;

            case motorAction::MEDIUMRIGHT:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorSlowForwardR;
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

    void detectMovement(int array[], int arraySize)
    {   
        float result = 0;
        for(int i = 0; i < arraySize; i++)
        {
            result += array[i];
        }

        result /= arraySize;

        if(result < motorAction::FORWARD + directionForwardMargin && result > motorAction::FORWARD - directionForwardMargin) Serial.println("I probably went forward");
        else if(result < motorAction::LEFT + directionLRMargin) Serial.println("I probably just turned left");
        else if(result > motorAction::RIGHT - directionLRMargin) Serial.println("I probably just turned right");
    }

    void followRight(int interval, int *servoPosition/*, int lastRightActions[], int lastActionsSize, int* actionRightIndex*/)
    {

        long started = millis();

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
        bool ignoreFront = false;

        if(*servoPosition != frontPosition)
        {
            ignoreFront = true;       
        }

        long ledTimer = millis();

        //loop time fairly consistent, no need to take in accound in the deltaD

        while(time + interval > millis())
        {
            if(!ignoreFront)
            {
                distanceFront = getDistanceF();
            }
            else
            {
                distanceFront = 100;
                if(*servoPosition - frontPosition < 0) *servoPosition += servoStep;
                else if(*servoPosition - frontPosition > 0) *servoPosition -= servoStep;

                servoMotor.write(*servoPosition);
            }
            distanceRight = getDistanceR();   

            while(started + 500 > millis())
            {
                distanceFront = 100;
            }         

            long myTime = millis();
            //Serial.println(distanceRight);

            mesureR2 = getDistanceR();

            if(distanceRight < keepDistanceR - margin - bigTurnMargin)
            {
                action = motorAction::MEDIUMLEFT;
                //Serial.println("\n Setting action to left\n");
            }
            else if (distanceRight > keepDistanceR + margin + bigTurnMargin)
            {
                action = motorAction::MEDIUMRIGHT;
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
            /*
            
            if(!(*actionRightIndex < lastActionsSize))
            {
                shiftArray(lastRightActions, lastActionsSize);
                lastRightActions[lastActionsSize - 1] = action;
            }
            else
            {
                lastRightActions[*actionRightIndex] = action;
                *actionRightIndex++;
            }
            */

            takeAction(&action, &lmotorSpeed, &rmotorSpeed, (mesureR2 - mesureR1));
            //detectMovement(lastRightActions, lastActionsSize);

            mesureR1 = getDistanceR();

            // we are getting ~13ms of loop time with Serial comm
            /*
            Serial.print("One loop took ");
            Serial.println(millis() - myTime);
            */
        }
        
        /*
        action = motorAction::STOPPED;

        servoMotor.write(leftPosition);

        while(lmotorSpeed != motorStopL || rmotorSpeed != motorStopR)
        {   
            takeAction(&action, &lmotorSpeed, &rmotorSpeed, 0);
            delay(20);
        }
        */
    }

    //In progress
    void followLeft(int interval, int* servoPosition/*, int lastRightActions[], int lastActionsSize, int* actionRightIndex*/)
    {

        while(*servoPosition != leftPosition)
        {
            if(*servoPosition - leftPosition < 0) *servoPosition += motorGradientSmallStep;
            else if(*servoPosition - leftPosition > 0) *servoPosition -= motorGradientSmallStep;
            servoMotor.write(*servoPosition);
            delay(1);
        }

        bool ledState = false;
        bool needToCheckFront = true;
        bool lookingFront = false;

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

            distanceLeft = getDistanceL();          

            //Serial.println(distanceRight);

            mesureR2 = getDistanceR();

            if(distanceLeft < keepDistanceL - margin - bigTurnMargin)
            {
                action = motorAction::MEDIUMRIGHT;
                //Serial.println("\n Setting action to left\n");
            }
            else if (distanceLeft > keepDistanceL + margin + bigTurnMargin)
            {
                action = motorAction::MEDIUMLEFT;
                //Serial.println("\n Setting action to right\n");
            }
            else if (distanceLeft < keepDistanceL - margin - mediumTurnMargin)
            {
                action = motorAction::MEDIUMRIGHT;
                //Serial.println("\n Setting action to mediumLeft\n");
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

            if(ledTimer + 500 < millis())
            {
                toggle_led(&ledState);
                ledTimer = millis();
            }

            takeAction(&action, &lmotorSpeed, &rmotorSpeed, (mesureR2 - mesureR1));

            mesureR1 = getDistanceR();

        }

        /*
        
        action = motorAction::STOPPED;

        //servoMotor.write(leftPosition);

        while(lmotorSpeed != motorStopL || rmotorSpeed != motorStopR)
        {   
            takeAction(&action, &lmotorSpeed, &rmotorSpeed, 0);
            delay(20);
        }
        */
    }

}

#endif