//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#include "Gaffer/Context.h"
#include "GafferImage/Grade.h"
#include "IECore/BoxOps.h"
#include "IECore/BoxAlgo.h"

using namespace IECore;
using namespace Gaffer;

namespace GafferImage
{

IE_CORE_DEFINERUNTIMETYPED( Grade );

size_t Grade::g_firstPlugIndex = 0;

Grade::Grade( const std::string &name )
	:	ChannelDataProcessor( name )
{
	storeIndexOfNextChild( g_firstPlugIndex );
	addChild( new Color3fPlug( "blackPoint" ) );
	addChild( new Color3fPlug( "whitePoint" ) );
	addChild( new Color3fPlug( "lift" ) );
	addChild( new Color3fPlug( "gain" ) );
	addChild( new Color3fPlug( "multiply" ) );
	addChild( new Color3fPlug( "offset" ) );
	addChild( new Color3fPlug( "gamma" ) );
	addChild( new BoolPlug( "blackClamp" ) );
	addChild( new BoolPlug( "whiteClamp" ) );

	// Set the default values of the plugs.
	whitePointPlug()->setValue( Imath::V3f(1.f, 1.f, 1.f) );
	gainPlug()->setValue( Imath::V3f(1.f, 1.f, 1.f) );
	multiplyPlug()->setValue( Imath::V3f(1.f, 1.f, 1.f) );
	gammaPlug()->setValue( Imath::V3f(1.f, 1.f, 1.f) );
	blackClampPlug()->setValue( true );
}

Grade::~Grade()
{
}

Gaffer::Color3fPlug *Grade::blackPointPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex );
}

const Gaffer::Color3fPlug *Grade::blackPointPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex );
}

Gaffer::Color3fPlug *Grade::whitePointPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex+1 );
}

const Gaffer::Color3fPlug *Grade::whitePointPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex+1 );
}

Gaffer::Color3fPlug *Grade::liftPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex+2 );
}

const Gaffer::Color3fPlug *Grade::liftPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex+2 );
}

Gaffer::Color3fPlug *Grade::gainPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex+3 );
}

const Gaffer::Color3fPlug *Grade::gainPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex+3 );
}

Gaffer::Color3fPlug *Grade::multiplyPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex+4 );
}

const Gaffer::Color3fPlug *Grade::multiplyPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex+4 );
}

Gaffer::Color3fPlug *Grade::offsetPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex+5 );
}

const Gaffer::Color3fPlug *Grade::offsetPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex+5 );
}

Gaffer::Color3fPlug *Grade::gammaPlug()
{
	return getChild<Color3fPlug>( g_firstPlugIndex+6 );
}

const Gaffer::Color3fPlug *Grade::gammaPlug() const
{
	return getChild<Color3fPlug>( g_firstPlugIndex+6 );
}

Gaffer::BoolPlug *Grade::blackClampPlug()
{
	return getChild<BoolPlug>( g_firstPlugIndex+7 );
}

const Gaffer::BoolPlug *Grade::blackClampPlug() const
{
	return getChild<BoolPlug>( g_firstPlugIndex+7 );
}

Gaffer::BoolPlug *Grade::whiteClampPlug()
{
	return getChild<BoolPlug>( g_firstPlugIndex+8 );
}

const Gaffer::BoolPlug *Grade::whiteClampPlug() const
{
	return getChild<BoolPlug>( g_firstPlugIndex+8 );
}

bool Grade::enabled() const
{
	return ChannelDataProcessor::enabled();
}

void Grade::affects( const Gaffer::ValuePlug *input, AffectedPlugsContainer &outputs ) const
{
	ChannelDataProcessor::affects( input, outputs );

	// Process the children of the compound plugs. 
	for( unsigned int i = 0; i < 3; ++i )
	{
		if( input == blackPointPlug()->getChild(i) ||
				input == whitePointPlug()->getChild(i) ||
				input == liftPlug()->getChild(i) ||
				input == gainPlug()->getChild(i) ||
				input == multiplyPlug()->getChild(i) ||
				input == offsetPlug()->getChild(i) ||
				input == gammaPlug()->getChild(i)
		  )
		{
			outputs.push_back( outPlug()->channelDataPlug() );	
			return;
		}
	}

	// Process all other plugs.
	if( input == inPlug()->channelDataPlug() ||
			input == blackClampPlug() ||
			input == whiteClampPlug()
	  )
	{
		outputs.push_back( outPlug()->channelDataPlug() );	
		return;
	}

}

void Grade::hashChannelDataPlug( const GafferImage::ImagePlug *output, const Gaffer::Context *context, IECore::MurmurHash &h ) const
{
	const std::string &channelName = context->get<std::string>( ImagePlug::channelNameContextName );

	ContextPtr tmpContext = new Context( *Context::current() );
	Context::Scope scopedContext( tmpContext );	

	tmpContext->set( ImagePlug::channelNameContextName, channelName );
	inPlug()->channelDataPlug()->hash( h );

	// Hash all of the inputs.
	blackPointPlug()->hash( h );
	whitePointPlug()->hash( h );
	liftPlug()->hash( h );
	gainPlug()->hash( h );
	multiplyPlug()->hash( h );
	offsetPlug()->hash( h );
	gammaPlug()->hash( h );
	blackClampPlug()->hash( h );
	whiteClampPlug()->hash( h );
}

IECore::ConstFloatVectorDataPtr Grade::computeChannelData( const std::string &channelName, const Imath::V2i &tileOrigin, const Gaffer::Context *context, const ImagePlug *parent ) const
{
	ConstFloatVectorDataPtr inData = inPlug()->channelData( channelName, tileOrigin );
	
	// Calculate the valid data window that we are to merge.
	const int tileSize = ImagePlug::tileSize();
	const Imath::Box2i dataWindow = inPlug()->dataWindowPlug()->getValue();
	Imath::Box2i tile( tileOrigin, Imath::V2i( tileOrigin.x + tileSize - 1, tileOrigin.y + tileSize - 1 ) );
	
	// Allocate the new tile
	FloatVectorDataPtr outDataPtr = new FloatVectorData;
	std::vector<float> &outData = outDataPtr->writable();
	outData.resize( tileSize * tileSize, 0.0f );
		
	// Check that the gamma value is not 0.
	const int channelIdx = channelName == "R" ? 0 : channelName == "G" ? 1 : 2;
	const float gamma = gammaPlug()->getValue()[channelIdx];
	if ( gamma == 0. ) return inPlug()->channelDataPlug()->getValue();
	
	// Do some pre-processing.
	const float invGamma = 1. / gamma;	
	const float multiply = multiplyPlug()->getValue()[channelIdx];
	const float gain = gainPlug()->getValue()[channelIdx];
	const float lift = liftPlug()->getValue()[channelIdx];
	const float whitePoint = whitePointPlug()->getValue()[channelIdx];
	const float blackPoint = blackPointPlug()->getValue()[channelIdx];
	const float offset = offsetPlug()->getValue()[channelIdx];
	const bool whiteClamp = whiteClampPlug()->getValue();	
	const bool blackClamp = blackClampPlug()->getValue();	

	const float A = multiply * ( gain - lift ) / ( whitePoint - blackPoint );
	const float B = offset + lift - A * blackPoint;
	
	// Grade the tile.
	for( int y = tile.min.y; y<=tile.max.y; y++ )
	{
		const float *tilePtr = &(inData->readable()[0]) + (y - tileOrigin.y) * tileSize + (tile.min.x - tileOrigin.x);
		float *outPtr = &(outData[0]) + ( y - tileOrigin.y ) * tileSize + (tile.min.x - tileOrigin.x);
		for( int x = tile.min.x; x <= tile.max.x; x++ )
		{
			float colour = *tilePtr++;
			const float c = A * colour + B;
			colour = ( c >= 0.f && invGamma != 1.f ? (float)pow( c, invGamma ) : c );

			if ( blackClamp && colour < 0.f ) colour = 0.f;
			if ( whiteClamp && colour > 1.f ) colour = 1.f;

			*outPtr++ = colour;
		}
	}

	return outDataPtr;
}

} // namespace GafferImage
