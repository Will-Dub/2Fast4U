/*==================================================
* NOM: mainwindow.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe pour la fenêtre principale du menu
===================================================*/





#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "menuwidget.h"
#include <QStackedWidget>
#include "gamewidget.h"
#include "optionswidget.h"
#include "startwidget.h"
#include "pausewidget.h"
#include "endwidget.h"

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
	EndWidget* m_endWidget;

private slots:
    void showMenuScreen();
    void showOptionsScreen();
    void showStartScreen();
    void startGame(const QString& nom);
    void pauseGame();
    void quitter();
    void resumeGame();
    void showEndWidget(EndType type, const QString& nom, double temps);
};
#endif // MAINWINDOW_H
