#ifndef _ChgViewCmd_h_
#define _ChgViewCmd_h_

#include "Base.h"


#ifdef __cplusplus
////////////////
///CViewPanCmd ¿‡
class CVCadDoc ;
class CVCadView ;

class CViewPanCmd : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CViewPanCmd() ;
	~CViewPanCmd() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;
///////////////////////////////////////////////////////////////
/*	
 *	CZoomRgnCmd
 */
class CZoomRgnCmd : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CZoomRgnCmd() ;
	~CZoomRgnCmd() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
#endif // __cplusplus

#endif // _ModifyCmd_h_
