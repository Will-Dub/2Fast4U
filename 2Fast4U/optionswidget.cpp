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
    layout->addWidget(title);;

    QVBoxLayout* settingsLayout = new QVBoxLayout();
    settingsLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout* volumeLayout = new QHBoxLayout();
    volumeLayout->setAlignment(Qt::AlignCenter);
    volumeLayout->setSpacing(20);

    volumeLayout->addStretch();

    QLabel* volumeLabel = new QLabel(QString::fromUtf8("Volume :"), darkOverlay);
    volumeLabel->setFont(QFont("Helvetica", 20, QFont::Bold));
    volumeLabel->setStyleSheet("color: white; background: transparent;");

    QSlider* volumeSlider = new QSlider(Qt::Horizontal, darkOverlay);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(m_settings.value("sonds", 50).toInt());
    volumeSlider->setFixedWidth(300);

    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { border: 1px solid #bbb; background: white; height: 10px; border-radius: 4px; }"
        "QSlider::sub-page:horizontal { background: #4CAF50; height: 10px; border-radius: 4px; }"
        "QSlider::add-page:horizontal { background: #555; height: 10px; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: white; border: 2px solid #777; width: 20px; margin-top: -6px; margin-bottom: -6px; border-radius: 10px; }"
    );

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
        m_settings.setValue("sonds", volumeSlider->value());
        resetValueChanged();
        emit sauvegarderPressed();
    });
    connect(volumeSlider, &QSlider::valueChanged, this, &OptionsWidget::valueChanged);
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
