// progress.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <future>
#include <map>

#include <cstdlib>
#include <time.h>
#include <conio.h>
#include <windows.h>


const char* const a_to_z = "abcdefghijklmnopqrstuvwxyz";
const char* toString(CorsairError error) 
{
	switch (error) {
	case CE_Success : 
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

double getKeyboardWidth(CorsairLedPositions *ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
		[](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
		return clp1.left < clp2.left;
	});
	return minmaxLeds.second->left + minmaxLeds.second->width - minmaxLeds.first->left;
}

void highlightKey(CorsairLedId ledId)
{
	for (auto x = .0; x < 2; x += .01) {
		int val = (1 - abs(x - 1)) * 255;
		auto ledColor = CorsairLedColor{ ledId, 255, 0, 0 };
		CorsairSetLedsColors(1, &ledColor);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		ledColor = CorsairLedColor{ ledId, 0, 0, 0 };
		CorsairSetLedsColors(1, &ledColor);
	}
}
void driftKey(CorsairLedId ledId) {
	std::map<std::string, int> VK_Keys;
	VK_Keys["0"] = 0x30;
	VK_Keys["p"] = 0x50;
	VK_Keys[";"] = 0xBA;
	VK_Keys["."] = 0xBE;
	int musicLine[4][4] = { { 60,40 },{ 80,60 },{ 90,80 },{ 110,90 } };
	const auto ledPositions = CorsairGetLedPositions();
	if (ledPositions && ledPositions->numberOfLed > 0) {

		const auto keyboardWidth = getKeyboardWidth(ledPositions) * 1 / 2;
		const auto numberOfSteps = 50;
		std::cout << "Working... Press Escape to close program...";
		std::cout << keyboardWidth;
		for (auto n = 0; !GetAsyncKeyState(VK_ESCAPE); n++) {

			std::vector<CorsairLedColor> vec;
			const auto currWidth = double(keyboardWidth) * (n % (numberOfSteps + 1)) / numberOfSteps;

			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto ledPos = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				ledColor.ledId = ledPos.ledId;
				if (ledPos.left < currWidth && ledPos.top < musicLine[1][0] && ledPos.top > musicLine[1][1])
					ledColor.r = 255;
				vec.push_back(ledColor);
			}
			CorsairSetLedsColors(vec.size(), vec.data());
			if (!GetAsyncKeyState(VK_Keys["p"])) {
				std::cout << "wrong";
			} else {
				std::cout << "right";
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}
	CorsairRequestControl(CAM_ExclusiveLightingControl);

	std::cout << "Please, input a word... ";
	auto userInputStr = std::string();
	std::cin >> userInputStr;
		
	srand(time(NULL));
	int key = rand() % 26;
	bool toBreak = false;
	char pressed;
	pressed = _getch();
	const auto ledPositions = CorsairGetLedPositions();
	while (!toBreak) {
		int key = rand() % 26;
			/*if (pressed == 27) {
				toBreak = true;
			}*/
			auto ledId = CorsairGetLedIdForKeyName(a_to_z[key]);
			if (ledId != CLI_Invalid) highlightKey(ledId);
			if (false && (pressed = _getch()) == (int)a_to_z[key]) {
				std::cout << "Great!";
				//break;
			}
			else {
				std::cout << "Wrong!";
			}
		/*	short tabKeyState = GetAsyncKeyState(VK_ESCAPE);
			if ((1 << 16) & tabKeyState)
			{
				std::cout << "ESCAPE!";
				continue;
			}*/
			//std::this_thread::sleep_for(std::chrono::milliseconds(0));
		}
	/*for (const auto &symbol : userInputStr) {
		int key = rand() % 26;
		std::cout << a_to_z[key];
		auto ledId = CorsairGetLedIdForKeyName(symbol);
		if (ledId != CLI_Invalid)
			highlightKey(ledId);
	}*/


    return 0;
}

