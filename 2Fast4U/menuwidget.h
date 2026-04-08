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

