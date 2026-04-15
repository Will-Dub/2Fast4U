/*==================================================
* NOM: menuwidget.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans menuwidget.h
===================================================*/
#include "menuwidget.h"

MenuWidget::MenuWidget(QWidget* parent) : QWidget(parent)
{
    // Fond d'écran
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("MenuWidget { border-image: url(:/images/background.png) 0 0 0 0 stretch stretch; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* darkOverlay = new QFrame(this);
    darkOverlay->setStyleSheet("QFrame { background-color: rgba(0, 0, 0, 150); }");
    mainLayout->addWidget(darkOverlay);

    // Layout titre et boutons
    QVBoxLayout* layout = new QVBoxLayout(darkOverlay);
    layout->addSpacerItem(new QSpacerItem(20, 60, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Titre
    QLabel* title = new QLabel("2Fast4U");
    QFont titleFont("Helvetica", 50, QFont::Bold);
    title->setFont(titleFont);
    title->setStyleSheet("color: white; background: transparent;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Espace
    layout->addSpacerItem(new QSpacerItem(20, 150, QSizePolicy::Minimum, QSizePolicy::Expanding));
    QFont fontBoutons("Helvetica", 24, QFont::Bold);

    // Bouton commencer
    HoverButton* boutonCommencer = new HoverButton("Commencer");
    boutonCommencer->setFixedSize(400, 80);
    boutonCommencer->setFont(fontBoutons);
    layout->addWidget(boutonCommencer, 0, Qt::AlignCenter);

    layout->addSpacing(15);

    // Bouton options
    HoverButton* boutonOption = new HoverButton("Options");
    boutonOption->setFixedSize(400, 80);
    boutonOption->setFont(fontBoutons);
    layout->addWidget(boutonOption, 0, Qt::AlignCenter);

    layout->addSpacing(15);

    // Bouton quitter
    HoverButton* boutonQuitter = new HoverButton("Quitter");
    boutonQuitter->setFixedSize(400, 80);
    boutonQuitter->setFont(fontBoutons);
    layout->addWidget(boutonQuitter, 0, Qt::AlignCenter);

    layout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Connection signal et slots
    connect(boutonCommencer, &QPushButton::clicked, this, &MenuWidget::commencerPressed);
    connect(boutonOption, &QPushButton::clicked, this, &MenuWidget::optionPressed);
    connect(boutonQuitter, &QPushButton::clicked, this, &MenuWidget::quitterPressed);
}
