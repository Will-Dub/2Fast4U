#pragma once
#include <QWidget>
#include "hoverbutton.h"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSlider>
#include <QSettings>

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

private slots:
	void valueChanged();

signals:
	void retourPressed();
	void sauvegarderPressed();
};

