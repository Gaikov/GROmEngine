#include "Triangulate2.h"
#include "Utils.h"
#include "math/TriMath.h"

nsTriangulate2::nsTriangulate2()
{
	m_verts = 0;
	m_vertCount = 0;

	m_idx = 0;
	m_idxCount = 0;
	m_idxSize = 0;

	m_shape = 0;
	m_shapeCount = 0;
	m_shapeSize = 0;

	m_shapeNext = 0;
	m_shapeNextCount = 0;
	m_shapeNextSize = 0;

	m_norm = 0;
	m_normCount = 0;
	m_normSize = 0;
}

nsTriangulate2::~nsTriangulate2()
{
	if ( m_idx )		free( m_idx );
	if ( m_shape )		free( m_shape );
	if ( m_shapeNext )	free( m_shapeNext );
	if ( m_norm )		free( m_norm );
}

//---------------------------------------------------------
// nsTriangulate2::SetData:
//---------------------------------------------------------
void nsTriangulate2::SetData( const nsVec2 *verts, int vertCount )
{
	m_verts = verts;
	m_vertCount = vertCount;
    m_shapeCount = 0;
    m_shapeNextCount = 0;
}

//---------------------------------------------------------
// nsTriangulate2::Triangulate:
//---------------------------------------------------------
bool nsTriangulate2::Triangulate()
{
	if ( !m_verts && m_vertCount <= 0 ) return false;
	
	ClearIndexes();

	//������ ��������� ������
	for ( int i = 0; i < m_vertCount; i++ )
		AddNextShapeIndex( i );

	SwapShapes();

	do
	{
		m_shapeNextCount = 0;	//������� m_shapeNext
		CalcShapeNormals();

		int	i1 = 0;
		int	i2 = 1;
		int	i3 = 2;
		bool	idxAdd = false;
		for ( ; i3 < m_shapeCount; )
		{
			nsVec2	lineVec = m_verts[m_shape[i3]] - m_verts[m_shape[i1]];
			lineVec.Norm();

			if ( lineVec.Dot( m_norm[i2] ) < 0 && CheckTriangle( i1, i2, i3 ) )
			{
				AddPrimIndex( m_shape[i1] );
				AddPrimIndex( m_shape[i2] );
				AddPrimIndex( m_shape[i3] );
                idxAdd = true;
				if ( i3 == m_shapeCount - 1 && m_shapeNextCount )
					AddNextShapeIndex( m_shape[i3] );
                i2 ++;
                i3 ++;
			}
			else if ( !m_shapeNextCount )
			{
            	if ( idxAdd )
                {
                	AddNextShapeIndex( m_idx[m_idxCount-3] );
                    AddNextShapeIndex( m_idx[m_idxCount-1] );
                    i1 = m_idx[m_idxCount-1];
                }
                else
                {
                    AddNextShapeIndex( m_shape[i1] );
                    AddNextShapeIndex( m_shape[i2] );
                    i1 = i2;
                }
				if ( i3 == m_shapeCount - 1 )
					AddNextShapeIndex( m_shape[i3] );
                i2 = i1 + 1;
                i3 = i1 + 2;
			}
			else
            {
            	/*if ( idxAdd )
                {
                    AddNextShapeIndex( m_idx[m_idxCount-1] );
                    i1 = m_idx[m_idxCount-1];
                }
                else//*/
                {
                	AddNextShapeIndex( m_shape[i2] );
                    i1 = i2;
                }
				if ( i3 == m_shapeCount - 1 )
					AddNextShapeIndex( m_shape[i3] );
                i2 = i1 + 1;
                i3 = i1 + 2;
            }
		}

		SwapShapes();
	}
	while ( m_shapeCount > 0 );
    return true;
}

//---------------------------------------------------------
// nsTriangulate2::AddIndex:
//---------------------------------------------------------
void nsTriangulate2::AddPrimIndex( word idx )
{
	if ( m_idxCount < m_idxSize )
		m_idx[ m_idxCount ] = idx;
	else
		assert("add to array");//AddToArray( &m_idx, m_idxSize, idx );
	m_idxCount ++;
}

//---------------------------------------------------------
// nsTriangulate2::ClearIndexes:
//---------------------------------------------------------
void nsTriangulate2::ClearIndexes()
{
	m_idxCount = 0;
}

//---------------------------------------------------------
// nsTriangulate2::SwapShapes:
//---------------------------------------------------------
void nsTriangulate2::SwapShapes()
{
	SwapType( m_shape, m_shapeNext );
	SwapType( m_shapeCount, m_shapeNextCount );
	SwapType( m_shapeSize, m_shapeNextSize );
}

//---------------------------------------------------------
// nsTriangulate2::AddShapeIndex:
//---------------------------------------------------------
void nsTriangulate2::AddNextShapeIndex( word idx )
{
	if ( m_shapeNextCount < m_shapeNextSize )
		m_shapeNext[ m_shapeNextCount ] = idx;
	else
		assert("add to array");//AddToArray( &m_shapeNext, m_shapeNextSize, idx );
	m_shapeNextCount ++;
}

//---------------------------------------------------------
// nsTriangulate2::CalcShapeNormals:
//---------------------------------------------------------
void nsTriangulate2::CalcShapeNormals()
{
	m_normCount = 0;
	for ( int i = 0; i < m_shapeCount; i++ )
	{
		nsVec2	p1 = m_verts[m_shape[i]];
		nsVec2	p2 = (i+1 >= m_shapeCount) ? m_verts[m_shape[0]] : m_verts[m_shape[i+1]];
		nsVec2	n;
		(p2 - p1).GetRight( n );
		n.Norm();

		if ( m_normCount < m_normSize )
			m_norm[ m_normCount ] = n;
		else
			assert("add to array");//AddToArray( &m_norm, m_normSize, n );
		m_normCount++;		
	}
}

//---------------------------------------------------------
// nsTriangulate2::CheckTriangle:
//---------------------------------------------------------
bool nsTriangulate2::CheckTriangle( int i1, int i2, int i3 )
{
	for ( int i = 0; i < m_shapeCount; i++ )
	{
		if ( i != i1 && i != i2 && i != i3 )
		{
			if ( nsTriMath::PointInTriangle(
				m_verts[m_shape[i]],
				m_verts[m_shape[i1]],
				m_verts[m_shape[i2]],
				m_verts[m_shape[i3]] ) )
				return false;
		}
	}
	return true;
}