/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONTABCONTENT_H
#define RIBBONTABCONTENT_H

#include <QWidget>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QSize>

#include "ribbonbuttongroup.h"

namespace Ui {
class RibbonTabContent;
}

/// The content area of a single ribbon tab.
///
/// Contains an ordered, horizontally-arranged list of RibbonButtonGroup
/// objects inside a scroll area.  Groups are created automatically when a
/// button, action, widget, or separator is added to a group that does not
/// yet exist.
class RibbonTabContent : public QWidget
{
  Q_OBJECT

public:
  explicit RibbonTabContent(QWidget *parent = nullptr);
  virtual ~RibbonTabContent();

  // ── Group management ──────────────────────────────────────────────────────

  /// Add a new button group with the given name.
  ///
  /// \param[in] groupName Name of the group
  void addGroup(const QString &groupName);

  /// Remove the named button group and all its contents.
  ///
  /// \param[in] groupName Name of the group
  void removeGroup(const QString &groupName);

  /// Get the number of button groups in this tab.
  ///
  /// \return The number of groups
  int groupCount() const;

  // ── Legacy QToolButton API ────────────────────────────────────────────────

  /// Add a raw QToolButton to the named group.
  /// The group is created if it does not exist.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] button    The button
  void addButton(const QString &groupName, QToolButton *button);

  /// Remove a raw QToolButton from the named group.
  /// The group is removed if it becomes empty.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] button    The button
  void removeButton(const QString &groupName, QToolButton *button);

  // ── Action-based API ─────────────────────────────────────────────────────

  /// Add a QAction as a button to the named group.
  /// The group is created if it does not exist.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  void addAction(const QString &groupName, QAction *action,
                 RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton);

  /// Add a QAction with an explicit popup mode.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  /// \param[in] popupMode Popup mode for the QToolButton
  void addAction(const QString &groupName, QAction *action,
                 RibbonButtonGroup::ButtonSize size,
                 QToolButton::ToolButtonPopupMode popupMode);

  /// Remove the button associated with the given action.
  /// The group is removed if it becomes empty.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action
  void removeAction(const QString &groupName, QAction *action);

  // ── Menu convenience ──────────────────────────────────────────────────────

  /// Add a QMenu as a button to the named group.
  ///
  /// \param[in] groupName  Name of the group
  /// \param[in] menu       The menu
  /// \param[in] size       LargeButton, MediumButton, or SmallButton
  /// \param[in] popupMode  Popup mode (default: InstantPopup)
  void addMenu(const QString &groupName, QMenu *menu,
               RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton,
               QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);

  // ── Widget embedding ──────────────────────────────────────────────────────

  /// Embed an arbitrary QWidget inside the named group.
  /// The group does NOT take ownership of the widget.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] widget    The widget
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  void addWidget(const QString &groupName, QWidget *widget,
                 RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton);

  /// Remove a previously embedded widget from the named group.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] widget    The widget
  void removeWidget(const QString &groupName, QWidget *widget);

  // ── Separator ─────────────────────────────────────────────────────────────

  /// Add a vertical separator to the named group.
  /// The group is created if it does not exist.
  ///
  /// \param[in] groupName Name of the group
  void addSeparator(const QString &groupName);

  // ── Per-group properties ───────────────────────────────────────────────────

  /// Set the named group as expanding (fills available horizontal space).
  /// Useful for groups that contain gallery or combo-box widgets.
  /// Does nothing if the group does not exist.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] expanding True to expand
  void setGroupExpanding(const QString &groupName, bool expanding = true);

  /// Set an option action on the named group's title bar (▾ button).
  /// Pass nullptr to hide the option button.
  /// Does nothing if the group does not exist.
  ///
  /// \param[in] groupName Name of the group
  /// \param[in] action    The action, or nullptr
  void setGroupOptionAction(const QString &groupName, QAction *action);

  // ── Layout mode ───────────────────────────────────────────────────────────

  /// Set the layout mode applied to all groups (existing and future).
  ///
  /// \param[in] mode ThreeRowMode or TwoRowMode
  void setLayoutMode(RibbonButtonGroup::LayoutMode mode);

  /// Get the current layout mode.
  ///
  /// \return The layout mode
  RibbonButtonGroup::LayoutMode layoutMode() const;

  // ── Global appearance ─────────────────────────────────────────────────────

  /// Set the fixed height of the title label in all groups.
  ///
  /// \param[in] height Height in pixels
  void setPanelTitleHeight(int height);

  /// Show or hide the title bar in all groups.
  ///
  /// \param[in] visible True to show
  void setShowPanelTitle(bool visible);

  /// Set the spacing (in pixels) between button groups.
  ///
  /// \param[in] spacing Spacing in pixels
  void setPanelSpacing(int spacing);

  /// Get the current spacing between button groups.
  ///
  /// \return Spacing in pixels
  int panelSpacing() const;

  /// Set the icon size for Large buttons in all groups.
  ///
  /// \param[in] size The icon size
  void setLargeIconSize(const QSize &size);

  /// Set the icon size for Small buttons in all groups.
  ///
  /// \param[in] size The icon size
  void setSmallIconSize(const QSize &size);

Q_SIGNALS:
  /// Emitted when any action in any group of this tab is triggered.
  void actionTriggered(QAction *action);

private:
  /// Find and return the named button group, or nullptr if not found.
  RibbonButtonGroup *findGroup(const QString &groupName) const;

  /// Create group if needed, then call the provided function on it.
  RibbonButtonGroup *getOrCreateGroup(const QString &groupName);

  Ui::RibbonTabContent *ui;
  RibbonButtonGroup::LayoutMode m_layoutMode;   ///< Mode applied to new and existing groups
  int   m_panelTitleHeight;  ///< -1 means use default
  bool  m_showPanelTitle;
  int   m_panelSpacing;
  QSize m_largeIconSize;
  QSize m_smallIconSize;
};

#endif // RIBBONTABCONTENT_H
