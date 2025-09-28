// moonwalk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "communication.hpp"
#include "utilities.hpp"
#include "win32.h"
#include "globals.hpp"
#include "winternl.h"

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        log("Invalid parameters");
        wait();
        return -1;
    }

    std::cout << R"(
                                                                   ..;===+.
                                                                .:=iiiiii=+=
    astroware                                                .=i))=;::+)i=+,
                                                          ,=i);)I)))I):=i=;
                                                       .=i==))))ii)))I:i++
                                                     +)+))iiiiiiii))I=i+:'
                                .,:;;++++++;:,.       )iii+:::;iii))+i='
                             .:;++=iiiiiiiiii=++;.    =::,,,:::=i));=+'
                           ,;+==ii)))))))))))ii==+;,      ,,,:=i))+=:
                         ,;+=ii))))))IIIIII))))ii===;.    ,,:=i)=i+
                        ;+=ii)))IIIIITIIIIII))))iiii=+,   ,:=));=,
                      ,+=i))IIIIIITTTTTITIIIIII)))I)i=+,,:+i)=i+
                     ,+i))IIIIIITTTTTTTTTTTTI))IIII))i=::i))i='
                    ,=i))IIIIITLLTTTTTTTTTTIITTTTIII)+;+i)+i`
                    =i))IIITTLTLTTTTTTTTTIITTLLTTTII+:i)ii:'
                   +i))IITTTLLLTTTTTTTTTTTTLLLTTTT+:i)))=,
                   =))ITTTTTTTTTTTLTTTTTTLLLLLLTi:=)IIiii;
                  .i)IIITTTTTTTTLTTTITLLLLLLLT);=)I)))))i;
                  :))IIITTTTTLTTTTTTLLHLLLLL);=)II)IIIIi=:
                  :i)IIITTTTTTTTTLLLHLLHLL)+=)II)ITTTI)i=
                  .i)IIITTTTITTLLLHHLLLL);=)II)ITTTTII)i+
                  =i)IIIIIITTLLLLLLHLL=:i)II)TTTTTTIII)i'
                +i)i)))IITTLLLLLLLLT=:i)II)TTTTLTTIII)i;
              +ii)i:)IITTLLTLLLLT=;+i)I)ITTTTLTTTII))i;
             =;)i=:,=)ITTTTLTTI=:i))I)TTTLLLTTTTTII)i;
           +i)ii::,  +)IIITI+:+i)I))TTTTLLTTTTTII))=,
         :=;)i=:,,    ,i++::i))I)ITTTTTTTTTTIIII)=+'
       .+ii)i=::,,   ,,::=i)))iIITTTTTTTTIIIII)=+
      ,==)ii=;:,,,,:::=ii)i)iIIIITIIITIIII))i+:'
     +=:))i==;:::;=iii)+)=  `:i)))IIIII)ii+'
   .+=:))iiiiiiii)))+ii;
  .+=;))iiiiii)));ii+
 .+=i:)))))))=+ii+
.;==i+::::=)i=;
,+==iiiiii+,
`+=+++;`";
)";
    std::cout << std::endl;
    std::cout << std::endl;

    CHAR win_dir[MAX_PATH];
    if (!GetEnvironmentVariableA("SystemDrive", win_dir, MAX_PATH)) {
        log("SystemDrive not found");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }

    strcat_s(win_dir, MAX_PATH, "\\map.dat");

    // Open file
    HANDLE hFile = INVALID_HANDLE_VALUE;
    hFile = CreateFileA(win_dir,                // name of the write
        GENERIC_READ | GENERIC_WRITE,          // open for writing
        FILE_SHARE_READ | FILE_SHARE_WRITE,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,             // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        log("Failed to find file, last error: ");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }
   
    
    INFO info{};
    
    PVOID shared_page = VirtualAlloc(nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!shared_page) {
        log("Failed to allocate memory...");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }
    
    globals::shared_page = shared_page;
 
    
    info.process_id = GetCurrentProcessId();
    info.shared_page = (UINT64)shared_page;
    
    DWORD bytes_read;
    if (!WriteFile(hFile, &info, sizeof(INFO), (LPDWORD)&bytes_read, nullptr)) {
        log("Failed writing to file...");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }
    CloseHandle(hFile);
    
    ULONG len;
    NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemModuleInformation, NULL, 0, &len);
     
        

    PSYSTEM_MODULE_INFORMATION lpModuleInfo = (PSYSTEM_MODULE_INFORMATION)VirtualAlloc(nullptr, len, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!lpModuleInfo) {
        log("Failed allocating memory for module information...");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }

    if (!NT_SUCCESS(NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemModuleInformation, lpModuleInfo, len, &len))) {
        log("Failed querying system information 2...");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }

    UINT64 base_address = 0;
    ULONG image_size = 0;
    const char* target = argv[1];
    for (UINT32 i = 0; i < lpModuleInfo->ModulesCount; i++) {
        if (strstr(lpModuleInfo->Modules[i].Name,target) != 0) {
            base_address = (UINT64)lpModuleInfo->Modules[i].ImageBaseAddress;
            image_size = lpModuleInfo->Modules[i].ImageSize;
            break;
        }
    }

    PVOID dumped_driver = VirtualAlloc(nullptr, image_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!dumped_driver) {
        log("Failed allocating memory for dump...");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }
    ZeroMemory(dumped_driver, image_size);
    log("Press key to dump...");
    getchar();
    
    if (!NT_SUCCESS(kread(base_address, image_size, dumped_driver)))
        return -1;

    FILE* dmp = nullptr;
    fopen_s(&dmp,"dump.bin", "w+");
    if (!dmp) {
        log("Failed to create dump");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }

    SIZE_T _bytes = fwrite(dumped_driver, sizeof(BYTE),  image_size, dmp);
    if (_bytes <= 0) {
        log("bytes was 0");
        log("GetLastError(): %d", GetLastError());
        wait()
        return -1;
    }
    fclose(dmp);

    log("Finished");
 



    
}

