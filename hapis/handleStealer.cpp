
#include "handleStealer.h"


/* since we're putting our names by our code nathan made this shit same */
NTSTATUS enumerateHandles(std::function<NTSTATUS(PSYSTEM_HANDLE_TABLE_ENTRY_INFO)> callback)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	void* buffer = NULL;
	unsigned long bufferSize = 0;

	while (true) {
		status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation, buffer, bufferSize, &bufferSize);
		if (!NT_SUCCESS(status)) {
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				if (buffer != NULL)
					VirtualFree(buffer, 0, MEM_RELEASE);
				buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
				continue;
			}
			break;
		}
		else {
			PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer;
			for (unsigned long i = 0; i < handleInfo->NumberOfHandles; i++) {
				auto handle = &handleInfo->Handles[i];
				status = callback(handle);
				if (NT_SUCCESS(status))
					break;
			}
			break;
		}
	}

	if (buffer != NULL)
		VirtualFree(buffer, 0, MEM_RELEASE);
	return status;
}

HANDLE getHandle(wchar_t* process) {
	DWORD pid;
	HANDLE hProc;

	/* convert stupid ass utf-16 string to multibyte for strcmp */
	char mbproc[64];
	std::wcstombs(mbproc, process, 11);

	/* Get Process ID */
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (strcmp(entry.szExeFile, (const char*)process) == 0) {
				pid = entry.th32ProcessID;
				delete mbproc;
			}
		}
	}


	/* Get Handle (imagine how retarded this would b with no c++ magic me) */
	enumerateHandles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) -> NTSTATUS {
		if (pid != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;

		bool found = false;
		void* buffer = NULL;
		unsigned long bufferSize = 0x100;
		NTSTATUS status;

		if (handle->ObjectTypeIndex == ProcessTypeIndex) {
			wchar_t processPath[MAX_PATH] = {};

			if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
				wchar_t* filename = PathFindFileNameW(processPath);
				if (!wcscmp(process, filename)) {
					found = TRUE;
					hProc = (HANDLE)handle->HandleValue;
				}
			}
		}

		// stop enumeration
		if (found) return STATUS_SUCCESS;

		return STATUS_UNSUCCESSFUL;
	});

	return hProc;
}
