// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file AppInfo.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_AppInfo_H_
#define	_AppInfo_H_

#include "DataWriter.h"

//---------------------------------------------------------
// IAppInfo: ��������� ���������� ��� ���� (������) � ����������
//---------------------------------------------------------
struct IAppInfo
{
    virtual bool            Init() { return true; }

	virtual const char*		GetAppName() = 0;
	
	//���������� ������ ���� � ����������������� ����� �� ���������
	//����� ���������� 0, ���� � ���� "scripts/cfg/default.cfg"
	virtual const char*		GetDefCfgPath() = 0;
	
	//������� � ������� ����� ��� �������� �������, ���� �� ����������
	//� ������������� �����, �������� ����� ������ �� ������� �����
	//���� 0, �� ������ �������� �� "GetAppName().cfg"
	virtual IDataReader*	GetCfgReader() { return 0; }
	
	//������� � ������� ������� ��� ��������� �������, ���� �� ������
	//���� ������� ��� ����� ������, �������� �� �������
	//���� 0, �� ������ ������� � "GetAppName().cfg"
	virtual IDataWriter*	GetCfgWriter() { return 0; }
};

IAppInfo*	App_GetInfo();	//������ ���� ����������� � ���������� (exe)

#endif //_AppInfo_H_