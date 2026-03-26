/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbontabcontent.h"
#include "ui_ribbontabcontent.h"
#include "ribbonbuttongroup.h"

RibbonTabContent::RibbonTabContent(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::RibbonTabContent)
  , m_layoutMode(RibbonButtonGroup::ThreeRowMode)
  , m_panelTitleHeight(-1)
  , m_showPanelTitle(true)
  , m_panelSpacing(0)
  , m_largeIconSize(32, 32)
  , m_smallIconSize(16, 16)
{
  ui->setupUi(this);
}

RibbonTabContent::~RibbonTabContent()
{
  delete ui;
}

// ── Private helpers ───────────────────────────────────────────────────────────

RibbonButtonGroup *RibbonTabContent::findGroup(const QString &groupName) const
{
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget())
      continue;
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(li->widget());
    if (group->title().toLower() == groupName.toLower())
      return group;
  }
  return nullptr;
}

RibbonButtonGroup *RibbonTabContent::getOrCreateGroup(const QString &groupName)
{
  RibbonButtonGroup *group = findGroup(groupName);
  if (!group)
    addGroup(groupName);
  return findGroup(groupName);
}

// ── Group management ──────────────────────────────────────────────────────────

void RibbonTabContent::addGroup(const QString &groupName)
{
  RibbonButtonGroup *group = new RibbonButtonGroup;
  group->setTitle(groupName);
  group->setLayoutMode(m_layoutMode);
  group->setLargeIconSize(m_largeIconSize);
  group->setSmallIconSize(m_smallIconSize);

  if (m_panelTitleHeight >= 0)
    group->setPanelTitleHeight(m_panelTitleHeight);
  if (!m_showPanelTitle)
    group->setTitleVisible(false);

  // Propagate actionTriggered upward
  connect(group, &RibbonButtonGroup::actionTriggered,
          this,  &RibbonTabContent::actionTriggered);

  ui->ribbonHorizontalLayout->addWidget(group);
}

void RibbonTabContent::removeGroup(const QString &groupName)
{
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget())
      continue;
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(li->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ui->ribbonHorizontalLayout->removeWidget(group);
      delete group;
      break;
    }
  }
}

int RibbonTabContent::groupCount() const
{
  return ui->ribbonHorizontalLayout->count();
}

// ── Legacy QToolButton API ────────────────────────────────────────────────────

void RibbonTabContent::addButton(const QString &groupName, QToolButton *button)
{
  getOrCreateGroup(groupName)->addButton(button);
}

void RibbonTabContent::removeButton(const QString &groupName, QToolButton *button)
{
  RibbonButtonGroup *group = findGroup(groupName);
  if (!group)
    return;
  group->removeButton(button);
  if (group->buttonCount() == 0)
    removeGroup(groupName);
}

// ── Action-based API ──────────────────────────────────────────────────────────

void RibbonTabContent::addAction(const QString &groupName, QAction *action,
                                 RibbonButtonGroup::ButtonSize size)
{
  getOrCreateGroup(groupName)->addAction(action, size);
}

void RibbonTabContent::addAction(const QString &groupName, QAction *action,
                                 RibbonButtonGroup::ButtonSize size,
                                 QToolButton::ToolButtonPopupMode popupMode)
{
  getOrCreateGroup(groupName)->addAction(action, size, popupMode);
}

void RibbonTabContent::removeAction(const QString &groupName, QAction *action)
{
  RibbonButtonGroup *group = findGroup(groupName);
  if (!group)
    return;
  group->removeAction(action);
  if (group->buttonCount() == 0)
    removeGroup(groupName);
}

// ── Menu convenience ──────────────────────────────────────────────────────────

void RibbonTabContent::addMenu(const QString &groupName, QMenu *menu,
                               RibbonButtonGroup::ButtonSize size,
                               QToolButton::ToolButtonPopupMode popupMode)
{
  getOrCreateGroup(groupName)->addMenu(menu, size, popupMode);
}

// ── Widget embedding ──────────────────────────────────────────────────────────

void RibbonTabContent::addWidget(const QString &groupName, QWidget *widget,
                                 RibbonButtonGroup::ButtonSize size)
{
  getOrCreateGroup(groupName)->addWidget(widget, size);
}

void RibbonTabContent::removeWidget(const QString &groupName, QWidget *widget)
{
  RibbonButtonGroup *group = findGroup(groupName);
  if (!group)
    return;
  group->removeWidget(widget);
  if (group->buttonCount() == 0)
    removeGroup(groupName);
}

// ── Separator ─────────────────────────────────────────────────────────────────

void RibbonTabContent::addSeparator(const QString &groupName)
{
  getOrCreateGroup(groupName)->addSeparator();
}

// ── Per-group properties ───────────────────────────────────────────────────────

void RibbonTabContent::setGroupExpanding(const QString &groupName, bool expanding)
{
  RibbonButtonGroup *group = findGroup(groupName);
  if (group)
    group->setExpanding(expanding);
}

void RibbonTabContent::setGroupOptionAction(const QString &groupName, QAction *action)
{
  RibbonButtonGroup *group = findGroup(groupName);
  if (group)
    group->setOptionAction(action);
}

// ── Layout mode ───────────────────────────────────────────────────────────────

void RibbonTabContent::setLayoutMode(RibbonButtonGroup::LayoutMode mode)
{
  m_layoutMode = mode;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget()) continue;
    static_cast<RibbonButtonGroup*>(li->widget())->setLayoutMode(mode);
  }
}

RibbonButtonGroup::LayoutMode RibbonTabContent::layoutMode() const
{
  return m_layoutMode;
}

// ── Global appearance ─────────────────────────────────────────────────────────

void RibbonTabContent::setPanelTitleHeight(int height)
{
  m_panelTitleHeight = height;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget()) continue;
    static_cast<RibbonButtonGroup*>(li->widget())->setPanelTitleHeight(height);
  }
}

void RibbonTabContent::setShowPanelTitle(bool visible)
{
  m_showPanelTitle = visible;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget()) continue;
    static_cast<RibbonButtonGroup*>(li->widget())->setTitleVisible(visible);
  }
}

void RibbonTabContent::setPanelSpacing(int spacing)
{
  m_panelSpacing = spacing;
  ui->ribbonHorizontalLayout->setSpacing(spacing);
}

int RibbonTabContent::panelSpacing() const
{
  return m_panelSpacing;
}

void RibbonTabContent::setLargeIconSize(const QSize &size)
{
  m_largeIconSize = size;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget()) continue;
    static_cast<RibbonButtonGroup*>(li->widget())->setLargeIconSize(size);
  }
}

void RibbonTabContent::setSmallIconSize(const QSize &size)
{
  m_smallIconSize = size;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    QLayoutItem *li = ui->ribbonHorizontalLayout->itemAt(i);
    if (!li || !li->widget()) continue;
    static_cast<RibbonButtonGroup*>(li->widget())->setSmallIconSize(size);
  }
}
