#include "ChristmasLights.h"
#include "Twinkles.h"
#include "ShootingStar.h"
#include "KnightRider.h"

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 3

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

long frameTimeMs = 10;

enum Mode
{
	kTwinkles,
	kKnightRider,
	kNumModes
};

static const unsigned int framesPerMode[] = 
{
	100 * 60, // kTwinkles
	100 * 10, // kKnightRider
};

static Mode mode = kKnightRider;
static unsigned int modeFrameCount = 0;

ColourArray final;

void setup()
{
	pixels.begin(); // This initializes the NeoPixel library.

	// Initialize serial communications (if there is a PC attached) at 57600 bps:
	Serial.begin(57600);
	randomSeed(analogRead(0));  
}

static BrightnessArray twinkles;

static const Colour kRed( 255, 60, 30 );
static const Colour kGreen( 50, 255, 50 );
static const Colour kBlue( 20, 40, 255 );
static const Colour kNightRider( 255, 40, 4 );

void loop()
{
	long startTimeMs = millis();
	final.Clear();
	
	if( ++modeFrameCount == framesPerMode[mode] )
	{
		modeFrameCount = 0;
		mode = (Mode) (((unsigned int) mode) + 1);
		if( mode == kNumModes )
		{
			mode = (Mode) 0;
		}
	}

	switch( mode )
	{
		case kTwinkles:
			TwinklesUpdate();
			
			twinkles.Clear();
			TwinklesRender( twinkles, 0, 8 );
			final.Add( twinkles, kRed, 2 );
			twinkles.Clear();
			TwinklesRender( twinkles, 8, 8 );
			final.Add( twinkles, kGreen );
			twinkles.Clear();
			TwinklesRender( twinkles, 16, 8 );
			final.Add( twinkles, kBlue );
			TwinklesRender( twinkles, 24, 8 );
			final.Add( twinkles, Colour::kWhite );
			
			if( ShootingStarUpdate() )
			{
				twinkles.Clear();
				ShootingStarRender( twinkles );
				//final.Add( twinkles, Colour::kWhite );
				final.AddLinear( twinkles );
			}
			else
			{
				if( random(500) == 1 )
				{
					ShootingStarStart();
				}
			}
			break;
			
		case kKnightRider:
			KnightRiderUpdate();
			twinkles.Clear();
			KnightRiderRender( twinkles );
			final.AddLinear( twinkles, kNightRider, 0 );
			break;
	}

	final.Show( pixels );
	
	long nowTimeMs = millis();
	if( (nowTimeMs - startTimeMs) < frameTimeMs )
	{
		delay( frameTimeMs - (nowTimeMs - startTimeMs) ); // Delay for a period of time (in milliseconds).
	}
}
