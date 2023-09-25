/*	
 *	Header File of Class CModifyCmd
 */
#ifndef _ModifyCmd_h_
#define _ModifyCmd_h_

#include "base.h"

#ifdef __cplusplus

class CVPlotDoc ;
class CVPlotView ;
///////////////////////////////////////////////////////////////
/*	
 *	CMove
 */
class CMove : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CMove() ;
	~CMove() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;
///////////////////////////////////////////////////////////////
/*	
 *	CRotate
 */
class CRotate : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CRotate() ;
	~CRotate() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;
///////////////////////////////////////////////////////////////
/*	
 *	CMirror
 */

class CMirror : public CCommand
{
private:
	Position m_basePos;
	Position m_desPos;
public:
	CMirror() ;
	~CMirror() ;
	
	int		GetType();
	int		OnLButtonDown(UINT nFlags, const Position& pos) ;
	int		OnMouseMove(UINT nFlags, const Position& pos) ;
	int		OnRButtonDown(UINT nFlags, const Position& pos) ;

	int		Cancel() ;
} ;

///////////////////////////////////////////////////////////////
#endif // __cplusplus

#endif // _ModifyCmd_h_
