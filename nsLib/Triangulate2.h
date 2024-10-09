#ifndef _TRIANGULATE2_H_
#define _TRIANGULATE2_H_

#include "headers.h"
#include "comm_types.h"
#include "MathTools.h"
#include "Vec2.h"

class nsTriangulate2
{
public:
	nsTriangulate2();
	virtual ~nsTriangulate2();

	void			SetData( const nsVec2 *verts, int vertCount );
	bool			Triangulate();

	const word*		GetIndexes() { return m_idx; }
	int				GetIndexCount() { return m_idxCount; }

private:
	const nsVec2	*m_verts;
	int				m_vertCount;

	word			*m_idx;
	int				m_idxCount;
	int				m_idxSize;			//размер массива индексов

	word			*m_shape;
	int				m_shapeCount;
	int				m_shapeSize;
	
	nsVec2			*m_norm;
	int				m_normCount;
	int				m_normSize;

	word			*m_shapeNext;
	int				m_shapeNextCount;
	int				m_shapeNextSize;

private:
	void			AddPrimIndex( word idx );		//добавляет индекс в массив индексов
	void			ClearIndexes();				//очищает массив индексов, не удаляя
	void			SwapShapes();				//меняет местами m_shape и m_shapeNext
	
	void			AddNextShapeIndex( word idx );				//добавляет индекс в m_shapeNext
	void			CalcShapeNormals();						//вычисляет нормали в m_shape
	bool			CheckTriangle( int i1, int i2, int i3 );//проверяет треугольник на валидность (не попадают ли в него другие точки из m_shape)
	
};

#endif
