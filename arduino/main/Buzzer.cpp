#include "include/Buzzer.h"

Buzzer::Buzzer(int buzzer_pin)
    : Buzzer_PIN(buzzer_pin) {
}

void Buzzer::setup() {
    pinMode(Buzzer_PIN, OUTPUT);
}


void Buzzer::BuzzerRun() {
    digitalWrite(Buzzer_PIN, HIGH);
}

void Buzzer::BuzzerStop() {
    digitalWrite(Buzzer_PIN, LOW);
}
