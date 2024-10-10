#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_

#include "Local.h"
#include "D3d_enum.h"
#include "TexManager.h"
#include "RenStateMgr.h"
#include "Engine/RenDevice.h"

class CRenderState : public IRenDevice
{
public:
	void SyncRender();
	void SetBrightness( float value );
	bool ResetDevice();
	bool ReloadResources();
	void release( bool bUserData );
	bool init();
	CRenderState();
	virtual ~CRenderState(); 

	HWND				m_hWnd;
	IDirect3D8			*m_pd3d;
	IDirect3DDevice8	*m_pdev;
	D3DDEVTYPE			m_dev_type;
	CModeArray			m_modes;

	DWORD				m_color;

	//bool					m_bActive;
	bool					m_bDevLost;
	bool					m_bFullScreen;
	D3DPRESENT_PARAMETERS	m_pp;

	bool					m_bRestart;

	D3DXMATRIX				m_mIdent;
	/*D3DXMATRIX				m_mProj, m_mView, m_mIdent, m_mInvView;
	D3DXVECTOR3				m_vCamPos, m_vCamAt, m_vCamUp;//*/

	float					m_prev_bright;

	nsTexture				*curr_tex;

	bool					caps_texgen;
	bool					caps_managed;
	//bool					m_lightEnabled;
	
	rasterConfig_t			m_currConfig;

protected:
	nsRenStateMgr			*m_rsMgr;

protected:
	void UpdateCurrConfig( D3DDISPLAYMODE &d3dmode );
	void AdjustWindow( HWND hWnd, int width, int height, bool fullscreen );
	void CheckCaps( UINT dev, D3DDEVTYPE type );
	void InvalidateResources();
	void SetRamp( float gamma, float contrast, float brightness );

public:
	virtual const char*		GetName();

	//init render
	virtual bool					ConfigDlg( uint hWnd, const char *localPath, const char *locaPostfix ); //������ ������������, ���������� ����������� ������ ����� ������� �������������
	virtual bool					Init( void *hWnd );	//�������������, ���������� ����������� ����� ������� �������������
	virtual void					Release(); //������������, ���������� ����������� ����� ������������� 
	virtual int						GetModeList( int **modes );
	virtual const rasterConfig_t*	GetCurrentConfig();

	//������� ���� ����������� ���������� (�� �����)
	virtual void			SetColor( const float c[4] ); 

	//textures
	virtual ITexture*		TextureLoad( const char *filename, bool mipmap, texfmt_t fmt );
	virtual ITexture*		TextureGenerate( int width, int height, const void *data, texfmt_t fmt, bool mipmap );
	virtual void			TextureRelease( ITexture *tex );
	virtual void			TextureBind( ITexture *tex );
	virtual void			TextureTranform( const float *offs2, const float *scale2 ); //in 
	
	virtual IRenState*		StateLoad( const char *fileName );
	virtual void			StateRelease( IRenState *state );
	virtual void			StateApply( IRenState *state );

	//scene functions
	virtual void			ClearScene( uint flags );
	virtual bool			BeginScene();
	virtual	void			EndScene();

	virtual void			LoadProjMatrix( const float m[16] );
	virtual void			LoadViewMartix( const float m[16] );

	//������ �� ���������� ��������, ���������� �� WORLD_MATRIX (OBJECT_MATRIX)
	virtual void			LoadMatrix( const float m[16] );
	virtual void			MultMatrix( const float m[16] );
	virtual void			MultMatrixLocal( const float m[16] );
	
	//========= 2d graphics ==========
	//pictures
	virtual void			DrawSprite( float x, float y, float width, float height, float tu1 = 0, float tv1 = 0, float tu2 = 1, float tv2 = 1 );
	virtual void			DrawLinedSprite( float x1, float y1, float x2, float y2, float width, float height );
	virtual void			DrawFadeScreen(); //0 - no fade, 1 - full fade
	
	virtual void			DrawChar( float x, float y, rchar_t *ch );
	virtual void			DrawCharScaled( float x, float y, rchar_t *ch, float sx, float sy );

	//========= 3d graphics ==========
	virtual IVertexBuffer*	VerticesCreate( uint vertsCount, uint indexCount, bool dynamic = false );
	virtual void			VerticesRelease( IVertexBuffer* vb );
	virtual void			VerticesDraw( IVertexBuffer* vb );

	virtual void			DrawLine( float, float, float, float, float, float );
		
	//sprites
	virtual void			DrawSprite3D( const nsMatrix &view, const nsVec3 &pos, float width, float height, float angle, float w_scale = 1 );
	virtual void			DrawChar3D( rchar_t *ch, nsVec3 &pos, float sx, float sy );
	virtual void			DrawPlaneSprite( const nsMatrix &view, const nsVec3& p1, const nsVec3& p2, float width, uint wrap_count );
	
	//������� 2-� ���������� �����, �������� ������ � 3D ������
	virtual void			Lighting( bool enable );
	virtual void			LightEnable( int source, const nsVec3 &dir, const float c[4] );
	virtual void			LightDisable( int source );
	virtual void			SetLightAmbient( const float c[4] );

};

extern	CRenderState	rs;

#endif
