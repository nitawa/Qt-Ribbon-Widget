/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONTABWIDGET_H
#define RIBBONTABWIDGET_H

#include <QTabWidget>
#include <QToolButton>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QColor>
#include <QSize>
#include <QList>

#include "ribbonbuttongroup.h"
#include "ribbontabcontent.h"
#include "ribbontabbar.h"

/// The main ribbon widget.
///
/// Wraps QTabWidget and adds:
/// - Action-based button API (addAction, addMenu, addWidget with Large / Medium / Small sizing)
/// - Ribbon style (ThreeRowStyle / TwoRowStyle)
/// - Minimized mode (collapses to tab bar; double-click any tab to toggle)
/// - Tab alignment (left / expanding)
/// - Application button (top-left corner widget)
/// - Quick access bar (independent QToolBar, caller places it)
/// - Context tabs (colored, shown / hidden on demand)
/// - Panel title height, panel spacing, and icon size control
/// - actionTriggered signal bubbled up from every group in every tab
///
/// The existing QToolButton-based API (addButton / removeButton) is preserved
/// for backward compatibility.
class Ribbon : public QTabWidget
{
  Q_OBJECT

public:
  /// Ribbon display mode.
  enum RibbonMode
  {
    NormalMode,    ///< Panels visible below tab bar (standard mode)
    MinimizedMode  ///< Only the tab bar is shown; double-click to restore
  };
  Q_ENUM(RibbonMode)

  /// Panel row layout style.
  enum RibbonStyle
  {
    ThreeRowStyle, ///< 3 small buttons per column (Office-style)
    TwoRowStyle    ///< 2 small buttons per column (compact / WPS-style)
  };
  Q_ENUM(RibbonStyle)

  /// Tab bar alignment.
  enum TabAlignment
  {
    TabAlignLeft,      ///< Tabs are only as wide as their content (left-packed)
    TabAlignExpanding  ///< Tabs expand to fill the tab bar width
  };
  Q_ENUM(TabAlignment)

  explicit Ribbon(QWidget *parent = nullptr);

  // ── Tab management ────────────────────────────────────────────────────────

  /// Add a tab to the ribbon.
  ///
  /// \param[in] tabName Name of the tab
  void addTab(const QString &tabName);

  /// Add a tab to the ribbon with an icon.
  ///
  /// \param[in] tabIcon Icon of the tab
  /// \param[in] tabName Name of the tab
  void addTab(const QIcon &tabIcon, const QString &tabName);

  /// Remove a tab from the ribbon.
  /// Does nothing if the tab does not exist.
  ///
  /// \param[in] tabName Name of the tab (case-insensitive)
  void removeTab(const QString &tabName);

  // ── Group management ──────────────────────────────────────────────────────

  /// Add a group to the named tab.
  /// The tab is created if it does not exist.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  void addGroup(const QString &tabName, const QString &groupName);

  // ── Legacy QToolButton API ────────────────────────────────────────────────

  /// Add a raw QToolButton to the named group.
  /// The group and tab are created if they do not exist.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] button    The button
  void addButton(const QString &tabName, const QString &groupName,
                 QToolButton *button);

  /// Remove a raw QToolButton from the named group.
  /// Empty groups and tabs are removed automatically.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] button    The button
  void removeButton(const QString &tabName, const QString &groupName,
                    QToolButton *button);

  // ── Action-based API ─────────────────────────────────────────────────────

  /// Add a QAction as a button to the named group.
  /// The group and tab are created if they do not exist.
  /// Also searches hidden context tabs.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  void addAction(const QString &tabName, const QString &groupName,
                 QAction *action,
                 RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton);

  /// Add a QAction with an explicit popup mode.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  /// \param[in] popupMode Popup mode for the QToolButton
  void addAction(const QString &tabName, const QString &groupName,
                 QAction *action,
                 RibbonButtonGroup::ButtonSize size,
                 QToolButton::ToolButtonPopupMode popupMode);

  /// Remove an action from the named group.
  /// Empty groups are removed automatically.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action
  void removeAction(const QString &tabName, const QString &groupName,
                    QAction *action);

  // ── Menu convenience ──────────────────────────────────────────────────────

  /// Add a QMenu as a button to the named group.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] menu      The menu
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  /// \param[in] popupMode Popup mode (default: InstantPopup)
  void addMenu(const QString &tabName, const QString &groupName,
               QMenu *menu,
               RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton,
               QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);

  // ── Widget embedding ──────────────────────────────────────────────────────

  /// Embed an arbitrary QWidget inside the named group.
  /// The group does NOT take ownership of the widget.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] widget    The widget to embed
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  void addWidget(const QString &tabName, const QString &groupName,
                 QWidget *widget,
                 RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton);

  /// Remove a previously embedded widget from the named group.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] widget    The widget
  void removeWidget(const QString &tabName, const QString &groupName,
                    QWidget *widget);

  // ── Separator ─────────────────────────────────────────────────────────────

  /// Add a vertical separator to the named group.
  /// The group and tab are created if they do not exist.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  void addSeparator(const QString &tabName, const QString &groupName);

  // ── Per-group properties ───────────────────────────────────────────────────

  /// Set the named group within the named tab as expanding horizontally.
  /// Useful for groups that contain gallery or combo-box widgets.
  /// Does nothing if the tab or group does not exist.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] expanding True to expand
  void setGroupExpanding(const QString &tabName, const QString &groupName,
                         bool expanding = true);

  /// Set an option action on the named group's title bar (▾ button).
  /// Pass nullptr to hide the option button.
  /// Does nothing if the tab or group does not exist.
  ///
  /// \param[in] tabName   Name of the tab
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action, or nullptr
  void setGroupOptionAction(const QString &tabName, const QString &groupName,
                            QAction *action);

  // ── Ribbon style ──────────────────────────────────────────────────────────

  /// Set the panel row layout style for all tabs.
  ///
  /// \param[in] style ThreeRowStyle or TwoRowStyle
  void setRibbonStyle(RibbonStyle style);

  /// Get the current ribbon style.
  ///
  /// \return The current style
  RibbonStyle ribbonStyle() const;

  // ── Minimized mode ────────────────────────────────────────────────────────

  /// Set the ribbon mode.
  /// In MinimizedMode only the tab bar is visible; double-clicking any tab
  /// toggles back to NormalMode.
  ///
  /// \param[in] mode NormalMode or MinimizedMode
  void setRibbonMode(RibbonMode mode);

  /// Get the current ribbon mode.
  ///
  /// \return The current mode
  RibbonMode ribbonMode() const;

  /// Toggle between NormalMode and MinimizedMode.
  void toggleRibbonMode();

  // ── Tab alignment ─────────────────────────────────────────────────────────

  /// Set the tab bar alignment.
  ///
  /// \param[in] align TabAlignLeft or TabAlignExpanding
  void setTabAlignment(TabAlignment align);

  /// Get the current tab bar alignment.
  ///
  /// \return The alignment
  TabAlignment tabAlignment() const;

  // ── Application button ────────────────────────────────────────────────────

  /// Set the application button shown in the top-left corner of the tab bar.
  /// Pass nullptr to remove it.
  ///
  /// \param[in] button The button, or nullptr
  void setApplicationButton(QToolButton *button);

  /// Get the current application button, or nullptr if none is set.
  ///
  /// \return The application button
  QToolButton *applicationButton() const;

  // ── Quick access bar ──────────────────────────────────────────────────────

  /// Get the quick access toolbar.
  /// Place this toolbar anywhere in your layout.  It is not automatically
  /// positioned by the Ribbon.
  ///
  /// \return The quick access toolbar
  QToolBar *quickAccessBar() const;

  /// Add an action to the quick access toolbar.
  ///
  /// \param[in] action The action
  void addQuickAccessAction(QAction *action);

  /// Remove an action from the quick access toolbar.
  ///
  /// \param[in] action The action
  void removeQuickAccessAction(QAction *action);

  // ── Context tabs ──────────────────────────────────────────────────────────

  /// Add a context tab (initially hidden).
  /// Use showContextTab() / hideContextTab() to control visibility.
  /// Buttons and actions can be added while the tab is hidden.
  ///
  /// \param[in] tabName Name of the context tab
  /// \param[in] color   Color strip drawn above the tab (invalid = blue)
  void addContextTab(const QString &tabName,
                     const QColor &color = QColor());

  /// Show a previously added context tab (appended after normal tabs).
  ///
  /// \param[in] tabName Name of the context tab (case-insensitive)
  void showContextTab(const QString &tabName);

  /// Hide a visible context tab (content is preserved).
  ///
  /// \param[in] tabName Name of the context tab (case-insensitive)
  void hideContextTab(const QString &tabName);

  // ── Global panel appearance ───────────────────────────────────────────────

  /// Set the fixed height of the title label in all groups of all tabs.
  ///
  /// \param[in] height Height in pixels
  void setPanelTitleHeight(int height);

  /// Show or hide the title bar in all groups of all tabs.
  ///
  /// \param[in] visible True to show
  void setShowPanelTitle(bool visible);

  /// Set the spacing between button groups in all tabs.
  ///
  /// \param[in] spacing Spacing in pixels
  void setPanelSpacing(int spacing);

  /// Set the icon size for Large buttons in all groups of all tabs.
  ///
  /// \param[in] size The icon size
  void setLargeIconSize(const QSize &size);

  /// Set the icon size for Small buttons in all groups of all tabs.
  ///
  /// \param[in] size The icon size
  void setSmallIconSize(const QSize &size);

Q_SIGNALS:
  /// Emitted when the ribbon mode changes.
  void ribbonModeChanged(Ribbon::RibbonMode mode);

  /// Emitted when the ribbon style changes.
  void ribbonStyleChanged(Ribbon::RibbonStyle style);

  /// Emitted when any action in any tab or group is triggered.
  void actionTriggered(QAction *action);

private:
  struct ContextTab
  {
    QString           name;
    QColor            color;
    RibbonTabContent *content; ///< Kept alive even when the tab is hidden
    bool              visible;
  };

  /// Return the RibbonTabContent for the named tab, searching visible tabs
  /// first, then hidden context tabs.  Returns nullptr if not found.
  RibbonTabContent *findTabContent(const QString &tabName) const;

  /// Apply all current global settings to a newly created RibbonTabContent.
  void applyGlobalSettings(RibbonTabContent *content) const;

  RibbonTabBar  *m_tabBar;           ///< Custom tab bar (owned by QTabWidget)
  RibbonMode     m_ribbonMode;       ///< Current display mode
  RibbonStyle    m_ribbonStyle;      ///< Current layout style
  TabAlignment   m_tabAlignment;     ///< Current tab alignment
  QToolButton   *m_appButton;        ///< Application button (corner widget)
  QToolBar      *m_quickAccessBar;   ///< Quick access toolbar
  QList<ContextTab> m_contextTabs;   ///< Registered context tabs

  // Global settings applied to all tabs / groups
  int   m_panelTitleHeight;  ///< -1 = use default
  bool  m_showPanelTitle;
  int   m_panelSpacing;
  QSize m_largeIconSize;
  QSize m_smallIconSize;
};

#endif // RIBBONTABWIDGET_H
