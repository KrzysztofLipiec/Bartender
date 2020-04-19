#include <AccelStepper.h>

#ifndef stepperData_h
#define stepperData_h
class StepperData
{
public:
    int endPin;
    int steps_per_mm;
    int direction;
    AccelStepper *stepper;

    StepperData(int endPin, int steps_per_mm, int direction, AccelStepper *stepper)
    {
        this->endPin = endPin;
        this->steps_per_mm = steps_per_mm;
        this->direction = direction;
        this->stepper = stepper;
        pinMode(endPin, INPUT);
    }
};
#endif