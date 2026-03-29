class RibbonButtonGroup : QWidget
{
%TypeHeaderCode
#include <ribbonbuttongroup.h>
%End

public:
  enum ButtonSize
  {
    LargeButton,
    MediumButton,
    SmallButton
  };

  enum LayoutMode
  {
    ThreeRowMode,
    TwoRowMode
  };

  explicit RibbonButtonGroup(QWidget *parent /TransferThis/ = 0);
  virtual ~RibbonButtonGroup();

  void setTitle(const QString &title);
  QString title() const;

  void setTitleVisible(bool visible);
  bool isTitleVisible() const;

  void setPanelTitleHeight(int height);
  void setOptionAction(QAction *action);

  void setLayoutMode(LayoutMode mode);
  LayoutMode layoutMode() const;

  void setExpanding(bool expanding = true);
  bool isExpanding() const;

  void setLargeIconSize(const QSize &size);
  QSize largeIconSize() const;

  void setSmallIconSize(const QSize &size);
  QSize smallIconSize() const;

  int buttonCount() const;

  void addButton(QToolButton *button);
  void removeButton(QToolButton *button);

  void addAction(QAction *action, ButtonSize size = LargeButton);
  void addAction(QAction *action, ButtonSize size, QToolButton::ToolButtonPopupMode popupMode);
  void removeAction(QAction *action);

  void addMenu(QMenu *menu, ButtonSize size = LargeButton, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);

  void addWidget(QWidget *widget, ButtonSize size = LargeButton);
  void removeWidget(QWidget *widget);

  void addSeparator();

signals:
  void actionTriggered(QAction *action);
};
