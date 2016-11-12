// progress.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

//#include "../examples/color_pulse/color_pulse/color_pulse.h"
	
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

#include <conio.h>  
#include <iostream>
#include <cstdlib>
//#include <time.h>

const int prestige[10][3] = {
	{255,0,0},
	{0,255,255},
	{255,0,255},
	{ 255,255,0 },
	{100,200,55},
	{100,100,155},
	{0,255,0},
	{255,255,255},
	{55,0,255},
	{0,100,255}
};

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

void *displayPowerBar(int score) {
	int scoreLine[2] = { 40,20 };
	const auto ledPositions = CorsairGetLedPositions();
	const auto keyboardWidth = getKeyboardWidth(ledPositions) * 1 / 2;

	std::vector<CorsairLedColor> vec;
	for (auto i = 0; i < ledPositions->numberOfLed; i++) {

		const auto ledPos = ledPositions->pLedPosition[i];
		auto ledColor = CorsairLedColor();
		ledColor.ledId = ledPos.ledId;

		const auto currWidth = double(keyboardWidth) * (score % (16 + 1)) / 16;
		const auto prestigeIdx = (int)(score / (16 + 1));
		if (ledPos.top < scoreLine[0]) {
			if (ledPos.left < currWidth) {
				if (score < 0) {
					ledColor.r = 255;
				}
				else {
					ledColor.r = prestige[prestigeIdx + 1][0];
					ledColor.g = prestige[prestigeIdx + 1][1];
					ledColor.b = prestige[prestigeIdx + 1][2];
				}
			}
			vec.push_back(ledColor);
		}
	}
	CorsairSetLedsColors(vec.size(), vec.data());
	return "";
}

void *spawnNote(int key, int score[], bool superPowerActive[]) {
	int VK_Keys[4] = { 0x30 ,0x50,0x4C,0xBE };
	int ledIds[4] = { 21,CorsairGetLedIdForKeyName('p'),CorsairGetLedIdForKeyName('l'),57 };
	int musicLine[4][2] = { { 60,40 },{ 80,60 },{ 90,80 },{ 120,100 } };
	const auto ledPositions = CorsairGetLedPositions();

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
				if (ledPos.left < keyboardWidth && ledPos.left >(keyboardWidth - 20) && ledPos.top > musicLine[0][1]) {
					ledColor.g = 255;
					vec.push_back(ledColor);
				}
				/*if (ledPos.left < keyboardWidth-20 && ledPos.left >(keyboardWidth - 50) && ledPos.top > musicLine[0][1]) {
					ledColor.g = 150;
					ledColor.r = 150;
					vec.push_back(ledColor);
				}*/
				//vec.push_back(ledColor);
				ledColorPrev.ledId = ledColor.ledId;
			}
			CorsairSetLedsColors(vec.size(), vec.data());

			std::this_thread::sleep_for(std::chrono::milliseconds(40));

			if ((n % (numberOfSteps + 1) <= (numberOfSteps + 5)) && (n % (numberOfSteps + 1) > (numberOfSteps - 10))) {
				if (GetAsyncKeyState(VK_Keys[key])) {
					std::cout << "right";
					score[0] = score[0] + 1;
					lineancy = true;
					if ((score[0] % 16) == 0 && score[0] > 0) {
						superPowerActive[0] = true;
					}
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
			std::thread t1(displayPowerBar, score[0]);
			t1.detach();
			
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
std::vector<CorsairLedColor> getAvailableKeys()
{
	auto colorsVector = std::vector<CorsairLedColor>();
	for (auto deviceIndex = 0; deviceIndex < CorsairGetDeviceCount(); deviceIndex++) {
		if (auto deviceInfo = CorsairGetDeviceInfo(deviceIndex)) {
			switch (deviceInfo->type) {
			case CDT_Mouse: {
				auto numberOfKeys = deviceInfo->physicalLayout - CPL_Zones1 + 1;
				for (auto i = 0; i < numberOfKeys; i++) {
					auto ledId = static_cast<CorsairLedId>(CLM_1 + i);
					colorsVector.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
				}
			} break;
			case CDT_Keyboard: {
				auto ledPositions = CorsairGetLedPositions();
				if (ledPositions) {
					for (auto i = 0; i < ledPositions->numberOfLed; i++) {
						auto ledId = ledPositions->pLedPosition[i].ledId;
						colorsVector.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
					}
				}
			} break;
			case CDT_Headset: {
				colorsVector.push_back(CorsairLedColor{ CLH_LeftLogo, 0, 0, 0 });
				colorsVector.push_back(CorsairLedColor{ CLH_RightLogo, 0, 0, 0 });
			} break;
			}
		}
	}
	return colorsVector;
}

void performPulseEffect(std::vector<CorsairLedColor> &ledColorsVec)
{
	static auto waveDuration = 500;
	const auto timePerFrame = 25;
	for (auto x = .0; x < 2; x += static_cast<double>(timePerFrame) / waveDuration) {
		auto val = (1 - pow(x - 1, 2)) * 255;
		for (auto &ledColor : ledColorsVec)
			ledColor.g = val;
		CorsairSetLedsColorsAsync(ledColorsVec.size(), ledColorsVec.data(), nullptr, nullptr);

		if (GetAsyncKeyState(VK_OEM_PLUS) && waveDuration > 100)
			waveDuration -= 100;
		if (GetAsyncKeyState(VK_OEM_MINUS) && waveDuration < 2000)
			waveDuration += 100;
		std::this_thread::sleep_for(std::chrono::milliseconds(timePerFrame));
	}
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
	bool superPowerActive[1] = { false };
	while (!GetAsyncKeyState(VK_ESCAPE)) {
		/*std::thread t1(PrintHello, (void*)1);
		std::thread t2(PrintHello, (void*)2);
		t1.detach();
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		t2.detach();*/
		//getAvailableKeys();
		if (superPowerActive[0]) {
			while (!GetAsyncKeyState(0x42)) {
				auto ledId = CorsairGetLedIdForKeyName('b');
				if (ledId != CLI_Invalid) highlightKey(ledId);
			}
			for (int i = 0; i < 12; i++) {
				performPulseEffect(getAvailableKeys());
			}
			superPowerActive[0] = false;
		}
		int note = rand() % 4;
		std::thread t1(spawnNote, note, score, superPowerActive);
		t1.detach();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		
	}
	//t1.join();
	return 0;
}

