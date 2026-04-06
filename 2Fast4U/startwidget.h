#pragma once
#include <QWidget>
#include <QPushButton>
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

private:
	QLineEdit* m_nomInput;

signals:
	void commencerPressed(const QString& nom);
	void retourPressed();
};

