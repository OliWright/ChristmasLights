#include "Twinkles.h"
#include "ChristmasLights.h"

static const FixedPt kLifeBrightnessScale( 0x200 );
static const ZeroToOne kMinTwinkle(0.4f);
static const FixedPt kTwinkleBrightnessScale( 0x400 );
static const ZeroToOne kMaxHalfWidth(0.1f);
static const ZeroToOne kMinHalfWidth(0.02f);
static const ZeroToOne kMinBrightness(0.1f);
static const ZeroToOne kMaxBrightness(0.3f);
static const ZeroToOne kLifeStep(0.04f);
static const ZeroToOne kMaxLifeStep(0.004f);
static const ZeroToOne kMinLifeStep(0.001f);
static const ZeroToOne kMaxSpeed(0.002f);
static const FixedPt kTwinkleBoost( 0x800 );
static const uint16_t kTwinkleAttack( 0x10 );
static const uint8_t kTwinkleDecay( 0x08 );

struct Twinkle
{
	Twinkle()
	{
		Reset();
		m_life = ZeroToOne::Random();
	}

	void Reset();
	void Update( unsigned int idx );
	
	ZeroToOne m_pos;
	ZeroToOne m_life;
	uint8_t m_twinkle;
	uint8_t m_twinkleSmoothed;
	ZeroToOne m_speed;
};

static Twinkle twinkles[ kNumTwinkles ];

void Twinkle::Reset()
{
	m_pos = ZeroToOne::Random();
	m_life = ZeroToOne::kZero;
	m_twinkle = 0;
	m_twinkleSmoothed = 0;
	m_speed = ZeroToOne::Random( ZeroToOne::kZero, kMaxSpeed );
	//m_speed = ZeroToOne::kZero;
}

void Twinkle::Update( unsigned int idx )
{
	if( random(100) == 1 )
	{
		m_twinkle = 255;
	}
	else
	{
		if( m_twinkle > kTwinkleDecay )
		{
			m_twinkle -= kTwinkleDecay;
		}
		else
		{
			m_twinkle = 0;
		}
	}
	if( m_twinkleSmoothed < m_twinkle )
	{
		uint16_t ts = (uint16_t) m_twinkleSmoothed + kTwinkleAttack;
		if( ts > (uint16_t) m_twinkle )
		{
			m_twinkleSmoothed = m_twinkle;
		}
		else
		{
			m_twinkleSmoothed = ts;
		}
	}
	else
	{
		m_twinkleSmoothed = m_twinkle;
	}
	if( idx & 1 )
	{
		m_pos += m_speed;
	}
	else
	{
		m_pos -= m_speed;
	}
	
	ZeroToOne distribution( (idx & 7) << 12 );
	//distribution += ZeroToOne::Random( ZeroToOne::kZero, kMinLifeStep );
	ZeroToOne lifeStep = ((kMaxLifeStep - kMinLifeStep) * distribution) + kMinLifeStep;
	
	m_life += ZeroToOne::Random( kMinLifeStep, kMaxLifeStep );
	if( m_life == ZeroToOne::kOne )
	{
		Reset();
	}
}

void TwinklesUpdate()
{
	for( unsigned int i = 0; i < kNumTwinkles; ++i )
	{
		twinkles[i].Update( i );
	}
}

void TwinklesRender( BrightnessArray& brightnesses, unsigned int first, unsigned int howMany  )
{
	unsigned end = first + howMany;
	for( unsigned int i = first; i < end; ++i )
	{
		const Twinkle& twinkle = twinkles[i];
		ZeroToOne distribution( (i & 7) << 12 );
		ZeroToOne halfWidth = ((kMaxHalfWidth - kMinHalfWidth) * distribution) + kMinHalfWidth;
		
		ZeroToOne lifeBrightness = (ZeroToOne::kHalf - AbsDiff( twinkle.m_life, 0.5f )) * kLifeBrightnessScale;
		ZeroToOne brightness = kMaxBrightness * lifeBrightness;
		FixedPt twinkleBoost = (kTwinkleBoost * FixedPt( (uint16_t) twinkle.m_twinkleSmoothed )) + FixedPt::kOne;
		brightness = brightness * twinkleBoost;
		
		brightnesses.Blob( twinkle.m_pos, halfWidth, brightness.ToBrightness() );
	}
}
