#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

enum AppleEmbeddedNVMeControllerAction {
    kNVMECTL_sendNVMECommandAction = 2,
    kNVMECTL_isBFHModeAction = 3,
    kNVMECTL_performBFHAction = 4,
    kNVMECTL_getNandDescriptorAction = 5,
    kNVMECTL_setNVMeStateAction = 6,
    kNVMECTL_setPCIPortStateAction = 7,
    kNVMECTL_setBFHGPIOAction = 8,
}; // from https://github.com/userlandkernel/iokitstuff

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

kern_return_t NVMeUpdateLib::SetBFHMode(bool bfhMode) {

	uint64_t output = 0;
	uint64_t outputCount = 1;
	const uint64_t input = bfhMode;

	kern_return_t result = IOConnectCallMethod(svc, kNVMECTL_setBFHGPIOAction, &input,  0, 0, &output, &outputCnt, 0, 0);
	if(!result) {
		result = (uint32_t)output;
	}
	return result;
}

NVMeUpdateLib::~NVMeUpdateLib() {
	// This is the destructor
	IOServiceClose(conn);
	IOObjectRelease(svc);
}


