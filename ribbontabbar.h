/*
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONTABBAR_H
#define RIBBONTABBAR_H

#include <QTabBar>
#include <QColor>
#include <QMap>

/// A QTabBar subclass that can draw a colored indicator above individual tabs.
///
/// Used by Ribbon to mark context tabs with a distinctive color strip,
/// matching the behavior of Microsoft Office context tab groups.
class RibbonTabBar : public QTabBar
{
  Q_OBJECT

public:
  explicit RibbonTabBar(QWidget *parent = nullptr);

  /// Set a color indicator for the tab with the given name.
  /// The indicator is a colored strip drawn at the top of the tab rect.
  /// If the tab name is not found the call is ignored (the color is stored
  /// and applied when a tab with that name appears).
  ///
  /// \param[in] tabName Name of the tab (case-insensitive)
  /// \param[in] color   Indicator color
  void setTabColor(const QString &tabName, const QColor &color);

  /// Remove the color indicator for the named tab.
  ///
  /// \param[in] tabName Name of the tab (case-insensitive)
  void clearTabColor(const QString &tabName);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QMap<QString, QColor> m_tabColors; ///< key = tab name in lower-case
};

#endif // RIBBONTABBAR_H
