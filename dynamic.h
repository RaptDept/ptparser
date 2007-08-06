/////////////////////////////////////////////////////////////////////////////
// Name:            dynamic.h
// Purpose:         Stores and renders a dynamic
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 13, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNAMIC_H__
#define __DYNAMIC_H__

/// Stores and renders a dynamic
class Dynamic : public PowerTabObject
{
friend class DynamicTestSuite;

// Constants
public:
    // Default constants
    static const wxWord         DEFAULT_SYSTEM;                     ///< Default value for the system member variable
    static const wxByte         DEFAULT_STAFF;                      ///< Default value for the staff member variable
    static const wxByte         DEFAULT_POSITION;                   ///< Default value for the position member variable
    static const wxWord         DEFAULT_DATA;                       ///< Default value for the data member variable
    static const wxByte         DEFAULT_STAFF_VOLUME;               ///< Default value for the staff volume
    static const wxByte         DEFAULT_RHYTHM_SLASH_VOLUME;        ///< Default value for the dynamic volume
    
    // System Constants
    static const wxUint32       MIN_SYSTEM;                         ///< Minimum allowed value for the system member variable
    static const wxUint32       MAX_SYSTEM;                         ///< Maximum allowed value for the system member variable

    // Staff Constants
    static const wxUint32       MIN_STAFF;                          ///< Minimum allowed value for the staff member variable
    static const wxUint32       MAX_STAFF;                          ///< Maximum allowed value for the staff member variable

    // Position Constants
    static const wxUint32       MIN_POSITION;                       ///< Minimum allowed value for the position member variable
    static const wxUint32       MAX_POSITION;                       ///< Maximum allowed value for the position member variable

    // Volume constants
    enum volumeLevels
    {
        notSet  = (wxByte)0xff,
        fff     = (wxByte)104,
        ff      = (wxByte)91,
        f       = (wxByte)78,
        mf      = (wxByte)65,
        mp      = (wxByte)52,
        p       = (wxByte)39,
        pp      = (wxByte)26,
        ppp     = (wxByte)13,
        off     = (wxByte)0
    };

// Member Variables
protected:
    wxWord      m_system;       ///< Zero-based index of the system where the dynamic is anchored
    wxByte      m_staff;        ///< Zero-based index of the staff within the system where the dynamic is anchored
    wxByte      m_position;     ///< Zero-based index of the position within the system where the dynamic is anchored
    wxWord      m_data;         ///< Volume level (see volumeLevels enum for values; top byte = staff volume, bottom byte = dynamic volume)

// Constructors/Destructors
public:
    Dynamic();
    Dynamic(wxUint32 system, wxUint32 staff, wxUint32 position,
        wxByte staffVolume, wxByte rhythmSlashVolume);
    Dynamic(const Dynamic& dynamic);
    ~Dynamic();
    
// Creation Functions
    /// Creates an exact duplicate of the object
    /// @return The duplicate object
    PowerTabObject* CloneObject() const                         
        {return (new Dynamic(*this));}
    
// Operators
    const Dynamic& operator=(const Dynamic& dynamic);
    bool operator==(const Dynamic& dynamic) const;
    bool operator!=(const Dynamic& dynamic) const;
    
// Serialization functions
protected:
    bool DoSerialize(PowerTabOutputStream& stream);
    bool DoDeserialize(PowerTabInputStream& stream, wxWord version);

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    wxString GetMFCClassName() const                            
        {return (wxT("CDynamic"));}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    wxWord GetMFCClassSchema() const                            
        {return ((wxWord)1);}
    
// System Functions
    /// Determines whether a system is valid
    /// @param system System to validate
    /// @return True if the system is valid, false if not
    static bool IsValidSystem(wxUint32 system)
        {return ((system >= MIN_SYSTEM) && (system <= MAX_SYSTEM));}
    /// Sets the system within the system where the dynamic is anchored
    /// @param system Zero-based index of the system where the dynamic is
    /// anchored
    /// @return True if the system was set, false if not
    bool SetSystem(wxUint32 system)
    {
        wxCHECK(IsValidSystem(system), false);
        m_system = (wxWord)system;
        return (true);
    }
    /// Gets the system within the system where the dynamic is anchored
    /// @return The system within the system where the dynamic is anchored
    wxUint32 GetSystem() const                           
        {return (m_system);}
        
// Staff Functions
    /// Determines whether a staff is valid
    /// @param staff Staff to validate
    /// @return True if the staff is valid, false if not
    static bool IsValidStaff(wxUint32 staff)
        {return ((staff >= MIN_STAFF) && (staff <= MAX_STAFF));}
    /// Sets the staff within the system where the dynamic is anchored
    /// @param staff Zero-based index of the staff where the dynamic is anchored
    /// @return True if the staff was set, false if not
    bool SetStaff(wxUint32 staff)
    {
        wxCHECK(IsValidStaff(staff), false);
        m_staff = (wxByte)staff;
        return (true);
    }
    /// Gets the staff within the system where the dynamic is anchored
    /// @return The staff within the system where the dynamic is anchored
    wxUint32 GetStaff() const                           
        {return (m_staff);}
        
// Position Functions
    /// Determines whether a position is valid
    /// @param position Position to validate
    /// @return True if the position is valid, false if not
    static bool IsValidPosition(wxUint32 position)
        {return ((position >= MIN_POSITION) && (position <= MAX_POSITION));}
    /// Sets the position within the system where the dynamic is anchored
    /// @param position Zero-based index of the position within the system where
    /// the dynamic is anchored
    /// @return True if the position was set, false if not
    bool SetPosition(wxUint32 position)
    {
        wxCHECK(IsValidPosition(position), false);
        m_position = (wxByte)position;
        return (true);
    }
    /// Gets the position within the system where the dynamic is anchored
    /// @return The position within the system where the dynamic is anchored
    wxUint32 GetPosition() const                           
        {return (m_position);}
    
// Volume Functions
    /// Determines if a volume is valid
    /// @param volume Volume to validate
    /// @return True if the volume is valid, false if not
    static bool IsValidVolume(wxByte volume)
    {
        return ((volume == notSet) || (volume == fff) || (volume == ff) ||
            (volume == f) || (volume == mf) || (volume == mp) ||
            (volume == p) || (volume == pp) || (volume == ppp) ||
            (volume == off));
    }
// Staff Volume Functions
    /// Sets the staff volume
    /// @param volume Volume to set
    /// @return True if the volume was set, false if not
    bool SetStaffVolume(wxByte volume)
        {return (SetVolume(false, volume));}
    /// Gets the staff volume
    /// @return The staff volume
    wxByte GetStaffVolume() const
        {return (GetVolume(false));}
    /// Determines if the staff volume is set
    /// @return True if the staff volume is set, false if not
    bool IsStaffVolumeSet() const
        {return (IsVolumeSet(false));}

// Rhythm Slash Functions
    bool SetRhythmSlashVolume(wxByte volume)
        {return (SetVolume(true, volume));}
    /// Gets the dynamic volume
    /// @return The dynamic volume
    wxByte GetRhythmSlashVolume() const
        {return (GetVolume(true));}
    /// Determines if the dynamic volume is set
    /// @return True if the dynamic volume is set, false if not
    bool IsRhythmSlashVolumeSet() const
        {return (IsVolumeSet(true));}
protected:
    bool SetVolume(bool rhythmSlashes, wxByte volume);
    wxByte GetVolume(bool rhythmSlashes) const;
    bool IsVolumeSet(bool rhythmSlashes) const;

// Operations
public:
    wxString GetText(bool rhythmSlashes) const;
};

WX_DEFINE_POWERTABARRAY(Dynamic*, DynamicArray);

#endif
