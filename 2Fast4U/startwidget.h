#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

class StartWidget : public QWidget
{
	Q_OBJECT
public:
	explicit StartWidget(QWidget* parent = nullptr);

signals:
	void commencerPressed();
	void retourPressed();
};

