#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <string>

void printDevMode(const DEVMODE& dm) {
	std::wcout << std::left
		<< std::setw(11) << std::to_wstring(dm.dmPelsWidth) + L"x" + std::to_wstring(dm.dmPelsHeight) + L"  "
		<< std::setw(7) << std::to_wstring(dm.dmDisplayFrequency) + L"hz  "
		<< std::setw(7) << std::to_wstring(dm.dmBitsPerPel) + L"bit  "
		<< std::setw(7);

	switch (dm.dmDisplayFixedOutput)
	{
	case DMDFO_DEFAULT:
		std::wcout << L"DEFAULT";
		break;
	case DMDFO_CENTER:
		std::wcout << L"CENTER";
		break;
	case DMDFO_STRETCH:
		std::wcout << L"STRETCH";
		break;
	default:
		std::wcout << L"UNKNOWN";
		break;
	}
	std::wcout << std::endl;
}

void printAllModes() {
	DISPLAY_DEVICE dd;
	DEVMODE dm;

	for (DWORD iDevNum = 0; true; iDevNum++) {
		ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
		dd.cb = sizeof(DISPLAY_DEVICE);
		if (!EnumDisplayDevices(nullptr, iDevNum, &dd, 0)) {
			break;
		}

		if (!(dd.StateFlags & DISPLAY_DEVICE_ACTIVE) || dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) {
			continue;
		}

		std::wcout << &dd.DeviceName[4];
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
			std::wcout << L" (Primary)";
		}
		std::wcout << L":" << std::endl;

		for (DWORD iModeNum = 0; true; iModeNum++) {
			ZeroMemory(&dm, sizeof(DEVMODE));
			dm.dmSize = sizeof(DEVMODE);
			dm.dmDriverExtra = 0;
			if (!EnumDisplaySettings(dd.DeviceName, iModeNum, &dm)) {
				break;
			}
			printDevMode(dm);
		}

		std::wcout << std::endl;
	}
}

void printCurrentModes() {
	DISPLAY_DEVICE dd;
	DEVMODE dm;

	for (DWORD iDevNum = 0; true; iDevNum++) {
		ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
		dd.cb = sizeof(DISPLAY_DEVICE);
		if (!EnumDisplayDevices(nullptr, iDevNum, &dd, 0)) {
			break;
		}

		if (!(dd.StateFlags & DISPLAY_DEVICE_ACTIVE) || dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) {
			continue;
		}

		std::wcout << &dd.DeviceName[4];
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
			std::wcout << L" (Primary)";
		}
		std::wcout << L":" << std::endl;

		ZeroMemory(&dm, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		dm.dmDriverExtra = 0;
		if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
			continue;
		}
		printDevMode(dm);

		std::wcout << std::endl;
	}
}

int main()
{
	printAllModes();
}
