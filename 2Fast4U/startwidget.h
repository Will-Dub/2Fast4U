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

private:
	bool isValidInput(const QString& nom);

	QLineEdit* m_nomInput;
	HoverButton* m_commencerButton;

private slots:
	void nomTextChanged(const QString& newNom);

signals:
	void commencerPressed(const QString& nom);
	void retourPressed();
};

