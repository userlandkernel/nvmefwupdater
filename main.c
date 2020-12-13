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

// TODO: populated struct array
struct option options[] = {

};

size_t file_get_contents(const char *path, char **outBuffer) {
  int fd = open(fileName, 0, 384);
  
  if ( fd == - 1)
  {
    printf("Error opening file %s\n", path);
    exit(1);
  }
  
  if ( fstat(fd, &stat) )
  {
    printf("Error - could not stat %s\n", path);
    close(fd);
    exit(1);
  }
  
  size_t statSize = stat.st_size;
  char* buffer = valloc(stat.st_size);
  bzero(buffer, statSize);
  size_t size = pread(fd, buffer, statSize, 0);
  if ( size != stat.st_size )
  {
    puts("Error - pread failed!");
    free(buffer);
    exit(1);
  }
  
  close(fd);
  
  *outBuffer = (char *)buffer;
  return stat.st_size;
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
  return 0;
}
