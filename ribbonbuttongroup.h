/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONBUTTONGROUP_H
#define RIBBONBUTTONGROUP_H

#include <QWidget>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>
#include <QSize>

namespace Ui {
class RibbonButtonGroup;
}

/// A group of buttons inside a ribbon tab, displayed with a title label at the
/// bottom and a vertical separator on the right side.
///
/// Buttons can be added as raw QToolButton objects (legacy API), as QAction
/// objects with a specified size (Large, Medium or Small), as QMenu objects,
/// or as arbitrary QWidget objects.
///
/// When using the action-based API the group manages the layout automatically
/// according to the current LayoutMode:
///   - LargeButton : occupies the full column height
///   - MediumButton: in ThreeRowMode, two Mediums in one column take 2/3 + 1/3
///                   of the column height; a lone Medium fills the column.
///                   In TwoRowMode Medium is treated like SmallButton.
///   - SmallButton : stacked vertically, up to maxRows() per column
class RibbonButtonGroup : public QWidget
{
  Q_OBJECT

public:
  /// Size of a button inside the group.
  enum ButtonSize
  {
    LargeButton,  ///< Full-height: icon above text (ToolButtonTextUnderIcon)
    MediumButton, ///< Two-thirds height in ThreeRowMode; icon beside text
    SmallButton   ///< Row-height: icon beside text (ToolButtonTextBesideIcon)
  };

  /// Panel layout mode.
  enum LayoutMode
  {
    ThreeRowMode, ///< 3 small buttons per column (Office-style)
    TwoRowMode    ///< 2 small buttons per column (compact style)
  };

  explicit RibbonButtonGroup(QWidget *parent = nullptr);
  virtual ~RibbonButtonGroup();

  // ── Title ─────────────────────────────────────────────────────────────────

  /// Set the title shown below the buttons.
  ///
  /// \param[in] title The title text
  void setTitle(const QString &title);

  /// Get the title of the button group.
  ///
  /// \return The title text
  QString title() const;

  /// Show or hide the title bar (separator line + title label).
  ///
  /// \param[in] visible True to show
  void setTitleVisible(bool visible);

  /// Check whether the title bar is visible.
  ///
  /// \return True if visible
  bool isTitleVisible() const;

  /// Set the fixed height of the title label in pixels.
  ///
  /// \param[in] height Height in pixels
  void setPanelTitleHeight(int height);

  /// Set an optional action triggered by the option button (▾) in the title bar.
  /// Pass nullptr to hide the option button.
  ///
  /// \param[in] action The action, or nullptr
  void setOptionAction(QAction *action);

  // ── Layout ────────────────────────────────────────────────────────────────

  /// Set the layout mode used for action-based buttons.
  /// Rebuilds the layout immediately.
  ///
  /// \param[in] mode ThreeRowMode or TwoRowMode
  void setLayoutMode(LayoutMode mode);

  /// Get the current layout mode.
  ///
  /// \return The layout mode
  LayoutMode layoutMode() const;

  /// Expand this group horizontally to fill available space.
  /// Useful when the group contains gallery or combo-box widgets.
  ///
  /// \param[in] expanding True to expand
  void setExpanding(bool expanding = true);

  /// Check whether the group expands horizontally.
  ///
  /// \return True if expanding
  bool isExpanding() const;

  // ── Icon sizes ────────────────────────────────────────────────────────────

  /// Set the icon size for Large buttons (default 32×32).
  ///
  /// \param[in] size The icon size
  void setLargeIconSize(const QSize &size);

  /// Get the icon size for Large buttons.
  ///
  /// \return The large icon size
  QSize largeIconSize() const;

  /// Set the icon size for Small (and Medium) buttons (default 16×16).
  ///
  /// \param[in] size The icon size
  void setSmallIconSize(const QSize &size);

  /// Get the icon size for Small buttons.
  ///
  /// \return The small icon size
  QSize smallIconSize() const;

  // ── Button count ──────────────────────────────────────────────────────────

  /// Get the total number of non-separator items (legacy + action-based).
  ///
  /// \return The button / widget count
  int buttonCount() const;

  // ── Legacy QToolButton API ────────────────────────────────────────────────

  /// Add a raw QToolButton (legacy API).
  /// The button is styled as a large button (48×48, icon above text).
  ///
  /// \param[in] button The button
  void addButton(QToolButton *button);

  /// Remove a raw QToolButton (legacy API).
  ///
  /// \param[in] button The button
  void removeButton(QToolButton *button);

  // ── Action-based API ─────────────────────────────────────────────────────

  /// Add a QAction as a button.
  ///
  /// \param[in] action    The action
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  void addAction(QAction *action, ButtonSize size = LargeButton);

  /// Add a QAction as a button with an explicit popup mode.
  ///
  /// \param[in] action    The action (may carry a QMenu)
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  /// \param[in] popupMode Popup mode for the QToolButton
  void addAction(QAction *action, ButtonSize size,
                 QToolButton::ToolButtonPopupMode popupMode);

  /// Remove the button associated with the given action.
  /// Does nothing if the action was not added to this group.
  ///
  /// \param[in] action The action
  void removeAction(QAction *action);

  // ── Menu convenience ──────────────────────────────────────────────────────

  /// Add a QMenu as a button.  Uses menu->menuAction() internally.
  ///
  /// \param[in] menu      The menu
  /// \param[in] size      LargeButton, MediumButton, or SmallButton
  /// \param[in] popupMode Popup mode (default: InstantPopup)
  void addMenu(QMenu *menu, ButtonSize size = LargeButton,
               QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);

  // ── Widget embedding ──────────────────────────────────────────────────────

  /// Embed an arbitrary QWidget inside the group.
  /// The group does NOT take ownership of the widget.
  ///
  /// \param[in] widget The widget to embed
  /// \param[in] size   LargeButton (full height), MediumButton, or SmallButton
  void addWidget(QWidget *widget, ButtonSize size = LargeButton);

  /// Remove a previously embedded widget.
  /// Does nothing if the widget was not added to this group.
  ///
  /// \param[in] widget The widget to remove
  void removeWidget(QWidget *widget);

  // ── Separator ─────────────────────────────────────────────────────────────

  /// Add a vertical separator line between columns of buttons.
  void addSeparator();

Q_SIGNALS:
  /// Emitted when any action managed by this group is triggered.
  void actionTriggered(QAction *action);

private:
  struct PanelItem
  {
    enum Type { ActionType, WidgetType, SeparatorType };
    Type       type;
    QAction   *action;    ///< ActionType: the action
    QWidget   *widget;    ///< WidgetType: caller-owned external widget
    ButtonSize size;
    QToolButton::ToolButtonPopupMode popupMode;
  };

  /// Create a styled QToolButton for the given action, size, and popup mode.
  QToolButton *createButton(QAction *action, ButtonSize size,
                            QToolButton::ToolButtonPopupMode popupMode
                              = QToolButton::DelayedPopup);

  /// Rebuild the entire action-based layout from m_panelItems.
  void rebuildActionLayout();

  /// Return the maximum number of small buttons per column.
  int maxRows() const;

  Ui::RibbonButtonGroup *ui;
  QString      m_title;          ///< Title of the button group
  LayoutMode   m_layoutMode;     ///< Current layout mode
  QSize        m_largeIconSize;  ///< Icon size for large buttons
  QSize        m_smallIconSize;  ///< Icon size for small / medium buttons
  QList<PanelItem> m_panelItems; ///< Ordered list of all action/widget/separator items
  QWidget     *m_actionWidget;   ///< Container for action-based content
  QHBoxLayout *m_actionLayout;   ///< Layout inside m_actionWidget
};

#endif // RIBBONBUTTONGROUP_H
