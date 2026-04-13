/*==================================================
* NOM: pausewidget.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans pausewidget.h
===================================================*/

#include "pausewidget.h"

PauseWidget::PauseWidget(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* darkOverlay = new QFrame(this);
    darkOverlay->setStyleSheet("QFrame { background-color: rgba(0, 0, 0, 150); }");
    mainLayout->addWidget(darkOverlay);

    // Layout titre et boutons
    QVBoxLayout* layout = new QVBoxLayout(darkOverlay);
    layout->addStretch(1);

    // Titre
    QLabel* title = new QLabel("Pause", darkOverlay);
    QFont titleFont("Helvetica", 50, QFont::Bold);
    title->setFont(titleFont);
    title->setStyleSheet("color: white; background: transparent;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    layout->addStretch(8);

    QFont fontBoutons("Helvetica", 24, QFont::Bold);

    QHBoxLayout* boutonLayout = new QHBoxLayout();
    boutonLayout->setAlignment(Qt::AlignCenter);
    boutonLayout->setSpacing(15);

    // Bouton retour
    HoverButton* boutonReprendre = new HoverButton("Reprendre", darkOverlay);
    boutonReprendre->setFixedSize(250, 80);
    boutonReprendre->setFont(fontBoutons);
    boutonLayout->addWidget(boutonReprendre);

    // Bouton sauvegarder
    HoverButton* boutonQuitter = new HoverButton("Quitter", darkOverlay);
    boutonQuitter->setFixedSize(200, 80);
    boutonQuitter->setFont(fontBoutons);
    boutonLayout->addWidget(boutonQuitter);

    layout->addLayout(boutonLayout);
    layout->addStretch(1);

    // Connection signal et slots
    connect(boutonReprendre, &QPushButton::clicked, this, &PauseWidget::reprendrePressed);
    connect(boutonQuitter, &QPushButton::clicked, this, &PauseWidget::quitterPressed);
}

void PauseWidget::setGameSnapshot(const QPixmap& snapshot)
{
    m_gameSnapshot = snapshot;
    update();
}

void PauseWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (!m_gameSnapshot.isNull()) {
        painter.drawPixmap(this->rect(), m_gameSnapshot);
    }

    QWidget::paintEvent(event);
}

void PauseWidget::keyPressEvent(QKeyEvent* event)
{
    // Empeche de maintenir escape
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        this->setEnabled(false);
        emit reprendrePressed();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

void PauseWidget::showEvent(QShowEvent* event)
{
    this->setEnabled(true);

    QWidget::showEvent(event);
}