#pragma once
#ifndef COLOR_PULSE_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define COLOR_PULSE_H
#include "CUESDK.h"

#include <windows.h>
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#endif

//std::vector<CorsairLedColor> getAvailableKeys();

void performPulseEffect(std::vector<CorsairLedColor> &ledColorsVec);

