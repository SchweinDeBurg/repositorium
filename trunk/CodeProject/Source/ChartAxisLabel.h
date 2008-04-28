/*
 *
 *	ChartAxisLabel.h
 *
 *	Written by C�dric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#if !defined(AFX_CHARTAXISLABEL_H__0E5519C8_A2F4_4CED_9681_32A56B25D0C5__INCLUDED_)
#define AFX_CHARTAXISLABEL_H__0E5519C8_A2F4_4CED_9681_32A56B25D0C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartObject.h"
#include <string>


class CChartAxis;

class CChartAxisLabel : public CChartObject  
{
	friend CChartAxis;

public:
	void SetText(std::string NewText)  { m_strLabelText = NewText; }
	std::string GetText() const        { return m_strLabelText;    }

	void SetFont(int nPointSize,std::string strFaceName)
	{
		m_iFontSize = nPointSize;
		m_strFontName = strFaceName;
	}

	CChartAxisLabel(CChartCtrl* pParent, bool bHorizontal);
	virtual ~CChartAxisLabel();

private:
	void SetPosition(int LeftBorder, int TopBorder, CDC *pDC);
	void Draw(CDC* pDC);
	CSize GetSize(CDC* pDC) const;

	bool m_bIsHorizontal;      // Specifies if the axis is horizontal or not
	int  m_iFontSize;
	std::string m_strFontName;
	std::string m_strLabelText;
};

#endif // !defined(AFX_CHARTAXISLABEL_H__0E5519C8_A2F4_4CED_9681_32A56B25D0C5__INCLUDED_)
