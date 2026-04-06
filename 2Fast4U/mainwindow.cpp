#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_stack = new QStackedWidget(this);

    m_menuWidget = new MenuWidget(this);
    m_gameWidget = new GameWidget(this);
    m_startWidget = new StartWidget(this);
    m_optionsWidget = new OptionsWidget(this);

    m_stack->addWidget(m_menuWidget); // Menu = 0
    m_stack->addWidget(m_gameWidget); // Jeu = 1
    m_stack->addWidget(m_startWidget); // Start = 2
    m_stack->addWidget(m_optionsWidget); // Options = 3

    setCentralWidget(m_stack);

    // Menu
    connect(m_menuWidget, &MenuWidget::commencerPressed, this, &MainWindow::showStartScreen);
    connect(m_menuWidget, &MenuWidget::optionPressed, this, &MainWindow::showOptionsScreen);
    connect(m_menuWidget, &MenuWidget::quitterPressed, this, &MainWindow::quitter);

    // Options
    connect(m_optionsWidget, &OptionsWidget::retourPressed, this, &MainWindow::showMenuScreen);

    // Commencer
    connect(m_startWidget, &StartWidget::retourPressed, this, &MainWindow::showMenuScreen);
    connect(m_startWidget, &StartWidget::commencerPressed, this, &MainWindow::startGame);
}

void MainWindow::showMenuScreen()
{
    m_stack->setCurrentIndex(0);
    m_gameWidget->setFocus();
}

void MainWindow::showOptionsScreen()
{
    m_stack->setCurrentIndex(3);
    m_gameWidget->setFocus();
}

void MainWindow::showStartScreen()
{
    m_stack->setCurrentIndex(2);
    m_gameWidget->setFocus();
}

void MainWindow::startGame()
{
    m_stack->setCurrentIndex(1);
    m_gameWidget->setFocus();
    m_gameWidget->
}

void MainWindow::quitter()
{
    QCoreApplication::quit();
}

void MainWindow::handleEnd(EndType type)
{
}
