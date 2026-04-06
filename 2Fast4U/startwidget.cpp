#include "startwidget.h"

StartWidget::StartWidget(QWidget* parent) : QWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);

    // Fond d'écran
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("StartWidget { border-image: url(:/images/background.png) 0 0 0 0 stretch stretch; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* darkOverlay = new QFrame(this);
    darkOverlay->setStyleSheet("QFrame { background-color: rgba(0, 0, 0, 150); }");
    mainLayout->addWidget(darkOverlay);

    // Layout titre et boutons
    QVBoxLayout* layout = new QVBoxLayout(darkOverlay);

    layout->addStretch(1);

    // Titre
    QLabel* title = new QLabel("2Fast4U", darkOverlay);
    QFont titleFont("Helvetica", 50, QFont::Bold);
    title->setFont(titleFont);
    title->setStyleSheet("color: white; background: transparent;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    layout->addStretch(1);

    // classement
    QLabel* classementLabel = new QLabel("Classement:");
    QFont classementLabelFont("Helvetica", 30);
    classementLabel->setFont(classementLabelFont);
    classementLabel->setStyleSheet("color: white; background: transparent;");
    classementLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(classementLabel);

    QList<Score> scores = RaceManager::getTopThree();
    QFont scoreFont("Courier", 24, QFont::Bold);

    // Aucun score
    if (scores.isEmpty()) {
        QLabel* aucunScoreLabel = new QLabel("Aucun temps enregistre", darkOverlay);
        aucunScoreLabel->setFont(scoreFont);
        aucunScoreLabel->setStyleSheet("color: gray; background: transparent;");
        aucunScoreLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(aucunScoreLabel);
    }
    else {
        for (int i = 0; i < scores.size(); i++) {
            QString texteColor;
            if (i == 0) {
                texteColor = "#FFD700";
            }
            else if (i == 1) {
                texteColor = "#C0C0C0";
            }
            else if (i == 2) {
                texteColor = "#B87333";
            }
            else {
                texteColor = "white";
            }

            QString texteScore = QString("%1. %2 - %3")
                .arg(i + 1)
                .arg(scores[i].nom)
                .arg(scores[i].temps);

            QLabel* scoreLabel = new QLabel(texteScore, darkOverlay);
            scoreLabel->setFont(scoreFont);
            scoreLabel->setStyleSheet(QString("color: %1; background: transparent;").arg(texteColor));
            scoreLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(scoreLabel);

            if (i < scores.size() - 1) {
                layout->addSpacing(10);
            }
        }
    }

    layout->addStretch(2);


    // Input nom
    QLabel* nomInputLabel = new QLabel("Entrez votre nom:");
    QFont nomInputLabelFont("Helvetica", 30);
    nomInputLabel->setFont(nomInputLabelFont);
    nomInputLabel->setStyleSheet("color: white; background: transparent;");
    nomInputLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(nomInputLabel);

    m_nomInput = new QLineEdit();
    QFont nomInputFont("Helvetica", 20, QFont::Bold);
    m_nomInput->setMaxLength(15);
    m_nomInput->setFont(nomInputFont);
    m_nomInput->setFixedSize(500, nomInputFont.pointSize() + 30);
    m_nomInput->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_nomInput, 0, Qt::AlignCenter);
    layout->addStretch(1);

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
    QPushButton* boutonCommencer = new QPushButton("Commencer", darkOverlay);
    boutonCommencer->setFixedSize(250, 80);
    boutonCommencer->setFont(fontBoutons);
    boutonCommencer->setStyleSheet(styleBoutons);
    boutonLayout->addWidget(boutonCommencer);

    layout->addLayout(boutonLayout);

    layout->addStretch(1);

    // Connection signal et slots
    connect(boutonRetour, &QPushButton::clicked, this, &StartWidget::retourPressed);
    connect(boutonCommencer, &QPushButton::clicked, this, [this]() {
        const QString nom = m_nomInput->text();
        emit commencerPressed(nom);
    });
}

void StartWidget::resetInput()
{
    m_nomInput->clear();
}
