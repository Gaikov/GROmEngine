#ifndef _CONFIGDLG_H_
#define _CONFIGDLG_H_


#include "Local.h"
#include "D3d_enum.h"
#include "Core/Parser.h"
#include "Core/WinDialog.h"

class nsFile;

class CConfigDlg : public nsWinDialog
{
public:
	void SetLocalDesc( const char *path, const char *postfix );
	int OnDefaultButton();
	int OnOK();
	int OnCancel();
	int OnCommand( int control, int notify );
	int OnInitDialog();
	BOOL CallBackProc( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	CConfigDlg();
	virtual ~CConfigDlg();

protected:
	void UpdateCtrlLocalText();
	void OnFullScreenCheck();
	bool UpdateView();
	void UpdateTextureFmt();
	void UpdateCheckBoxes();
	void UpdateColorBits();
	void UpdateModeList( int width, int height );
	void OnChangeDevice();
	void OnClose();
	void MessageBox( char *message, char *title );

	IDirect3D8	*m_pd3d;
	CModeArray	modes;		//��������� ��� �������� dll (������������� ������� ����� ���� �� ���������)
	uint		m_currPixelBits;

	//local desc
	script_state_t	*m_localSS;
	nsFile			*m_localFile;
};

extern	CConfigDlg dlgCfg;

#endif
