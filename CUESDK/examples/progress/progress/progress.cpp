// progress.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <map>
#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <vector>
#include <windows.h>

#include <cstdlib>
#include <time.h>

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

int main()
{
	CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}
	/*std::map<std::string, int> VK_Keys;
	VK_Keys["0"] = 0x30;
	VK_Keys["p"] = 0x50;
	VK_Keys["l"] = 0x4C;
	VK_Keys["."] = 0xBE;*/
	int VK_Keys[4] = { 0x30 ,0x50,0x4C,0xBE };
	char ledIds[4] = { '0','p','l','.' };
	int musicLine[4][4] = { { 60,40 },{ 80,60 },{ 90,80 },{ 120,100 } };
	const auto ledPositions = CorsairGetLedPositions();
	if (ledPositions && ledPositions->numberOfLed > 0) {

		int key = 0;
		bool lineancy = false;
		srand(time(NULL));
		const auto keyboardWidth = getKeyboardWidth(ledPositions) * 1 / 2;
		const auto numberOfSteps = 50;
		std::cout << "Working... Press Escape to close program...";
		std::cout << keyboardWidth;
		for (auto n = 0; !GetAsyncKeyState(VK_ESCAPE); n++) {
			if ((n % (numberOfSteps + 1)) == 0) {
				//auto ledColorTarget = CorsairLedColor{ CorsairGetLedIdForKeyName(ledIds[key]), 0, 0, 0 };
				//CorsairSetLedsColors(1, &ledColorTarget);
				lineancy = false;
				key = rand() % 4;
			}

			std::vector<CorsairLedColor> vec;
			std::vector<CorsairLedColor> vecTarget;
			const auto currWidth = double(keyboardWidth) * (n % (numberOfSteps + 1)) / numberOfSteps;
			

			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto ledPos = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				
				ledColor.ledId = ledPos.ledId;
				if (ledPos.left < currWidth && ledPos.top < musicLine[key][0] && ledPos.top > musicLine[key][1])
					ledColor.b = 255;
				vec.push_back(ledColor);
			}
			CorsairSetLedsColors(vec.size(), vec.data());
			auto ledColorTarget = CorsairLedColor{ CorsairGetLedIdForKeyName(ledIds[key]), 0, 255, 0 };
			CorsairSetLedsColors(1, &ledColorTarget);
			//ledColor = CorsairLedColor{ ledId, 0, 0, 0 };
			//CorsairSetLedsColors(1, &ledColor);

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if ( (n % (numberOfSteps + 1) <= (numberOfSteps+1)) && (n % (numberOfSteps + 1) > (numberOfSteps-10))) {
				if (GetAsyncKeyState(VK_Keys[key])) {
					std::cout << "right";
					lineancy = true;
				}
				else if(!lineancy && n % (numberOfSteps + 1) == (numberOfSteps )){ // wrong if you did not press
					std::cout << "wrong";
				}
			}
			else if (GetAsyncKeyState(VK_Keys[key])) {//wrong if press out of range
				std::cout << "wrong";
			}
			
		}
	}
	return 0;
}

