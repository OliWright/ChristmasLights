#ifndef TWINKLES_H
#define TWINKLES_H

static const unsigned int kNumTwinkles = 32;

void TwinklesUpdate();
void TwinklesRender( struct BrightnessArray& brightnesses, unsigned int first, unsigned int howMany );

#endif