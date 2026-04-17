/*==================================================
* NOM: startwidget.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe pour l'écran de préparation pour la course
===================================================*/

#pragma once
#include <QWidget>
#include "hoverbutton.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include "racemanager.h"

class StartWidget : public QWidget
{
	Q_OBJECT
public:
	explicit StartWidget(QWidget* parent = nullptr);
	void resetInput();
	void refreshLeaderboard();

private:
	bool isValidInput(const QString& nom);

	QLineEdit* m_nomInput;
	HoverButton* m_commencerButton;
	QVBoxLayout* m_leaderboardLayout;

private slots:
	void nomTextChanged(const QString& newNom);

signals:
	void commencerPressed(const QString& nom);
	void retourPressed();
};

