#include "win32.h"
UINT64 get_module_base(DWORD dwProcessID, const wchar_t* lpszModuleName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
    UINT64 dwModuleBaseAddress = 0;
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
       
        MODULEENTRY32 ModuleEntry32 = { 0 };
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &ModuleEntry32))
        {
            do
            {
                if (wcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
                {
                    dwModuleBaseAddress = (UINT64)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    return dwModuleBaseAddress;
}

UINT32 get_pid(const char* window_title) {
    HWND target_hwnd = FindWindowA(nullptr, window_title);
    if (target_hwnd == INVALID_HANDLE_VALUE)
        return -1;

    DWORD pid;
    GetWindowThreadProcessId(target_hwnd, &pid);
    return pid;
}