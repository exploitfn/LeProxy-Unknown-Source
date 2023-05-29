#include "Driver.h"

DWORD64 ModuleBase{};
DWORD64 ProcessId{};

void* hooked_func = nullptr;

typedef struct DRIVERINFO
{
	void* buffer_address;
	UINT_PTR address;
	ULONGLONG size;
	ULONG pid;
	BOOLEAN write;
	BOOLEAN read;
	BOOLEAN req_base;
	void* output;
	const char* module_name;
	ULONG64 base_address;
}KOTO;
DWORD64 DRIVERFUNC::GetModuleBase(const char* module_name)
{
	KOTO instructions;
	instructions = { 0 };
	instructions.pid = ProcessId;
	instructions.req_base = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.module_name = module_name;
	CallHook(&instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;
}
void DRIVERFUNC::ReadMemory(UINT_PTR read_address, void* buffer, size_t size)
{
	KOTO instructions;
	instructions.pid = ProcessId;
	instructions.size = size;
	instructions.address = read_address;
	instructions.read = TRUE;
	instructions.write = FALSE;
	instructions.req_base = FALSE;
	instructions.output = buffer;
	CallHook(&instructions);
}
BOOL DRIVERFUNC::WriteMemory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size)
{
	KOTO instructions;
	instructions.address = write_address;
	instructions.pid = ProcessId;
	instructions.write = TRUE;
	instructions.read = FALSE;
	instructions.req_base = FALSE;
	instructions.buffer_address = (void*)source_address;
	instructions.size = write_size;

	CallHook(&instructions);

	return true;
}
std::string DRIVERFUNC::ReadString(UINT_PTR address) {
	KOTO instructions;

	std::vector<char> buffer(sizeof(std::string), char{ 0 });

	instructions.pid = ProcessId;
	instructions.size = buffer.size();
	instructions.address = address;
	instructions.read = TRUE;
	instructions.write = FALSE;
	instructions.req_base = FALSE;
	instructions.output = static_cast<void*>(&buffer[0]);

	CallHook(&instructions);

	return std::string(buffer.data());
}
std::string DRIVERFUNC::ReadStringPointer(UINT_PTR address) {
	if (read<int>(address + 0x10) > 15)
		address = read<uint32_t>(address);
	std::string res;
	char buf;
	for (int i = 0; i < 0x1000; i++) {
		buf = read<char>(address + i);
		if (!buf)
			break;
		res += buf;
	}
	return res;
}

DRIVERFUNC Drv;

