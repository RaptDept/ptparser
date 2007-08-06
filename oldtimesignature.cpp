/////////////////////////////////////////////////////////////////////////////
// Name:            oldtimesignature.cpp
// Purpose:         Used to retrieve time signatures in older file versions
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 29, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "oldtimesignature.h"

#include "timesignature.h"      // Needed for ConstructTimeSignature
#include <math.h>               // Needed for pow

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Default Constants
const wxWord        OldTimeSignature::DEFAULT_SYSTEM            = 0;
const wxByte        OldTimeSignature::DEFAULT_POSITION          = 0;
const wxUint32      OldTimeSignature::DEFAULT_DATA              = 0;

/// Default Constructor
OldTimeSignature::OldTimeSignature() :
    m_system(DEFAULT_SYSTEM), m_position(DEFAULT_POSITION), m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Jan 3, 2005
}

/// Destructor
OldTimeSignature::~OldTimeSignature()
{
    //------Last Checked------//
    // - Jan 3, 2005
}

// Construction Functions
/// Constructs a TimeSignature object using OldTimeSignature data
/// @param timeSignature TimeSignature object to construct
/// @return True if the object was constructed, false if not
bool OldTimeSignature::ConstructTimeSignature(
    TimeSignature& timeSignature) const
{
    //------Last Checked------//
    // - Jan 3, 2005
    
    wxByte byTemp = HIBYTE(HIWORD(m_data));

	wxByte beatsPerMeasure = (wxByte)(((byTemp >> 3) & 0x1f) + 1);	            // Top 5 bits = bpm, where 0 = 1, 1 = 2, etc.
	wxByte beatAmount = (wxByte)pow((double)2, (double)(byTemp & 0x07));		// Bottom 3 bits = ba in power of 2

    if (!timeSignature.SetMeter(beatsPerMeasure, beatAmount))
        return (false);
     
    if ((m_data & 0x400000) == 0x400000)
        timeSignature.SetCommonTime();
    else if ((m_data & 0x800000) == 0x800000)
        timeSignature.SetCutTime();

    // Each beam part is stored in 5 bit segments
	wxUint32 beamingPattern = 
        MAKELONG(
	        MAKEWORD(m_data & 0x1f, ((m_data & 0x3e0) >> 5)),
	        MAKEWORD(((m_data & 0x7c00) >> 10), ((m_data & 0xf8000) >> 15))
	    );
	
	if (!timeSignature.SetBeamingPatternFromwxUint32(beamingPattern))
	    return (false);
	
    return (true);
}

// Operators
/// Assignment Operator
const OldTimeSignature& OldTimeSignature::operator=(
    const OldTimeSignature& oldTimeSignature)
{
    //------Last Checked------//
    // - Jan 5, 2005
    
    // Check for assignment to self
    if (this != &oldTimeSignature)
    {
        m_system = oldTimeSignature.m_system;
        m_position = oldTimeSignature.m_position;
        m_data = oldTimeSignature.m_data;
    }
    return (*this);
}

/// Equality Operator
bool OldTimeSignature::operator==(
    const OldTimeSignature& oldTimeSignature) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (
        (m_system == oldTimeSignature.m_system) &&
        (m_position == oldTimeSignature.m_position) &&
        (m_data == oldTimeSignature.m_data)
    );
}

/// Inequality Operator
bool OldTimeSignature::operator!=(
    const OldTimeSignature& oldTimeSignature) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (!operator==(oldTimeSignature));
}
    
/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool OldTimeSignature::DoDeserialize(PowerTabInputStream& stream,
    wxWord WXUNUSED(version))
{
    //------Last Checked------//
    // - Apr 22, 2007
    stream >> m_system >> m_position >> m_data;
    wxCHECK(stream.CheckState(), false);
    
    return (stream.CheckState());
}
