#include "ShootingStar.h"
#include "ChristmasLights.h"

static const FixedPt kHalfWidth( 0x20 );
static const FixedPt kStep( 0x30 );
static const FixedPt kStart( kHalfWidth + FixedPt(0x200) );
static const FixedPt kEnd( FixedPt(0x100) - kHalfWidth );
static const ZeroToOne kTrailMax( 0.2f );
static const ZeroToOne kTrailDecay( 0.005f );

static bool isRunning = false;
static FixedPt shootingStarPos;
static ZeroToOne trail;

void debug( char* label, int val )
{
	Serial.print( label );
	Serial.print( ": " );
	Serial.println( val );
}

void ShootingStarStart()
{
	isRunning = true;
	shootingStarPos = kStart;
	trail = kTrailMax;
}

bool ShootingStarUpdate()
{
	if( isRunning )
	{
		shootingStarPos -= kStep;
		trail -= kTrailDecay;
		if( trail == ZeroToOne::kZero )
		{
			isRunning = false;
		}
	}
	return isRunning;
}

static PixelIdx posToPixelIdx( FixedPt pos )
{
	uint32_t temp = ((uint32_t) (pos - FixedPt::kOne)) * (NUMPIXELS >> 1);
	PixelIdx pixelIdx = (PixelIdx) (temp >> 8);
	if( pixelIdx >= (NUMPIXELS >> 1) )
	{
		pixelIdx = (NUMPIXELS >> 1)-1;
	}
	return pixelIdx;
}

void ShootingStarRender( BrightnessArray& brightnesses )
{
	FixedPt left = shootingStarPos - kHalfWidth;
	PixelIdx firstPixel = posToPixelIdx(left);
	FixedPt right = shootingStarPos + kHalfWidth;
	PixelIdx finalPixel = posToPixelIdx(right);
	// FixedPt recipHalfWidth;
	// recipHalfWidth.SetRecip( halfWidth );
	// ZeroToOne ztoBrightness;
	// ztoBrightness.FromBrightness( brightness );
	for( PixelIdx i = firstPixel; i <= finalPixel; ++i )
	{
		// ZeroToOne pixelPos;
		// pixelPos.FromPixelIdx(i);
		// ZeroToOne dist = AbsDiff( pixelPos, pos );
		// ZeroToOne normDist = dist * recipHalfWidth;
		// ZeroToOne pixBrightness = ZeroToOne((uint16_t) ZeroToOne::kOne - (uint16_t) normDist) * ztoBrightness;
		
		//uint16_t brightness = brightnesses[i] +
		brightnesses[i] = 255;//add( 255,  );
	}
	Brightness brightness = trail.ToBrightness();
	for( PixelIdx i = finalPixel; i < (NUMPIXELS >> 1); ++i )
	{
		brightnesses[i] = brightness;
	}
	for( PixelIdx i = 0; i < (NUMPIXELS >> 1); ++i )
	{
		brightnesses[ NUMPIXELS - 1 - i ] = brightnesses[i];
	}
}
