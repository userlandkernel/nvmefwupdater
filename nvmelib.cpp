#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "nvmelib.hpp"

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


NVMeUpdateLib::~NVMeUpdateLib() {
	// This is the destructor
	IOServiceClose(conn);
	IOObjectRelease(svc);
}


