/*==================================================
* NOM: optionswidget.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans optionswidget.h
===================================================*/

#include "optionswidget.h"

OptionsWidget::OptionsWidget(QWidget* parent) : QWidget(parent)
{
    // Fond d'�cran
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

	QLineEdit* distanceAffichageInput = new QLineEdit(darkOverlay);
	distanceAffichageInput->setValidator(new QIntValidator(20, 1000, distanceAffichageInput));
	distanceAffichageInput->setText(QString::number(m_settings.value("distance_affichage", 600).toInt()));
	distanceAffichageInput->setFixedWidth(100);
	distanceAffichageInput->setAlignment(Qt::AlignCenter);
	distanceAffichageInput->setFont(QFont("Helvetica", 20, QFont::Bold));

	distanceAffichageLayout->addWidget(distanceAffichageLabel);
	distanceAffichageLayout->addWidget(distanceAffichageInput);
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

    QLineEdit* hauteurCameraInput = new QLineEdit(darkOverlay);
    hauteurCameraInput->setValidator(new QIntValidator(20, 700, hauteurCameraInput));
    hauteurCameraInput->setText(QString::number(m_settings.value("hauteur_camera", 15).toInt()));
    hauteurCameraInput->setFixedWidth(100);
    hauteurCameraInput->setAlignment(Qt::AlignCenter);
    hauteurCameraInput->setFont(QFont("Helvetica", 20, QFont::Bold));

    hauteurCameraLayout->addWidget(hauteurCameraLabel);
    hauteurCameraLayout->addWidget(hauteurCameraInput);
    hauteurCameraLayout->addStretch();
    settingsLayout->addLayout(hauteurCameraLayout);

    layout->addLayout(settingsLayout);

    layout->addStretch(4);

    auto validateInputs = [=]() {
        m_didValueChange = true;

        bool distanceValid = distanceAffichageInput->hasAcceptableInput();
        bool hauteurValid = hauteurCameraInput->hasAcceptableInput();

        m_boutonSauvegarder->setEnabled(distanceValid && hauteurValid);
    };

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
            m_settings.setValue("distance_affichage", distanceAffichageInput->text().toInt());
            m_settings.setValue("hauteur_camera", hauteurCameraInput->text().toInt());
            resetValueChanged();
            emit sauvegarderPressed();
            return;
        }
    });
    connect(volumeSlider, &QSlider::valueChanged, this, validateInputs);
    connect(distanceAffichageInput, &QLineEdit::textChanged, this, validateInputs);
    connect(hauteurCameraInput, &QLineEdit::textChanged, this, validateInputs);
}

void OptionsWidget::valueChanged() {
    m_didValueChange = true;
    m_boutonSauvegarder->setEnabled(true);
}

void OptionsWidget::resetValueChanged()
{
    m_didValueChange = false;
    m_boutonSauvegarder->setEnabled(false);
}
