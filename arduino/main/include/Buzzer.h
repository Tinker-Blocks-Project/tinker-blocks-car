//
// Created by USER-Q on 6/4/2025.
//

#ifndef BUZZER_H
#define BUZZER_H


#include <Arduino.h>

class Buzzer
{
private:
    int Buzzer_PIN;

public:
    Buzzer(int buzzer_pin);

    void setup();
    void BuzzerRun();
    void BuzzerStop();
};


#endif //BUZZER_H
