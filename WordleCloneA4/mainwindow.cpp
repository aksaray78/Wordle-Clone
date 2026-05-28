#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFont>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "keyboard.h"
#include "wordlegrid.h"
#include <QDebug>
#include <QKeyEvent>
#include <QFrame>
#include <QScrollArea>
#include <QGraphicsBlurEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowTitle("Puzzle Kata");
    setMinimumSize(900,700);

    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    judulGame = new QLabel(this);
    judulGame->setText("PUZZLE KATA");
    judulGame->setGeometry(280, 25, 340, 40);
    QFont fontJudul;
    fontJudul.setPointSize(20);
    fontJudul.setBold(true);
    judulGame->setFont(fontJudul);
    judulGame->setAlignment(Qt::AlignCenter);
    judulGame->raise();

    labelInfo = new QLabel(this);
    labelInfo->setGeometry(280, 60, 340, 25);
    labelInfo->setAlignment(Qt::AlignCenter);
    labelInfo->setStyleSheet("color: #00A651; font-size:12px; font-weight:normal;");
    labelInfo->raise();

    QDialog *dialogNama = new QDialog(this);
    dialogNama->setWindowTitle("Selamat Datang!");
    dialogNama->setFixedSize(320, 150);
    dialogNama->setModal(true);

    QVBoxLayout *layoutDialog = new QVBoxLayout(dialogNama);
    QLabel *labelTeks = new QLabel("Masukkan nama kamu:");
    inputNama = new QLineEdit();
    QPushButton *btnOkDialog = new QPushButton("OK");

    layoutDialog->addWidget(labelTeks);
    layoutDialog->addWidget(inputNama);
    layoutDialog->addWidget(btnOkDialog);

    connect(btnOkDialog, &QPushButton::clicked, dialogNama, &QDialog::accept);

    if (dialogNama->exec() == QDialog::Accepted) {
        namaPemain = inputNama->text().trimmed();
        if (namaPemain.isEmpty()) namaPemain = "Pemain Misterius";
        labelInfo->setText("🎉 Selamat datang, " + namaPemain + "! Tebak kata hari ini.");
    }

    delete dialogNama;

    connect(ui->btnTutorial, &QPushButton::clicked,
            this, &MainWindow::tampilkanTutorial);

    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);

    layout->setContentsMargins(20,20,20,20);
    layout->setSpacing(10);

    layout->addWidget(
        judulGame,
        0,
        Qt::AlignHCenter
        );

    layout->addWidget(
        labelInfo,
        0,
        Qt::AlignHCenter
        );

    layout->addSpacing(20);

    QWidget *middleArea = new QWidget;

    QVBoxLayout *middleLayout =
        new QVBoxLayout(middleArea);

    middleLayout->addStretch();

    gameGrid = new WordleGrid(this);

    middleLayout->addWidget(
        gameGrid,
        0,
        Qt::AlignCenter
        );

    middleLayout->addStretch();

    layout->addWidget(
        middleArea,
        1
        );

    gameKeyboard = new Keyboard(this);

    layout->addWidget(
        gameKeyboard,
        0,
        Qt::AlignBottom
        );

    connect(gameKeyboard, &Keyboard::keyTyped, this, [this](const QString &key){
        processInput(key);
    });
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QString keyText = event->text().toUpper();

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        processInput("ENTER");
    } else if (event->key() == Qt::Key_Backspace) {
        processInput("BACK");
    } else if (!keyText.isEmpty() && keyText[0].isLetter()) {
        processInput(keyText);
    }
}

void MainWindow::processInput(const QString &key) {
    if (gameKeyboard) {
        gameKeyboard->animateKeyPress(key);
    }

    qDebug() << "Input Received:" << key;

    if (key == "BACK") {
        gameGrid->removeLetter();
    } else if (key == "ENTER") {
    } else if (key.length() == 1) {
        gameGrid->insertLetter(key);
    }
}

void MainWindow::simpanNamaUser() {}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tampilkanTutorial()
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(10);
    ui->centralwidget->setGraphicsEffect(blur);

    QDialog dialog(this);
    dialog.setModal(true);
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog.setAttribute(Qt::WA_TranslucentBackground);
    dialog.resize(this->size());

    dialog.setStyleSheet(
        "QDialog { background-color: rgba(0, 0, 0, 145); }"
        "#guideCard { background-color: #1b1f2a; border-radius: 22px; border: 2px solid #2f3545; }"
        "#mainTitle { color: #f5f5f5; font-size: 30px; font-weight: 900; }"
        "#subtitle { color: #bfc7d5; font-size: 15px; }"
        "#sectionTitle { color: #ffffff; font-size: 18px; font-weight: 800; }"
        "#bodyText { color: #d6dbe5; font-size: 15px; }"
        "#closeButton { background-color: #2f3545; color: white; border: none; border-radius: 14px; font-size: 18px; font-weight: bold; }"
        "#closeButton:hover { background-color: #444b5f; }"
        "#startButton { background-color: #4f8cff; color: white; border: none; border-radius: 12px; padding: 10px 18px; font-size: 15px; font-weight: 700; }"
        "#startButton:hover { background-color: #6fa1ff; }"
        );

    QVBoxLayout *overlayLayout = new QVBoxLayout(&dialog);
    overlayLayout->setContentsMargins(35, 25, 35, 25);
    overlayLayout->addStretch();

    QFrame *card = new QFrame;
    card->setObjectName("guideCard");
    card->setFixedSize(760, 560);
    overlayLayout->addWidget(card, 0, Qt::AlignCenter);
    overlayLayout->addStretch();

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(30, 24, 30, 24);
    cardLayout->setSpacing(14);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    QVBoxLayout *titleLayout = new QVBoxLayout;
    QLabel *title = new QLabel("Panduan Bermain");
    title->setObjectName("mainTitle");
    QLabel *subtitle = new QLabel("Sini gwe tutorin deks.");
    subtitle->setObjectName("subtitle");
    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);

    QPushButton *closeButton = new QPushButton("×");
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(38, 38);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(closeButton, 0, Qt::AlignTop);
    cardLayout->addLayout(headerLayout);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #394050; max-height: 1px;");
    cardLayout->addWidget(line);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background: transparent;");

    QWidget *content = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(16);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *ruleTitle = new QLabel("Misi Utama");
    ruleTitle->setObjectName("sectionTitle");
    contentLayout->addWidget(ruleTitle);

    QLabel *rules = new QLabel(
        "Cara mainnya coba nebak satu kata rahasia sebelum kesempatan habis.<br>"
        "Setiap tebakan akan diberi petunjuk lewat warna kotak.<br><br>"
        "<b>Aturan singkat:</b><br>"
        "• Pemain memiliki maksimal <b>6 percobaan</b>.<br>"
        "• Kata yang dimasukkan harus sesuai jumlah huruf.<br>"
        "• Tekan <b>Enter</b> untuk mengirim jawaban.<br>"
        "• Warna kotak akan membantu menentukan tebakan berikutnya."
        );
    rules->setObjectName("bodyText");
    rules->setWordWrap(true);
    rules->setTextFormat(Qt::RichText);
    contentLayout->addWidget(rules);

    QLabel *colorTitle = new QLabel("Kode Warna");
    colorTitle->setObjectName("sectionTitle");
    contentLayout->addWidget(colorTitle);

    auto makeColorBox = [](const QString &text, const QString &color, const QString &desc) -> QWidget*
    {
        QWidget *row = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(row);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(14);

        QLabel *box = new QLabel(text);
        box->setAlignment(Qt::AlignCenter);
        box->setFixedSize(50, 50);
        box->setStyleSheet(QString(
                               "background-color: %1;"
                               "color: white;"
                               "border-radius: 10px;"
                               "font-size: 24px;"
                               "font-weight: 900;"
                               ).arg(color));

        QLabel *label = new QLabel(desc);
        label->setObjectName("bodyText");
        label->setWordWrap(true);
        label->setTextFormat(Qt::RichText);

        layout->addWidget(box);
        layout->addWidget(label);
        layout->addStretch();

        return row;
    };

    contentLayout->addWidget(makeColorBox("A", "#3fa66b", "<b>Hijau</b> berarti huruf benar dan posisinya juga benar."));
    contentLayout->addWidget(makeColorBox("B", "#d4a93f", "<b>Kuning</b> berarti huruf ada di kata, tetapi posisinya belum tepat."));
    contentLayout->addWidget(makeColorBox("C", "#5b6270", "<b>Abu-abu</b> berarti huruf tidak ada di kata jawaban."));

    QLabel *tipsTitle = new QLabel("Tips Cepat");
    tipsTitle->setObjectName("sectionTitle");
    contentLayout->addWidget(tipsTitle);
    QLabel *tips = new QLabel(
        "• Mulai dengan kata yang punya huruf berbeda-beda.<br>"
        "• Jangan ulang huruf abu-abu kalau tidak perlu.<br>"
        "• Gunakan huruf kuning di posisi lain.<br>"
        "• Prioritaskan huruf hijau karena posisinya sudah terkunci."
        );
    tips->setObjectName("bodyText");
    tips->setWordWrap(true);
    tips->setTextFormat(Qt::RichText);
    contentLayout->addWidget(tips);

    QLabel *difficultyTitle = new QLabel("Mode Difficulty");
    difficultyTitle->setObjectName("sectionTitle");
    contentLayout->addWidget(difficultyTitle);
    QLabel *difficulty = new QLabel(
        "• <b>Normal</b>: 5 huruf, cocok untuk pemula.<br>"
        "• <b>Hard</b>: 6 huruf, lebih panjang lebih menantang.<br>"
        "• <b>Extreme</b>: 7 huruf, mikir kids."
        );
    difficulty->setObjectName("bodyText");
    difficulty->setWordWrap(true);
    difficulty->setTextFormat(Qt::RichText);
    contentLayout->addWidget(difficulty);

    contentLayout->addStretch();
    scrollArea->setWidget(content);
    cardLayout->addWidget(scrollArea);

    QHBoxLayout *footerLayout = new QHBoxLayout;
    footerLayout->addStretch();
    QPushButton *startButton = new QPushButton("Mengerti");
    startButton->setObjectName("startButton");
    footerLayout->addWidget(startButton);
    cardLayout->addLayout(footerLayout);

    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(startButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.exec();
    ui->centralwidget->setGraphicsEffect(nullptr);
}