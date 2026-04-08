#pragma once
#include <QWidget>
#include "hoverbutton.h"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>

class OptionsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit OptionsWidget(QWidget* parent = nullptr);

signals:
	void retourPressed();
	void sauvegarderPressed();
};

