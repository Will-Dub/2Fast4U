/*==================================================
* NOM: optionswidget.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe pour l'écran des options
===================================================*/
#pragma once
#include <QWidget>
#include "hoverbutton.h"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSlider>
#include <QSettings>
#include <QSpinBox>
#include <QIntValidator>

class OptionsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit OptionsWidget(QWidget* parent = nullptr);
	void resetValueChanged();

private:
	bool m_didValueChange = false;
	HoverButton* m_boutonSauvegarder;
	QSettings m_settings;
	QSpinBox* m_distanceAffichageInput;
	QSpinBox* m_hauteurCameraInput;

private slots:
	void valueChanged();

signals:
	void retourPressed();
	void sauvegarderPressed();
};

