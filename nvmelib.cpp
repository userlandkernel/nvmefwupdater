#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

typedef struct NVMeIdentifyControllerStruct {
    char unknown[0x1000];
} NVMeIdentifyControllerStruct; // Unimplemented

class NVMeUpdateLib {

	public:
		NVMeUpdateLib();
		~NVMeUpdateLib();
		void DumpIdentifyData();
		void FirmwareDownload(void* nandDescriptor, void* responseData);
		void FirmwareValidate(char* fileBuffer);
		void FirmwareVersionCheck(char* data, size_t size, uint32_t mspType);
		kern_return_t GetClogMismatch(bool *misMatchout);
		kern_return_t GetMSPType(uint32_t *mspTypeOut);
		kern_return_t GetNANDDescriptor(uint64_t* nandDescriptor);
		kern_return_t IdentifyController(NVMeIdentifyControllerStruct* identity);
		kern_return_t IsBFHMode(bool* bfhModeOut);
		kern_return_t PerformBFH(char* bfhData, size_t bfhSize);
		kern_return_t SendNVMeCommand(uint32_t selector, const uint64_t* command, uint64_t commandSize);
		kern_return_t SetBFHMode(bool bfhMode);
		kern_return_t SetNVMeState(bool enableA, bool enableB);
		kern_return_t SetPCIPortState(bool A, bool B, uint32_t C);

	private:
		io_service_t svc;
		io_connect_t conn;

};

NVMeUpdateLib::NVMeUpdateLib() {

	kern_return_t err = KERN_SUCCESS;

	// Lookup nand service
	svc = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleEmbeddedNVMeController"));
	if( svc == IO_OBJECT_NULL) {
		throw KERN_FAILURE;
	}

	// Connect to it
	err = IOServiceOpen(svc, mach_task_self(), 0, &conn);
	if(err) {
		throw err;
	}
}

NVMeUpdateLib::~NVMeUpdateLib() {
	// This is the destructor
	IOServiceClose(conn);
	IOObjectRelease(svc);
}

