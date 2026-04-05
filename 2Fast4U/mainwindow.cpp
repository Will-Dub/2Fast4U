#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_stack = new QStackedWidget(this);

    m_menuWidget = new MenuWidget(this);
    m_gameWidget = new GameWidget(this);

    m_stack->addWidget(m_menuWidget);
    m_stack->addWidget(m_gameWidget);

    setCentralWidget(m_stack);

    connect(m_menuWidget, &MenuWidget::commencerPressed, this, &MainWindow::startGame);
}

void MainWindow::showMenu()
{

}

void MainWindow::showOptions()
{

}

void MainWindow::showStartScreen()
{
}

void MainWindow::startGame()
{
    m_stack->setCurrentIndex(1);
    m_gameWidget->setFocus();
}

void MainWindow::handleEnd(EndType type)
{
}
