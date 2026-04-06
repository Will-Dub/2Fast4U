#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "menuwidget.h"
#include <QStackedWidget>
#include "gamewidget.h"
#include "optionswidget.h"
#include "startwidget.h"

enum class EndType {
    Crash,
    MotorExploded,
    Win
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget* m_stack;
    MenuWidget* m_menuWidget;
    GameWidget* m_gameWidget;
    OptionsWidget* m_optionsWidget;
    StartWidget* m_startWidget;

private slots:
    void showMenuScreen();
    void showOptionsScreen();
    void showStartScreen();
    void startGame();
    void quitter();
    void handleEnd(EndType type);
};
#endif // MAINWINDOW_H
