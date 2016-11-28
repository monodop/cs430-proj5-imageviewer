
#include "../headers/res_manager.h"
#include "../../resource.h"
#include <Windows.h>

char* load_resource(int res, char* res_type) {
	HMODULE hModule = GetModuleHandle(NULL);
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(res), res_type);
	HGLOBAL hMemory = LoadResource(hModule, hResource);
	DWORD dwSize = SizeofResource(hModule, hResource);
	LPVOID lpAddress = LockResource(hMemory);
	char* bytes = malloc(sizeof(char) * dwSize + 1);
	memcpy(bytes, lpAddress, dwSize);
	bytes[dwSize] = 0;
	return bytes;
}