#pragma once

#include <Arduino.h>

#include "config.h"
#include "const.h"

void logInit();

void logDebug(const char* val, ...);
void logInfo(const char* val, ...);
void logWarn(const char* val, ...);
void logError(const char* val, ...);
void logFatal(const char* val, ...);