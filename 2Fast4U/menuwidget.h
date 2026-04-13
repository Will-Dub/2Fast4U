/*==================================================
* NOM: menuwidget.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe pour l'écran du menu principal
===================================================*/

#pragma once
#include <QWidget>
#include "hoverbutton.h"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include "racemanager.h"

class MenuWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MenuWidget(QWidget* parent = nullptr);

signals:
	void commencerPressed();
	void optionPressed();
	void quitterPressed();
};

