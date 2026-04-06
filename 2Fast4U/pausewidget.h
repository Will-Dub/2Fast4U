#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QKeyEvent>

class PauseWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PauseWidget(QWidget* parent = nullptr);
	void setGameSnapshot(const QPixmap& snapshot);
protected:
	void paintEvent(QPaintEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	QPixmap m_gameSnapshot;

signals:
	void reprendrePressed();
	void quitterPressed();
};

