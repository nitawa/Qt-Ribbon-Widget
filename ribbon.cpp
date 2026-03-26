/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbon.h"
#include "ribbontabcontent.h"

#include <QApplication>
#include <QStyleOption>
#include <QPainter>

Ribbon::Ribbon(QWidget *parent)
  : QTabWidget(parent)
  , m_tabBar(new RibbonTabBar(this))
  , m_ribbonMode(NormalMode)
  , m_ribbonStyle(ThreeRowStyle)
  , m_tabAlignment(TabAlignLeft)
  , m_appButton(nullptr)
  , m_quickAccessBar(new QToolBar(this))
  , m_panelTitleHeight(-1)
  , m_showPanelTitle(true)
  , m_panelSpacing(0)
  , m_largeIconSize(32, 32)
  , m_smallIconSize(16, 16)
{
  // Install the custom tab bar (must be done before adding any tabs)
  setTabBar(m_tabBar);
  m_tabBar->setExpanding(false); // default: tabs are left-packed

  // Determine default colours from the application palette
  QColor bg  = qApp->palette().color(QPalette::Background);
  QColor mid = qApp->palette().color(QPalette::Mid);

  setAutoFillBackground(true);

  // Set stylesheet
  QString styleSheetText = QString(
    "QTabWidget::pane {"
    "  border-top: 1px solid rgb(%4, %5, %6);"
    "  position: absolute;"
    "  top: -1px;"
    "}"
    "QTabBar::tab {"
    "  padding-top: 5px;"
    "  padding-bottom: 5px;"
    "  padding-left: 10px;"
    "  padding-right: 10px;"
    "  margin-top: 1px;"
    "}"
    "QTabBar::tab::!selected {"
    "  border-bottom: 1px solid rgb(%4, %5, %6);"
    "  background-color:#ffffff;"
    "}"
    "QTabBar::tab:selected {"
    "  background-color: rgb(%1, %2, %3);"
    "  border-top: 1px solid rgb(%4, %5, %6);"
    "  border-right: 1px solid rgb(%4, %5, %6);"
    "  border-left: 1px solid rgb(%4, %5, %6);"
    "  border-bottom: 1px solid rgb(%1, %2, %3);"
    "}"
    "QTabBar::tab:hover {"
    "  background-color: rgb(205, 232, 255);"
    "}"
    "QTabBar::tab:selected:hover {"
    "  background-color: rgb(%1, %2, %3);"
    "}"
    ).arg(bg.red()).arg(bg.green()).arg(bg.blue())
     .arg(mid.red()).arg(mid.green()).arg(mid.blue());

  setStyleSheet(styleSheetText);

  QPalette pal = palette();
  pal.setColor(QPalette::Background, Qt::white);
  setPalette(pal);

  // Quick access bar: icon-only, small icons
  m_quickAccessBar->setIconSize(QSize(16, 16));
  m_quickAccessBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

  // Double-clicking any tab toggles minimized mode
  connect(m_tabBar, &QTabBar::tabBarDoubleClicked,
          this, [this](int) { toggleRibbonMode(); });
}

// ── Private helpers ───────────────────────────────────────────────────────────

RibbonTabContent *Ribbon::findTabContent(const QString &tabName) const
{
  for (int i = 0; i < count(); ++i)
  {
    if (tabText(i).toLower() == tabName.toLower())
      return static_cast<RibbonTabContent*>(QTabWidget::widget(i));
  }
  for (const ContextTab &ctx : m_contextTabs)
  {
    if (!ctx.visible && ctx.name.toLower() == tabName.toLower())
      return ctx.content;
  }
  return nullptr;
}

void Ribbon::applyGlobalSettings(RibbonTabContent *content) const
{
  content->setLayoutMode(m_ribbonStyle == ThreeRowStyle
                         ? RibbonButtonGroup::ThreeRowMode
                         : RibbonButtonGroup::TwoRowMode);
  content->setLargeIconSize(m_largeIconSize);
  content->setSmallIconSize(m_smallIconSize);
  content->setPanelSpacing(m_panelSpacing);
  if (m_panelTitleHeight >= 0)
    content->setPanelTitleHeight(m_panelTitleHeight);
  if (!m_showPanelTitle)
    content->setShowPanelTitle(false);
}

// ── Tab management ────────────────────────────────────────────────────────────

void Ribbon::addTab(const QString &tabName)
{
  // Note: superclass QTabWidget also has a function addTab()
  RibbonTabContent *content = new RibbonTabContent;
  applyGlobalSettings(content);
  connect(content, &RibbonTabContent::actionTriggered,
          this,    &Ribbon::actionTriggered);
  QTabWidget::addTab(content, tabName);
}

void Ribbon::addTab(const QIcon &tabIcon, const QString &tabName)
{
  // Note: superclass QTabWidget also has a function addTab()
  RibbonTabContent *content = new RibbonTabContent;
  applyGlobalSettings(content);
  connect(content, &RibbonTabContent::actionTriggered,
          this,    &Ribbon::actionTriggered);
  QTabWidget::addTab(content, tabIcon, tabName);
}

void Ribbon::removeTab(const QString &tabName)
{
  for (int i = 0; i < count(); i++)
  {
    if (tabText(i).toLower() == tabName.toLower())
    {
      QWidget *tab = QTabWidget::widget(i);
      QTabWidget::removeTab(i);
      delete tab;
      break;
    }
  }
}

// ── Group management ──────────────────────────────────────────────────────────

void Ribbon::addGroup(const QString &tabName, const QString &groupName)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addGroup(groupName);
  }
  else
  {
    addTab(tabName);
    addGroup(tabName, groupName);
  }
}

// ── Legacy QToolButton API ────────────────────────────────────────────────────

void Ribbon::addButton(const QString &tabName, const QString &groupName,
                       QToolButton *button)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addButton(groupName, button);
  }
  else
  {
    addTab(tabName);
    addButton(tabName, groupName, button);
  }
}

void Ribbon::removeButton(const QString &tabName, const QString &groupName,
                          QToolButton *button)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (!content)
    return;
  content->removeButton(groupName, button);
  if (content->groupCount() == 0)
    removeTab(tabName);
}

// ── Action-based API ──────────────────────────────────────────────────────────

void Ribbon::addAction(const QString &tabName, const QString &groupName,
                       QAction *action, RibbonButtonGroup::ButtonSize size)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addAction(groupName, action, size);
  }
  else
  {
    addTab(tabName);
    addAction(tabName, groupName, action, size);
  }
}

void Ribbon::addAction(const QString &tabName, const QString &groupName,
                       QAction *action, RibbonButtonGroup::ButtonSize size,
                       QToolButton::ToolButtonPopupMode popupMode)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addAction(groupName, action, size, popupMode);
  }
  else
  {
    addTab(tabName);
    addAction(tabName, groupName, action, size, popupMode);
  }
}

void Ribbon::removeAction(const QString &tabName, const QString &groupName,
                          QAction *action)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (!content)
    return;
  content->removeAction(groupName, action);
  // Remove tab if now empty (only for visible tabs)
  for (int i = 0; i < count(); ++i)
  {
    if (tabText(i).toLower() == tabName.toLower())
    {
      if (static_cast<RibbonTabContent*>(QTabWidget::widget(i))->groupCount() == 0)
        removeTab(tabName);
      break;
    }
  }
}

// ── Menu convenience ──────────────────────────────────────────────────────────

void Ribbon::addMenu(const QString &tabName, const QString &groupName,
                     QMenu *menu, RibbonButtonGroup::ButtonSize size,
                     QToolButton::ToolButtonPopupMode popupMode)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addMenu(groupName, menu, size, popupMode);
  }
  else
  {
    addTab(tabName);
    addMenu(tabName, groupName, menu, size, popupMode);
  }
}

// ── Widget embedding ──────────────────────────────────────────────────────────

void Ribbon::addWidget(const QString &tabName, const QString &groupName,
                       QWidget *widget, RibbonButtonGroup::ButtonSize size)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addWidget(groupName, widget, size);
  }
  else
  {
    addTab(tabName);
    addWidget(tabName, groupName, widget, size);
  }
}

void Ribbon::removeWidget(const QString &tabName, const QString &groupName,
                          QWidget *widget)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
    content->removeWidget(groupName, widget);
}

// ── Separator ─────────────────────────────────────────────────────────────────

void Ribbon::addSeparator(const QString &tabName, const QString &groupName)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
  {
    content->addSeparator(groupName);
  }
  else
  {
    addTab(tabName);
    addSeparator(tabName, groupName);
  }
}

// ── Per-group properties ───────────────────────────────────────────────────────

void Ribbon::setGroupExpanding(const QString &tabName, const QString &groupName,
                               bool expanding)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
    content->setGroupExpanding(groupName, expanding);
}

void Ribbon::setGroupOptionAction(const QString &tabName, const QString &groupName,
                                  QAction *action)
{
  RibbonTabContent *content = findTabContent(tabName);
  if (content)
    content->setGroupOptionAction(groupName, action);
}

// ── Ribbon style ──────────────────────────────────────────────────────────────

void Ribbon::setRibbonStyle(RibbonStyle style)
{
  if (m_ribbonStyle == style)
    return;
  m_ribbonStyle = style;

  RibbonButtonGroup::LayoutMode mode = (style == ThreeRowStyle)
      ? RibbonButtonGroup::ThreeRowMode
      : RibbonButtonGroup::TwoRowMode;

  for (int i = 0; i < count(); ++i)
    static_cast<RibbonTabContent*>(QTabWidget::widget(i))->setLayoutMode(mode);

  for (const ContextTab &ctx : m_contextTabs)
  {
    if (!ctx.visible)
      ctx.content->setLayoutMode(mode);
  }

  emit ribbonStyleChanged(style);
}

Ribbon::RibbonStyle Ribbon::ribbonStyle() const
{
  return m_ribbonStyle;
}

// ── Minimized mode ────────────────────────────────────────────────────────────

void Ribbon::setRibbonMode(RibbonMode mode)
{
  if (m_ribbonMode == mode)
    return;
  m_ribbonMode = mode;

  if (mode == MinimizedMode)
  {
    setFixedHeight(tabBar()->sizeHint().height() + 2);
  }
  else
  {
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);
    adjustSize();
    updateGeometry();
  }

  emit ribbonModeChanged(mode);
}

Ribbon::RibbonMode Ribbon::ribbonMode() const
{
  return m_ribbonMode;
}

void Ribbon::toggleRibbonMode()
{
  setRibbonMode(m_ribbonMode == NormalMode ? MinimizedMode : NormalMode);
}

// ── Tab alignment ─────────────────────────────────────────────────────────────

void Ribbon::setTabAlignment(TabAlignment align)
{
  m_tabAlignment = align;
  m_tabBar->setExpanding(align == TabAlignExpanding);
}

Ribbon::TabAlignment Ribbon::tabAlignment() const
{
  return m_tabAlignment;
}

// ── Application button ────────────────────────────────────────────────────────

void Ribbon::setApplicationButton(QToolButton *button)
{
  m_appButton = button;
  setCornerWidget(button, Qt::TopLeftCorner);
}

QToolButton *Ribbon::applicationButton() const
{
  return m_appButton;
}

// ── Quick access bar ──────────────────────────────────────────────────────────

QToolBar *Ribbon::quickAccessBar() const
{
  return m_quickAccessBar;
}

void Ribbon::addQuickAccessAction(QAction *action)
{
  m_quickAccessBar->addAction(action);
}

void Ribbon::removeQuickAccessAction(QAction *action)
{
  m_quickAccessBar->removeAction(action);
}

// ── Context tabs ──────────────────────────────────────────────────────────────

void Ribbon::addContextTab(const QString &tabName, const QColor &color)
{
  for (const ContextTab &ctx : m_contextTabs)
  {
    if (ctx.name.toLower() == tabName.toLower())
      return; // duplicate
  }

  RibbonTabContent *content = new RibbonTabContent(this);
  applyGlobalSettings(content);
  connect(content, &RibbonTabContent::actionTriggered,
          this,    &Ribbon::actionTriggered);
  content->hide();

  ContextTab ctx;
  ctx.name    = tabName;
  ctx.color   = color.isValid() ? color : QColor(0, 114, 188);
  ctx.content = content;
  ctx.visible = false;
  m_contextTabs.append(ctx);
}

void Ribbon::showContextTab(const QString &tabName)
{
  for (ContextTab &ctx : m_contextTabs)
  {
    if (ctx.name.toLower() != tabName.toLower() || ctx.visible)
      continue;

    int index = QTabWidget::addTab(ctx.content, ctx.name);
    m_tabBar->setTabColor(ctx.name, ctx.color);
    ctx.visible = true;
    setCurrentIndex(index);
    break;
  }
}

void Ribbon::hideContextTab(const QString &tabName)
{
  for (ContextTab &ctx : m_contextTabs)
  {
    if (ctx.name.toLower() != tabName.toLower() || !ctx.visible)
      continue;

    for (int i = 0; i < count(); ++i)
    {
      if (tabText(i).toLower() == tabName.toLower())
      {
        QTabWidget::removeTab(i);
        break;
      }
    }

    // Reparent so the content stays alive
    ctx.content->setParent(this);
    ctx.content->hide();
    m_tabBar->clearTabColor(ctx.name);
    ctx.visible = false;
    break;
  }
}

// ── Global panel appearance ───────────────────────────────────────────────────

void Ribbon::setPanelTitleHeight(int height)
{
  m_panelTitleHeight = height;
  for (int i = 0; i < count(); ++i)
    static_cast<RibbonTabContent*>(QTabWidget::widget(i))->setPanelTitleHeight(height);
  for (const ContextTab &ctx : m_contextTabs)
    ctx.content->setPanelTitleHeight(height);
}

void Ribbon::setShowPanelTitle(bool visible)
{
  m_showPanelTitle = visible;
  for (int i = 0; i < count(); ++i)
    static_cast<RibbonTabContent*>(QTabWidget::widget(i))->setShowPanelTitle(visible);
  for (const ContextTab &ctx : m_contextTabs)
    ctx.content->setShowPanelTitle(visible);
}

void Ribbon::setPanelSpacing(int spacing)
{
  m_panelSpacing = spacing;
  for (int i = 0; i < count(); ++i)
    static_cast<RibbonTabContent*>(QTabWidget::widget(i))->setPanelSpacing(spacing);
  for (const ContextTab &ctx : m_contextTabs)
    ctx.content->setPanelSpacing(spacing);
}

void Ribbon::setLargeIconSize(const QSize &size)
{
  m_largeIconSize = size;
  for (int i = 0; i < count(); ++i)
    static_cast<RibbonTabContent*>(QTabWidget::widget(i))->setLargeIconSize(size);
  for (const ContextTab &ctx : m_contextTabs)
    ctx.content->setLargeIconSize(size);
}

void Ribbon::setSmallIconSize(const QSize &size)
{
  m_smallIconSize = size;
  for (int i = 0; i < count(); ++i)
    static_cast<RibbonTabContent*>(QTabWidget::widget(i))->setSmallIconSize(size);
  for (const ContextTab &ctx : m_contextTabs)
    ctx.content->setSmallIconSize(size);
}
