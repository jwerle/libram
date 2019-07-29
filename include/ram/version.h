#ifndef RAM_VERSION_H
#define RAM_VERSION_H

#include "platform.h"

/**
 * Returns the version string for the library.
 */
RAM_EXPORT const char *
ram_version_string();

/**
 * Returns the 32 bit version number that encodes the
 * major, minor, patch, and revision version parts.
 */
RAM_EXPORT const unsigned long int
ram_version();

/**
 * Returns the major version part.
 */
RAM_EXPORT const unsigned long int
ram_version_major();

/**
 * Returns the minor version part.
 */
RAM_EXPORT const unsigned long int
ram_version_minor();

/**
 * Returns the minor patch part.
 */
RAM_EXPORT const unsigned long int
ram_version_patch();

/**
 * Returns the minor revision part.
 */
RAM_EXPORT const unsigned long int
ram_version_revision();

#endif
