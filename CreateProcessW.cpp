#include <Windows.h>
#include <iostream>

int executeProcess();

int main() {

    if (executeProcess) {
        std::cout << "Program Terbuka" << std::endl;
    }
    else {
        std::cerr << "Program failed terbuka" << GetLastError() << std::endl;
    }


    return 0;
}

int executeProcess() {
    STARTUPINFOW si; // Struktur kosong yang digunakan untuk menyimpan informasi bagaimana kita menjalankan aplikasi ( CreateProcessW harus dengan STARTUPINFOW , bukan STARTUPINFO )
    PROCESS_INFORMATION pi; // Membuat struktur kosong yang windows akan mengisi nantinya

    ZeroMemory(&si, sizeof(si)); // Kita zero ( kosongkan memory junk yang tidak perlu dalam proses information dan startup information )
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    LPCSTR appName = "C:\\Windows\\notepad.exe";

    BOOL startProcess = CreateProcessW(
        appName,        //Module name, bisa full PATH to applikasi
        NULL,           // Command Line ( biasanya diisi argument untuk app yang akan dijalankan )
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // SET handle inheritance to false
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi
    );
    if (!startProcess) {
        std::cerr << "Gagal buka notepad !!" << GetLastError() << std::endl;
        return 0;
    }
    else {
        std::cout << "Notepad terbuka ^" << std::endl;
    }
    CloseHandle(pi.hThread); // Close handle agar tidak terjadi buffer memory leak
    CloseHandle(pi.hProcess);
    return 1; // return 1 dalam fungsi akan mengembalikan angka 1 ke caller , TRUE
}