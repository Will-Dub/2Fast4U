#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "menuwidget.h"
#include <QStackedWidget>
#include "gamewidget.h"
#include "optionswidget.h"
#include "startwidget.h"
#include "pausewidget.h"

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
    PauseWidget* m_pauseWidget;

private slots:
    void showMenuScreen();
    void showOptionsScreen();
    void showStartScreen();
    void startGame();
    void pauseGame();
    void quitter();
    void resumeGame();
    void handleEnd(EndType type);
};
#endif // MAINWINDOW_H
