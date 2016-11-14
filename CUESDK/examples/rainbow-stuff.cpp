#include "CUESDK.h"

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <windows.h>
#include <future>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#define BUFFER 128

double getKeyboardHeight(CorsairLedPositions *ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
		[](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
		return clp1.top < clp2.top;
	});
	return minmaxLeds.second->top + minmaxLeds.second->height - minmaxLeds.first->top;
}

double getKeyboardWidth(CorsairLedPositions *ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
		[](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
		return clp1.left < clp2.left;
	});
	return minmaxLeds.second->left + minmaxLeds.second->width - minmaxLeds.first->left;
}

bool rectangle(CorsairLedPosition led, double left, double right, double top, double bottom, double height, double width)
{
	//if (left > 1 || right > 1 || top > 1 || bottom > 1) return false;
	if (led.left > width-left && led.left <= width-right && led.top >= top && led.top < bottom)
		return true;
	return false;
}

void flashingPowerBar()
{
	const auto ledPositions = CorsairGetLedPositions();
	if (ledPositions && ledPositions->numberOfLed > 0) {
		const auto keyboardWidth = getKeyboardWidth(ledPositions);
		const auto keyboardHeight = getKeyboardHeight(ledPositions);
		const auto numberOfSteps = 100;
		double inner;
		int counter = 0;


		for (auto n = 0; !GetAsyncKeyState(VK_ESCAPE); n++) {
			const auto currWidth = double(keyboardWidth) * (n % (numberOfSteps + 1))/ numberOfSteps;
			
			std::vector<CorsairLedColor> vec;
			int randn = rand() % ledPositions->numberOfLed;
			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto curLed = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				ledColor.ledId = curLed.ledId;
				inner = abs((curLed.left - keyboardWidth) / keyboardWidth);
                
                // Top fkey rainbow
				if (rectangle(curLed, keyboardWidth, 0, 0.21*keyboardHeight, keyboardHeight, keyboardHeight, keyboardWidth)){ //0.21*keyboardHeight + 12, keyboardHeight, currWidth)) {//top > 0.21
					ledColor.r = (128 + sin(inner * 10 + 2) * 127)*(n % 30) / 30;
					ledColor.g = (128 + sin(inner * 10 + 4) * 127)*(n % 30) / 30;
					ledColor.b = (128 + sin(inner * 10 + 6) * 127)*(n % 30) / 30;
				}
				// Spazz random dots
				if (i == randn) {
					ledColor.r = 255;
					ledColor.g = 255;
					ledColor.b = 255;
				}
				
                // Body pulsing rainbow
				if (rectangle(curLed, 12*5, 0, 0.21*keyboardHeight, 0.21*keyboardHeight + 12, keyboardHeight, currWidth)) {
					ledColor.r = 128 + sin(inner * 10 + 0) * 127;
					ledColor.g = 128 + sin(inner * 10 + 2) * 127;
					ledColor.b = 128 + sin(inner * 10 + 4) * 127;
				}
				else if (rectangle(curLed, 0, -12 * 5, 0.21*keyboardHeight, 0.21*keyboardHeight + 12, keyboardHeight, currWidth)) {
					ledColor.r = 128 + sin(inner * 10 + 0) * 127;
					ledColor.g = 128 + sin(inner * 10 + 2) * 127;
					ledColor.b = 128 + sin(inner * 10 + 4) * 127;
				}
				else if (rectangle(curLed, keyboardWidth, 12*5, 0.21*keyboardHeight, 0.21*keyboardHeight + 12, keyboardHeight, currWidth)) {
					ledColor.r = 255;
					ledColor.g = 255;
					ledColor.b = 255;
				}
				else if (rectangle(curLed, -12*5, -keyboardWidth, 0.21*keyboardHeight, 0.21*keyboardHeight + 12, keyboardHeight, currWidth)) {
					ledColor.r = 255;
					ledColor.g = 255;
					ledColor.b = 255;
				}
                
                // Numpad in all yellow
                /*
				if (rectangle(curLed, 12 * 6, 0, 10, keyboardHeight, keyboardHeight, keyboardWidth)) {
					ledColor.r = 255;
					ledColor.g = 255;
					ledColor.b = 0;
				}
                */
                // Numpad letter B in Red
				/*
				if ((curLed.ledId == CLK_NumLock) || (curLed.ledId == CLK_KeypadSlash) || (curLed.ledId == CLK_Keypad9) || (curLed.ledId == CLK_Keypad5) || (curLed.ledId == CLK_Keypad3) || (curLed.ledId == CLK_Keypad0) || (curLed.ledId == CLK_Keypad1) || (curLed.ledId == CLK_Keypad4) || (curLed.ledId == CLK_Keypad7))
				{
					ledColor.r = 255;
					ledColor.g = 0;
					ledColor.b = 0;
				}*/
				vec.push_back(ledColor);
			}
			CorsairSetLedsColors(vec.size(), vec.data());
			std::this_thread::sleep_for(std::chrono::milliseconds(800 / numberOfSteps));
		}
	}
}