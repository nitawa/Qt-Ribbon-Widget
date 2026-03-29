/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QComboBox>
#include <QFontComboBox>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Hide ribbon dock title bar
  ui->ribbonDockWidget->setTitleBarWidget(new QWidget());

  Ribbon *ribbon = ui->ribbonTabWidget;

  // ── Global appearance (demonstrates global setters) ───────────────────────
  ribbon->setLargeIconSize(QSize(32, 32));
  ribbon->setSmallIconSize(QSize(16, 16));
  ribbon->setPanelSpacing(2);
  ribbon->setPanelTitleHeight(16);  // fixed title-bar height across all groups

  // ── Application button ────────────────────────────────────────────────────
  QToolButton *appButton = new QToolButton;
  appButton->setText(tr("File"));
  appButton->setToolTip(tr("Open application menu"));
  appButton->setIcon(QIcon(":/icons/briefcase_1.svg"));
  appButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  appButton->setIconSize(QSize(24, 24));
  appButton->setAutoRaise(true);
  QMenu *appMenu = new QMenu(this);
  appMenu->addAction(tr("New project"));
  appMenu->addAction(tr("Open project..."));
  appMenu->addSeparator();
  appMenu->addAction(tr("Exit"));
  appButton->setMenu(appMenu);
  appButton->setPopupMode(QToolButton::InstantPopup);
  ribbon->setApplicationButton(appButton);

  // ── Quick access bar ──────────────────────────────────────────────────────
  // Demonstrates addQuickAccessAction(); bar placed as top toolbar so it
  // is actually visible in the window.
  QAction *qaNew  = new QAction(QIcon(":/icons/create_new_2.svg"),  tr("New"),  this);
  QAction *qaOpen = new QAction(QIcon(":/icons/live_folder_2.svg"), tr("Open"), this);
  QAction *qaSave = new QAction(QIcon(":/icons/save_2.svg"),        tr("Save"), this);
  ribbon->addQuickAccessAction(qaNew);
  ribbon->addQuickAccessAction(qaOpen);
  ribbon->addQuickAccessAction(qaSave);
  addToolBar(Qt::TopToolBarArea, ribbon->quickAccessBar());

  // ── "Project" tab – legacy QToolButton API (backward compatibility) ───────
  ribbon->addTab(QIcon(":/icons/briefcase_1.svg"), "Project");

  // "Project" group – raw QToolButton path
  QToolButton *openProjectButton = new QToolButton;
  openProjectButton->setText(tr("Open"));
  openProjectButton->setToolTip(tr("Open existing project"));
  openProjectButton->setIcon(QIcon(":/icons/live_folder_2.svg"));
  ribbon->addButton("Project", "Project", openProjectButton);

  QToolButton *newProjectButton = new QToolButton;
  newProjectButton->setText(tr("New"));
  newProjectButton->setToolTip(tr("Create new project"));
  newProjectButton->setIcon(QIcon(":/icons/create_new_2.svg"));
  ribbon->addButton("Project", "Project", newProjectButton);

  QToolButton *saveProjectButton = new QToolButton;
  saveProjectButton->setText(tr("Save"));
  saveProjectButton->setToolTip(tr("Save project"));
  saveProjectButton->setIcon(QIcon(":/icons/save_2.svg"));
  saveProjectButton->setEnabled(false);
  ribbon->addButton("Project", "Project", saveProjectButton);

  // "Import" group – legacy QToolButton with MenuButtonPopup
  QToolButton *openFileButton = new QToolButton;
  openFileButton->setText(tr("File"));
  openFileButton->setToolTip(tr("Open file or directory"));
  openFileButton->setIcon(QIcon(":/icons/add_folder_2.svg"));
  openFileButton->setPopupMode(QToolButton::MenuButtonPopup);
  QMenu *fileMenu = new QMenu(this);
  fileMenu->addAction(QIcon(":/icons/file_2.svg"), tr("Recent file 1"));
  fileMenu->addAction(QIcon(":/icons/file_2.svg"), tr("Recent file 2"));
  fileMenu->addAction(QIcon(":/icons/file_2.svg"), tr("Recent file 3"));
  openFileButton->setMenu(fileMenu);
  ribbon->addButton("Project", "Import", openFileButton);

  QToolButton *openDatabaseButton = new QToolButton;
  openDatabaseButton->setText(tr("Database"));
  openDatabaseButton->setToolTip(tr("Connect to database"));
  openDatabaseButton->setIcon(QIcon(":/icons/add_database_2.svg"));
  ribbon->addButton("Project", "Import", openDatabaseButton);

  QToolButton *connectWebButton = new QToolButton;
  connectWebButton->setText(tr("Web service"));
  connectWebButton->setToolTip(tr("Connect to web service"));
  connectWebButton->setIcon(QIcon(":/icons/add_link_2.svg"));
  ribbon->addButton("Project", "Import", connectWebButton);

  // "Ribbon" group – action API: ribbon layout / appearance controls
  QAction *toggleContextAction = new QAction(
      QIcon(":/icons/engineering_1.svg"), tr("Show Table"), this);
  toggleContextAction->setCheckable(true);
  toggleContextAction->setToolTip(tr("Show or hide the Table context tab"));
  connect(toggleContextAction, &QAction::toggled, this, [ribbon](bool checked) {
    checked ? ribbon->showContextTab("Table") : ribbon->hideContextTab("Table");
  });
  ribbon->addAction("Project", "Ribbon", toggleContextAction,
                    RibbonButtonGroup::LargeButton);

  QAction *minimizeAction = new QAction(
      QIcon(":/icons/monitor_1.svg"), tr("Minimize"), this);
  minimizeAction->setToolTip(tr("Collapse ribbon to tab bar (double-click to restore)"));
  connect(minimizeAction, &QAction::triggered, ribbon, &Ribbon::toggleRibbonMode);
  ribbon->addAction("Project", "Ribbon", minimizeAction,
                    RibbonButtonGroup::SmallButton);

  QAction *twoRowAction = new QAction(tr("2-Row"), this);
  twoRowAction->setToolTip(tr("Switch to compact two-row layout"));
  twoRowAction->setCheckable(true);
  connect(twoRowAction, &QAction::toggled, this, [ribbon](bool on) {
    ribbon->setRibbonStyle(on ? Ribbon::TwoRowStyle : Ribbon::ThreeRowStyle);
  });
  ribbon->addAction("Project", "Ribbon", twoRowAction,
                    RibbonButtonGroup::SmallButton);

  QAction *hideTitleAction = new QAction(tr("No Title"), this);
  hideTitleAction->setToolTip(tr("Hide group title bars"));
  hideTitleAction->setCheckable(true);
  connect(hideTitleAction, &QAction::toggled, this, [ribbon](bool on) {
    ribbon->setShowPanelTitle(!on);
  });
  ribbon->addAction("Project", "Ribbon", hideTitleAction,
                    RibbonButtonGroup::SmallButton);

  // Tab alignment toggle – demonstrates setTabAlignment()
  QAction *expandTabsAction = new QAction(tr("Expand Tabs"), this);
  expandTabsAction->setToolTip(tr("Stretch tab bar to fill the full width"));
  expandTabsAction->setCheckable(true);
  connect(expandTabsAction, &QAction::toggled, this, [ribbon](bool on) {
    ribbon->setTabAlignment(on ? Ribbon::TabAlignExpanding : Ribbon::TabAlignLeft);
  });
  ribbon->addAction("Project", "Ribbon", expandTabsAction,
                    RibbonButtonGroup::SmallButton);

  // "Themes" group – demonstrates setTheme()
  QAction *defaultThemeAction = new QAction(tr("Default"), this);
  connect(defaultThemeAction, &QAction::triggered, this, [ribbon]() {
    ribbon->setTheme(Ribbon::DefaultTheme);
  });
  ribbon->addAction("Project", "Themes", defaultThemeAction, RibbonButtonGroup::SmallButton);

  QAction *office2013ThemeAction = new QAction(tr("Office 2013"), this);
  connect(office2013ThemeAction, &QAction::triggered, this, [ribbon]() {
    ribbon->setTheme(Ribbon::Office2013Theme);
  });
  ribbon->addAction("Project", "Themes", office2013ThemeAction, RibbonButtonGroup::SmallButton);

  QAction *office2016ThemeAction = new QAction(tr("Office 2016"), this);
  connect(office2016ThemeAction, &QAction::triggered, this, [ribbon]() {
    ribbon->setTheme(Ribbon::Office2016BlueTheme);
  });
  ribbon->addAction("Project", "Themes", office2016ThemeAction, RibbonButtonGroup::SmallButton);

  QAction *darkThemeAction = new QAction(tr("Dark"), this);
  connect(darkThemeAction, &QAction::triggered, this, [ribbon]() {
    ribbon->setTheme(Ribbon::DarkTheme);
  });
  ribbon->addAction("Project", "Themes", darkThemeAction, RibbonButtonGroup::SmallButton);

  // ── "Format" tab ──────────────────────────────────────────────────────────
  ribbon->addTab(QIcon(":/icons/engineering_1.svg"), "Format");

  // "Font Family" group – QFontComboBox (Large) + QSpinBox (Small)
  // Demonstrates addWidget() with multiple sizes and setGroupExpanding()
  QFontComboBox *fontCombo = new QFontComboBox;
  fontCombo->setMinimumWidth(140);
  fontCombo->setToolTip(tr("Font family"));
  ribbon->addWidget("Format", "Font Family", fontCombo, RibbonButtonGroup::LargeButton);

  QSpinBox *fontSizeSpin = new QSpinBox;
  fontSizeSpin->setRange(6, 96);
  fontSizeSpin->setValue(11);
  fontSizeSpin->setSuffix(tr(" pt"));
  fontSizeSpin->setToolTip(tr("Font size in points"));
  fontSizeSpin->setMinimumWidth(60);
  ribbon->addWidget("Format", "Font Family", fontSizeSpin, RibbonButtonGroup::SmallButton);

  // Mark the group as expanding so the font combo stretches to fill space
  ribbon->setGroupExpanding("Format", "Font Family", true);

  // "Font" group: Large + separator + Small buttons
  // Also demonstrates setGroupOptionAction() – the ▾ button in the title bar
  QAction *boldAction = new QAction(tr("Bold"), this);
  boldAction->setToolTip(tr("Bold text"));
  boldAction->setCheckable(true);
  ribbon->addAction("Format", "Font", boldAction, RibbonButtonGroup::LargeButton);

  QAction *italicAction = new QAction(tr("Italic"), this);
  italicAction->setToolTip(tr("Italic text"));
  italicAction->setCheckable(true);
  ribbon->addAction("Format", "Font", italicAction, RibbonButtonGroup::LargeButton);

  ribbon->addSeparator("Format", "Font");

  QAction *underlineAction = new QAction(tr("Underline"), this);
  underlineAction->setCheckable(true);
  ribbon->addAction("Format", "Font", underlineAction, RibbonButtonGroup::SmallButton);

  QAction *strikeAction = new QAction(tr("Strike"), this);
  strikeAction->setCheckable(true);
  ribbon->addAction("Format", "Font", strikeAction, RibbonButtonGroup::SmallButton);

  QAction *superAction = new QAction(tr("Super"), this);
  ribbon->addAction("Format", "Font", superAction, RibbonButtonGroup::SmallButton);

  // Option button (▾) on the Font group – opens a full font dialog
  QAction *fontDialogAction = new QAction(tr("Font dialog..."), this);
  fontDialogAction->setToolTip(tr("Open the full font dialog"));
  ribbon->setGroupOptionAction("Format", "Font", fontDialogAction);

  // "Paragraph" group: Medium buttons (2/3 + 1/3 column split in ThreeRowMode)
  QAction *alignLeftAction   = new QAction(tr("Left"),    this);
  QAction *alignRightAction  = new QAction(tr("Right"),   this);
  QAction *alignCenterAction = new QAction(tr("Center"),  this);
  QAction *justifyAction     = new QAction(tr("Justify"), this);
  ribbon->addAction("Format", "Paragraph", alignLeftAction,   RibbonButtonGroup::MediumButton);
  ribbon->addAction("Format", "Paragraph", alignRightAction,  RibbonButtonGroup::MediumButton);
  ribbon->addAction("Format", "Paragraph", alignCenterAction, RibbonButtonGroup::MediumButton);
  ribbon->addAction("Format", "Paragraph", justifyAction,     RibbonButtonGroup::MediumButton);

  // "Style" group: embedded QComboBox widget (demonstrates addWidget at LargeButton)
  QComboBox *styleCombo = new QComboBox;
  styleCombo->addItem(tr("Normal"));
  styleCombo->addItem(tr("Heading 1"));
  styleCombo->addItem(tr("Heading 2"));
  styleCombo->addItem(tr("Quote"));
  styleCombo->setMinimumWidth(120);
  ribbon->addWidget("Format", "Style", styleCombo, RibbonButtonGroup::LargeButton);

  // "Insert" group: QMenu via addMenu with InstantPopup
  QMenu *insertMenu = new QMenu(tr("Insert"), this);
  insertMenu->addAction(tr("Image..."));
  insertMenu->addAction(tr("Table..."));
  insertMenu->addAction(tr("Hyperlink..."));
  ribbon->addMenu("Format", "Insert", insertMenu,
                  RibbonButtonGroup::LargeButton, QToolButton::InstantPopup);

  // ── "View" tab ────────────────────────────────────────────────────────────
  ribbon->addTab(QIcon(":/icons/monitor_1.svg"), "View");

  // "Zoom" group: plain Large buttons + separator + split button
  // The split button demonstrates addAction(tab, group, action, size, popupMode)
  // with MenuButtonPopup: clicking the button triggers the action directly;
  // clicking the arrow opens the zoom-level submenu.
  QAction *zoomInAction = new QAction(QIcon(":/icons/monitor_1.svg"), tr("Zoom In"), this);
  zoomInAction->setToolTip(tr("Zoom in one step"));
  ribbon->addAction("View", "Zoom", zoomInAction, RibbonButtonGroup::LargeButton);

  QAction *zoomOutAction = new QAction(QIcon(":/icons/monitor_1.svg"), tr("Zoom Out"), this);
  zoomOutAction->setToolTip(tr("Zoom out one step"));
  ribbon->addAction("View", "Zoom", zoomOutAction, RibbonButtonGroup::LargeButton);

  ribbon->addSeparator("View", "Zoom");

  QAction *zoomAction = new QAction(QIcon(":/icons/monitor_1.svg"), tr("Zoom"), this);
  zoomAction->setToolTip(tr("Reset zoom to 100%; arrow opens zoom presets"));
  QMenu *zoomMenu = new QMenu(this);
  zoomMenu->addAction(tr("50%"));
  zoomMenu->addAction(tr("75%"));
  zoomMenu->addAction(tr("100%"));
  zoomMenu->addAction(tr("150%"));
  zoomMenu->addAction(tr("200%"));
  zoomAction->setMenu(zoomMenu);
  ribbon->addAction("View", "Zoom", zoomAction,
                    RibbonButtonGroup::LargeButton,
                    QToolButton::MenuButtonPopup);  // split button

  // "Display" group: Small checkable toggles
  QAction *gridAction = new QAction(tr("Grid"), this);
  gridAction->setCheckable(true);
  gridAction->setToolTip(tr("Show grid overlay"));
  ribbon->addAction("View", "Display", gridAction, RibbonButtonGroup::SmallButton);

  QAction *rulersAction = new QAction(tr("Rulers"), this);
  rulersAction->setCheckable(true);
  rulersAction->setToolTip(tr("Show rulers"));
  ribbon->addAction("View", "Display", rulersAction, RibbonButtonGroup::SmallButton);

  QAction *guidesAction = new QAction(tr("Guides"), this);
  guidesAction->setCheckable(true);
  guidesAction->setToolTip(tr("Show alignment guides"));
  ribbon->addAction("View", "Display", guidesAction, RibbonButtonGroup::SmallButton);

  // ── "Help" tab ────────────────────────────────────────────────────────────
  ribbon->addTab(QIcon(":/icons/information_1.svg"), "Help");

  // "Help" group: Large buttons
  QAction *manualAction = new QAction(QIcon(":/icons/information_1.svg"), tr("Manual"), this);
  manualAction->setToolTip(tr("Open the user manual"));
  ribbon->addAction("Help", "Help", manualAction, RibbonButtonGroup::LargeButton);

  QAction *aboutAction = new QAction(QIcon(":/icons/information_1.svg"), tr("About"), this);
  aboutAction->setToolTip(tr("About this application"));
  ribbon->addAction("Help", "Help", aboutAction, RibbonButtonGroup::LargeButton);

  // "Updates" group: Small buttons
  QAction *checkUpdatesAction = new QAction(tr("Check Updates"), this);
  checkUpdatesAction->setToolTip(tr("Check for available updates online"));
  ribbon->addAction("Help", "Updates", checkUpdatesAction, RibbonButtonGroup::SmallButton);

  QAction *releaseNotesAction = new QAction(tr("Release Notes"), this);
  releaseNotesAction->setToolTip(tr("View release notes for this version"));
  ribbon->addAction("Help", "Updates", releaseNotesAction, RibbonButtonGroup::SmallButton);

  // ── "Table" context tab (initially hidden, green color strip) ────────────
  ribbon->addContextTab("Table", QColor(21, 128, 61));

  QAction *insertTableAction = new QAction(tr("Insert"), this);
  insertTableAction->setToolTip(tr("Insert a new table"));
  ribbon->addAction("Table", "Table", insertTableAction, RibbonButtonGroup::LargeButton);

  QAction *deleteTableAction = new QAction(tr("Delete"), this);
  deleteTableAction->setToolTip(tr("Delete table"));
  ribbon->addAction("Table", "Table", deleteTableAction, RibbonButtonGroup::LargeButton);

  ribbon->addSeparator("Table", "Table");

  QAction *mergeAction = new QAction(tr("Merge"), this);
  mergeAction->setToolTip(tr("Merge selected cells"));
  ribbon->addAction("Table", "Cells", mergeAction, RibbonButtonGroup::MediumButton);

  QAction *splitAction = new QAction(tr("Split"), this);
  splitAction->setToolTip(tr("Split cell"));
  ribbon->addAction("Table", "Cells", splitAction, RibbonButtonGroup::MediumButton);

  QAction *addRowAction = new QAction(tr("Add Row"), this);
  ribbon->addAction("Table", "Rows & Cols", addRowAction, RibbonButtonGroup::SmallButton);

  QAction *addColAction = new QAction(tr("Add Col"), this);
  ribbon->addAction("Table", "Rows & Cols", addColAction, RibbonButtonGroup::SmallButton);

  QAction *delRowAction = new QAction(tr("Del Row"), this);
  ribbon->addAction("Table", "Rows & Cols", delRowAction, RibbonButtonGroup::SmallButton);

  // ── Global signal: show triggered action name in the status bar ───────────
  connect(ribbon, &Ribbon::actionTriggered, this, [this](QAction *action) {
    statusBar()->showMessage(action->text(), 2000);
  });
}

MainWindow::~MainWindow()
{
  delete ui;
}
