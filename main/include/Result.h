#ifndef RESULT_H
#define RESULT_H

#include <Arduino.h>

struct Result
{
    bool success;
    String failure_reason;
    String success_result;

    String toJSON();
};

#endif // RESULT_H