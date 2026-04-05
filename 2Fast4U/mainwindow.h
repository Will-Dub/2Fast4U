#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <menuwidget.h>
#include <QStackedWidget>
#include <gamewidget.h>

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

private slots:
    void showMenu();
    void showConfig();
    void showStartScreen();
    void startGame();
    void handleEnd(EndType type);
};
#endif // MAINWINDOW_H
