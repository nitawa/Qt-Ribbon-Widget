class Ribbon : QTabWidget
{
%TypeHeaderCode
#include <ribbon.h>
%End

public:
  enum RibbonMode
  {
    NormalMode,
    MinimizedMode
  };

  enum RibbonStyle
  {
    ThreeRowStyle,
    TwoRowStyle
  };

  enum TabAlignment
  {
    TabAlignLeft,
    TabAlignExpanding
  };

  enum RibbonTheme
  {
    DefaultTheme,
    Office2013Theme,
    Office2016BlueTheme,
    DarkTheme
  };

  explicit Ribbon(QWidget *parent /TransferThis/ = 0);

  void setTheme(RibbonTheme theme);
  RibbonTheme theme() const;

  void addTab(const QString &tabName);
  void addTab(const QIcon &tabIcon, const QString &tabName);
  void removeTab(const QString &tabName);

  void addGroup(const QString &tabName, const QString &groupName);

  void addButton(const QString &tabName, const QString &groupName, QToolButton *button);
  void removeButton(const QString &tabName, const QString &groupName, QToolButton *button);

  void addAction(const QString &tabName, const QString &groupName, QAction *action, RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton);
  void addAction(const QString &tabName, const QString &groupName, QAction *action, RibbonButtonGroup::ButtonSize size, QToolButton::ToolButtonPopupMode popupMode);
  void removeAction(const QString &tabName, const QString &groupName, QAction *action);

  void addMenu(const QString &tabName, const QString &groupName, QMenu *menu, RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);

  void addWidget(const QString &tabName, const QString &groupName, QWidget *widget, RibbonButtonGroup::ButtonSize size = RibbonButtonGroup::LargeButton);
  void removeWidget(const QString &tabName, const QString &groupName, QWidget *widget);

  void addSeparator(const QString &tabName, const QString &groupName);

  void setGroupExpanding(const QString &tabName, const QString &groupName, bool expanding = true);
  void setGroupOptionAction(const QString &tabName, const QString &groupName, QAction *action);

  void setRibbonStyle(RibbonStyle style);
  RibbonStyle ribbonStyle() const;

  void setRibbonMode(RibbonMode mode);
  RibbonMode ribbonMode() const;
  void toggleRibbonMode();

  void setTabAlignment(TabAlignment align);
  TabAlignment tabAlignment() const;

  void setApplicationButton(QToolButton *button);
  QToolButton *applicationButton() const;

  QToolBar *quickAccessBar() const;
  void addQuickAccessAction(QAction *action);
  void removeQuickAccessAction(QAction *action);

  void addContextTab(const QString &tabName, const QColor &color = QColor());
  void showContextTab(const QString &tabName);
  void hideContextTab(const QString &tabName);

  void setPanelTitleHeight(int height);
  void setShowPanelTitle(bool visible);
  void setPanelSpacing(int spacing);
  void setLargeIconSize(const QSize &size);
  void setSmallIconSize(const QSize &size);

signals:
  void ribbonModeChanged(Ribbon::RibbonMode mode);
  void ribbonStyleChanged(Ribbon::RibbonStyle style);
  void actionTriggered(QAction *action);
};
