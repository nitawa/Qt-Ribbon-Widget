/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbontoolbutton.h"
#include <QStyleOptionToolButton>
#include <QPainter>
#include <QTextOption>

RibbonToolButton::RibbonToolButton(QWidget *parent)
  : QToolButton(parent)
  , m_buttonSize(Large)
  , m_wordWrap(true)
{
  setButtonSize(Large);
}

RibbonToolButton::RibbonToolButton(QAction *action, QWidget *parent)
  : QToolButton(parent)
  , m_buttonSize(Large)
  , m_wordWrap(true)
{
  setDefaultAction(action);
  setButtonSize(Large);
}

void RibbonToolButton::setButtonSize(ButtonSize size)
{
  m_buttonSize = size;
  if (size == Large)
  {
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  }
  else
  {
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  }
  updateGeometry();
}

void RibbonToolButton::setWordWrap(bool on)
{
  m_wordWrap = on;
  updateGeometry();
  update();
}

QSize RibbonToolButton::sizeHint() const
{
  QSize baseSize = QToolButton::sizeHint();
  if (m_buttonSize == Large)
  {
    // Ensure enough width for two lines of text if word wrap is enabled
    int fontHeight = fontMetrics().height();
    int minWidth = qMax(baseSize.width(), 48);
    int minHeight = qMax(baseSize.height(), fontHeight * 2 + 40);
    return QSize(minWidth, minHeight);
  }
  return baseSize;
}

QSize RibbonToolButton::minimumSizeHint() const
{
  return sizeHint();
}

void RibbonToolButton::paintEvent(QPaintEvent *event)
{
  if (m_buttonSize == Small || !m_wordWrap)
  {
    QToolButton::paintEvent(event);
    return;
  }

  // Custom painting for Large mode with Word Wrap
  QPainter painter(this);
  QStyleOptionToolButton opt;
  initStyleOption(&opt);

  // Draw background and frame only (clear icon/text to avoid double painting)
  QStyleOptionToolButton frameOpt = opt;
  frameOpt.icon = QIcon();
  frameOpt.text = QString();
  style()->drawComplexControl(QStyle::CC_ToolButton, &frameOpt, &painter, this);

  QRect rect = opt.rect;
  int spacing = 2;

  // Draw Icon
  QIcon::Mode mode = opt.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
  if (opt.state & QStyle::State_Selected) mode = QIcon::Selected;
  if (opt.state & QStyle::State_MouseOver) mode = QIcon::Active;

  QPixmap pixmap = opt.icon.pixmap(opt.iconSize, mode, QIcon::Off);
  QRect iconRect = rect;
  iconRect.setHeight(rect.height() - fontMetrics().height() * 2 - spacing * 2);
  
  style()->drawItemPixmap(&painter, iconRect, Qt::AlignCenter, pixmap);

  // Draw Text (Wrapped)
  QRect textRect = rect;
  textRect.setTop(iconRect.bottom() + spacing);
  
  QTextOption textOption(Qt::AlignCenter);
  textOption.setWrapMode(QTextOption::WordWrap);
  
  painter.drawText(textRect, opt.text, textOption);
}
