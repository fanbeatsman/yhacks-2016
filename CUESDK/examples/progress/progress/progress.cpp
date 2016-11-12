// progress.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <map>
#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <vector>
#include <time.h>
//#define HAVE_STRUCT_TIMESPEC
//#include <pthread.h>
#include <windows.h>

#include <iostream>
#include <cstdlib>
//#include <time.h>

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

void *spawnNote(int key, int score[]) {
	int VK_Keys[4] = { 0x30 ,0x50,0x4C,0xBE };
	int ledIds[4] = { 21,CorsairGetLedIdForKeyName('p'),CorsairGetLedIdForKeyName('l'),57 };
	int musicLine[4][2] = { { 60,40 },{ 80,60 },{ 90,80 },{ 120,100 } };
	const auto ledPositions = CorsairGetLedPositions();
	double currStateWidth[12] = { 0.0 };
	int currStep[12] = { -1 };

	if (ledPositions && ledPositions->numberOfLed > 0) {

		//std::map<std::string, int> currState;
		//std::vector<double> currState;

		/*for (int i = 0; i < 12; i++) {
		currState.push_back(0.0);
		}*/
		//int key = rand() % 4;
		//int key = 0;
		bool lineancy = false;
		const auto keyboardWidth = getKeyboardWidth(ledPositions) * 1 / 2;
		int numberOfSteps = 50;
		for (auto n = 0; (n % (numberOfSteps + 1)) != numberOfSteps; n++) {
			if ((n % (numberOfSteps + 1)) == 0) {
				lineancy = false;
			}

			std::vector<CorsairLedColor> vec;
			const auto currWidth = double(keyboardWidth) * (n % (numberOfSteps + 1)) / numberOfSteps;

			bool init = true;
			auto ledColorPrev = CorsairLedColor();
			for (auto i = 0; i < ledPositions->numberOfLed; i++) {

				const auto ledPos = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				//const auto ledPosPrev = ledPositions->pLedPosition[i-1];
				//auto ledColorPrev = CorsairLedColor();
				//const auto ledPosPrevPrev = ledPositions->pLedPosition[i - 2];
				//auto ledColorPrevPrev = CorsairLedColor();
				ledColor.ledId = ledPos.ledId;
				//ledColorPrev.ledId = ledPosPrev.ledId;
				//ledColorPrevPrev.ledId = ledPosPrev.ledId;
				if (ledPos.left < currWidth && ledPos.left > currWidth - 20 && ledPos.top < musicLine[key][0] && ledPos.top > musicLine[key][1]) {
					ledColor.b = 255;
					//	vec.push_back(ledColorPrevPrev);
					vec.push_back(ledColorPrev);
					vec.push_back(ledColor);
				}
				if (ledPos.left < keyboardWidth && ledPos.left >(keyboardWidth - 20)) {
					ledColor.g = 255;
					vec.push_back(ledColor);
					//if (init) {
				//		ledColorPrev.ledId = ledPositions->pLedPosition[i - 1].ledId;
				//		vec.push_back(ledColorPrev);
				//		init = false;
				//	}
				}
				//vec.push_back(ledColor);
				ledColorPrev.ledId = ledColor.ledId;
			}
			CorsairSetLedsColors(vec.size(), vec.data());

			std::this_thread::sleep_for(std::chrono::milliseconds(25));

			if ((n % (numberOfSteps + 1) <= (numberOfSteps + 1)) && (n % (numberOfSteps + 1) > (numberOfSteps - 10))) {
				if (GetAsyncKeyState(VK_Keys[key])) {
					std::cout << "right";
					score[0] = score[0] + 1;
					lineancy = true;
				}
				else if (!lineancy && n % (numberOfSteps + 1) == (numberOfSteps)) { // wrong if you did not press
					std::cout << "wrong";
					score[0] = score[0] -1;
				}
			}
			else if (GetAsyncKeyState(VK_Keys[key])) {//wrong if press out of range
				std::cout << "wrong";
				score[0] = score[0] - 1;
			}
		}

		//Final sweep of the house to clean up
		std::vector<CorsairLedColor> vec;
			for (auto i = 0; i < ledPositions->numberOfLed; i++) {

				const auto ledPos = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				//const auto ledPosPrev = ledPositions->pLedPosition[i-1];
				//auto ledColorPrev = CorsairLedColor();
				//const auto ledPosPrevPrev = ledPositions->pLedPosition[i - 2];
				//auto ledColorPrevPrev = CorsairLedColor();
				ledColor.ledId = ledPos.ledId;
				//ledColorPrev.ledId = ledPosPrev.ledId;
				//ledColorPrevPrev.ledId = ledPosPrev.ledId;
				if (ledPos.top < musicLine[key][0] && ledPos.top > musicLine[key][1]) {
					ledColor.b = 0;
					vec.push_back(ledColor);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			CorsairSetLedsColors(vec.size(), vec.data());

	}
	//pthread_exit(NULL);
	std::cout << score[0];
	return "";
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
	int score[1] = {0};
	while (!GetAsyncKeyState(VK_ESCAPE)) {
		/*std::thread t1(PrintHello, (void*)1);
		std::thread t2(PrintHello, (void*)2);
		t1.detach();
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		t2.detach();*/
		int note = rand() % 4;
		std::thread t1(spawnNote, note, score);
		t1.detach();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		
	}
	//t1.join();
	return 0;
}

