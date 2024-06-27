#pragma once

#include "forward.hpp"

#include <windows.h>

_STD_API_BEGIN

using VOIDPTR = PVOID;

typedef struct {
	PIMAGE_DOS_HEADER pDos;
	PIMAGE_NT_HEADERS pNt;
} PORTABLE_EXECUTABLE_HEADERS;

_STD_API
PORTABLE_EXECUTABLE_HEADERS
bytes_to_pe_headers(PBYTE bytes) noexcept {
	if (!bytes) 
		return { nullptr, nullptr };
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)bytes;
	if (dos->e_magic != IMAGE_DOS_SIGNATURE)
		return { nullptr, nullptr };
	const auto nt_offset = dos->e_lfanew;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(bytes + nt_offset);
	if (nt->Signature != IMAGE_NT_SIGNATURE)
		return { nullptr, nullptr };
	return { dos, nt };
}

_STD_INLINE
PBYTE
get_module_base(LPCSTR moduleName) noexcept {
	return (PBYTE)GetModuleHandleA(moduleName);
}

// Hook the IAT of executable base. The module sModule's sFunction will be replaced with pTarget
LPVOID patch_iat_function(PBYTE base, LPCSTR sModule, LPCSTR sFunction, LPCVOID pTarget) {
	auto [pe, nt] = bytes_to_pe_headers(base);

	PIMAGE_DATA_DIRECTORY importsDirectory = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	PIMAGE_IMPORT_DESCRIPTOR descriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importsDirectory->VirtualAddress + (DWORD_PTR)base);

	LPCSTR libName = NULL;
	HMODULE libHandle = NULL;
	PIMAGE_IMPORT_BY_NAME funcName = NULL;

	LPVOID originalFunc = NULL;

	while (descriptor->Name != NULL) {
		libName = reinterpret_cast<DWORD_PTR>(base)
			+ reinterpret_cast<LPCSTR>(descriptor->Name);
		libHandle = LoadLibraryA(libName);

		if (libHandle) {
			PIMAGE_THUNK_DATA originalFirstThunk = NULL, firstThunk = NULL;
			originalFirstThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)base + descriptor->OriginalFirstThunk);
			firstThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)base + descriptor->FirstThunk);

			while (originalFirstThunk->u1.AddressOfData != NULL)
			{
				funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)base + originalFirstThunk->u1.AddressOfData);

				// Does the current function name match the requested one?
				if (std::string(funcName->Name).compare(sFunction) == 0)
				{
					SIZE_T bytesWritten = 0;
					DWORD oldProtect = 0;
					VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, PAGE_READWRITE, &oldProtect);

					originalFunc = (LPVOID)firstThunk->u1.Function;
					firstThunk->u1.Function = (DWORD_PTR)pTarget;
				}
				++originalFirstThunk;
				++firstThunk;
			}
		}

		descriptor++;
	}

	return originalFunc;
}

_STD_API_END
