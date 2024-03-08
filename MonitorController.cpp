#include <winsdkver.h>
#define _WIN32_WINNT 0x0601 // Win7
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN

#include <cwchar>
#include <iostream>
#include <iomanip>
#include <string>
#include <Windows.h>

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
	std::wcout << std::endl << std::right;
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

		for (DWORD iModeNum = 0; true;) {
			ZeroMemory(&dm, sizeof(DEVMODEW));
			dm.dmSize = sizeof(DEVMODEW);
			dm.dmDriverExtra = 0;
			if (!EnumDisplaySettingsW(dd.DeviceName, iModeNum++, &dm)) {
				break;
			}

			std::wcout << std::left << std::setw(4) << std::to_wstring(iModeNum) + L"." << L"  ";
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

int setMode(DWORD displayNum, DWORD modeNum) {
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
		return 1;
	}

	switch (ChangeDisplaySettingsExW(displayNamePtr, &dm, nullptr, CDS_UPDATEREGISTRY, nullptr))
	{
	case DISP_CHANGE_SUCCESSFUL:
		std::wcout << L"OK" << std::endl;
		return 0;
	case DISP_CHANGE_BADDUALVIEW:
		std::wcout << L"The settings change was unsuccessful because the system is DualView capable." << std::endl;
		return 1;
	case DISP_CHANGE_BADFLAGS:
		std::wcout << L"An invalid set of flags was passed in." << std::endl;
		return 1;
	case DISP_CHANGE_BADMODE:
		std::wcout << L"The graphics mode is not supported." << std::endl;
		return 1;
	case DISP_CHANGE_BADPARAM:
		std::wcout << L"An invalid parameter was passed in. This can include an invalid flag or combination of flags." << std::endl;
		return 1;
	case DISP_CHANGE_FAILED:
		std::wcout << L"The display driver failed the specified graphics mode." << std::endl;
		return 1;
	case DISP_CHANGE_NOTUPDATED:
		std::wcout << L"Unable to write settings to the registry." << std::endl;
		return 1;
	case DISP_CHANGE_RESTART:
		std::wcout << L"The computer must be restarted for the graphics mode to work." << std::endl;
		return 1;
	default:
		std::wcout << L"Unknown error." << std::endl;
		return 1;
	}
}

void printHelp() {
	std::wcout << std::left
		<< L"Monitor Controller v1.0\n"
		<< L"usage:\n"
		<< std::setw(40) << L"-h, --help" << "Print this help page.\n"
		<< std::setw(40) << L"-a, --list-all-modes" << "Print all modes for all active displays.\n"
		<< std::setw(40) << L"-c, --list-current-modes" << "Print current mode for all active displays.\n"
		<< std::setw(40) << L"-s, --set-mode [displayNum] modeNum" << "Set the display mode.\n"
		<< std::setw(40) << L"" << "displayNum is optional, if omitted mode will be set for the Primary display."
		<< std::endl << std::right;
}

void invalidArgsExit() {
	std::wcout << L"Invalid args. Use -h or --help to view usage instructions." << std::endl;
	exit(1);
}

int wmain(int argc, wchar_t *argv[])
{
	if (argc < 2 || argc > 4) {
		invalidArgsExit();
	}

	if (std::wcscmp(argv[1], L"-h") == 0 || std::wcscmp(argv[1], L"--help") == 0) {
		printHelp();
	}
	else if (std::wcscmp(argv[1], L"-a") == 0 || std::wcscmp(argv[1], L"--list-all-modes") == 0) {
		printAllModes();
	}
	else if (std::wcscmp(argv[1], L"-c") == 0 || std::wcscmp(argv[1], L"--list-current-modes") == 0) {
		printCurrentModes();
	}
	else if ((std::wcscmp(argv[1], L"-s") == 0 || std::wcscmp(argv[1], L"--set-mode") == 0) && argc > 2) {
		DWORD displayNum;
		DWORD modeNum;

		try {
			if (argc == 3) {
				displayNum = 0;
				modeNum = std::stoul(argv[2]);
			}
			else {
				displayNum = std::stoul(argv[2]);
				modeNum = std::stoul(argv[3]);
			}
		}
		catch (...) {
			invalidArgsExit();
		}

		return setMode(displayNum, modeNum);
	}
	else {
		invalidArgsExit();
	}
}
