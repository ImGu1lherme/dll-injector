#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <string>
using namespace std;

int get_process_id(const std::string& p_name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 structprocsnapshot = { 0 };
	structprocsnapshot.dwSize = sizeof(PROCESSENTRY32);
	if (snapshot == INVALID_HANDLE_VALUE)return 0;
	if (Process32First(snapshot, &structprocsnapshot) == FALSE)return 0;
	while (Process32Next(snapshot, &structprocsnapshot)) {
		if (!strcmp(structprocsnapshot.szExeFile, p_name.c_str())) {
			CloseHandle(snapshot);
			return structprocsnapshot.th32ProcessID;
		}
	}
	CloseHandle(snapshot);
	return 0;
}

void send_message(int color,string message) {
	system("cls");
	system(("color "+std::to_string(color)).c_str());
	cout << "                                          simple dll injector (LoadLibrary)" << endl;
	cout << message << endl;
	Sleep(3000);
}

bool loadLibrary(string process, string dllpath) {
	
	DWORD process_id = get_process_id(process);
	if (process_id == 0) {
		send_message(4, "Failed to get process id (process not found?)");
		return false;
	}

	std::ifstream dll_path(dllpath);
	if (!dll_path) {
		send_message(4, "Failed to get dll in path (dll not found?)");
		return false;
	}

	HANDLE process_handle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, process_id);
	
	if (process_handle == 0) {
		send_message(4, "Failed to get process handle (am i admin?)");
		return false;
	}

	LPVOID alloc_memory = VirtualAllocEx(process_handle, NULL, sizeof(dllpath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	WriteProcessMemory(process_handle, alloc_memory, dllpath.c_str(), sizeof(dllpath), NULL);
	
	CreateRemoteThread(process_handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, alloc_memory, 0, 0);
	
	CloseHandle(process_handle);
	send_message(2, "connection to the process successfully (injected)");
	return true;
}

void check_exit() {
	while (true) {
		if (GetAsyncKeyState(VK_ESCAPE)) {
			send_message(4,"goodbye. . .");
			Sleep(1000);
			exit(0);
		}
		Sleep(1000);
	}
}

int main() {
	CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)check_exit, nullptr, NULL, nullptr);
	while (true) {
		system("cls");
		system("color 5");
		string process_name;
		string dll_path;

		cout << "                                          simple dll injector (LoadLibrary)" << endl;
		cout << "                                                      esc to exit" << endl;
		cout << "  process name ->";
		cin >> process_name;
		cout << endl << "  dll path ->";
		cin >> dll_path;
		loadLibrary(process_name, dll_path);
	}
	return 0;
}
