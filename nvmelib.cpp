#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "nvmelib.hpp"
#define HIDWORD(dw, hw) LOWORD(dw) | (hw << 16)
#define LODWORD(dw, lw) (HIWORD(dw) << 16) | lw

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

kern_return_t NVMeUpdateLib::IsBFHMode(bool* bfhModeOut) {
	uint64_t output = 0;
	uint32_t outputCount = 1;
	kern_return_t result = IOConnectCallMethod(conn, kNVMECTL_isBFHModeAction, 0, 0, 0, 0, &output, &outputCount, 0, 0);

	*bfhModeOut = output != 0;

	return result;
}

kern_return_t NVMeUpdateLib::GetMSPType(unsigned int* mspTypeOut) {
	uint64_t output = 0;
	uint32_t outputCount = 1;
	kern_return_t result = IOConnectCallMethod(conn, kNVMECTL_getMSPTypeAction, 0, 0, 0, 0, &output, &outputCount, 0, 0);

	*mspTypeOut = output;

	return result;
}

kern_return_t NVMeUpdateLib::GetNANDDescriptor(unsigned long long* nandDescriptorOut) {
	uint64_t output = 0;
	uint32_t outputCnt = 1;
	kern_return_t result = IOConnectCallMethod(conn, kNVMECTL_getNandDescriptorAction, 0, 0, 0, 0, &output, &outputCnt, 0, 0);

	*nandDescriptorOut = output;

	return result;
}

kern_return_t NVMeUpdateLib::SetBFHMode(bool bfhMode) {
	uint64_t output = 0;
	uint32_t outputCount = 1;
	const uint64_t input = bfhMode;

	kern_return_t result = IOConnectCallMethod(conn, kNVMECTL_setBFHGPIOAction, &input, 1, 0, 0, &output, &outputCount, 0, 0);
	if(!result) {
		result = (uint32_t)output;
	}
	return result;
}

kern_return_t NVMeUpdateLib::SetNVMeState(bool enableA, bool enableB) {
	uint64_t output = 0;
	uint32_t outputCount = 1;
	const uint64_t input = enableA;
	// TODO: Get initialization of DWORD read at this+4
	return KERN_FAILURE; // Unimplemented
}


kern_return_t NVMeUpdateLib::PerformBFH(char* bfhData, size_t bfhSize) {
	uint64_t output = 0;
	uint32_t outputCount = 1;
	uint64_t input = (uint64_t)bfhData;
	// May be decompiled incorrect, input Size might be bfhSize
	kern_return_t result = IOConnectCallMethod(conn, kNVMECTL_performBFHAction, &input, 2, 0, 0, &output, &outputCount, 0, 0);
	if(!result) {
		result = (uint32_t)output;
	}
	return result;
}

kern_return_t NVMeUpdateLib::FirmwareVersionCheck(char* data, size_t size, uint32_t mspType) {
	char* input = NULL;
	char* output = NULL;
	char* vbuffer = NULL;
	uint32_t outputCount = 1;
	uint64_t unknown1 = 0;
	if(mspType != 2) 
	{
		printf("FW Revision from update file : %d.%02d.%02d\n", ((*(uint32_t*)data+1) >> 20) & 0x7F, (uint16_t)*((uint32_t*)data+1) >> 8, 1);
		input = *((uint32_t*)data+1);
		unknown1 = 4;
	}
	else {
		uint32_t index = (uint32_t)(*((uint32_t*)data + 1)+16);
		unknown1 = 16;
		vbuffer = (char*)valloc(0x10);
		*(uint64_t*)vbuffer = 0;
		*((uint64_t*)vbuffer+1) = 0;
		bcopy(&data[index], vbuffer, 0x10);
	}
	input = vbuffer;
	kern_return_t result = IOConnectCallMethod(conn, kNVMECTL_firmwareVersionCheckAction, &input, 2, 0, 0, &output, &outputCount, 0, 0);
	if(vbuffer)
		free(vbuffer);
	return result;
}

int FirmwareValidate(char* fileBuffer) {
	
	char* input = NULL;
	NVMeIdentifyControllerStruct controllerIdentity = {};
	kern_return_t err = KERN_SUCCESS;
	uint32_t numConfigs = 0;
	uint8_t config = 'E';

	if(!fileBuffer) {
		puts("FirmwareValidate - file buffer is NULL. Nothing to validate.");
		return -1;
	}

	err = NVMeUpdateLib->SendNVMeCommand(2, (const uint64_t*)&controllerIdentity, 1);
	if(err) {
		printf("FirmwareValidate - Identify controller failed with return status 0x%X nvme status 0x%X.\n", err, HIDWORD(err));
		return -1;
	}

	numConfigs = (*((uint32_t*)fileBuffer+16) >> 1) & 0xFFFFFFF;
	if(controllerIdentity.unknown[3100]) { // X
		config = 'X';
	}
	printf("Current config: S3%c, Rev %X\n", config, (uint32_t)controllerIdentity.unknown[3101] + 0xA0);
	printf("    NANDDeviceID 0x%04x\n", *((unsigned __int16 *)controllerIdentity + 1551));
	printf("    ECCVersionNANDRevision 0x%04x\n", controllerIdentity.unknown[3104]);
	 printf(
    "    FTL Versions: CLog %d.%d, DM %d\n",
    controllerIdentity.unknown[3105],
    controllerIdentity.unknown[3106],
    controllerIdentity.unknown[3107]);
	printf("    FTL Util FMT %d\n", controllerIdentity.unknown[3109]);
	if ( !*((uint32_t*)controllerIdentity + 129) )
    	puts("Blank NAND detected. Will skip Util, DM and FTL version checking");
    printf("Num configurations in update file: %d\n", numConfigs);
    if(!numConfigs)
    	return -1;
    // TBC

}

NVMeUpdateLib::~NVMeUpdateLib() {
	// This is the destructor
	IOServiceClose(conn);
	IOObjectRelease(svc);
}


