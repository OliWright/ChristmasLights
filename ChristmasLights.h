#ifndef CHRISTMAS_LIGHTS_H
#define CHRISTMAS_LIGHTS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      150

typedef uint8_t ColourComponent;
typedef uint8_t Brightness;
typedef uint16_t PixelIdx;

struct ZeroToOne
{
	ZeroToOne()
	{}
	ZeroToOne( uint16_t val )
	: m_val( val )
	{}
	ZeroToOne( float val );
	ZeroToOne( const ZeroToOne& other )
	: m_val( other.m_val )
	{}
	
	void operator = ( const ZeroToOne& rhs )
	{
		m_val = rhs.m_val;
	}
	
	operator uint16_t() const { return m_val; }
	
	ZeroToOne operator * ( const ZeroToOne& rhs ) const
	{
		uint32_t temp = ((uint32_t) m_val) * ((uint32_t) rhs.m_val);
		return ZeroToOne( (uint16_t) (temp >> 16) );
	}
	
	const ZeroToOne& operator *= ( const ZeroToOne& rhs )
	{
		*this = *this * rhs;
		return *this;
	}
	
	ZeroToOne operator + ( const ZeroToOne& rhs ) const
	{
		uint32_t temp = ((uint32_t) m_val) + ((uint32_t) rhs.m_val);
		if( temp > kOne ) temp = kOne;
		return ZeroToOne( (uint16_t) temp );
	}
	
	const ZeroToOne& operator += ( const ZeroToOne& rhs )
	{
		*this = *this + rhs;
		return *this;
	}
	
	ZeroToOne operator - ( const ZeroToOne& rhs ) const
	{
		uint32_t temp = ((uint32_t) m_val) - ((uint32_t) rhs.m_val);
		if( temp > kOne ) temp = kZero; // Wrap
		return ZeroToOne( (uint16_t) temp );
	}
	
	const ZeroToOne& operator -= ( const ZeroToOne& rhs )
	{
		*this = *this - rhs;
		return *this;
	}
	
	void FromPixelIdx( PixelIdx pixelIdx )
	{
		m_val = kRecipNumPixels.m_val * pixelIdx;
	}
	
	PixelIdx ToPixelIdx() const
	{
		uint32_t temp = ((uint32_t) m_val) * NUMPIXELS;
		return (PixelIdx) (temp >> 16);
	}

	void FromBrightness( Brightness brightness )
	{
		m_val = brightness << 8;
	}

	Brightness ToBrightness() const
	{
		return m_val >> 8;
	}
	
	static ZeroToOne Random();
	static ZeroToOne Random( ZeroToOne minimum, ZeroToOne maximum );
	
	static const ZeroToOne kZero;
	static const ZeroToOne kHalf;
	static const ZeroToOne kOne;
	static const ZeroToOne kRecipNumPixels;
	
private:
	uint16_t m_val;
};

inline const ZeroToOne AbsDiff( ZeroToOne a, ZeroToOne b )
{
	if( a > b )
	{
		return ZeroToOne( a - b );
	}
	else
	{
		return ZeroToOne( b - a );
	}
}

struct FixedPt
{
	FixedPt()
	{}
	FixedPt( uint16_t val )
	: m_val( val )
	{}
	FixedPt( const FixedPt& other )
	: m_val( other.m_val )
	{}
	
	operator uint16_t() const { return m_val; }
	
	FixedPt operator * ( const FixedPt& rhs ) const
	{
		uint32_t temp = ((uint32_t) m_val) * ((uint32_t) rhs.m_val);
		return FixedPt( (uint16_t) (temp >> 8) );
	}
	
	const FixedPt& operator *= ( const FixedPt& rhs )
	{
		*this = *this * rhs;
		return *this;
	}
	
	FixedPt operator + ( const FixedPt& rhs ) const
	{
		uint32_t temp = ((uint32_t) m_val) + ((uint32_t) rhs.m_val);
		if( temp > 0xffff ) temp = 0xffff;
		return FixedPt( (uint16_t) temp );
	}
	
	const FixedPt& operator += ( const FixedPt& rhs )
	{
		*this = *this + rhs;
		return *this;
	}
	
	FixedPt operator - ( const FixedPt& rhs ) const
	{
		uint32_t temp = ((uint32_t) m_val) - ((uint32_t) rhs.m_val);
		if( temp > 0xffff ) temp = kZero; // Wrap
		return FixedPt( (uint16_t) temp );
	}
	
	const FixedPt& operator -= ( const FixedPt& rhs )
	{
		*this = *this - rhs;
		return *this;
	}
	
	void SetRecip( ZeroToOne val )
	{
		m_val = 0x1000000 / (uint32_t) val;
		if( m_val > 0xffff ) m_val = 0xffff;
	}
	
	static FixedPt Random( FixedPt minimum, FixedPt maximum );
	
	static const FixedPt kOne;
	static const FixedPt kZero;
	
private:
	uint16_t m_val;
};
	

ZeroToOne operator * ( ZeroToOne, FixedPt );

struct Colour
{
	Colour()
	{
		m_components[0] = 0;
		m_components[1] = 0;
		m_components[2] = 0;
	}
	
	Colour( ColourComponent r, ColourComponent g, ColourComponent b )
	{
		m_components[0] = r;
		m_components[1] = g;
		m_components[2] = b;
	}
	
	ColourComponent R() const { return m_components[0]; }
	ColourComponent G() const { return m_components[1]; }
	ColourComponent B() const { return m_components[2]; }
	
	void Clear() { *this = kBlack; }
	void Add( const Colour& colour, FixedPt brightness );
	void AddLinear( Brightness brightness );

	const Colour& operator = ( const Colour& rhs )
	{
		m_components[0] = rhs.m_components[0];
		m_components[1] = rhs.m_components[1];
		m_components[2] = rhs.m_components[2];
	}
	
	static const Colour kBlack;
	static const Colour kWhite;
	
	ColourComponent m_components[3];
};

struct BrightnessArray
{
	BrightnessArray()
	{
		Clear();
	}
	
	void Clear();
	
	Brightness operator [] (unsigned int idx ) const { return m_array[ idx ]; }
	Brightness& operator [] (unsigned int idx ) { return m_array[ idx ]; }
	
	void Blob( ZeroToOne pos, ZeroToOne halfWidth, Brightness brightness );
	void Pixel( ZeroToOne pos, Brightness brightness );
	
	Brightness m_array[ NUMPIXELS ];
};

struct ColourArray
{
	ColourArray()
	{}
	
	Colour operator [] (unsigned int idx ) const { return m_array[ idx ]; }
	
	void Clear();
	void Add( const BrightnessArray& brightnesses, const Colour& colour, uint16_t shift = 0 );
	void AddLinear( const BrightnessArray& brightnesses, const Colour& colour, uint16_t shift = 0 );
	void AddLinear( const BrightnessArray& brightnesses );
	
	void Show( Adafruit_NeoPixel& pixels ) const;
	
	Colour m_array[ NUMPIXELS ];
};

#endif