// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file AppInfo.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "DataWriter.h"
#include "nsLib/utils/NoCopyable.h"

//---------------------------------------------------------
// IAppInfo: ��������� ���������� ��� ���� (������) � ����������
//---------------------------------------------------------
class IAppInfo : public nsNoCopyable
{
public:
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

//the function should be implemented on app side
IAppInfo*	App_GetInfo();