#ifndef _COLOR_H_
#define _COLOR_H_
//
// color.h
//
// Code base on the original article as found on the microsoft 
// knowledge base site (msdn.microsoft.com) article Q29240.
//
// The LightenColor() and DarkenColor() methods are taken from the
// BCMenu code written by:
// 
// Author : Brent Corkum
// Email :  corkum@rocscience.com
// Latest Version : http://www.rocscience.com/~corkum/BCMenu.html
//

#include "../standard.h"

#define HLSMAX		240
#define RGBMAX		255
#define UNDEFINED	( HLSMAX * 2 / 3 )

// Do not want this warning...
#pragma warning( disable : 4244 )

// A class that implements RGB-->HLS (Hue, Lightness and Saturation) 
// conversion and vice versa.
class ClsColor
{
public:
	// Construction/destruction.
	ClsColor() : m_Red( 0 ), m_Green( 0 ), m_Blue( 0 ), m_Hue( 0 ), m_Saturation( 0 ), m_Lightness( 0 ) {;}
	ClsColor( COLORREF crColor ) {	SetRGB( crColor ); }
	ClsColor( WORD wHue, WORD wLightness, WORD wSaturation ) { SetHLS( wHue, wLightness, wSaturation ); }
	virtual ~ClsColor() {;}
	
	// Lighten the color.
	void LightenColor( double dRed, double dGreen, double dBlue )
	{
		_ASSERT(( dRed   >= 0.0 && dRed   <= 1.0 ) &&
			( dGreen >= 0.0 && dGreen <= 1.0 ) &&
			( dBlue  >= 0.0 && dBlue  <= 1.0 ));

		m_Red   = (( dRed   * ( 255 - m_Red   )) + m_Red   );
		m_Green = (( dGreen * ( 255 - m_Green )) + m_Green );
		m_Blue  = (( dBlue  * ( 255 - m_Blue  )) + m_Blue  );
	}

	// Darken the color.
	void DarkenColor( double dRed, double dGreen, double dBlue )
	{
		_ASSERT(( dRed   >= 0.0 && dRed   <= 1.0 ) &&
			( dGreen >= 0.0 && dGreen <= 1.0 ) &&
			( dBlue  >= 0.0 && dBlue  <= 1.0 ));

		m_Red   = ( m_Red   - ( dRed   * m_Red   ));
		m_Green = ( m_Green - ( dGreen * m_Green ));
		m_Blue  = ( m_Blue  - ( dBlue  * m_Blue  ));
	}

	// Convert RGB values to HLS values.
	void SetRGB( COLORREF crColor )
	{
		// Store values.
		m_Red   = GetRValue( crColor );
		m_Green = GetGValue( crColor );
		m_Blue  = GetBValue( crColor );

		// Compute Lightness.
		BYTE cMin = ( BYTE )min( min( m_Red, m_Green ), m_Blue );
		BYTE cMax = ( BYTE )max( max( m_Red, m_Green ), m_Blue );
		m_Lightness = ((( cMax + cMin ) * HLSMAX ) + RGBMAX ) / ( 2 * RGBMAX );

		// R=G=B --> Achromatic case.
		if ( cMax == cMin )
		{
			m_Saturation = 0;
			m_Hue = UNDEFINED;
		}
		else
		// Chromatic case.
		{
			// Saturation.
			if ( m_Lightness <= ( HLSMAX / 2 )) m_Saturation = ((( cMax - cMin ) * HLSMAX ) + (( cMax + cMin ) / 2 )) / ( cMax + cMin );
			else				    m_Saturation = ((( cMax - cMin ) * HLSMAX ) + (( 2 * RGBMAX - cMax - cMin ) / 2 )) / ( 2 * RGBMAX - cMax - cMin );
		
			// Hue.
			WORD Rdelta = ((( cMax - m_Red   ) * ( HLSMAX / 6 )) + (( cMax - cMin ) / 2 )) / ( cMax - cMin );
			WORD Gdelta = ((( cMax - m_Green ) * ( HLSMAX / 6 )) + (( cMax - cMin ) / 2 )) / ( cMax - cMin );
			WORD Bdelta = ((( cMax - m_Blue  ) * ( HLSMAX / 6 )) + (( cMax - cMin ) / 2 )) / ( cMax - cMin );

			if (      m_Red   == cMax ) m_Hue = Bdelta - Gdelta;
			else if ( m_Green == cMax ) m_Hue = ( HLSMAX / 3 ) + Rdelta - Bdelta;
			else			    m_Hue = (( 2 * HLSMAX ) / 3 ) + Gdelta - Rdelta;

			if ( m_Hue < 0      ) m_Hue += HLSMAX;
			if ( m_Hue > HLSMAX ) m_Hue -= HLSMAX;
		}
	}

	// Convert HLS values to RGB values.
	void SetHLS( WORD wHue, WORD wLightness, WORD wSaturation )
	{
		// Save values.
		m_Hue	     = wHue;
		m_Lightness  = wLightness;
		m_Saturation = wSaturation;

		// Achromatic case.
		if ( wSaturation == 0 )
		{
			m_Red = m_Green = m_Blue = ( wLightness * RGBMAX ) / HLSMAX;
			_ASSERT( wHue == UNDEFINED );
		}
		else
		// Chromatic case.
		{
			// Setup magic numbers.
			WORD wMagic1, wMagic2;
			if ( wLightness <= ( HLSMAX / 2 )) wMagic2 = ( wLightness * ( HLSMAX + wSaturation ) + ( HLSMAX / 2 )) / HLSMAX;
			else				   wMagic2 = wLightness + wSaturation - (( wLightness * wSaturation ) + ( HLSMAX / 2 )) / HLSMAX;
			wMagic1 = 2 * wLightness - wMagic2;

			// Get RGB, change units from HLSMAX to RGBMAX.
			m_Red   = ( HueToRGB( wMagic1, wMagic2, wHue + ( HLSMAX / 3 )) * RGBMAX + ( HLSMAX / 2 )) / HLSMAX ;
			m_Green = ( HueToRGB( wMagic1, wMagic2, wHue ) * RGBMAX + ( HLSMAX / 2 )) / HLSMAX;
			m_Blue  = ( HueToRGB( wMagic1, wMagic2, wHue - ( HLSMAX / 3 )) * RGBMAX + ( HLSMAX / 2 )) / HLSMAX;
		}
	}

	// Inlined methods to access color data.
	inline WORD Red()	   { return m_Red; }
	inline WORD Green()	   { return m_Green; }
	inline WORD Blue()	   { return m_Blue; }
	inline WORD Hue()	   { return m_Hue; }
	inline WORD Lightness()    { return m_Lightness; }
	inline WORD Saturation()   { return m_Saturation; }
	inline COLORREF Color()	   { return RGB( m_Red, m_Green, m_Blue ); }
	inline operator COLORREF() { return RGB( m_Red, m_Green, m_Blue ); }
	
protected:
	// Utility for HLStoRGB().
	WORD HueToRGB( WORD n1, WORD n2, WORD hue )
	{
		// Range check. Note values passed add/subtract thirds of range.
		if ( hue < 0      ) hue += HLSMAX;
		if ( hue > HLSMAX ) hue -= HLSMAX;

		// Return R,G or B value from this tridrant.
		if ( hue < ( HLSMAX / 6 ))	  return ( n1 + ((( n2 - n1 ) * hue + ( HLSMAX / 12 )) / ( HLSMAX / 6 )));
		if ( hue < ( HLSMAX / 2 ))	  return ( n2 );
		if ( hue < (( HLSMAX * 2 ) / 3 )) return ( n1 + ((( n2 - n1 ) * ((( HLSMAX * 2 ) / 3 ) - hue ) + ( HLSMAX / 12 )) / ( HLSMAX / 6 )));
		else				  return ( n1 );
	}

	// Data.
	WORD	m_Red;
	WORD	m_Green;
	WORD	m_Blue;
	WORD	m_Hue;
	WORD	m_Saturation;
	WORD	m_Lightness;
};

// Back to default...
#pragma warning( default : 4244 )

#endif // _COLOR_H_