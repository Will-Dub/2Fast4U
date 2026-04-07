#include "endwidget.h"

EndWidget::EndWidget(QWidget* parent) : QWidget(parent)
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
    m_titre = new QLabel("", darkOverlay);
    QFont titreFont("Helvetica", 50, QFont::Bold);
    m_titre->setFont(titreFont);
    m_titre->setStyleSheet("color: white; background: transparent;");
    m_titre->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_titre);

    m_sousTitre = new QLabel("", darkOverlay);
    QFont sousTitreFont("Helvetica", 35, QFont::Bold);
    m_sousTitre->setFont(sousTitreFont);
    m_sousTitre->setStyleSheet("color: white; background: transparent;");
    m_sousTitre->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_sousTitre);

    layout->addStretch(3);

    QFont fontBoutons("Helvetica", 24, QFont::Bold);
    QString styleBoutons = "background-color: white; color: #1a1a1a; border-radius: 10px;";

    QHBoxLayout* boutonLayout = new QHBoxLayout();
    boutonLayout->setAlignment(Qt::AlignCenter);
    boutonLayout->setSpacing(15);

    // Bouton retour
    QPushButton* boutonRecommencer = new QPushButton("Recommencer", darkOverlay);
    boutonRecommencer->setFixedSize(250, 80);
    boutonRecommencer->setFont(fontBoutons);
    boutonRecommencer->setStyleSheet(styleBoutons);
    boutonLayout->addWidget(boutonRecommencer);

    // Bouton sauvegarder
    QPushButton* boutonQuitter = new QPushButton("Quitter", darkOverlay);
    boutonQuitter->setFixedSize(200, 80);
    boutonQuitter->setFont(fontBoutons);
    boutonQuitter->setStyleSheet(styleBoutons);
    boutonLayout->addWidget(boutonQuitter);

    layout->addLayout(boutonLayout);
    layout->addStretch(1);

    // Connection signal et slots
    connect(boutonRecommencer, &QPushButton::clicked, this, [=]() {
		emit recommencerPressed(m_nom);
    });
    connect(boutonQuitter, &QPushButton::clicked, this, &EndWidget::quitterPressed);
}

void EndWidget::setText(const QString& titre, const QString& sousTitre)
{
	m_titre->setText(titre);
	m_sousTitre->setText(sousTitre);
}

void EndWidget::setGameSnapshot(const QPixmap& snapshot)
{
    m_gameSnapshot = snapshot;
    update();
}

void EndWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (!m_gameSnapshot.isNull()) {
        painter.drawPixmap(this->rect(), m_gameSnapshot);
    }

    QWidget::paintEvent(event);
}