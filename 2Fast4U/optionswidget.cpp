#include "optionswidget.h"

OptionsWidget::OptionsWidget(QWidget* parent) : QWidget(parent)
{
    // Fond d'écran
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("OptionsWidget { border-image: url(:/images/background.png) 0 0 0 0 stretch stretch; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* darkOverlay = new QFrame(this);
    darkOverlay->setStyleSheet("QFrame { background-color: rgba(0, 0, 0, 150); }");
    mainLayout->addWidget(darkOverlay);

    // Layout titre et boutons
    QVBoxLayout* layout = new QVBoxLayout(darkOverlay);

    layout->addStretch(1);

    // Titre
    QLabel* title = new QLabel("Options", darkOverlay);
    QFont titleFont("Helvetica", 50, QFont::Bold);
    title->setFont(titleFont);
    title->setStyleSheet("color: white; background: transparent;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    layout->addStretch(8);

    QFont fontBoutons("Helvetica", 24, QFont::Bold);
    QString styleBoutons = "background-color: white; color: #1a1a1a; border-radius: 10px;";

    QHBoxLayout* boutonLayout = new QHBoxLayout();
    boutonLayout->setAlignment(Qt::AlignCenter);
    boutonLayout->setSpacing(15);

    // Bouton retour
    QPushButton* boutonRetour = new QPushButton("Retour", darkOverlay);
    boutonRetour->setFixedSize(200, 80);
    boutonRetour->setFont(fontBoutons);
    boutonRetour->setStyleSheet(styleBoutons);
    boutonLayout->addWidget(boutonRetour);

    // Bouton sauvegarder
    QPushButton* boutonSauvegarder = new QPushButton("Sauvegarder", darkOverlay);
    boutonSauvegarder->setFixedSize(250, 80);
    boutonSauvegarder->setFont(fontBoutons);
    boutonSauvegarder->setStyleSheet(styleBoutons);
    boutonLayout->addWidget(boutonSauvegarder);

    layout->addLayout(boutonLayout);

    // Petite marge en bas
    layout->addStretch(1);

    // Connection signal et slots
    connect(boutonRetour, &QPushButton::clicked, this, &OptionsWidget::retourPressed);
    connect(boutonSauvegarder, &QPushButton::clicked, this, &OptionsWidget::sauvegarderPressed);
}