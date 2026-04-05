#include "menuwidget.h"

MenuWidget::MenuWidget(QWidget* parent)
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
    QString styleBoutons = "background-color: white; color: #1a1a1a; border-radius: 10px;";

    // Bouton commencer
    QPushButton* boutonCommencer = new QPushButton("Commencer");
    boutonCommencer->setFixedSize(400, 80);
    boutonCommencer->setFont(fontBoutons);
    boutonCommencer->setStyleSheet(styleBoutons);
    layout->addWidget(boutonCommencer, 0, Qt::AlignCenter);

    layout->addSpacing(15);

    // Bouton options
    QPushButton* boutonOption = new QPushButton("Options");
    boutonOption->setFixedSize(400, 80);
    boutonOption->setFont(fontBoutons);
    boutonOption->setStyleSheet(styleBoutons);
    layout->addWidget(boutonOption, 0, Qt::AlignCenter);

    layout->addSpacing(15);

    // Bouton quitter
    QPushButton* boutonQuitter = new QPushButton("Quitter");
    boutonQuitter->setFixedSize(400, 80);
    boutonQuitter->setFont(fontBoutons);
    boutonQuitter->setStyleSheet(styleBoutons);
    layout->addWidget(boutonQuitter, 0, Qt::AlignCenter);

    layout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Connection signal et slots
    connect(boutonCommencer, &QPushButton::clicked, this, &MenuWidget::commencerPressed);
    connect(boutonCommencer, &QPushButton::clicked, this, &MenuWidget::optionPressed);
    connect(boutonCommencer, &QPushButton::clicked, this, &MenuWidget::quitterPressed);
}
