#include <AccelStepper.h>
#include <ArduinoJson.h>
#include "./stepperData.cpp"
#include <Arduino.h>
#ifndef EngineController_h
#define EngineController_h

class EngineController
{

private:
    int motorInterfaceType = 1;
    int position;
    int calibrationSpeed = 10000;

    int machineWidth = 380;
    int currentSpeed = 10000;

    StepperData *xAxis;
    StepperData *zAxis;
    JsonObject *sdConfig;

    void goToPosition(StepperData *axis, int pos)
    {
        int stepPos = pos * axis->steps_per_mm;
        int dir = 1;
        if (stepPos > axis->stepper->currentPosition())
        {
            dir = -1;
        }
        while (axis->stepper->currentPosition() != stepPos &&
               axis->stepper->currentPosition() <= (machineWidth * axis->steps_per_mm) &&
               axis->stepper->currentPosition() >= 0)
        {
            axis->stepper->setSpeed(currentSpeed * dir * axis->direction);
            axis->stepper->runSpeed();
        }
    }

    void moveTo(StepperData *axis, int pos)
    {
        int stepPos = pos * axis->steps_per_mm;
        int dir = 1;
        if (stepPos > axis->stepper->currentPosition())
        {
            dir = -1;
        }
        axis->stepper->setSpeed(currentSpeed);
        axis->stepper->runToNewPosition(stepPos);
    };

    void calibrateStepper(StepperData *axis)
    {
        position = 0;
        axis->stepper->setCurrentPosition(0);
        goToPosition(axis, 5);
        while (!digitalRead(axis->endPin) && axis->stepper->currentPosition() < machineWidth * axis->steps_per_mm)
        {
            axis->stepper->setSpeed(calibrationSpeed * axis->direction);
            axis->stepper->runSpeed();
        }
        axis->stepper->setCurrentPosition(0);
        goToPosition(axis, 5);
    }

public:
    EngineController(JsonObject *sdConfig)
    {
        this->sdConfig = sdConfig;
        xAxis = new StepperData(6, 5 * 4, -1, new AccelStepper(motorInterfaceType, 5, 8));
        zAxis = new StepperData(7, 25 * 16, -1, new AccelStepper(motorInterfaceType, 3, 2));
        xAxis->stepper->setAcceleration(5000);

        xAxis->stepper->setMaxSpeed(30000);
        xAxis->stepper->setCurrentPosition(0);
        zAxis->stepper->setMaxSpeed(30000);
        zAxis->stepper->setCurrentPosition(0);
    }
    void prepareDrink(int *array, int size)
    {
        this->calibrate();
        for (int i = 0; i < size; i++)
        {
            this->moveTo(xAxis, (*this->sdConfig)["positions"][array[i]].as<int>());
            delay(1000);
            this->goToPosition(zAxis, 50);
            delay(1000);
            this->goToPosition(zAxis, 0);
        }
    }
    void setSpeed(int speed)
    {
        this->currentSpeed = speed;
    }
    void moveTo(String *parameters, String *values, int size)
    {
        for (int i = 0; i < size; i++)
        {
            if (parameters[i].equals("s"))
            {
                this->setSpeed(values[i].toInt());
            }
            else if (parameters[i].equals("x"))
            {
                this->goToPosition(xAxis, values[i].toInt());
            }
            else if (parameters[i].equals("z"))
            {
                this->goToPosition(zAxis, values[i].toInt());
            }
        }
    }

    void calibrate()
    {
        calibrateStepper(xAxis);
        calibrateStepper(zAxis);
    }
};
#endif