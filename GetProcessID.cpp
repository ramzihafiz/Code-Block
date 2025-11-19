#include <Windows.h>
#include <TlHelp32.h>
#include <wchar.h>
#include <iostream>


// Fungsi untuk mengambil PID dari proses yang berjalan

DWORD GetProcId(const wchar_t* procName) {                              // Requiring parameter proses name contoh : notepad.exe
    DWORD procId = 0;                                                   // Inisialisasi PID ke 0
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);     // Memulai Snapshot dari semua proses yang berjalan

    if (hSnap != INVALID_HANDLE_VALUE) {                                // Kondisi jika snapshot berhasil dilakukan
        PROCESSENTRY32 procEntry32;                                     // adalah struktur (struct) di Windows API yang digunakan untuk menyimpan informasi tentang satu proses ketika kamu melakukan
                                                                            // enumerasi (mengambil daftar) proses menggunakan fungsi ToolHelp API seperti: 
                                                                            // CreateToolhelp32Snapshot / Process32First / Process32Next
        procEntry32.dwSize = sizeof(procEntry32);                       // Inisialisasi size dari procEntry dynamically sesuai yang dibutuhkan variablenya

        if (Process32First(hSnap, &procEntry32)) {                      // Akan bernilai true dan melanjutkan ke 'do'
            do {                                                        // Karna true maka akan melakukan perbandingan if
                if (!_wcsicmp(procEntry32.szExeFile, procName)) {       // Membandingkan jika proses yang di enumerasi (nama .exe nya) sama dengan variable procName
                    procId = procEntry32.th32ProcessID;                 // Jika sama maka akan menyimpan PID-nya
                    break;                                              // Loop berhenti ketika menemukan proccess name
                }
            } while (Process32Next(hSnap, &procEntry32));               // Akan terus loop sampe mendapatkan break;
        }

    }
    CloseHandle(hSnap);                                                 // Menutup handle untuk menghindari memory leak
    return procId;                                                      // Return PID ke fungsi 

}