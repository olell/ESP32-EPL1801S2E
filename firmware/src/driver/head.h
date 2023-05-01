/**
 * ESP32 Driver for EPL1801S2E
 * Header file for head driver code
 *
 * (C) 2023 Ole Lange
 */
#pragma once

#include <stdint.h>

void headInit();

uint8_t headReadRank();
void headWriteRow(const uint8_t *row);
void headStrobe(uint16_t time);
uint16_t headCalculateStrobeTime(float vp);