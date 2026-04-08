#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>
#include <QEvent>
#include <QSoundEffect>

class HoverButton : public QPushButton
{
    Q_OBJECT

public:
    explicit HoverButton(const QString& text, QWidget* parent = nullptr);
	void setEnabled(bool isEnabled);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    bool m_isEnabled = true;
	QSoundEffect* m_clickSound;
};

#endif // HOVERBUTTON_H