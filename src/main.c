#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <agon/mos.h>
#include "crc32.h"

#define BUFFERSIZE 1024
#define PATHMAX 1024

void putstring(const char *str, bool crlf) {
  mos_puts(str, strlen(str), 0);
  if(crlf) putchar('\n');
}

void usage(void) {
  putstring("Usage: crc32 filename [filename...]", true);
}

void print_uint32_hex(uint32_t value) {
    const char hex_chars[] = "0123456789abcdef";

    // Print each of the 8 hex digits (from most significant nibble)
    for (int i = 7; i >= 0; i--) {
        uint32_t nibble = (value >> (i * 4)) & 0xF;
        putchar(hex_chars[nibble]);
    }
}

// Returns 1 if the string str matches the given pattern.
// Pattern may contain * for multiple matching characters, and/or ? for a single matching character
int match(const char *pattern, const char *filename) {
    const char *star = NULL;      // Last position of '*'
    const char *star_fname = NULL; // Filename position corresponding to last '*'

    while (*filename) {
        if (*pattern == *filename || *pattern == '?') {
            // Match current character or '?'
            pattern++;
            filename++;
        } else if (*pattern == '*') {
            // Record the position of '*' and try to match zero characters first
            star = pattern++;
            star_fname = filename;
        } else if (star) {
            // Mismatch, backtrack to last '*'
            pattern = star + 1;
            filename = ++star_fname;
        } else {
            // No '*' to backtrack to, mismatch
            return 0;
        }
    }

    // Skip trailing '*' in pattern
    while (*pattern == '*') pattern++;

    return (*pattern == '\0');
}

// Returns 1 is the name contains any wildcard characters (*/?)
int containsWildCard(const char *name) {
  int i = 0;
  while (name[i] != '\0') {  // loop until null terminator
      if (name[i] == '*' || name[i] == '?') return 1;  // found either '*' or '&'
      i++;
  }
  return 0;  // neither found
}

const char *getFilename(const char *pattern) {
  int len = strlen(pattern);

  while(1) {
    if(len == 0) break;
    if(pattern[len - 1] == '/') break;
    len--;
  }
  return &(pattern[len]);
}

void calc_printCRC32(const char *filename) {
  FILE *fp;
  char buffer[BUFFERSIZE];
  unsigned int readsize;
  uint32_t crc32_result;

  if((fp = fopen(filename, "rb")) == NULL) {
    putstring("Error opening ", false);
    putstring(filename, true);
    return;
  }

  crc32_initialize();
  while((readsize = fread(buffer, BUFFERSIZE, 1, fp) != 0)) {
    crc32(buffer, readsize);
  }       
  crc32_result = crc32_finalize();

  print_uint32_hex(crc32_result);
  putchar(' ');
  putstring(getFilename(filename), true);

  fclose(fp);
}

void getDirectoryPath(char *dirpath, const char *filename) {
  char temppath[PATHMAX];
  unsigned int dirpathlength;
  int len = strlen(filename);

  while(1) {
    if(len == 0) break;
    if(filename[len - 1] == '/') break;
    len--;
  }
  memcpy(temppath, filename, len);
  temppath[len] = '\0';

  if(filename[0] == '/') { // absolute path
    strcpy(dirpath, temppath);
  }
  else {
    if(ffs_getcwd(dirpath, PATHMAX)) {
      putstring("Error getting current directory", true);
      return;
    }
    dirpathlength = strlen(dirpath);
    // Add ending '/' to current directory name
    if(dirpath[dirpathlength - 1] != '/') {
      if(dirpathlength == PATHMAX) {
        putstring("Error - max directory name length", true);
        return;
      }
      dirpath[dirpathlength] = '/';     // concatenate at end-of-string position
      dirpath[dirpathlength + 1] = 0;   // end string
    }
    // Add relative pathname
    strcat(dirpath, temppath); 
  }
}

void process_matchednames(const char *fullpattern) {
  //char pattern[255];
  const char *pattern;
  char dirpath[PATHMAX];
  char fullfilename[PATHMAX];
  DIR dirhandle;
  FILINFO file;

  getDirectoryPath(dirpath, fullpattern);
  pattern = getFilename(fullpattern);

  if(ffs_dopen(&dirhandle, dirpath)) {
    putstring("Error opening directory ", false);
    putstring(dirpath, true);
    return;
  }

  // process all files
  while(1) {
    if(ffs_dread(&dirhandle, &file)) {
      putstring("Error reading directory", true);
      break;
    }
    if(strlen(file.fname) == 0) break;
    if(match(pattern, file.fname) && ((file.fattrib & AM_DIR) == 0)) {
      strcpy(fullfilename, dirpath);
      strcat(fullfilename, file.fname);
      calc_printCRC32(fullfilename);
    }
  }

  ffs_dclose(&dirhandle);
}

void process_arguments(int argcount, char **argnames) {
  for(int i = 0; i < argcount; i++) {
    if(containsWildCard(argnames[i])) {
      process_matchednames(argnames[i]);
    } else {
      calc_printCRC32(argnames[i]);
    }
  }
}

int main(int argc, char **argv) {
  if(argc == 1) {
    usage();
    return 0;
  }

  process_arguments(argc - 1, argv + 1);
  return 0;
}
