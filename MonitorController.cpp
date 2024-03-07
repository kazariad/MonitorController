#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <string>

void printDevMode(const DEVMODEW& dm) {
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
	DISPLAY_DEVICEW dd;
	DEVMODEW dm;

	for (DWORD iDevNum = 0; true; iDevNum++) {
		ZeroMemory(&dd, sizeof(DISPLAY_DEVICEW));
		dd.cb = sizeof(DISPLAY_DEVICEW);
		if (!EnumDisplayDevicesW(nullptr, iDevNum, &dd, 0)) {
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
			ZeroMemory(&dm, sizeof(DEVMODEW));
			dm.dmSize = sizeof(DEVMODEW);
			dm.dmDriverExtra = 0;
			if (!EnumDisplaySettingsW(dd.DeviceName, iModeNum, &dm)) {
				break;
			}
			printDevMode(dm);
		}
		std::wcout << std::endl;
	}
}

void printCurrentModes() {
	DISPLAY_DEVICEW dd;
	DEVMODEW dm;

	for (DWORD iDevNum = 0; true; iDevNum++) {
		ZeroMemory(&dd, sizeof(DISPLAY_DEVICEW));
		dd.cb = sizeof(DISPLAY_DEVICEW);
		if (!EnumDisplayDevicesW(nullptr, iDevNum, &dd, 0)) {
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

		ZeroMemory(&dm, sizeof(DEVMODEW));
		dm.dmSize = sizeof(DEVMODEW);
		dm.dmDriverExtra = 0;
		if (!EnumDisplaySettingsW(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
			continue;
		}
		printDevMode(dm);
		std::wcout << std::endl;
	}
}

void changeMode(int displayNum, int modeNum) {
	LPCWSTR displayNamePtr = nullptr;
	std::wstring displayName{ L"\\\\.\\DISPLAY" };
	if (displayNum > 0) {
		displayName.append(std::to_wstring(displayNum));
		displayNamePtr = displayName.c_str();
	}

	DEVMODEW dm;
	ZeroMemory(&dm, sizeof(DEVMODEW));
	dm.dmSize = sizeof(DEVMODEW);
	dm.dmDriverExtra = 0;
	if (!EnumDisplaySettingsW(displayNamePtr, modeNum - 1, &dm)) {
		std::wcout << L"The display and/or mode number is invalid." << std::endl;
		return;
	}

	switch (ChangeDisplaySettingsExW(displayNamePtr, &dm, nullptr, CDS_UPDATEREGISTRY, nullptr))
	{
	case DISP_CHANGE_SUCCESSFUL:
		std::wcout << L"OK";
		break;
	case DISP_CHANGE_BADDUALVIEW:
		std::wcout << L"The settings change was unsuccessful because the system is DualView capable.";
		break;
	case DISP_CHANGE_BADFLAGS:
		std::wcout << L"An invalid set of flags was passed in.";
		break;
	case DISP_CHANGE_BADMODE:
		std::wcout << L"The graphics mode is not supported.";
		break;
	case DISP_CHANGE_BADPARAM:
		std::wcout << L"An invalid parameter was passed in. This can include an invalid flag or combination of flags.";
		break;
	case DISP_CHANGE_FAILED:
		std::wcout << L"The display driver failed the specified graphics mode.";
		break;
	case DISP_CHANGE_NOTUPDATED:
		std::wcout << L"Unable to write settings to the registry.";
		break;
	case DISP_CHANGE_RESTART:
		std::wcout << L"The computer must be restarted for the graphics mode to work.";
		break;
	default:
		std::wcout << L"Unknown error.";
		break;
	}
	std::wcout << std::endl;
}

int wmain()
{
	printAllModes();
}
