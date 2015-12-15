#include "ChristmasLights.h"

const ZeroToOne ZeroToOne::kZero( (uint16_t) 0x0000 );
const ZeroToOne ZeroToOne::kHalf( (uint16_t) 0x8000 );
const ZeroToOne ZeroToOne::kOne ( (uint16_t) 0xffff );
const ZeroToOne ZeroToOne::kRecipNumPixels( (uint16_t) (0xffff / NUMPIXELS) );

const FixedPt FixedPt::kZero( (uint16_t) 0x0000 );
const FixedPt FixedPt::kOne ( (uint16_t) 0x0100 );

const Colour Colour::kBlack(0,0,0);
const Colour Colour::kWhite(255,255,255);

static ColourComponent colourComponentMAD( ColourComponent a, FixedPt b, ColourComponent c )
{
	uint32_t temp;
	temp = ((uint32_t) a) * ((uint32_t) b);
	uint16_t temp2 = (uint16_t) (temp >> 8);
	temp2 += c;
	if( temp2 > 255 ) temp2 = 255;
	return temp2;
}

static ColourComponent colourComponentADD( ColourComponent a, Brightness b )
{
	uint16_t temp = (uint16_t) a + (uint16_t) b;
	if( temp > 255 ) temp = 255;
	return temp;
}

static FixedPt percuptualMultiplier( Brightness brightness, uint16_t shift )
{
	return FixedPt(((uint16_t) brightness * (uint16_t) brightness) >> (8 - shift));
}

ZeroToOne::ZeroToOne( float val )
: m_val( val * 0xffff )
{}

ZeroToOne ZeroToOne::Random()
{
	return ZeroToOne( (uint16_t) random( 0x10000 ) );
}

ZeroToOne ZeroToOne::Random( ZeroToOne minimum, ZeroToOne maximum )
{
	return ((maximum - minimum) * Random()) + minimum;
}

FixedPt FixedPt::Random( FixedPt minimum, FixedPt maximum )
{
	return ((((uint32_t) maximum.m_val - (uint32_t) minimum.m_val) * random(256)) >> 8) + minimum;
}

void Colour::Add( const Colour& colour, FixedPt brightness )
{
	m_components[0] = colourComponentMAD( colour.R(), brightness, R() );
	m_components[1] = colourComponentMAD( colour.G(), brightness, G() );
	m_components[2] = colourComponentMAD( colour.B(), brightness, B() );
}

void Colour::AddLinear( Brightness brightness )
{
	m_components[0] = colourComponentADD( R(), brightness );
	m_components[1] = colourComponentADD( G(), brightness );
	m_components[2] = colourComponentADD( B(), brightness );
}

void ColourArray::Add( const BrightnessArray& brightnesses, const Colour& colour, uint16_t shift )
{
	for( int i = 0; i < NUMPIXELS; i++ )
	{
		FixedPt multiplier = percuptualMultiplier( brightnesses[i], shift );
		m_array[i].Add( colour, multiplier );
	}
}

void ColourArray::AddLinear( const BrightnessArray& brightnesses, const Colour& colour, uint16_t shift )
{
	for( int i = 0; i < NUMPIXELS; i++ )
	{
		FixedPt multiplier( brightnesses[i] << shift );
		m_array[i].Add( colour, multiplier );
	}
}

void ColourArray::AddLinear( const BrightnessArray& brightnesses )
{
	for( int i = 0; i < NUMPIXELS; i++ )
	{
		m_array[i].AddLinear( brightnesses[i] );
	}
}

void ColourArray::Show( Adafruit_NeoPixel& pixels ) const
{
	for( int i = 0; i < NUMPIXELS; i++ )
	{
		const Colour& colour = m_array[i];
		pixels.setPixelColor( i, pixels.Color( colour.R(), colour.G(), colour.B() ) );
	}
	pixels.show();
}

void ColourArray::Clear()
{
	for( unsigned int i = 0; i < NUMPIXELS; ++i )
	{
		m_array[i].Clear();
	}
}

void BrightnessArray::Clear()
{
	for( unsigned int i = 0; i < NUMPIXELS; ++i )
	{
		m_array[i] = 0;
	}
}

ZeroToOne operator * ( ZeroToOne a, FixedPt b )
{
	uint32_t temp = ((uint32_t) a * (uint32_t) b) >> 8;
	if( temp > 0xffff )
	{
		return ZeroToOne::kOne;
	}
	return ZeroToOne( (uint16_t) temp );
}

static Brightness add( Brightness a, Brightness b )
{
	uint16_t result = (uint16_t) a + (uint16_t) b;
	if( result > 0xff ) result = 0xff;
	return (Brightness) result;
}

void BrightnessArray::Blob( ZeroToOne pos, ZeroToOne halfWidth, Brightness brightness )
{
	ZeroToOne left = pos - halfWidth;
	PixelIdx firstPixel = left.ToPixelIdx();
	ZeroToOne right = pos + halfWidth;
	PixelIdx finalPixel = right.ToPixelIdx();
	FixedPt recipHalfWidth;
	recipHalfWidth.SetRecip( halfWidth );
	ZeroToOne ztoBrightness;
	ztoBrightness.FromBrightness( brightness );
	for( PixelIdx i = firstPixel; i <= finalPixel; ++i )
	{
		ZeroToOne pixelPos;
		pixelPos.FromPixelIdx(i);
		ZeroToOne dist = AbsDiff( pixelPos, pos );
		ZeroToOne normDist = dist * recipHalfWidth;
		ZeroToOne pixBrightness = ZeroToOne((uint16_t) ZeroToOne::kOne - (uint16_t) normDist) * ztoBrightness;
		
		m_array[i] = add( pixBrightness.ToBrightness(), m_array[i] );
	}
}

void BrightnessArray::Pixel( ZeroToOne pos, Brightness brightness )
{
	PixelIdx pixelIdx = pos.ToPixelIdx();
	m_array[pixelIdx] = brightness;
}
