/*==================================================
* NOM: hoverbutton.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans hoverbutton.h
===================================================*/

#include "hoverbutton.h"

HoverButton::HoverButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    this->setStyleSheet("background-color: white; color: #1a1a1a; border-radius: 10px;");

    QFont fontBoutons("Helvetica", 24, QFont::Bold);
    this->setFont(fontBoutons);

    m_clickSound = new QSoundEffect(this);
    m_clickSound->setSource(QUrl("qrc:/audio/audio/click.wav"));

    m_clickSound->setVolume(0.5f);

    connect(this, &QPushButton::clicked, m_clickSound, &QSoundEffect::play);
}

void HoverButton::setEnabled(bool isEnabled)
{
    m_isEnabled = isEnabled;
    if (isEnabled) {
        this->setStyleSheet("background-color: white; color: #1a1a1a; border-radius: 10px;");
    } else {
        this->setStyleSheet("background-color: gray; color: #1a1a1a; border-radius: 10px;");
	}
	QPushButton::setEnabled(isEnabled);
}

void HoverButton::enterEvent(QEnterEvent* event)
{
    if (m_isEnabled) {
        this->setStyleSheet("background-color: #e0e0e0; color: #1a1a1a; border-radius: 10px;");
    }
    QPushButton::enterEvent(event);
}

void HoverButton::leaveEvent(QEvent* event)
{
    if (m_isEnabled) {
        this->setStyleSheet("background-color: white; color: #1a1a1a; border-radius: 10px;");
    }
    QPushButton::leaveEvent(event);
}