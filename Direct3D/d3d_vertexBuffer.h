// d3d_vertexBuffer.h : 
//

#pragma once

#include <vector>
#include <d3d9.h>

class Vertex        // 16 Byte
{ 
public:
    Vertex( float const fx, float const fy, float const fz, DWORD const dwColor) :
		m_fx     ( fx ), 
		m_fy     ( fy ), 
		m_fz     ( fz ), 
		m_dwColor( dwColor ) 
{};

private:
    float m_fx, m_fy, m_fz; 
    DWORD m_dwColor; 
};     

class VertexBuffer
{
public:
    explicit VertexBuffer( ULONG const ulSize );
    ~VertexBuffer( ) { };

    void  AddVertex( float const fx, float const fy, DWORD const dwColor) 
	{ 
		m_vertexVector.push_back( Vertex( fx, fy, 0.0f, dwColor ) ); 
	}
    
	void  ResetVertexBuffer( )                                            
	{ 
		m_vertexVector.clear(); 
	}
    
	ULONG GetNrOfVertices( ) const                                        
	{ 
		return static_cast<ULONG>( m_vertexVector.size() ); 
	}

    HRESULT LoadVertices( LPDIRECT3DVERTEXBUFFER9 const, IDirect3DDevice9 * const ) const;   // lock m_d3d_vertexBuffer and load the vertices into it

private:
    std::vector<Vertex> m_vertexVector;
};

