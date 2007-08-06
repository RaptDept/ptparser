/////////////////////////////////////////////////////////////////////////////
// Name:            timesignature.cpp
// Purpose:         Stores and renders time signatures
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 12, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "timesignature.h"
#include <math.h>           // Needed for pow()

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Note: The MIDI_xxx duration constants found in this class are defined in generalmidi.h

const wxUint32   TimeSignature::DEFAULT_DATA            = 0x1a018000;
const wxByte     TimeSignature::DEFAULT_PULSES          = 4;

const wxByte     TimeSignature::MIN_BEATSPERMEASURE     = 1;
const wxByte     TimeSignature::MAX_BEATSPERMEASURE     = 32;

const wxByte     TimeSignature::MIN_BEATAMOUNT          = 2;
const wxByte     TimeSignature::MAX_BEATAMOUNT          = 32;
    
const wxByte     TimeSignature::MIN_PULSES              = 0;                
const wxByte     TimeSignature::MAX_PULSES              = 32;             

// Constructor/Destructor
/// Default Constructor
TimeSignature::TimeSignature() :
    m_data(DEFAULT_DATA), m_pulses(DEFAULT_PULSES)
{
    //------Last Checked------//
    // - Dec 12, 2004
}

/// Primary Constructor
/// @param beatsPerMeasure The number of beats in a measure (numerator)
/// @param beatAmount The amount given to each beat (denominator)
TimeSignature::TimeSignature(wxByte beatsPerMeasure, wxByte beatAmount) :
    m_data(DEFAULT_DATA), m_pulses(DEFAULT_PULSES)
{
    //------Last Checked------//
    // - Dec 13, 2004
    SetMeter(beatsPerMeasure, beatAmount);
}

/// Copy Constructor
TimeSignature::TimeSignature(const TimeSignature& timeSignature)
{
    //------Last Checked------//
    // - Dec 12, 2004
    *this = timeSignature;
}

/// Destructor
TimeSignature::~TimeSignature()
{
    //------Last Checked------//
    // - Dec 12, 2004
}

// Operators
/// Assignment Operator
const TimeSignature& TimeSignature::operator=(
    const TimeSignature& timeSignature)
{
    //------Last Checked------//
    // - Dec 12, 2004
    
    // Check for assignment to self
    if (this != &timeSignature)
    {
        m_data = timeSignature.m_data;
        m_pulses = timeSignature.m_pulses;
    }
    return (*this);
}

/// Equality Operator
bool TimeSignature::operator==(const TimeSignature& timeSignature) const
{
    //------Last Checked------//
    // - Dec 10, 2004
    return (
        (m_data == timeSignature.m_data) &&
        (m_pulses == timeSignature.m_pulses)
    );
}

/// Inequality Operator
bool TimeSignature::operator!=(const TimeSignature& timeSignature) const
{
    //------Last Checked------//
    // - Dec 10, 2004
    return (!operator==(timeSignature));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool TimeSignature::DoSerialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Dec 12, 2004
    stream << m_data << m_pulses;
    wxCHECK(stream.CheckState(), false);
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool TimeSignature::DoDeserialize(PowerTabInputStream& stream,
    wxWord WXUNUSED(version))
{
    //------Last Checked------//
    // - Apr 22, 2007
    stream >> m_data >> m_pulses;
    wxCHECK(stream.CheckState(), false);
    return (stream.CheckState());
}

// Meter Functions
/// Sets the meter (i.e. 2/4)
/// @param beatsPerMeasure The number of beats in a measure (numerator)
/// @param beatAmount The amount given to each beat (denominator)
/// @return True if the meter was set, false if not
bool TimeSignature::SetMeter(wxByte beatsPerMeasure, wxByte beatAmount)
{
    //------Last Checked------//
    // - Dec 13, 2004
    if (!SetBeatsPerMeasure(beatsPerMeasure))
        return (false);
    if (!SetBeatAmount(beatAmount))
        return (false);
        
    // Set the default beaming pattern, based on the meter's beat amount and
    // basic beat
    beatAmount = GetBeatAmount();
    wxUint32 basicBeat = GetBasicBeat();

    // Calculate the number of 8th notes in basic beat
    if (beatAmount <= 8)
    {
        // 4/4 Time, group in 4s
        if ((beatAmount == 4) && (GetBeatsPerMeasure() == 4))
            SetBeamingPattern(4);
        else
            SetBeamingPattern((wxByte)(basicBeat / (MIDI_PPQN / 2)));
    }
    // Calculate the number of 16th notes in basic beat
    else if (beatAmount == 16)
        SetBeamingPattern((wxByte)(basicBeat / (MIDI_PPQN / 4)));
    // Calculate the number of 32nd notes in basic beat
    else if (beatAmount == 32)
        SetBeamingPattern((wxByte)(basicBeat / (MIDI_PPQN / 8)));

    // Set the default pulses
    SetPulses((wxByte)(GetMeasureTotal() / basicBeat));
    
    return (true);
}

/// Gets the meter (i.e. 2/4)
/// @param beatsPerMeasure The number of beats in a measure (numerator)
/// @param beatAmount The amount given to each beat (denominator)
void TimeSignature::GetMeter(wxByte& beatsPerMeasure, wxByte& beatAmount) const
{
    //------Last Checked------//
    // - Dec 12, 2004
    beatsPerMeasure = GetBeatsPerMeasure();
    beatAmount = GetBeatAmount();
}

/// Determines if the meter is compound time
/// @return True if the meter is compound time, false if not
bool TimeSignature::IsCompoundTime() const
{
    //------Last Checked------//
    // - Dec 12, 2004
    wxByte beatsPerMeasure, beatAmount;
    GetMeter(beatsPerMeasure, beatAmount);
    return (((beatsPerMeasure % 3) == 0) && (beatsPerMeasure != 3));
}

/// Determines if the meter is quadruple time
/// @return True if the meter is quadruple time, false if not
bool TimeSignature::IsQuadrupleTime() const
{
    //------Last Checked------//
    // - Dec 12, 2004
    wxByte beatsPerMeasure, beatAmount;
    GetMeter(beatsPerMeasure, beatAmount);
    return ((beatsPerMeasure == 4) || (beatsPerMeasure == 12));
}

/// Gets the total time for a measure that uses the time signature, in MIDI
/// units
/// @returns The total time for a measure that uses the time signature, in MIDI
/// units
wxUint32 TimeSignature::GetMeasureTotal() const
{
    //------Last Checked------//
    // - Dec 12, 2004
    return (((MIDI_PPQN * 4) / GetBeatAmount()) * GetBeatsPerMeasure());
}

/// Gets the basic beat value for the time signature
/// @return The basic beat value for the time signature, in MIDI units
wxUint32 TimeSignature::GetBasicBeat() const
{
    //------Last Checked------//
    // - Dec 12, 2004
    
    // Compound time = dotted beat
    if (IsCompoundTime())
        return ((MIDI_PPQN * 4) * 3 / GetBeatAmount());
    return ((MIDI_PPQN * 4) / GetBeatAmount());
}

// Beats Per Measure Functions
/// Sets the beats per measure (numerator)
/// @param beatsPerMeasure The beats per measure to set
/// @return True if the beats per measure was set, false if not
bool TimeSignature::SetBeatsPerMeasure(wxByte beatsPerMeasure)
{
    //------Last Checked------//
    // - Dec 13, 2004
    wxCHECK(IsValidBeatsPerMeasure(beatsPerMeasure), false);

    // Automatically clear the common and cut time flags
    ClearFlag(commonTime | cutTime);
    
    // Beats per measure are zero-based in storage
    beatsPerMeasure--;

    m_data &= ~beatsPerMeasureMask;
    m_data |= (beatsPerMeasure << 27);
    return (true);
}

/// Gets the time signature's beats per measure value (numerator)
/// @return The time signature's beats per measure value
wxByte TimeSignature::GetBeatsPerMeasure() const
{
    //------Last Checked------//
    // - Dec 12, 2004
    
    // Common Time
    if (IsCommonTime())
        return (4);
    // Cut Time
    else if (IsCutTime())
        return (2);

    // beatsPerMeasure is stored in zero-based format, so add one
    wxByte beatsPerMeasure = (wxByte)((m_data & beatsPerMeasureMask) >> 27);
    beatsPerMeasure++;
    return (beatsPerMeasure);
}

// Beat Amount Functions
/// Sets the beat amount
/// @param beatAmount Beat amount to set
/// @return True if the beat amount was set, false if not
bool TimeSignature::SetBeatAmount(wxByte beatAmount)
{
    //------Last Checked------//
    // - Dec 13, 2004
    wxCHECK(IsValidBeatAmount(beatAmount), false);

    // Automatically clear the common and cut time flags
    ClearFlag(commonTime | cutTime);
        
    // The beat amount is stored in power of 2 form
    if (beatAmount == 2)
        beatAmount = 1;
    else if (beatAmount == 4)
        beatAmount = 2;
    else if (beatAmount == 8)
        beatAmount = 3;
    else if (beatAmount == 16)
        beatAmount = 4;
    else if (beatAmount == 32)
        beatAmount = 5;

    m_data &= ~beatAmountMask;
    m_data |= (beatAmount << 24);
    return (true);
}

/// Gets the time signature's beat amount (denominator)
/// @return The time signature's beat amount
wxByte TimeSignature::GetBeatAmount() const
{
    //------Last Checked------//
    // - Dec 13, 2004
    
    // Common Time
    if (IsCommonTime())
        return (4);
    // Cut Time
    else if (IsCutTime())
        return (2);

    // The beat amount is stored in power of 2 form
    wxByte beatAmount = (wxByte)((m_data & beatAmountMask) >> 24);
    beatAmount = (wxByte)(pow((double)2, (double)beatAmount));
    return (beatAmount);
}

// Beaming Pattern Functions
// Sets the beaming pattern
/// @param beat1 Number of items to beam in the first beat 
/// @param beat2 Number of items to beam in the second beat
/// @param beat3 Number of items to beam in the third beat
/// @param beat4 Number of items to beam in the fourth beat
/// @return True if the beaming pattern was set, false if not
bool TimeSignature::SetBeamingPattern(wxByte beat1, wxByte beat2, wxByte beat3,
    wxByte beat4)
{
    //------Last Checked------//
    // - Dec 13, 2004
    wxCHECK(IsValidBeamingPatternBeat(beat1, true), false);
    wxCHECK(IsValidBeamingPatternBeat(beat2, false), false);
    wxCHECK(IsValidBeamingPatternBeat(beat3, false), false);
    wxCHECK(IsValidBeamingPatternBeat(beat4, false), false);

    // Clear the old beam data
    m_data &= ~beamingPatternMask;

    // Data is stored in zero-based format
    if (beat1 > 0)
        beat1--;

    if (beat2 > 0)
        beat2--;

    if (beat3 > 0)
        beat3--;

    if (beat4 > 0)
        beat4--;

    // Construct the beaming data
    wxUint32 beaming = 0;

    beaming = beat1;
    beaming <<= 5;
    beaming |= beat2;
    beaming <<= 5;
    beaming |= beat3;
    beaming <<= 5;
    beaming |= beat4;

    // Update the beaming pattern
    m_data |= beaming;
    
    return (true);
}

/// Gets the beaming pattern used by the time signature
/// @param beat1 - Beaming pattern for the first beat in the time signature
/// @param beat2 - Beaming pattern for the second beat in the time signature
/// @param beat3 - Beaming pattern for the third beat in the time signature
/// @param beat4 - Beaming pattern for the fourth beat in the time signature
void TimeSignature::GetBeamingPattern(wxByte & beat1, wxByte & beat2,
    wxByte & beat3, wxByte & beat4) const
{
    //------Last Checked------//
    // - Dec 13, 2004
    
    // All the beam data is stored in zero-based format
    beat1 = (wxByte)((m_data & beamingPatternBeat1Mask) >> 15);
    beat1++;

    beat2 = (wxByte)((m_data & beamingPatternBeat2Mask) >> 10);
    if (beat2 > 0)
        beat2++;
        
    beat3 = (wxByte)((m_data & beamingPatternBeat3Mask) >> 5);
    if (beat3 > 0)
        beat3++;
        
    beat4 = (wxByte)(m_data & beamingPatternBeat4Mask);
    if (beat4 > 0)
        beat4++;
}

/// Sets the beaming pattern using a wxUint32 format value (wxUint32 format =
/// beat1 - LOBYTE of LOWORD, beat2 - HIBYTE of LOWORD,
/// beat3 - LOBYTE of HIWORD, beat4 - HIBYTE of HIWORD)
/// @param beamingPattern Beaming pattern to set, in wxUint32 format
/// @return True if the beaming pattern was set, false if not
bool TimeSignature::SetBeamingPatternFromwxUint32(wxUint32 beamingPattern)
{
    //------Last Checked------//
    // - Dec 13, 2004
    return (SetBeamingPattern(
        LOBYTE(LOWORD(beamingPattern)),
        HIBYTE(LOWORD(beamingPattern)),
        LOBYTE(HIWORD(beamingPattern)),
        HIBYTE(HIWORD(beamingPattern))
        )
    );
}

/// Gets the beaming pattern used by the time signature, in wxUint32 format
/// (wxUint32 format = beat1 - LOBYTE of LOWORD, beat2 - HIBYTE of LOWORD,
/// beat3 - LOBYTE of HIWORD, beat4 - HIBYTE of HIWORD)
/// @return The beaming pattern for the time signature, in wxUint32 format
wxUint32 TimeSignature::GetBeamingPatternAswxUint32() const
{
    //------Last Checked------//
    // - Dec 12, 2004
    wxByte beat1, beat2, beat3, beat4;
    GetBeamingPattern(beat1, beat2, beat3, beat4);
    return (MAKELONG(MAKEWORD(beat1, beat2), MAKEWORD(beat3, beat4)));
}

// Flag Functions
/// Sets a flag used by the TimeSignature object
/// @param flag The flag to set
bool TimeSignature::SetFlag(wxUint32 flag)
{
    //------Last Checked------//
    // - Dec 13, 2004
    wxCHECK(IsValidFlag(flag), false);
    
    // Perform any mutual exclusive flag clearing
    if ((flag & commonTime) == commonTime)
        ClearFlag(cutTime);
    if ((flag & cutTime) == cutTime)
        ClearFlag(commonTime);
    m_data |= flag;
    return (true);
}

// Text Functions
/// Gets a text representation of the meter
/// @param type Type of text to get (see textTypes enum in .h for values)
/// @return Text representation of the meter 
wxString TimeSignature::GetText(wxUint32 type) const
{
    //------Last Checked------//
    // - Dec 13, 2004
    int beatsPerMeasure = GetBeatsPerMeasure();
    int beatAmount = GetBeatAmount();

    wxString returnValue;

    // Beats per measure only
    if (type == textBeatsPerMeasure)
        returnValue = wxString::Format(wxT("%d"), beatsPerMeasure);
    // Beat amount only
    else if (type == textBeatAmount)
        returnValue = wxString::Format(wxT("%d"), beatAmount);
    // Full meter
    else
    {
        returnValue = wxString::Format(wxT("%d/%d"), beatsPerMeasure,
            beatAmount);
    }

    return (returnValue);
}
