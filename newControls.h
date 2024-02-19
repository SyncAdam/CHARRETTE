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


    float lastMesures[40];
 
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

        servoMotor.write(leftPosition);

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

        servoMotor.write(frontPosition);

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
        /*
        long time = millis();
        Serial.print("Left motor speed = ");
        Serial.println(*lmotorSpeed);
        Serial.print("Right motor speed = ");
        Serial.println(*rmotorSpeed);
        Serial.print("DeltaD = ");
        Serial.println(deltaD);
        */
       
        int newlmotorSpeed = *lmotorSpeed;
        int newrmotorSpeed = *rmotorSpeed;

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

            case motorAction::MEDIUMLEFT:
                wantedLSpeed = motorMediumForwardL;
                wantedRSpeed = motorForwardR;

                if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
                else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed - motorGradientStep;
                if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;

                break;

            case motorAction::MEDIUMRIGHT:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorMediumForwardR;

                if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
                if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;
                else if(*rmotorSpeed < wantedRSpeed) newrmotorSpeed = *rmotorSpeed + motorGradientStep;

                break;


            case motorAction::LEFT:
                wantedLSpeed = motorMediumBackwardL;
                wantedRSpeed = motorForwardR;

                if(*lmotorSpeed < wantedLSpeed) newlmotorSpeed = *lmotorSpeed + motorGradientStep;
                else if(*lmotorSpeed > wantedLSpeed) newlmotorSpeed = *lmotorSpeed - motorGradientStep;
                if(*rmotorSpeed > wantedRSpeed) newrmotorSpeed = *rmotorSpeed - motorGradientStep;

                break;

            case motorAction::RIGHT:
                wantedLSpeed = motorForwardL;
                wantedRSpeed = motorMediumBackwardR;

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

    void followRight(int interval)
    {

        servoMotor.write(frontPosition);
        delay(500);

        bool ledState = false;

        float distanceFront = getDistanceL();
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
            distanceFront = getDistanceL();
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

    //En développement
    void followLeft(int interval)
    {

        servoMotor.write(frontPosition);
        delay(500);

        bool ledState = false;

        float distanceFront = getDistanceF();
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

        //loop time fairly consistent, no need to take in accound in the deltaD

        while(time + interval > millis())
        {
            long myTime = millis();
            distanceFront = getDistanceF();
            distanceLeft = getDistanceL();

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