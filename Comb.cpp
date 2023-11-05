#include "stdafx.h"
#include "math.h"

#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"

#include "Entity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//define Rectangle class
IMPLEMENT_SERIAL(CComb, MEntity, 0)

CComb::CComb()
{
	Init();
}

CComb::CComb(const CComb& rect) : MEntity(rect)
{
	Init();
	name = rect.name;
	m_LeftTop = rect.m_LeftTop;
	m_RightBottom = rect.m_RightBottom;
	entities = rect.entities;
}

CComb::CComb(const CString& name, const Position& m_LeftTop, const Position& m_RightBottom, const std::vector<MEntity*>& entities)
{
	Init();
	this->name = name;
	this->m_LeftTop = m_LeftTop;
	this->m_RightBottom = m_RightBottom;
	this->entities = entities;
}

CComb::~CComb()
{
}

MEntity* CComb::Copy()
{
	std::vector<MEntity*> newEntities;
	for (MEntity* entity : entities) {
		newEntities.push_back(entity->Copy());
	}
	return new CComb(this->name, this->m_LeftTop, this->m_RightBottom, newEntities);
}

int CComb::GetType()
{
	return etComb;
}

CString CComb::GetName() {
	return name;
}

Position CComb::GetLeftTop() {
	return m_LeftTop;
}

Position CComb::GetRightBottom() {
	return m_RightBottom;
}

std::vector<MEntity*> CComb::GetEntities() {
	return entities;
}

void CComb::Draw(CDC* pDC, int drawMode /* = dmNormal */)
{
	for (MEntity* entity : entities) {
		entity->Draw(pDC, drawMode);
	}
}

BOOL CComb::Pick(const Position& pos, const double pick_radius)
{
	Position objPos = pos;
	BOX2D sourceBox, desBox;
	GetBox(&sourceBox); // 得到直线段的最小包围盒
	// 将最小包围盒向四周放大，得到测试包围盒
	desBox.min[0] = sourceBox.min[0] - pick_radius;
	desBox.min[1] = sourceBox.min[1] - pick_radius;
	desBox.max[0] = sourceBox.max[0] + pick_radius;
	desBox.max[1] = sourceBox.max[1] + pick_radius;
	// 判断拾取点是否在测试包围盒中，如果不是，则图元未被选中
	if (!objPos.IsInBox(desBox))
		return FALSE;

	// 如果选中了矩形的四条边之一，则认为矩形被选中
	Position left_bottom(m_LeftTop.x, m_RightBottom.y);
	Position right_top(m_RightBottom.x, m_LeftTop.y);
	MLines line1(m_LeftTop, left_bottom);
	MLines line2(left_bottom, m_RightBottom);
	MLines line3(m_RightBottom, right_top);
	MLines line4(right_top, m_LeftTop);
	if (line1.Pick(pos, pick_radius) ||
		line2.Pick(pos, pick_radius) ||
		line3.Pick(pos, pick_radius) ||
		line4.Pick(pos, pick_radius))
		return TRUE;

	return FALSE;
}

void CComb::Move(const Position& basePos, const Position& desPos, BOOL bTemp)
{
	m_LeftTop = m_LeftTop.Offset(desPos - basePos);
	m_RightBottom = m_RightBottom.Offset(desPos - basePos);
	for (int i = 0; i < entities.size(); i++) {
		MEntity* entity = entities[i];
		entity->Move(basePos, desPos, bTemp);
	}
}

void CComb::GetBox(BOX2D* pBox)
{
	pBox->min[0] = min(m_LeftTop.x, m_RightBottom.x);
	pBox->min[1] = min(m_LeftTop.y, m_RightBottom.y);
	pBox->max[0] = max(m_LeftTop.x, m_RightBottom.x);
	pBox->max[1] = max(m_LeftTop.y, m_RightBottom.y);
}

void CComb::Init()
{
	MEntity::Init();
	m_type = etComb;
	entities.clear();
}

void CComb::Serialize(CArchive& ar)
{
}
