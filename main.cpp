#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <getopt.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include "nvmelib.hpp"

NVMeUpdateLib* _nvmeUpdateLib = NULL;

// TODO: populated struct array
static struct option options[] = {
   {"query", 2, 0, 'q'},
   {"queryVendor", 0, 0, 'f'},
   {"queryUpdate", 1, 0, 'p'},
};

size_t file_get_contents(const char *path, char **outBuffer) {

  struct stat st = {};

  int fd = open(path, 0, 384);
  if ( fd == - 1) {
    printf("Error opening file %s\n", path);
    exit(1);
  }

  if ( fstat(fd, &st) ) {
    printf("Error - could not stat %s\n", path);
    close(fd);
    exit(1);
  }

  size_t statSize = st.st_size;

  char* buffer = (char*)valloc(st.st_size);
  bzero(buffer, statSize);

  size_t size = pread(fd, buffer, statSize, 0);
  if ( size != st.st_size )
  {
    puts("Error - pread failed!");
    free(buffer);
    exit(1);
  }
  close(fd);

  *outBuffer = (char *)buffer; // Copy out

  return st.st_size;
}


void enter_bfh_mode(uint64_t bfhSize) {
	char* loaderPath = NULL;
	uint32_t mspType = 0;
	uint32_t bfhLoaderID = 0;
	uint64_t descriptor = 0xFFFFFF;
	bool isBFHMode = 0;
	uint32_t sz = 0;

	kern_return_t err = KERN_SUCCESS;

	err = _nvmeUpdateLib->GetMSPType(&mspType);
	if (err) {
		printf("Failed getting MSP type. Error=0x%x\n", err);
		goto fail_and_exit;
	}

	bfhLoaderID = mspType + 1;
	if( mspType+1 >= 5 ) {
		printf("error. Unknown MSP type: 0x%x\n", mspType);
		goto fail_and_exit;
	}

	if( !_nvmeUpdateLib->IsBFHMode(&isBFHMode) ) {

		if(!isBFHMode) {
			fprintf(stderr, "Not in BFH mode\n");
			goto fail_and_exit;
		}

		if (bfhLoaderID == 3) {
			goto get_firmware;
		}

		if (bfhSize) {
			fprintf(stderr, "Override BFH case\n");
			loaderPath = NULL;
			sz = bfhSize;
		}
		else {
//			loaderPath = kBFHLoaderPaths[bfhLoaderID]; // TODO
//			fprintf(stderr, "Non-override BFH case. BFH loader path: %s\n", kBFHLoaderPaths[bfhLoaderID]);
			sz = 0;
		}
		// status =  perform_bfh(loaderPath, sz);
		// if(status) {
		//	printf("perform_bfh failed. status=0x%x\n", status);
		// 	goto fail_and_exit;
		// }
		sleep(1);
		if( !_nvmeUpdateLib->IsBFHMode(&isBFHMode)) {

get_firmware:
			err = _nvmeUpdateLib->GetNANDDescriptor(&descriptor);
			if(!err) {
			//get_nand_firmware_path(descriptor, mspType);
				exit(0);
			}
			fprintf(stderr, "Failed getting NAND descriptor. Error=0x%x\n", err);
fail_and_exit:
			exit(1);
		}
	}
	puts("Failed getting BFH status.");
	goto fail_and_exit;
}

void usage(char* programName)
{
  printf(
    "Usage: %s [--query] [--validate <path-to-version-plist>] [--update <path-to-fw-update>] [--identify] [--updatestdin]"
    " [--bfh path] [--pci state] [ --skip]\n",
    programName);
  puts("Only one option processed per invocation");
  exit(1);
}


int main(int argc, char *argv[]) {

  if( (argc & 0xFFFFFFFE) != 2) {
	puts("Missing/excess options/args");
	usage(argv[0]);
  }

  _nvmeUpdateLib = new NVMeUpdateLib;

  return 0;
}
