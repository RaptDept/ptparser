/////////////////////////////////////////////////////////////////////////////
// Name:            note.cpp
// Purpose:         Stores and renders a note
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 17, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "note.h"

#include "powertabfileheader.h"             // Needed for file version constants

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Default Constants
const wxByte Note::DEFAULT_STRING_DATA      = 0;
const wxWord Note::DEFAULT_SIMPLE_DATA      = 0;

// String Constants
const wxByte Note::MIN_STRING               = 0;
const wxByte Note::MAX_STRING               = 6;
    
// Fret Number Constants
const wxByte Note::MIN_FRET_NUMBER          = 0;
const wxByte Note::MAX_FRET_NUMBER          = 24;

// Bend Constants
const wxByte Note::MAX_BEND_PITCH           = 12;
const wxByte Note::MAX_BEND_DURATION        = 8;

// Constructor/Destructor
/// Default Constructor
Note::Note() :
    m_stringData(DEFAULT_STRING_DATA), m_simpleData(DEFAULT_SIMPLE_DATA)
{
    //------Last Checked------//
    // - Jan 17, 2005
	ClearComplexSymbolArrayContents();
}

/// Primary Constructor
/// @param string Zero-based number of the string the note is played on
/// @param fretNumber Fret number used to play the note
Note::Note(wxUint32 string, wxByte fretNumber) :
    m_stringData(DEFAULT_STRING_DATA), m_simpleData(DEFAULT_SIMPLE_DATA)
{
    //------Last Checked------//
    // - Jan 17, 2005
    wxASSERT(IsValidString(string));
    wxASSERT(IsValidFretNumber(fretNumber));
    
    SetString(string);
    SetFretNumber(fretNumber);
	ClearComplexSymbolArrayContents();
}

/// Copy Constructor
Note::Note(const Note& note) :
    m_stringData(DEFAULT_STRING_DATA), m_simpleData(DEFAULT_SIMPLE_DATA)
{
    //------Last Checked------//
    // - Dec 17, 2004
    *this = note;
}

/// Destructor
Note::~Note()
{
    //------Last Checked------//
    // - Jan 17, 2005
}

/// Assignment Operator
const Note& Note::operator=(const Note& note)
{
    //------Last Checked------//
    // - Dec 17, 2004
    
    // Check for assignment to self
    if (this != &note)
    {
        m_stringData = note.m_stringData;
        m_simpleData = note.m_simpleData;

        ClearComplexSymbolArrayContents();
        size_t count = note.GetComplexSymbolCount();
        size_t i = 0;
        for (; i < count; i++)
	        m_complexSymbolArray[i] = note.m_complexSymbolArray[i];
    }
	return (*this);
}

/// Equality Operator
bool Note::operator==(const Note& note) const
{
    //------Last Checked------//
    // - Jan 24, 2005
    
    // Note: Complex symbols aren't necessarily in the same slot in the array
    wxArrayInt thisComplexSymbolArray;
    wxArrayInt thatComplexSymbolArray;
    
	wxUint32 i = 0;
	for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
	{
	    thisComplexSymbolArray.Add(m_complexSymbolArray[i]);
	    thatComplexSymbolArray.Add(note.m_complexSymbolArray[i]);
	}
	
	thisComplexSymbolArray.Sort(wxCompareIntegers);
	thatComplexSymbolArray.Sort(wxCompareIntegers);

    i = 0;
	for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
	{
	    if (thisComplexSymbolArray[i] != thatComplexSymbolArray[i])
	        return (false);
	}

    return (
        (m_stringData == note.m_stringData) &&
        (m_simpleData == note.m_simpleData)
    );	
}

/// Inequality Operator
bool Note::operator!=(const Note& note) const
{
    //------Last Checked------//
    // - Dec 17, 2004
	return (!operator==(note));
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Note::DoSerialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Dec 17, 2004
   
    stream << m_stringData << m_simpleData;
    wxCHECK(stream.CheckState(), false);
    
	wxByte symbolCount = (wxByte)GetComplexSymbolCount();
	stream << symbolCount;
	wxCHECK(stream.CheckState(), false);

    size_t i = 0;
    for (; i < symbolCount; i++)
    {
	    stream << m_complexSymbolArray[i];
	    wxCHECK(stream.CheckState(), false);
    }
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Note::DoDeserialize(PowerTabInputStream& stream, wxWord version)
{
    //------Last Checked------//
    // - Dec 17, 2004
    
    // Version 1.0/1.0.2 artificial harm updated
	if (version == PowerTabFileHeader::FILEVERSION_1_0 || version == PowerTabFileHeader::FILEVERSION_1_0_2)
	{
		wxByte simple;
		stream >> m_stringData >> simple;
		wxCHECK(stream.CheckState(), false);

		// Simple is now stored as wxWord
		m_simpleData = simple;
    	
		ClearComplexSymbolArrayContents();
		wxByte symbolCount;
		stream >> symbolCount;
		wxCHECK(stream.CheckState(), false);

        size_t i = 0;
		for (; i < symbolCount; i++)
		{
			stream >> m_complexSymbolArray[i];
			wxCHECK(stream.CheckState(), false);

			wxByte symbolType = HIBYTE(HIWORD(m_complexSymbolArray[i]));

			if (symbolType == artificialHarmonic)
			{
				wxByte keyVariation = 1;
				wxByte usesFlats = HIBYTE(LOWORD(m_complexSymbolArray[i]));
				wxByte key = LOBYTE(LOWORD(m_complexSymbolArray[i]));

				// CASE: Using flats
				if (usesFlats == 1 && (key == 1 || key == 6))
					keyVariation = 2;
				// CASE: Using sharps 
				else if (usesFlats == 0 && (key == 3 || key == 8 || key == 10))
					keyVariation = 0;

				wxUint32 symbolData = artificialHarmonic;
				symbolData = (symbolData << 8) + 1;
				symbolData = (symbolData << 8) + keyVariation;
				symbolData = (symbolData << 8) + key;

				m_complexSymbolArray[i] = symbolData;
			}
			else if (symbolType == bend)
			{
				wxByte type = LOBYTE(HIWORD(m_complexSymbolArray[i]));
				wxByte bentPitch = LOBYTE(LOWORD(m_complexSymbolArray[i]));
				wxByte releasePitch = 0;
				wxByte duration = HIBYTE(LOWORD(m_complexSymbolArray[i]));
    			
				wxByte drawStart = (wxByte)((type < 6) ? 0 : 1);
				wxByte drawEnd = (wxByte)((type == 0 || type == 2 || type == 3 || type == 5 || type == 7) ? 1 : 0);
    	
    	        SetBend(type, bentPitch, releasePitch, duration, drawStart, drawEnd);
			}
		}
	}
	// CASE: Latest version
	else
	{
		stream >> m_stringData >> m_simpleData;
		wxCHECK(stream.CheckState(), false);
		
	    wxByte symbolCount = 0;
	    stream >> symbolCount;
	    wxCHECK(stream.CheckState(), false);

        size_t i = 0;
        for (; i < symbolCount; i++)
        {
	        stream >> m_complexSymbolArray[i];
	        wxCHECK(stream.CheckState(), false);
	    }
	}
    return (stream.CheckState());
}

// String Functions
/// Sets the string for the note
/// @param string String to set (zero-based)
/// @return True if the string was set, false if not
bool Note::SetString(wxUint32 string)
{
    //------Last Checked------//
    // - Jan 17, 2005
    wxCHECK(IsValidString(string), false);
    
    m_stringData &= ~stringMask;
    m_stringData |= (wxByte)(string << 5);
    
    return (true);
}

/// Gets the string for the note
/// @return The string for the note
wxUint32 Note::GetString() const
{
    //------Last Checked------//
    // - Jan 17, 2005
    return (wxByte)((m_stringData & stringMask) >> 5);
}

// Fret Number Functions
/// Sets the fret number for the note
/// @param fretNumber Fret number to set
/// @return True if the fret number was set, false if not
bool Note::SetFretNumber(wxByte fretNumber)
{
    //------Last Checked------//
    // - Jan 17, 2005
    wxCHECK(IsValidFretNumber(fretNumber), false);
    
    m_stringData &= ~fretNumberMask;
    m_stringData |= fretNumber;
    
    return (true);
}

/// Gets the fret number on a given string
/// @return The fret number on the string
wxByte Note::GetFretNumber() const
{
    //------Last Checked------//
    // - Jan 17, 2005
    return ((wxByte)(m_stringData & fretNumberMask));
}

// Simple Flag Functions
/// Sets a simple flag
/// @param flag Flag to set
/// @return True if the flag was set, false if not 
bool Note::SetSimpleFlag(wxWord flag)
{
    //------Last Checked------//
    // - Jan 24, 2005
    wxCHECK(IsValidSimpleFlag(flag), false); 
    
    // Mutually exclusive operations
    if ((flag & hammerPullMask) != 0)
        ClearSimpleFlag(hammerPullMask);
        
    if ((flag & octaveMask) != 0)
        ClearSimpleFlag(octaveMask);
        
    m_simpleData |= flag;
    return (true);
}
        
// Slide Into Functions
/// Sets (adds or updates) a slide into marker
/// @param type Slide into type to set (see slideIntoTypes enum for values)
/// @return True if the slide into marker was added or updated
bool Note::SetSlideInto(wxByte type)
{
    //------Last Checked------//
    // - Jan 21, 2005
    wxCHECK(IsValidSlideInto(type), false);
    
    if (type == slideIntoNone)
        return (ClearSlideInto());
        
    // Look for an existing slide first
    wxUint32 index = FindComplexSymbol(slide);
    
    // Slide exists; update its slide into data
    if (index != (wxUint32)-1)
    {
        m_complexSymbolArray[index] &= ~slideIntoTypeMask;
        m_complexSymbolArray[index] |= (wxUint32)(type << 16);
        return (true);
    }

    // Construct the symbol data, then add it to the array
    wxUint32 symbolData = MAKELONG(MAKEWORD(0, 0), MAKEWORD(type, slide));
    return (AddComplexSymbol(symbolData));
}

/// Gets the slide into data (if any)
/// @param type Holds the slide into type return value
/// @return True if the data was returned, false if not
bool Note::GetSlideInto(wxByte& type) const
{
    //------Last Checked------//
    // - Jan 21, 2005

    type = slideIntoNone;
    
    // Get the index of the slide
    wxUint32 index = FindComplexSymbol(slide);
    if (index == (wxUint32)-1)
        return (false);
    
    // Get the individual pieces that make up the slide into
    wxUint32 symbolData = m_complexSymbolArray[index];
	type = LOBYTE(HIWORD(symbolData));
	
    return (true);
}

/// Determines if the note has a slide into
/// @return True if the note has a slide into, false if not
bool Note::HasSlideInto() const
{
    //------Last Checked------//
    // - Jan 21, 2005
    
    wxByte type = slideIntoNone;
    if (!GetSlideInto(type))
        return (false);
    return (type != slideIntoNone);
}

/// Removes a slide into from the note
/// @return True if the slide into was removed, false if not
bool Note::ClearSlideInto()
{
    //------Last Checked------//
    // - Jan 21, 2005
    
    // Get the index of the slide
    wxUint32 index = FindComplexSymbol(slide);
    if (index == (wxUint32)-1)
        return (false);
    
    m_complexSymbolArray[index] &= ~slideIntoTypeMask;
    
    // Return if there is a slide out, otherwise remove the slide symbol
    if (HasSlideOutOf())
        return (true);
        
    return (RemoveComplexSymbol(slide));
}

// Slide Out Of Functions
/// Sets (adds or updates) a slide out of marker
/// @param type Slide out of type to set (see slideOutOf enum for values)
/// @param steps Number of steps to slide, in half tones
/// @return True if the slide out of marker was added or updated
bool Note::SetSlideOutOf(wxByte type, wxInt8 steps)
{
    //------Last Checked------//
    // - Jan 21, 2005
    wxCHECK(IsValidSlideOutOf(type), false);
    
    if (type == slideOutOfNone)
        return (ClearSlideOutOf());
        
    // Look for an existing slide first
    wxUint32 index = FindComplexSymbol(slide);
    
    // Slide exists; update its slide out of data
    if (index != (wxUint32)-1)
    {
        m_complexSymbolArray[index] &= ~(slideOutOfTypeMask | slideOutOfStepsMask);
        m_complexSymbolArray[index] |= (wxUint32)(type << 8);
        m_complexSymbolArray[index] |= (wxUint32)(steps);
        return (true);
    }

    // Construct the symbol data, then add it to the array
    wxUint32 symbolData = MAKELONG(MAKEWORD(steps, type), MAKEWORD(0, slide));
    return (AddComplexSymbol(symbolData));
}

/// Gets the slide out of data (if any)
/// @param type Holds the slide out of type return value
/// @param steps Holds the steps return value
/// @return True if the data was returned, false if not
bool Note::GetSlideOutOf(wxByte& type, wxInt8& steps) const
{
    //------Last Checked------//
    // - Jan 21, 2005

    type = slideIntoNone;
    steps = 0;
    
    // Get the index of the slide
    wxUint32 index = FindComplexSymbol(slide);
    if (index == (wxUint32)-1)
        return (false);
    
    // Get the individual pieces that make up the slide out of
    wxUint32 symbolData = m_complexSymbolArray[index];
	type = HIBYTE(LOWORD(symbolData));
	steps = LOBYTE(LOWORD(symbolData));
	
    return (true);
}

/// Determines if the note has a slide out of
/// @return True if the note has a slide out of, false if not
bool Note::HasSlideOutOf() const
{
    //------Last Checked------//
    // - Jan 21, 2005
    
    wxByte type = slideOutOfNone;
    wxInt8 steps = 0;
    if (!GetSlideOutOf(type, steps))
        return (false);
    return (type != slideOutOfNone);
}

/// Removes a slide out of from the note
/// @return True if the slide out of was removed, false if not
bool Note::ClearSlideOutOf()
{
    //------Last Checked------//
    // - Jan 21, 2005
    
    // Get the index of the slide
    wxUint32 index = FindComplexSymbol(slide);
    if (index == (wxUint32)-1)
        return (false);
    
    m_complexSymbolArray[index] &= ~(slideOutOfTypeMask | slideOutOfStepsMask);
    
    // Return if there is a slide in, otherwise remove the slide symbol
    if (HasSlideInto())
        return (true);
        
    return (RemoveComplexSymbol(slide));
}

// Bend Functions
/// Determines if bend data is valid
/// @param type Type to validate
/// @param bentPitch Bent pitch to validate
/// @param releasePitch Release pitch to validate
/// @param duration Duration to validate
/// @param drawStartPoint Draw start point to validate
/// @param drawEndPoint Draw end point to validate
/// @return True if the bend data is valid, false if not
bool Note::IsValidBend(wxByte type, wxByte bentPitch, wxByte releasePitch, wxByte duration, wxByte drawStartPoint, wxByte drawEndPoint)
{
    //------Last Checked------//
    // - Jan 24, 2005
    
    // 1st validate the individual pieces
    if (!IsValidBendType(type) || !IsValidBentPitch(bentPitch) || !IsValidReleasePitch(releasePitch) || !IsValidBendDuration(duration) || !IsValidDrawStartPoint(drawStartPoint) || !IsValidDrawEndPoint(drawEndPoint))
        return (false);
 
    bool returnValue = false;
                        
    if ((type == normalBend) || (type == bendAndHold))
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be zero
        // c) Duration may be any value
        // d) Draw start must be low or mid and end must be higher
        returnValue = ((bentPitch > 0) && (releasePitch == 0) && (drawStartPoint <= midPoint) && (drawEndPoint > drawStartPoint));
    }
    else if (type == bendAndRelease)
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be less than bent pitch
        // c) Duration must be zero
        // d) Draw start must be low or mid and drawEndPoint must be low or mid
        returnValue = ((bentPitch > 0) && (releasePitch < bentPitch) && (duration == 0) && (drawStartPoint <= midPoint) && (drawEndPoint <= midPoint));
    }
    else if ((type == preBend) || (type == preBendAndHold))
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be zero
        // c) Duration must be zero
        // d) Draw start must be low, and end must be higher
        returnValue = ((bentPitch > 0) && (releasePitch == 0) && (duration == 0) && (drawStartPoint == lowPoint) && (drawEndPoint > drawStartPoint));
    }
    else if (type == preBendAndRelease)
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be less than bent pitch
        // c) Duration must be zero
        // d) Draw start and end must be low
        returnValue = ((bentPitch > 0) && (releasePitch < bentPitch) && (duration == 0) && (drawStartPoint == lowPoint) && (drawEndPoint == lowPoint));
    }
    else if (type == gradualRelease)
    {
        // a) Bent pitch must be zero
        // b) Release pitch must be standard to 2 3/4
        // c) Duration must be > 0
        // d) Draw start must be high or mid point, draw end must be lower
        returnValue = ((bentPitch == 0) && (releasePitch <= 11) && (duration > 0) && (drawStartPoint >= midPoint) && (drawEndPoint < drawStartPoint));
    }
    else if (type == immediateRelease)
    {
        // a) Bent pitch must be zero
        // b) Release pitch must be zero
        // c) Duration must be zero
        // d) Draw start must be high or mid, and match draw end
        returnValue = ((bentPitch == 0) && (releasePitch == 0) && (duration == 0) && (drawStartPoint >= midPoint) && (drawStartPoint == drawEndPoint));
    }
        
    return (returnValue);
}

/// Sets (adds or updates) a bend
/// @param type Type of bend to set (see bendTypes enum for values)
/// @param bentPitch Amount of pitch to bend, in quarter steps
/// @param releasePitch Pitch to release to, in quarter steps
/// @param duration The duration of the bend (0 = default, occurs over 32nd note, 1 = occurs over duration of note, 2 = occurs over duration of note + next note, etc.)
/// @param drawStartPoint Vertical draw start point for the bend (see bendDrawingPoints enum for values)
/// @param drawEndPoint Vertical draw end point for the bend (see bendDrawingPoints enum for values)
/// @return True if the bend was added or updated
bool Note::SetBend(wxByte type, wxByte bentPitch, wxByte releasePitch, wxByte duration, wxByte drawStartPoint, wxByte drawEndPoint)
{
    //------Last Checked------//
    // - Jan 23, 2005
    wxCHECK(IsValidBend(type, bentPitch, releasePitch, duration, drawStartPoint, drawEndPoint), false);

    // Construct the symbol data, then add it to the array    
    wxUint32 symbolData = MAKELONG(0, MAKEWORD(0, bend));
    symbolData |= (wxUint32)(type << 20);
    symbolData |= (wxUint32)(drawStartPoint << 18);
    symbolData |= (wxUint32)(drawEndPoint << 16);
    symbolData |= (wxUint32)(duration << 8);
    symbolData |= (wxUint32)(bentPitch << 4);
    symbolData |= (wxUint32)(releasePitch);
   
    return (AddComplexSymbol(symbolData));
}

/// Gets the bend data (if any)
/// @param type Holds the bend type return value
/// @param bentPitch Holds the bent pitch return value
/// @param releasePitch Holds the release pitch return value
/// @param duration Holds the duration return value
/// @param drawStartPoint Holds the draw start point return value
/// @param drawEndPoint Holds the draw end point return value
/// @return True if the data was returned, false if not
bool Note::GetBend(wxByte& type, wxByte& bentPitch, wxByte& releasePitch, wxByte& duration, wxByte& drawStartPoint, wxByte& drawEndPoint) const
{
    //------Last Checked------//
    // - Jan 24, 2005
    type = 0;
    duration = 0;
    bentPitch = 0;
    releasePitch = 0;
    drawStartPoint = 0;
    drawEndPoint = 0;
        
    // Get the index of the bend
    wxUint32 index = FindComplexSymbol(bend);
    if (index == (wxUint32)-1)
        return (false);
    
    // Get the individual pieces that make up the bend
    wxUint32 symbolData = m_complexSymbolArray[index];
    
    type = (wxByte)((symbolData & bendTypeMask) >> 20);
    duration = HIBYTE(LOWORD(symbolData));
    bentPitch = (wxByte)((symbolData & bentPitchMask) >> 4);
    releasePitch = (wxByte)(symbolData & releasePitchMask);
    drawStartPoint = (wxByte)((symbolData & drawStartMask) >> 18);
    drawEndPoint = (wxByte)((symbolData & drawEndMask) >> 16);
    
    return (true);
}

/// Determines if the note has a bend
/// @return True if the note has a bend, false if not
bool Note::HasBend() const
{
    //------Last Checked------//
    // - Jan 23, 2005
    return (FindComplexSymbol(bend) != (wxUint32)-1);
}

/// Removes a bend from the note
/// @return True if the bend was removed, false if not
bool Note::ClearBend()
{
    //------Last Checked------//
    // - Jan 23, 2005
    return (RemoveComplexSymbol(bend));
}
    
// Tapped Harmonic Functions
/// Sets (adds or updates) a tapped harmonic
/// @param tappedFretNumber Tapped fret number
/// @return True if the tapped harmonic was added or updated
bool Note::SetTappedHarmonic(wxByte tappedFretNumber)
{
    //------Last Checked------//
    // - Jan 19, 2005
    wxCHECK(IsValidTappedHarmonic(tappedFretNumber), false);
    
    // Construct the symbol data, then add it to the array
    wxUint32 symbolData = MAKELONG(MAKEWORD(tappedFretNumber, 0), MAKEWORD(0, tappedHarmonic));
    return (AddComplexSymbol(symbolData));
}

/// Gets the tapped harmonic data (if any)
/// @param tappedFretNumber Holds the tapped fret number return value
/// @return True if the data was returned, false if not
bool Note::GetTappedHarmonic(wxByte& tappedFretNumber) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    tappedFretNumber = 0;
    
    // Get the index of the tapped harmonic
    wxUint32 index = FindComplexSymbol(tappedHarmonic);
    if (index == (wxUint32)-1)
        return (false);
    
    // Get the individual pieces that make up the tapped harmonic
    wxUint32 symbolData = m_complexSymbolArray[index];
	tappedFretNumber = LOBYTE(LOWORD(symbolData));
	
    return (true);
}

/// Determines if the note has a tapped harmonic
/// @return True if the note has a tapped harmonic, false if not
bool Note::HasTappedHarmonic() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (FindComplexSymbol(tappedHarmonic) != (wxUint32)-1);
}

/// Removes a tapped harmonic from the note
/// @return True if the tapped harmonic was removed, false if not
bool Note::ClearTappedHarmonic()
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (RemoveComplexSymbol(tappedHarmonic));
}

// Trill Functions
/// Sets (adds or updates) a trill
/// @param trilledFretNumber Trilled fret number
/// @return True if the trill was added or updated
bool Note::SetTrill(wxByte trilledFretNumber)
{
    //------Last Checked------//
    // - Jan 19, 2005
    wxCHECK(IsValidTrill(trilledFretNumber), false);
    
    // Construct the symbol data, then add it to the array
    wxUint32 symbolData = MAKELONG(0, MAKEWORD(trilledFretNumber, trill));
    return (AddComplexSymbol(symbolData));	
}

/// Gets the trill data (if any)
/// @param trilledFretNumber Holds the trilled fret number return value
/// @return True if the data was returned, false if not
bool Note::GetTrill(wxByte& trilledFretNumber) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    trilledFretNumber = 0;
    
    // Get the index of the trill
    wxUint32 index = FindComplexSymbol(trill);
    if (index == (wxUint32)-1)
        return (false);
    
    // Get the individual pieces that make up the trill
    wxUint32 symbolData = m_complexSymbolArray[index];
	trilledFretNumber = LOBYTE(HIWORD(symbolData));
	
    return (true);
}

/// Determines if the note has a trill
/// @return True if the note has a trill, false if not
bool Note::HasTrill() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (FindComplexSymbol(trill) != (wxUint32)-1);
}

/// Removes a trill from the note
/// @return True if the trill was removed, false if not
bool Note::ClearTrill()
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (RemoveComplexSymbol(trill));
}

// Artificial Harmonic Functions
/// Sets (adds or updates) a artificial harmonic
/// @param key Key to set (see ChordName::keys enum for values)
/// @param keyVariation Key variation to set (see ChordName::keyVariation enum for values)
/// @param octave Octave to set
/// @return True if the artificial harmonic was added or updated
bool Note::SetArtificialHarmonic(wxByte key, wxByte keyVariation, wxByte octave)
{
    //------Last Checked------//
    // - Jan 19, 2005
    wxCHECK(IsValidArtificialHarmonic(key, keyVariation, octave), false);

    // Construct the symbol data, then add it to the array
    wxUint32 symbolData = MAKELONG(MAKEWORD(key, keyVariation), MAKEWORD(octave, artificialHarmonic));
    return (AddComplexSymbol(symbolData));
}

/// Gets the artificial harmonic data (if any)
/// @param key Holds the key return value
/// @param keyVariation Holds the key variation return value
/// @param octave Holds the octave return value
/// @return True if the data was returned, false if not
bool Note::GetArtificialHarmonic(wxByte& key, wxByte& keyVariation, wxByte& octave) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    key = 0;
    keyVariation = 0;
    octave = 0;
    
    // Get the index of the artificial harmonic
    wxUint32 index = FindComplexSymbol(artificialHarmonic);
    if (index == (wxUint32)-1)
        return (false);
    
    // Get the individual pieces that make up the artificial harmonic
    wxUint32 symbolData = m_complexSymbolArray[index];
    key = LOBYTE(LOWORD(symbolData));
    keyVariation = HIBYTE(LOWORD(symbolData));
    octave = LOBYTE(HIWORD(symbolData));
	
    return (true);
}

/// Determines if the note has a artificial harmonic
/// @return True if the note has a artificial harmonic, false if not
bool Note::HasArtificialHarmonic() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (FindComplexSymbol(artificialHarmonic) != (wxUint32)-1);
}

/// Removes a artificial harmonic from the note
/// @return True if the artificial harmonic was removed, false if not
bool Note::ClearArtificialHarmonic()
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (RemoveComplexSymbol(artificialHarmonic));
}
   
// Complex Symbol Array Functions
/// Adds a complex symbol to the complex symbol array
/// @param symbolData Data that makes up the symbol
/// @return True if the symbol was added or updated, false if not
bool Note::AddComplexSymbol(wxUint32 symbolData)
{   
    //------Last Checked------//
    // - Jan 19, 2005
    
    // Get and validate the symbol type
	wxByte type = HIBYTE(HIWORD(symbolData));
	wxCHECK(IsValidComplexSymbolType(type), false);

    bool returnValue = false;
    
    // Get the index in the complex array where the symbol is stored
    wxUint32 index = FindComplexSymbol(type);

	// Found symbol in the array, update the symbol data
	if (index != (wxUint32)-1)
    {
		m_complexSymbolArray[index] = symbolData;
		returnValue = true;
    }
    // Symbol was not found in the array, find the first free array slot and insert there
	else
	{
	    wxUint32 i = 0;
		for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
		{
			if (m_complexSymbolArray[i] == notUsed)
			{
				m_complexSymbolArray[i] = symbolData;
				returnValue = true;
				break;
			}
		}
	}
	
	return (returnValue);
}

/// Gets the number of complex symbols used by the note
/// @return The number of complex symbols used by the note
size_t Note::GetComplexSymbolCount() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    size_t returnValue = 0;
    size_t i = 0;
	for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
	{
	    // Slot is not used; break out
		if (m_complexSymbolArray[i] == notUsed)
			break;
	    returnValue++;
	}
	return (returnValue);
}

/// Gets the index of a given complex symbol type in the complex symbol array
/// @param type Type of symbol to find
/// @return Index within the array where the symbol was found, or -1 if not found
wxUint32 Note::FindComplexSymbol(wxByte type) const
{
    //------Last Checked------//
    // - Jan 19, 2005
	wxUint32 returnValue = (wxUint32)-1;

    wxUint32 i = 0;
	for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
	{
	    // Found the symbol type; break out
	    if (HIBYTE(HIWORD(m_complexSymbolArray[i])) == type)
	    {
	        returnValue = i;
		    break;
	    }
	}

	return (returnValue);
}

/// Removes a complex symbol from the complex symbol array
/// @param type Type of symbol to remove
/// @return True if the symbol was removed, false if not
bool Note::RemoveComplexSymbol(wxByte type)
{
    //------Last Checked------//
    // - Jan 19, 2005
    bool returnValue = false;
    
    wxUint32 i = 0;
	for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
	{
	    if (HIBYTE(HIWORD(m_complexSymbolArray[i])) == type)
		{
			m_complexSymbolArray[i] = notUsed;
			returnValue = true;
			break;
		}
	}
	
	return (returnValue);
}

/// Clears the contents of the symbol array (sets all elements to "not used")
void Note::ClearComplexSymbolArrayContents()
{
    //------Last Checked------//
    // - Jan 19, 2005   
    wxUint32 i = 0;
    for (; i < MAX_NOTE_COMPLEX_SYMBOLS; i++)
        m_complexSymbolArray[i] = notUsed;
}
