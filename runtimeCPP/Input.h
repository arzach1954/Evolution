//
//                                                                         
//  File:     Input.h                                                     
//                                                                        
//

#pragma once
         
#include <string>
#include <iostream>
#include <fstream>

using std::wstring;
using std::wifstream;

class InputBuffer
{
public:

    explicit InputBuffer( );
    void Open( wstring const & );
    void Close( );
    ~InputBuffer( );

    void            SetStartMarker  ( );
    void            SetReadAheadFlag( bool );
    wchar_t         ReadNextChar    ( );
    bool            IsFloat         ( ) const;
    double          ReadFloat       ( );
    unsigned long   ReadNumber      ( );

    wstring const & GetActLine      ( ) const { return m_wstrLine; };
    int             GetActLineNr    ( ) const { return m_iLineNr; };

    int             GetActStartPos  ( ) const;
    int             GetActEndPos    ( ) const;

private:
    wstring    m_wstrLine;       // buffer for script line
    int        m_iLineNr;        // actual line number  
    wchar_t  * m_pwchStart;      // pointer to start of current token 
    wchar_t  * m_pwchRead;       // pointer to next char in line 
    bool       m_fReadAhead;     // If TRUE, read ahead char has been read
    wifstream  m_ifstream;
};

