/////////////////////////////////////////////////////////////////////////////
// Name:            guitar.cpp
// Purpose:         Stores information about a guitar (tuning, effects, etc.)
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 8, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "guitar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Note: All MIDI constants and functions are defined in generalmidi.h
// Default constants
const wxByte         Guitar::DEFAULT_NUMBER                 = 0;
const wxChar*        Guitar::DEFAULT_DESCRIPTION            = wxT("Untitled");
const wxByte         Guitar::DEFAULT_PRESET                 = MIDI_PRESET_ACOUSTIC_GUITAR_STEEL;
const wxByte         Guitar::DEFAULT_INITIAL_VOLUME         = 104;
const wxByte         Guitar::DEFAULT_PAN                    = PAN_CENTER;
const wxByte         Guitar::DEFAULT_REVERB                 = MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const wxByte         Guitar::DEFAULT_CHORUS                 = MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const wxByte         Guitar::DEFAULT_TREMOLO                = MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const wxByte         Guitar::DEFAULT_PHASER                 = MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const wxByte         Guitar::DEFAULT_CAPO                   = 0;

const wxByte         Guitar::MIN_PRESET                     = FIRST_MIDI_PRESET;
const wxByte         Guitar::MAX_PRESET                     = LAST_MIDI_PRESET;

const wxByte         Guitar::MIN_INITIAL_VOLUME             = MIN_MIDI_CHANNEL_VOLUME;
const wxByte         Guitar::MAX_INITIAL_VOLUME             = MAX_MIDI_CHANNEL_VOLUME;
    
const wxByte         Guitar::MIN_PAN                        = MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const wxByte         Guitar::MAX_PAN                        = MAX_MIDI_CHANNEL_EFFECT_LEVEL;
const wxByte         Guitar::PAN_HARD_LEFT                  = MIN_PAN;
const wxByte         Guitar::PAN_CENTER                     = 64;
const wxByte         Guitar::PAN_HARD_RIGHT                 = MAX_PAN;

const wxByte         Guitar::MIN_REVERB                     = MIN_MIDI_CHANNEL_EFFECT_LEVEL;             
const wxByte         Guitar::MAX_REVERB                     = MAX_MIDI_CHANNEL_EFFECT_LEVEL;             

const wxByte         Guitar::MIN_CHORUS                     = MIN_MIDI_CHANNEL_EFFECT_LEVEL;             
const wxByte         Guitar::MAX_CHORUS                     = MAX_MIDI_CHANNEL_EFFECT_LEVEL;             

const wxByte         Guitar::MIN_TREMOLO                    = MIN_MIDI_CHANNEL_EFFECT_LEVEL;            
const wxByte         Guitar::MAX_TREMOLO                    = MAX_MIDI_CHANNEL_EFFECT_LEVEL;            

const wxByte         Guitar::MIN_PHASER                     = MIN_MIDI_CHANNEL_EFFECT_LEVEL;            
const wxByte         Guitar::MAX_PHASER                     = MAX_MIDI_CHANNEL_EFFECT_LEVEL;            

const wxByte         Guitar::MIN_CAPO                       = 0;
const wxByte         Guitar::MAX_CAPO                       = 12;

// Constructors/Destructors
/// Default Constructor
Guitar::Guitar() :
    m_number(DEFAULT_NUMBER), m_description(DEFAULT_DESCRIPTION), m_preset(DEFAULT_PRESET), m_initialVolume(DEFAULT_INITIAL_VOLUME),
    m_pan(DEFAULT_PAN), m_reverb(DEFAULT_REVERB), m_chorus(DEFAULT_CHORUS), m_tremolo(DEFAULT_TREMOLO), m_phaser(DEFAULT_PHASER), m_capo(DEFAULT_CAPO)
{
    //------Last Checked------//
    // - Dec 8, 2004
}

/// Primary Constructor
/// @param number Unique number identifing the guitar (zero based)
/// @param description Description for the guitar
/// @param preset MIDI preset
/// @param initialVolume The initial volume level of the guitar
/// @param pan MIDI pan level
/// @param reverb MIDI reverb level
/// @param chorus MIDI chorus level
/// @param tremolo MIDI tremolo level
/// @param phaser MIDI phaser level
/// @param capo Capo fret number (0 = no capo)
Guitar::Guitar(wxByte number, const wxChar* description, wxByte preset, wxByte initialVolume, wxByte pan, wxByte reverb, wxByte chorus, wxByte tremolo, wxByte phaser, wxByte capo) :
    m_number(number), m_description(description), m_preset(preset), m_initialVolume(initialVolume),
    m_pan(pan), m_reverb(reverb), m_chorus(chorus), m_tremolo(tremolo), m_phaser(phaser), m_capo(capo)
{
    //------Last Checked------//
    // - Dec 8, 2004
    wxASSERT(description != NULL);
    wxASSERT(wxIsValidMidiPreset(preset));
    wxASSERT(wxIsValidMidiChannelVolume(initialVolume));
    wxASSERT(IsValidPan(pan));
    wxASSERT(IsValidReverb(reverb));
    wxASSERT(IsValidChorus(chorus));
    wxASSERT(IsValidTremolo(tremolo));
    wxASSERT(IsValidPhaser(phaser));
    wxASSERT(IsValidCapo(capo));    
}

/// Copy Constructor
Guitar::Guitar (const Guitar& guitar) :
    m_number(DEFAULT_NUMBER), m_description(DEFAULT_DESCRIPTION), m_preset(DEFAULT_PRESET), m_initialVolume(DEFAULT_INITIAL_VOLUME),
    m_pan(DEFAULT_PAN), m_reverb(DEFAULT_REVERB), m_chorus(DEFAULT_CHORUS), m_tremolo(DEFAULT_TREMOLO), m_phaser(DEFAULT_PHASER), m_capo(DEFAULT_CAPO)
    
{
    //------Last Checked------//
    // - Dec 8, 2004
    *this = guitar;
}

/// Destructor
Guitar::~Guitar()
{
    //------Last Verified------//
    // - Nov 27, 2004
}

// Operators
/// Assignment operator
const Guitar& Guitar::operator=(const Guitar& guitar)
{
    //------Last Checked------//
    // - Dec 9, 2004
    
    // Check for assignment to self
    if (this != &guitar)
    {
        m_number = guitar.m_number;
        m_description = guitar.m_description;
        m_preset = guitar.m_preset;
        m_initialVolume = guitar.m_initialVolume;
        m_tuning = guitar.m_tuning;
        m_pan = guitar.m_pan;
        m_reverb = guitar.m_reverb;
        m_chorus = guitar.m_chorus;
        m_tremolo = guitar.m_tremolo;
        m_phaser = guitar.m_phaser;
        m_capo = guitar.m_capo;
    }
    return (*this);
}

/// Equality Operator
bool Guitar::operator==(const Guitar& guitar) const
{
    //------Last Checked------//
    // - Dec 8, 2004
    return (
        (m_number == guitar.m_number) &&
        (m_description == guitar.m_description) &&
        (m_preset == guitar.m_preset) &&
        (m_initialVolume == guitar.m_initialVolume) &&
        (m_tuning == guitar.m_tuning) &&
        (m_pan == guitar.m_pan) &&
        (m_reverb == guitar.m_reverb) &&
        (m_chorus == guitar.m_chorus) &&
        (m_tremolo == guitar.m_tremolo) &&
        (m_phaser == guitar.m_phaser) &&
        (m_capo == guitar.m_capo)
    );
}

/// Inequality Operator
bool Guitar::operator!=(const Guitar& guitar) const
{
    //------Last Checked------//
    // - Dec 8, 2004
    return (!operator==(guitar));
}
    
// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Guitar::DoSerialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Dec 8, 2004
    stream << m_number;
    wxCHECK(stream.CheckState(), false);
    
    stream.WriteMFCString(m_description);
    wxCHECK(stream.CheckState(), false);
    
    stream << m_preset << m_initialVolume << m_pan << m_reverb << m_chorus << m_tremolo << m_phaser << m_capo;
    wxCHECK(stream.CheckState(), false);
    
    m_tuning.Serialize(stream);
    wxCHECK(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Guitar::DoDeserialize(PowerTabInputStream& stream, wxWord version)
{
    //------Last Checked------//
    // - Dec 8, 2004
    stream >> m_number;
    wxCHECK(stream.CheckState(), false);
    
    stream.ReadMFCString(m_description);
    wxCHECK(stream.CheckState(), false);
    
    stream >> m_preset >> m_initialVolume >> m_pan >> m_reverb >> m_chorus >> m_tremolo >> m_phaser >> m_capo;
    wxCHECK(stream.CheckState(), false);
    
    m_tuning.Deserialize(stream, version);
    wxCHECK(stream.CheckState(), false);
    
    return (stream.CheckState());
}

// Operations
/// Gets the legend text for the guitar (i.e. Gtr. I - Acoustic (E A D G B E))
wxString Guitar::GetLegendText() const
{
    //------Last Checked------//
    // - Dec 8, 2004   
    wxString returnValue = wxString::Format(wxT("Gtr. %s - %s - %s"), wxArabicToRoman(GetNumber() + 1).c_str(), GetDescription().c_str(), m_tuning.GetSpelling().c_str());
    return (returnValue);
}
