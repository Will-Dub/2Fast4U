#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>

class EndWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EndWidget(QWidget* parent = nullptr);
	void setText(const QString& titre, const QString& sousTitre);
	void setNom(const QString& nom) { m_nom = nom; }
	void setGameSnapshot(const QPixmap& snapshot);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	QString m_nom;
	QLabel* m_titre;
	QLabel* m_sousTitre;
	QPixmap m_gameSnapshot;

signals:
	void quitterPressed();
	void recommencerPressed(const QString& nom);
};

