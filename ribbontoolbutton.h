/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONTOOLBUTTON_H
#define RIBBONTOOLBUTTON_H

#include <QToolButton>

/// A tool button specifically adapted for the ribbon interface.
///
/// Supports Large and Small display modes. In Large mode, the icon is shown
/// above the text, and text may wrap to two lines. In Small mode, the icon
/// is shown beside the text.
class RibbonToolButton : public QToolButton
{
  Q_OBJECT

public:
  enum ButtonSize
  {
    Large,
    Small
  };

  explicit RibbonToolButton(QWidget *parent = nullptr);
  explicit RibbonToolButton(QAction *action, QWidget *parent = nullptr);

  /// Set the button size (Large or Small).
  void setButtonSize(ButtonSize size);

  /// Get the current button size.
  ButtonSize buttonSize() const { return m_buttonSize; }

  /// Enable or disable word wrap for Large buttons.
  void setWordWrap(bool on);

  /// Check if word wrap is enabled.
  bool wordWrap() const { return m_wordWrap; }

  // Re-implemented from QWidget
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  ButtonSize m_buttonSize;
  bool       m_wordWrap;
};

#endif // RIBBONTOOLBUTTON_H
