/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbonbuttongroup.h"
#include "ui_ribbonbuttongroup.h"

#include <QFrame>

RibbonButtonGroup::RibbonButtonGroup(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::RibbonButtonGroup)
  , m_title("")
  , m_layoutMode(ThreeRowMode)
  , m_largeIconSize(32, 32)
  , m_smallIconSize(16, 16)
  , m_actionWidget(nullptr)
  , m_actionLayout(nullptr)
{
  ui->setupUi(this);

  // Container for all action-based content – appended after legacy buttons
  m_actionWidget = new QWidget(this);
  m_actionWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  m_actionLayout = new QHBoxLayout(m_actionWidget);
  m_actionLayout->setSpacing(0);
  m_actionLayout->setContentsMargins(0, 0, 0, 0);
  ui->horizontalLayout->addWidget(m_actionWidget);
}

RibbonButtonGroup::~RibbonButtonGroup()
{
  delete ui;
}

// ── Title ─────────────────────────────────────────────────────────────────────

void RibbonButtonGroup::setTitle(const QString &title)
{
  m_title = title;
  ui->label->setText(m_title);
}

QString RibbonButtonGroup::title() const
{
  return m_title;
}

void RibbonButtonGroup::setTitleVisible(bool visible)
{
  ui->label->setVisible(visible);
  ui->horizontalLine->setVisible(visible);
}

bool RibbonButtonGroup::isTitleVisible() const
{
  return ui->label->isVisible();
}

void RibbonButtonGroup::setPanelTitleHeight(int height)
{
  ui->label->setFixedHeight(height);
}

void RibbonButtonGroup::setOptionAction(QAction *action)
{
  ui->optionButton->setVisible(action != nullptr);
  if (action)
    ui->optionButton->setDefaultAction(action);
}

// ── Layout ────────────────────────────────────────────────────────────────────

void RibbonButtonGroup::setLayoutMode(LayoutMode mode)
{
  if (m_layoutMode == mode)
    return;
  m_layoutMode = mode;
  rebuildActionLayout();
}

RibbonButtonGroup::LayoutMode RibbonButtonGroup::layoutMode() const
{
  return m_layoutMode;
}

void RibbonButtonGroup::setExpanding(bool expanding)
{
  if (expanding)
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  else
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

bool RibbonButtonGroup::isExpanding() const
{
  return sizePolicy().horizontalPolicy() == QSizePolicy::Expanding;
}

// ── Icon sizes ────────────────────────────────────────────────────────────────

void RibbonButtonGroup::setLargeIconSize(const QSize &size)
{
  m_largeIconSize = size;
  rebuildActionLayout();
}

QSize RibbonButtonGroup::largeIconSize() const
{
  return m_largeIconSize;
}

void RibbonButtonGroup::setSmallIconSize(const QSize &size)
{
  m_smallIconSize = size;
  rebuildActionLayout();
}

QSize RibbonButtonGroup::smallIconSize() const
{
  return m_smallIconSize;
}

// ── Button count ──────────────────────────────────────────────────────────────

int RibbonButtonGroup::buttonCount() const
{
  // Legacy buttons are all items in horizontalLayout before m_actionWidget
  int legacyCount = ui->horizontalLayout->indexOf(m_actionWidget);
  if (legacyCount < 0)
    legacyCount = ui->horizontalLayout->count();

  // Action/widget items (excluding separators)
  int actionCount = 0;
  for (const PanelItem &item : m_panelItems)
  {
    if (item.type != PanelItem::SeparatorType)
      ++actionCount;
  }

  return legacyCount + actionCount;
}

// ── Legacy QToolButton API ────────────────────────────────────────────────────

void RibbonButtonGroup::addButton(QToolButton *button)
{
  button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  button->setMinimumSize(48, 48);
  button->setAutoRaise(true);
  button->setIconSize(m_largeIconSize);
  button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  // Insert before m_actionWidget so legacy buttons appear first
  int pos = ui->horizontalLayout->indexOf(m_actionWidget);
  if (pos < 0)
    ui->horizontalLayout->addWidget(button);
  else
    ui->horizontalLayout->insertWidget(pos, button);
}

void RibbonButtonGroup::removeButton(QToolButton *button)
{
  /// \todo What happens if button is not part of the layout?
  ui->horizontalLayout->removeWidget(button);
}

// ── Action-based API ──────────────────────────────────────────────────────────

void RibbonButtonGroup::addAction(QAction *action, ButtonSize size)
{
  addAction(action, size, QToolButton::DelayedPopup);
}

void RibbonButtonGroup::addAction(QAction *action, ButtonSize size,
                                  QToolButton::ToolButtonPopupMode popupMode)
{
  PanelItem item;
  item.type      = PanelItem::ActionType;
  item.action    = action;
  item.widget    = nullptr;
  item.size      = size;
  item.popupMode = popupMode;
  m_panelItems.append(item);
  rebuildActionLayout();
}

void RibbonButtonGroup::removeAction(QAction *action)
{
  for (int i = 0; i < m_panelItems.count(); ++i)
  {
    if (m_panelItems[i].type == PanelItem::ActionType &&
        m_panelItems[i].action == action)
    {
      m_panelItems.removeAt(i);
      break;
    }
  }
  rebuildActionLayout();
}

// ── Menu convenience ──────────────────────────────────────────────────────────

void RibbonButtonGroup::addMenu(QMenu *menu, ButtonSize size,
                                QToolButton::ToolButtonPopupMode popupMode)
{
  addAction(menu->menuAction(), size, popupMode);
}

// ── Widget embedding ──────────────────────────────────────────────────────────

void RibbonButtonGroup::addWidget(QWidget *widget, ButtonSize size)
{
  PanelItem item;
  item.type      = PanelItem::WidgetType;
  item.action    = nullptr;
  item.widget    = widget;
  item.size      = size;
  item.popupMode = QToolButton::DelayedPopup;
  m_panelItems.append(item);
  rebuildActionLayout();
}

void RibbonButtonGroup::removeWidget(QWidget *widget)
{
  for (int i = 0; i < m_panelItems.count(); ++i)
  {
    if (m_panelItems[i].type == PanelItem::WidgetType &&
        m_panelItems[i].widget == widget)
    {
      m_panelItems.removeAt(i);
      break;
    }
  }
  rebuildActionLayout();
}

// ── Separator ─────────────────────────────────────────────────────────────────

void RibbonButtonGroup::addSeparator()
{
  PanelItem item;
  item.type      = PanelItem::SeparatorType;
  item.action    = nullptr;
  item.widget    = nullptr;
  item.size      = LargeButton;
  item.popupMode = QToolButton::DelayedPopup;
  m_panelItems.append(item);
  rebuildActionLayout();
}

// ── Private helpers ───────────────────────────────────────────────────────────

int RibbonButtonGroup::maxRows() const
{
  return (m_layoutMode == ThreeRowMode) ? 3 : 2;
}

QToolButton *RibbonButtonGroup::createButton(QAction *action, ButtonSize size,
                                             QToolButton::ToolButtonPopupMode popupMode)
{
  QToolButton *btn = new QToolButton(m_actionWidget);
  btn->setDefaultAction(action);
  btn->setAutoRaise(true);

  // Medium icon is the geometric mean of large and small sizes
  QSize mediumIconSize(
      (m_largeIconSize.width()  + m_smallIconSize.width())  / 2,
      (m_largeIconSize.height() + m_smallIconSize.height()) / 2);

  if (size == LargeButton)
  {
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(m_largeIconSize);
    btn->setMinimumSize(48, 48);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  }
  else if (size == MediumButton)
  {
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btn->setIconSize(mediumIconSize);
    btn->setMinimumSize(0, 0);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  }
  else // SmallButton
  {
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btn->setIconSize(m_smallIconSize);
    btn->setMinimumSize(0, 0);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  }

  // Override popup mode when explicitly set or when the action has a menu
  if (action->menu() || popupMode != QToolButton::DelayedPopup)
    btn->setPopupMode(popupMode);

  // Forward triggered signal.  Use btn as context so the connection is
  // automatically removed when the button is deleted during a rebuild.
  connect(action, &QAction::triggered, btn, [this, action]() {
    emit actionTriggered(action);
  });

  return btn;
}

void RibbonButtonGroup::rebuildActionLayout()
{
  // Detach caller-owned widgets before clearing the layout so they survive
  for (const PanelItem &item : m_panelItems)
  {
    if (item.type == PanelItem::WidgetType && item.widget)
      item.widget->setParent(nullptr);
  }

  // Clear everything – deletes QToolButtons we created; external widgets
  // are already detached so they are not deleted here
  while (m_actionLayout->count() > 0)
  {
    QLayoutItem *li = m_actionLayout->takeAt(0);
    if (li->widget())
    {
      delete li->widget();
    }
    else if (li->layout())
    {
      QLayout *sub = li->layout();
      while (sub->count() > 0)
      {
        QLayoutItem *si = sub->takeAt(0);
        if (si->widget()) delete si->widget();
        delete si;
      }
      delete sub;
    }
    delete li;
  }

  // Re-populate according to the current layout mode
  enum ColumnType { None, SmallCol, MediumCol };
  ColumnType colType = None;
  QVBoxLayout *currentCol = nullptr;
  int colCount = 0;
  const int maxSmall = maxRows(); // 3 or 2

  for (const PanelItem &item : m_panelItems)
  {
    if (item.type == PanelItem::SeparatorType)
    {
      QFrame *sep = new QFrame(m_actionWidget);
      sep->setFrameShape(QFrame::VLine);
      sep->setFrameShadow(QFrame::Plain);
      sep->setStyleSheet("color: #c0c0c0;");
      sep->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
      m_actionLayout->addWidget(sep);
      colType = None; currentCol = nullptr; colCount = 0;
    }
    else if (item.type == PanelItem::ActionType)
    {
      if (item.size == LargeButton)
      {
        QToolButton *btn = createButton(item.action, LargeButton, item.popupMode);
        m_actionLayout->addWidget(btn);
        colType = None; currentCol = nullptr; colCount = 0;
      }
      else if (item.size == MediumButton && maxSmall == 3) // ThreeRowMode
      {
        if (colType != MediumCol || colCount >= 2)
        {
          currentCol = new QVBoxLayout();
          currentCol->setSpacing(0);
          currentCol->setContentsMargins(0, 0, 0, 0);
          m_actionLayout->addLayout(currentCol);
          colType = MediumCol; colCount = 0;
        }
        // First medium button takes 2/3 of column height, second takes 1/3
        int stretch = (colCount == 0) ? 2 : 1;
        QToolButton *btn = createButton(item.action, MediumButton, item.popupMode);
        currentCol->addWidget(btn, stretch);
        ++colCount;
      }
      else // SmallButton, or MediumButton in TwoRowMode
      {
        if (colType != SmallCol || colCount >= maxSmall)
        {
          currentCol = new QVBoxLayout();
          currentCol->setSpacing(0);
          currentCol->setContentsMargins(0, 0, 0, 0);
          m_actionLayout->addLayout(currentCol);
          colType = SmallCol; colCount = 0;
        }
        QToolButton *btn = createButton(item.action, SmallButton, item.popupMode);
        currentCol->addWidget(btn);
        ++colCount;
      }
    }
    else // WidgetType
    {
      item.widget->setParent(m_actionWidget);

      if (item.size == LargeButton)
      {
        item.widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        m_actionLayout->addWidget(item.widget);
        colType = None; currentCol = nullptr; colCount = 0;
      }
      else if (item.size == MediumButton && maxSmall == 3) // ThreeRowMode
      {
        if (colType != MediumCol || colCount >= 2)
        {
          currentCol = new QVBoxLayout();
          currentCol->setSpacing(0);
          currentCol->setContentsMargins(0, 0, 0, 0);
          m_actionLayout->addLayout(currentCol);
          colType = MediumCol; colCount = 0;
        }
        int stretch = (colCount == 0) ? 2 : 1;
        currentCol->addWidget(item.widget, stretch);
        ++colCount;
      }
      else // small widget, or medium in TwoRowMode
      {
        if (colType != SmallCol || colCount >= maxSmall)
        {
          currentCol = new QVBoxLayout();
          currentCol->setSpacing(0);
          currentCol->setContentsMargins(0, 0, 0, 0);
          m_actionLayout->addLayout(currentCol);
          colType = SmallCol; colCount = 0;
        }
        currentCol->addWidget(item.widget);
        ++colCount;
      }
    }
  }
}
