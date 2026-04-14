/*==================================================
* NOM: optionswidget.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans optionswidget.h
===================================================*/

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

    QVBoxLayout* settingsLayout = new QVBoxLayout();
    settingsLayout->setAlignment(Qt::AlignCenter);

     // Volume
    QHBoxLayout* volumeLayout = new QHBoxLayout();
    volumeLayout->setAlignment(Qt::AlignCenter);
    volumeLayout->setSpacing(20);

    volumeLayout->addStretch();

    QLabel* volumeLabel = new QLabel(QString::fromUtf8("Volume :"), darkOverlay);
    volumeLabel->setFont(QFont("Helvetica", 20, QFont::Bold));
    volumeLabel->setStyleSheet("color: white; background: transparent;");

    QSlider* volumeSlider = new QSlider(Qt::Horizontal, darkOverlay);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(m_settings.value("sons", 50).toInt());
    volumeSlider->setFixedWidth(300);

    QLabel* volumeValueLabel = new QLabel(QString::number(volumeSlider->value()) + "%", darkOverlay);
    volumeValueLabel->setFont(QFont("Helvetica", 20, QFont::Bold));
    volumeValueLabel->setStyleSheet("color: white; background: transparent;");
    volumeValueLabel->setFixedWidth(70);

    connect(volumeSlider, &QSlider::valueChanged, this, [=](int value) {
        volumeValueLabel->setText(QString::number(value) + "%");
    });

    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(volumeValueLabel);
    volumeLayout->addStretch();

    settingsLayout->addLayout(volumeLayout);

	// Distance d'affichage
	QHBoxLayout* distanceAffichageLayout = new QHBoxLayout();
	distanceAffichageLayout->setAlignment(Qt::AlignCenter);
	distanceAffichageLayout->setSpacing(20);
	distanceAffichageLayout->addStretch();

	QLabel* distanceAffichageLabel = new QLabel(QString::fromUtf8("Distance d'affichage :"), darkOverlay);
	distanceAffichageLabel->setFont(QFont("Helvetica", 20, QFont::Bold));
	distanceAffichageLabel->setStyleSheet("color: white; background: transparent;");
	distanceAffichageLabel->setFixedWidth(300);

	m_distanceAffichageInput = new QSpinBox(darkOverlay);
	m_distanceAffichageInput->setRange(20, 1000);
	m_distanceAffichageInput->setValue(m_settings.value("distance_affichage", 600).toInt());
	m_distanceAffichageInput->setFixedWidth(175);
	m_distanceAffichageInput->setAlignment(Qt::AlignCenter);
	m_distanceAffichageInput->setFont(QFont("Helvetica", 20, QFont::Bold));

	distanceAffichageLayout->addWidget(distanceAffichageLabel);
	distanceAffichageLayout->addWidget(m_distanceAffichageInput);
	distanceAffichageLayout->addStretch();
	settingsLayout->addLayout(distanceAffichageLayout);

    // hauteur camera
    QHBoxLayout* hauteurCameraLayout = new QHBoxLayout();
    hauteurCameraLayout->setAlignment(Qt::AlignCenter);
    hauteurCameraLayout->setSpacing(20);
    hauteurCameraLayout->addStretch();

    QLabel* hauteurCameraLabel = new QLabel(QString::fromUtf8("Hauteur caméra :"), darkOverlay);
    hauteurCameraLabel->setFont(QFont("Helvetica", 20, QFont::Bold));
    hauteurCameraLabel->setStyleSheet("color: white; background: transparent;");
    hauteurCameraLabel->setFixedWidth(300);

    m_hauteurCameraInput = new QSpinBox(darkOverlay);
    m_hauteurCameraInput->setRange(1, 40);
    m_hauteurCameraInput->setValue(m_settings.value("hauteur_camera", 15).toInt());
    m_hauteurCameraInput->setFixedWidth(175);
    m_hauteurCameraInput->setAlignment(Qt::AlignCenter);
    m_hauteurCameraInput->setFont(QFont("Helvetica", 20, QFont::Bold));

    hauteurCameraLayout->addWidget(hauteurCameraLabel);
    hauteurCameraLayout->addWidget(m_hauteurCameraInput);
    hauteurCameraLayout->addStretch();
    settingsLayout->addLayout(hauteurCameraLayout);

    layout->addLayout(settingsLayout);

    layout->addStretch(4);

    QFont fontBoutons("Helvetica", 24, QFont::Bold);

    QHBoxLayout* boutonLayout = new QHBoxLayout();
    boutonLayout->setAlignment(Qt::AlignCenter);
    boutonLayout->setSpacing(15);

    // Bouton retour
    HoverButton* boutonRetour = new HoverButton("Retour", darkOverlay);
    boutonRetour->setFixedSize(200, 80);
    boutonRetour->setFont(fontBoutons);
    boutonLayout->addWidget(boutonRetour);

    // Bouton sauvegarder
    m_boutonSauvegarder = new HoverButton("Sauvegarder", darkOverlay);
    m_boutonSauvegarder->setFixedSize(250, 80);
    m_boutonSauvegarder->setFont(fontBoutons);
    boutonLayout->addWidget(m_boutonSauvegarder);

    layout->addLayout(boutonLayout);

    // Petite marge en bas
    layout->addStretch(1);

    // Connection signal et slots
    connect(boutonRetour, &HoverButton::clicked, this, [=]() {
        if (m_didValueChange) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Confirme", "Voulez-vous quitter sans sauvegarder?",
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No) {
                return;
            }
        }
        emit retourPressed();
    });
    connect(m_boutonSauvegarder, &HoverButton::clicked, this, [=]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirme", "Voulez-vous vraiment sauvegarder?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            m_settings.setValue("volume", volumeSlider->value());
            m_settings.setValue("distance_affichage", m_distanceAffichageInput->value());
            m_settings.setValue("hauteur_camera", m_hauteurCameraInput->value());
            resetValueChanged();
            emit sauvegarderPressed();
            return;
        }
    });
    connect(volumeSlider, &QSlider::valueChanged, this, &OptionsWidget::valueChanged);
    connect(m_distanceAffichageInput, &QSpinBox::valueChanged, this, &OptionsWidget::valueChanged);
    connect(m_hauteurCameraInput, &QSpinBox::valueChanged, this, &OptionsWidget::valueChanged);
}

void OptionsWidget::valueChanged() {
    m_didValueChange = true;

    bool distanceValid = m_distanceAffichageInput->hasAcceptableInput();
    bool hauteurValid = m_hauteurCameraInput->hasAcceptableInput();

    m_boutonSauvegarder->setEnabled(distanceValid && hauteurValid);
}

void OptionsWidget::resetValueChanged()
{
	m_hauteurCameraInput->setValue(m_settings.value("hauteur_camera", 15).toInt());
	m_distanceAffichageInput->setValue(m_settings.value("distance_affichage", 600).toInt());

    m_didValueChange = false;
    m_boutonSauvegarder->setEnabled(false);
}
