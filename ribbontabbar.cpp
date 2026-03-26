/*
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbontabbar.h"

#include <QPainter>
#include <QPaintEvent>

RibbonTabBar::RibbonTabBar(QWidget *parent)
  : QTabBar(parent)
{
}

void RibbonTabBar::setTabColor(const QString &tabName, const QColor &color)
{
  m_tabColors[tabName.toLower()] = color;
  update();
}

void RibbonTabBar::clearTabColor(const QString &tabName)
{
  m_tabColors.remove(tabName.toLower());
  update();
}

void RibbonTabBar::paintEvent(QPaintEvent *event)
{
  // Draw the standard tab bar first
  QTabBar::paintEvent(event);

  // Draw a 3-pixel colored strip at the top of each context tab
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, false);

  for (int i = 0; i < count(); ++i)
  {
    const QString key = tabText(i).toLower();
    if (!m_tabColors.contains(key))
      continue;

    QRect r = tabRect(i);
    // Leave 2 px horizontal margin; 3 px height strip
    QRect strip(r.left() + 2, r.top() + 1, r.width() - 4, 3);
    painter.fillRect(strip, m_tabColors.value(key));
  }
}
