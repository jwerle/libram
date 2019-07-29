#include "ram/version.h"

#define SX(x) #x
#define S(x) SX(x)

#ifndef RAM_VERSION
#define RAM_VERSION 0
#endif

#ifndef RAM_NAME
#define RAM_NAME "libram"
#endif

#ifndef RAM_VERSION_MAJOR
#define RAM_VERSION_MAJOR 0
#endif

#ifndef RAM_VERSION_MINOR
#define RAM_VERSION_MINOR 0
#endif

#ifndef RAM_VERSION_PATCH
#define RAM_VERSION_PATCH 0
#endif

#ifndef RAM_VERSION_REVISION
#define RAM_VERSION_REVISION 0
#endif

#ifndef RAM_DATE_COMPILED
#define RAM_DATE_COMPILED ""
#endif

const char *
ram_version_string() {
  return RAM_NAME
    "@" S(RAM_VERSION_MAJOR)
    "." S(RAM_VERSION_MINOR)
    "." S(RAM_VERSION_PATCH)
    "." S(RAM_VERSION_REVISION) " (" RAM_DATE_COMPILED ")";
}

const unsigned long int
ram_version() {
  return (const unsigned long int) RAM_VERSION;
}

const unsigned long int
ram_version_major() {
  return RAM_VERSION >> 24 & 0xff;
}

const unsigned long int
ram_version_minor() {
  return RAM_VERSION >> 16 & 0xff;
}

const unsigned long int
ram_version_patch() {
  return RAM_VERSION >> 8 & 0xff;
}

const unsigned long int
ram_version_revision() {
  return RAM_VERSION & 0xff;
}
