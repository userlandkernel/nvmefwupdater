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

   _nvmeUpdateLib = new NVMeUpdateLib; // Get new NVMeUpdateLib instance

  return 0;
}
