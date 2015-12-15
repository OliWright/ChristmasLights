#include "KnightRider.h"
#include "ChristmasLights.h"

static const unsigned int kNumLights = 8;
static const ZeroToOne kAttackStep( 0.2f );
static const ZeroToOne kDecayMultiplier( 0.93f );
static const unsigned int kNumHoldFrames = 2;
static const ZeroToOne kHalfWidth( 0xffff / (kNumLights * 4) );

static ZeroToOne lightValues[ kNumLights ];
enum Phase
{
	kBrightening,
	kHolding
};
static Phase phase = kBrightening;
static bool direction = true;
static unsigned int lightIdx = 0;
static unsigned int holdCount = 0;


void KnightRiderUpdate()
{
	// Dim all lights
	for( unsigned int i = 0; i < kNumLights; ++i )
	{
		//lightValues[i] -= kDecayStep;
		lightValues[i] *= kDecayMultiplier;
	}

	switch( phase )
	{
		case kBrightening:
			lightValues[ lightIdx ] += kAttackStep;
			if( lightValues[ lightIdx ] == ZeroToOne::kOne )
			{
				phase = kHolding;
				holdCount = 0;
			}
			break;
		case kHolding:
			lightValues[ lightIdx ] = ZeroToOne::kOne;
			if( ++holdCount == kNumHoldFrames )
			{
				phase = kBrightening;
				if( direction )
				{
					++lightIdx;
					if( lightIdx == kNumLights )
					{
						lightIdx = kNumLights - 1;
						direction = false;
					}
				}
				else
				{
					if( lightIdx == 0 )
					{
						direction = true;
						lightIdx = 0;
					}
					else
					{
						--lightIdx;
					}
				}
			}
			break;
	}
}

void KnightRiderRender( BrightnessArray& brightnesses )
{
	for( unsigned int i = 0; i < kNumLights; ++i )
	{
		ZeroToOne l = lightValues[i];
		ZeroToOne pos = ZeroToOne( (uint16_t) kHalfWidth * i * 2) + kHalfWidth;
		brightnesses.Blob( pos, kHalfWidth, l.ToBrightness() );
		brightnesses.Blob( ZeroToOne::kOne - pos, kHalfWidth, l.ToBrightness() );
		//brightnesses[i] = l.ToBrightness();
	}
}
