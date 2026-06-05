#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <vector>
#include <QFileDialog>
#include <QPixmap>
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
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    setWindowTitle("char4char");
    setMinimumSize(550,750);

    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    judulGame = new QLabel(this);
    judulGame->setText("📄char4char");

    QFont fontJudul("Segoe UI", 28, QFont::Bold);
    judulGame->setFont(fontJudul);
    judulGame->setAlignment(Qt::AlignCenter);
    judulGame->setStyleSheet("color: white; margin-top: 10px;");

    labelInfo = new QLabel(this);
    labelInfo->setAlignment(Qt::AlignCenter);
    labelInfo->setStyleSheet("color: #4f8cff; font-size: 14px; font-weight: 500;");

    QDialog *dialogNama = new QDialog(this);
    dialogNama->setWindowTitle("Selamat bermain char4char!");
    dialogNama->setFixedSize(320, 140);
    dialogNama->setModal(true);

    QVBoxLayout *layoutDialog = new QVBoxLayout(dialogNama);
    layoutDialog->setContentsMargins(25, 15, 25, 15);
    layoutDialog->setSpacing(4);

    QLabel *labelTeks = new QLabel("Masukkan nama kamu:");
    labelTeks->setStyleSheet("margin: 0px; padding: 0px; color: white; font-weight: bold;");

    inputNama = new QLineEdit();
    inputNama->setPlaceholderText("Nama kamu...");

    QPushButton *btnOkDialog = new QPushButton("OK");
    btnOkDialog->setMinimumHeight(30);

    layoutDialog->addWidget(labelTeks);
    layoutDialog->addSpacing(10);
    layoutDialog->addWidget(inputNama);
    layoutDialog->addSpacing(10);
    layoutDialog->addWidget(btnOkDialog);

    layoutDialog->addStretch(1);
    connect(btnOkDialog, &QPushButton::clicked, dialogNama, &QDialog::accept);

    if (dialogNama->exec() == QDialog::Accepted) {
        namaPemain = inputNama->text().trimmed();
        if (namaPemain.isEmpty()) namaPemain = "John Doe";
        labelInfo->setText("Selamat datang, " + namaPemain + "! Mari bermain!");
    }

    delete dialogNama;

    QPushButton *btnTutorial = new QPushButton("?");
    btnTutorial->setFixedSize(40, 40);

    QPushButton *btnSettings = new QPushButton("⚙");
    btnSettings->setFixedSize(40, 40);

    connect(btnTutorial, &QPushButton::clicked, this, &MainWindow::tampilkanTutorial);
    connect(btnSettings, &QPushButton::clicked, this, &MainWindow::tampilkanSettings);

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->setContentsMargins(0, 0, 0, 0);
    topBar->addWidget(btnTutorial, 0, Qt::AlignLeft);
    topBar->addStretch();
    topBar->addWidget(btnSettings, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(30, 10, 30, 0);
    mainLayout->setSpacing(0);

    QPushButton *btnStatistik = new QPushButton("📊");
    btnStatistik->setFixedSize(40, 40);
    connect(btnStatistik, &QPushButton::clicked, this, &MainWindow::tampilkanStatistik);

    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->addWidget(btnTutorial, 0, Qt::AlignLeft | Qt::AlignTop);
    headerRow->addWidget(judulGame, 1, Qt::AlignHCenter | Qt::AlignTop);
    headerRow->addWidget(btnStatistik, 0, Qt::AlignRight | Qt::AlignTop);
    headerRow->addSpacing(8);
    headerRow->addWidget(btnSettings, 0, Qt::AlignRight | Qt::AlignTop);

    mainLayout->addLayout(headerRow);

    mainLayout->addSpacing(15);
    mainLayout->addWidget(labelInfo, 0, Qt::AlignHCenter);

    mainLayout->addSpacing(15);

    gameGrid = new WordleGrid(this);
    mainLayout->addWidget(gameGrid, 1, Qt::AlignCenter);

    mainLayout->addStretch(1);

    gameKeyboard = new Keyboard(this);
    mainLayout->addWidget(gameKeyboard, 0, Qt::AlignBottom);

    connect(gameKeyboard, &Keyboard::keyTyped, this, [this](const QString &key){
        processInput(key);
    });

    loadWordLists();
    startNewGame();
    applyTheme();
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
        if (!currentGuess.isEmpty()) {
            currentGuess.chop(1);
            currentLetterIndex--;
        }
    }
    else if (key == "ENTER") {
        if (currentGuess.length() < 5) {
            QMessageBox::warning(this, "Kata Belum Lengkap",
                                 "Kata belum lengkap! Pastikan kotak terisi 5 huruf.");
            return;
        }

        QString pesanHardMode;
        if (!cekHardMode(currentGuess.toUpper(), pesanHardMode)) {
            gameGrid->glowRowRed();

            QMessageBox::warning(this, "Hard Mode", pesanHardMode);

            for (int i = 0; i < currentGuess.length(); ++i) {
                gameGrid->removeLetter();
            }

            currentGuess = "";
            currentLetterIndex = 0;

            setFocus();

            return;
        }

        if (!validWordsList.contains(currentGuess.toUpper())) {
            gameGrid->glowRowRed();
            QCoreApplication::processEvents();
            QMessageBox::critical(this, "Kata Tidak Valid",
                                  "Kata tidak ada di dalam kamus resmi!");

            for (int i = 0; i < 5; ++i) {
                gameGrid->removeLetter();
            }

            currentGuess = "";
            currentLetterIndex = 0;
            return;
        }

        checkGuess();
        currentGuess = "";
        currentLetterIndex = 0;
    }
    else if (key.length() == 1) {
        if (currentLetterIndex < 5) {
            QString upperKey = key.toUpper();
            currentGuess.append(upperKey);
            currentLetterIndex++;
            gameGrid->insertLetter(upperKey);
        }
    }
}

void MainWindow::simpanNamaUser() {}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::gameSudahMulai() const
{
    return currentAttempt > 0 || !currentGuess.isEmpty();
}

void MainWindow::applyTheme()
{
    if (darkTheme) {
        this->setStyleSheet(
            "QMainWindow { background-color: #121213; }"
            "QWidget#centralwidget { background-color: #121213; }"
            "QPushButton { background-color: #3a3a3c; color: white; border-radius: 6px; }"
            "QPushButton:hover { background-color: #565758; }"
            );
        if (labelInfo) labelInfo->setStyleSheet("color: #4f8cff; font-size: 14px; font-weight: 500;");
        if (judulGame) judulGame->setStyleSheet("color: white;");
        if (gameGrid)
            gameGrid->setThemeColors("white", "#3a3a3c");
            gameGrid->updateEmptyCells("#ffffff", "#3a3a3c");
    } else {
        this->setStyleSheet(
            "QMainWindow { background-color: #f5f5f5; }"
            "QWidget#centralwidget { background-color: #f5f5f5; }"
            "QPushButton { background-color: #d3d6da; color: #121213; border-radius: 6px; }"
            "QPushButton:hover { background-color: #bfc2c6; }"
            );
        if (labelInfo) labelInfo->setStyleSheet("color: #1a73e8; font-size: 14px; font-weight: 500;");
        if (judulGame) judulGame->setStyleSheet("color: #121213;");
        if (gameGrid)
            gameGrid->setThemeColors("#121213", "#878a8c");
            gameGrid->updateEmptyCells("#121213", "#878a8c");
    }
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
        "#startButton { background-color: #565758; color: white; border: none; border-radius: 12px; padding: 10px 18px; font-size: 15px; font-weight: 700; }"
        "#startButton:hover { background-color: #4f8cff; }"
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
        "Tugas pemain adalah menebak kata rahasia dalam <b>6 percobaan</b>.<br>"
        "Setiap tebakan harus berupa kata valid dan berjumlah <b>5 huruf</b>.<br><br>"
        "<b>Aturan warna:</b><br>"
        "• Hijau: huruf benar dan posisinya benar.<br>"
        "• Kuning: huruf ada di kata, tetapi posisinya salah.<br>"
        "• Abu-abu: huruf tidak ada di kata jawaban.<br><br>"
        "<b>Mode permainan:</b><br>"
        "• Normal Mode: pemain bebas menebak kata valid apa saja.<br>"
        "• Hard Mode: petunjuk hijau dan kuning wajib digunakan pada tebakan berikutnya."
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
        "• <b>Hard Mode</b>: hanya bisa diubah sebelum pemain mulai mengetik.<br>"
        "• Setelah permainan dimulai, Hard Mode dikunci agar aturan tidak berubah di tengah game."
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

void MainWindow::tampilkanSettings()
{
    QDialog dialog(this);
    dialog.setModal(true);
    dialog.setWindowTitle("Settings");
    dialog.setFixedSize(460, 300);
    dialog.setStyleSheet(
        "QDialog { background-color: #121213; color: white; }"
        "QLabel { color: white; }"
        "QPushButton { background-color: #3a3a3c; color: white; border-radius: 8px; padding: 8px; }"
        "QPushButton:hover { background-color: #565758; }"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("SETTINGS");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold;");
    mainLayout->addWidget(title);

    bool gameMulai = gameSudahMulai();

    QLabel *hardTitle = new QLabel("Hard Mode");
    hardTitle->setStyleSheet("font-size: 18px; font-weight: bold;");

    QLabel *hardDesc = new QLabel(
        gameMulai
            ? "Hard Mode hanya bisa diubah sebelum pemain mulai mengetik."
            : "Petunjuk hijau dan kuning wajib dipakai pada tebakan berikutnya."
        );
    hardDesc->setWordWrap(true);
    hardDesc->setStyleSheet("color: #d7dadc; font-size: 13px;");

    QPushButton *btnHardMode = new QPushButton(hardMode ? "Hard Mode: ON" : "Hard Mode: OFF");

    btnHardMode->setEnabled(!gameMulai);
    btnHardMode->setStyleSheet(
        gameMulai
            ? "background-color: #333333; color: #777777; border-radius: 8px; padding: 8px;"
            : hardMode
                  ? "background-color: #4f8cff; color: white; border-radius: 8px; padding: 8px;"
                  : "background-color: #565758; color: white; border-radius: 8px; padding: 8px;"
        );

    connect(btnHardMode, &QPushButton::clicked, [&]() {
        if (gameSudahMulai()) {
            QMessageBox::warning(
                this,
                "Tidak Bisa Diubah",
                "Hard Mode hanya bisa diubah sebelum kamu mulai mengetik."
                );
            return;
        }

        hardMode = !hardMode;

        btnHardMode->setText(hardMode ? "Hard Mode: ON" : "Hard Mode: OFF");
        btnHardMode->setStyleSheet(
            hardMode
                ? "background-color: #4f8cff; color: white; border-radius: 8px; padding: 8px;"
                : "background-color: #565758; color: white; border-radius: 8px; padding: 8px;"
            );
    });

    QFrame *line1 = new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("background-color: #3a3a3c; max-height: 1px;");

    QLabel *darkTitle = new QLabel("Dark Theme");
    darkTitle->setStyleSheet("font-size: 18px; font-weight: bold;");

    QLabel *darkDesc = new QLabel("Mengubah tampilan utama game menjadi gelap atau terang.");
    darkDesc->setWordWrap(true);
    darkDesc->setStyleSheet("color: #d7dadc; font-size: 13px;");

    QPushButton *btnDarkTheme = new QPushButton(darkTheme ? "Dark Theme: ON" : "Dark Theme: OFF");
    btnDarkTheme->setStyleSheet(
        darkTheme
            ? "background-color: #4f8cff; color: white; border-radius: 8px; padding: 8px;"
            : "background-color: #565758; color: white; border-radius: 8px; padding: 8px;"
        );

    connect(btnDarkTheme, &QPushButton::clicked, [&]() {
        darkTheme = !darkTheme;

        btnDarkTheme->setText(darkTheme ? "Dark Theme: ON" : "Dark Theme: OFF");
        btnDarkTheme->setStyleSheet(
            darkTheme
                ? "background-color: #4f8cff; color: white; border-radius: 8px; padding: 8px;"
                : "background-color: #565758; color: white; border-radius: 8px; padding: 8px;"
            );

        applyTheme();
    });

    QPushButton *btnClose = new QPushButton("Tutup");
    btnClose->setStyleSheet(
        "QPushButton { background-color: #3a3a3c; color: white; border-radius: 8px; padding: 8px; }"
        "QPushButton:hover { background-color: #4f8cff; color: white; }"
    );

    connect(btnClose, &QPushButton::clicked, &dialog, &QDialog::accept);

    mainLayout->addWidget(hardTitle);
    mainLayout->addWidget(hardDesc);
    mainLayout->addWidget(btnHardMode);

    mainLayout->addWidget(line1);

    mainLayout->addWidget(darkTitle);
    mainLayout->addWidget(darkDesc);
    mainLayout->addWidget(btnDarkTheme);

    mainLayout->addStretch();
    mainLayout->addWidget(btnClose);

    dialog.exec();

    setFocus();
}

bool MainWindow::cekHardMode(const QString &guess, QString &pesanError)
{
    if (!hardMode) {
        return true;
    }

    for (int i = 0; i < lockedPattern.length(); ++i) {
        if (lockedPattern[i] != '_' && guess[i] != lockedPattern[i]) {
            pesanError = QString("Hard Mode aktif. Huruf %1 wajib tetap di posisi %2.")
            .arg(lockedPattern[i])
                .arg(i + 1);
            return false;
        }
    }

    for (int i = 0; i < requiredLetters.length(); ++i) {
        QChar huruf = requiredLetters.at(i);

        if (!guess.contains(huruf)) {
            pesanError = QString("Hard Mode aktif. Huruf %1 wajib digunakan lagi.")
            .arg(huruf);
            return false;
        }
    }

    return true;
}

void MainWindow::loadWordLists() {
    QString appDir = QCoreApplication::applicationDirPath();
    QDir dir(appDir);

    QString answersPath = dir.absoluteFilePath("words/wordle-answers-alphabetical.txt");
    QString allowedPath = dir.absoluteFilePath("words/nyt-wordle-allowed-guesses-2026-03-06.txt");

    QFile answersFile(answersPath);
    if (answersFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&answersFile);
        while (!in.atEnd()) {
            QString word = in.readLine().trimmed().toUpper();
            if (!word.isEmpty()) targetWordsList.append(word);
        }
        answersFile.close();
    } else {
        qDebug() << "ERROR: File jawaban tidak ditemukan di:" << answersPath;
    }

    QFile allowedFile(allowedPath);
    if (allowedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&allowedFile);
        while (!in.atEnd()) {
            QString word = in.readLine().trimmed().toUpper();
            if (!word.isEmpty()) validWordsList.append(word);
        }
        allowedFile.close();
    } else {
        qDebug() << "ERROR: File kata validasi tidak ditemukan di:" << allowedPath;
    }
}

void MainWindow::startNewGame() {
    if (targetWordsList.isEmpty()) {
        qDebug() << "Gagal memulai game: Daftar kata target kosong!";
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(targetWordsList.size());
    targetWord = targetWordsList.at(randomIndex).toUpper();

    currentAttempt = 0;
    currentLetterIndex = 0;
    currentGuess = "";
    lockedPattern = "_____";
    requiredLetters = "";

    qDebug() << "Game Dimulai! Kata rahasia game ini adalah:" << targetWord;
}

void MainWindow::resetGame() {
    gameGrid->clearGrid();
    gameKeyboard->clearKeyboard();

    currentGuess = "";
    currentLetterIndex = 0;

    startNewGame();

    labelInfo->setText("Main lagi! Tebak kata baru.");
}

void MainWindow::checkGuess() {
    QString guess = currentGuess.toUpper();
    QString target = targetWord.toUpper();

    std::vector<int> letterStatus(5, 0);
    std::vector<bool> targetUsed(5, false);

    for (int i = 0; i < 5; ++i) {
        if (guess[i] == target[i]) {
            letterStatus[i] = 2;
            targetUsed[i] = true;
        }
    }

    for (int i = 0; i < 5; ++i) {
        if (letterStatus[i] == 2) continue;

        for (int j = 0; j < 5; ++j) {
            if (!targetUsed[j] && guess[i] == target[j]) {
                letterStatus[i] = 1;
                targetUsed[j] = true;
                break;
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        QString currentLetter = QString(guess[i]);
        if (letterStatus[i] == 2) {
            gameGrid->colorCell(i, "#3fa66b");
            gameKeyboard->updateKeyColor(currentLetter, "#3fa66b");

            lockedPattern[i] = guess[i];

        } else if (letterStatus[i] == 1) {
            gameGrid->colorCell(i, "#d4a93f");
            gameKeyboard->updateKeyColor(currentLetter, "#d4a93f");

            if (!requiredLetters.contains(guess[i])) {
                requiredLetters.append(guess[i]);
            }

        } else {
            gameGrid->colorCell(i, "#5b6270");
            gameKeyboard->updateKeyColor(currentLetter, "#5b6270");
        }
    }

    if (guess == target) {
        QDialog *winDialog = new QDialog(this);
        winDialog->setWindowTitle("KAMU MENANG!");
        winDialog->setFixedSize(350, 180);
        winDialog->setStyleSheet("background-color: #1b1f2a; color: white; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(winDialog);

        QLabel *winLabel = new QLabel(QString(
                                          "<h2>Luar biasa, %1!</h2>"
                                          "<p>Kamu berhasil menebak kata: <b>%2</b></p>"
                                          "<p>Jumlah Percobaan: <b>%3 / 6</b></p>"
                                          ).arg(namaPemain, targetWord, QString::number(currentAttempt + 1)));
        winLabel->setAlignment(Qt::AlignCenter);

        QPushButton *btnMainLagi = new QPushButton("Main Lagi");
        btnMainLagi->setStyleSheet(
            "QPushButton { background-color: #3a3a3c; color: white; padding: 10px; font-weight: bold; border-radius: 5px; }"
            "QPushButton:hover { background-color: #4f8cff; }"
        );
        connect(btnMainLagi, &QPushButton::clicked, winDialog, &QDialog::accept);

        layout->addWidget(winLabel);
        layout->addWidget(btnMainLagi);

        winDialog->exec();
        delete winDialog;

        statPlayed++;
        statWins++;
        statCurrentStreak++;
        if (statCurrentStreak > statMaxStreak) statMaxStreak = statCurrentStreak;
        statGuessDistribution[currentAttempt]++;

        resetGame();
        return;
    }

    currentAttempt++;

    gameGrid->moveToNextRow();

    if (currentAttempt >= 6) {
        QDialog *loseDialog = new QDialog(this);
        loseDialog->setWindowTitle("Game Over");
        loseDialog->setFixedSize(350, 180);
        loseDialog->setStyleSheet("color: white; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(loseDialog);

        QLabel *loseLabel = new QLabel(QString(
                                           "<h2>Kesempatan Habis!</h2>"
                                           "<p>Jangan menyerah, %1!</p>"
                                           "<p>Kata yang benar adalah: <b style='color: #4f8cff; font-size: 18px;'>%2</b></p>"
                                           ).arg(namaPemain, targetWord));
        loseLabel->setAlignment(Qt::AlignCenter);

        QPushButton *btnCobaLagi = new QPushButton("Coba Lagi");
        btnCobaLagi->setStyleSheet("background-color: #4f8cff; color: white; padding: 10px; font-weight: bold; border-radius: 5px;");
        connect(btnCobaLagi, &QPushButton::clicked, loseDialog, &QDialog::accept);

        layout->addWidget(loseLabel);
        layout->addWidget(btnCobaLagi);

        loseDialog->exec();
        delete loseDialog;

        statPlayed++;
        statCurrentStreak = 0;

        resetGame();
        return;
    }
}

void MainWindow::tampilkanStatistik()
{
    if (statPlayed == 0) {
        QMessageBox::information(this, "Belum Ada Data", "Lakukan tebakan pertama!");
        return;
    }
    QDialog dialog(this);
    dialog.setModal(true);
    dialog.setWindowTitle("Statistik");
    dialog.setFixedSize(400, 280);
    dialog.setStyleSheet(
        "QDialog { background-color: #121213; color: white; }"
        "QLabel { color: white; }"
        "QPushButton { background-color: #3a3a3c; color: white; border-radius: 8px; padding: 8px; }"
        "QPushButton:hover { background-color: #4f8cff; }"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    QLabel *title = new QLabel("STATISTIK");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(title);

    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    auto makeStat = [](const QString &value, const QString &label) -> QWidget* {
        QWidget *w = new QWidget;
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(2);

        QLabel *val = new QLabel(value);
        val->setAlignment(Qt::AlignCenter);
        val->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");

        QLabel *lbl = new QLabel(label);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setWordWrap(true);
        lbl->setStyleSheet("font-size: 11px; color: #d7dadc;");

        l->addWidget(val);
        l->addWidget(lbl);
        return w;
    };

    int winPct = statPlayed > 0 ? (statWins * 100 / statPlayed) : 0;
    statsRow->addWidget(makeStat(QString::number(statPlayed), "Game\nDimainkan"));
    statsRow->addWidget(makeStat(QString::number(winPct), "Menang %"));
    statsRow->addWidget(makeStat(QString::number(statCurrentStreak), "Streak\nBertahan"));
    statsRow->addWidget(makeStat(QString::number(statMaxStreak), "Streak\nTerpanjang"));
    mainLayout->addLayout(statsRow);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #3a3a3c; max-height: 1px;");
    mainLayout->addWidget(line);

    QLabel *distTitle = new QLabel("DISTRIBUSI TEBAKAN");
    distTitle->setAlignment(Qt::AlignCenter);
    distTitle->setStyleSheet("font-size: 14px; font-weight: bold;");
    mainLayout->addWidget(distTitle);

    int maxVal = 0;
    for (int i = 0; i < 6; ++i)
        if (statGuessDistribution[i] > maxVal) maxVal = statGuessDistribution[i];

    QHBoxLayout *distRow = new QHBoxLayout();
    distRow->setSpacing(10);

    for (int i = 0; i < 6; ++i) {
        QVBoxLayout *col = new QVBoxLayout();
        col->setSpacing(4);
        col->setAlignment(Qt::AlignHCenter);

        QLabel *count = new QLabel(QString::number(statGuessDistribution[i]));
        count->setAlignment(Qt::AlignCenter);
        count->setFixedSize(40, 40);
        count->setStyleSheet(
            statGuessDistribution[i] > 0
                ? "background-color: #3fa66b; color: white; font-size: 13px; font-weight: bold; border-radius: 6px;"
                : "background-color: #3a3a3c; color: white; font-size: 13px; border-radius: 6px;"
            );

        QLabel *num = new QLabel(QString::number(i + 1));
        num->setAlignment(Qt::AlignCenter);
        num->setStyleSheet("color: white; font-size: 12px;");

        col->addWidget(count);
        col->addWidget(num);
        distRow->addLayout(col);
    }

    mainLayout->addLayout(distRow);


    QPushButton *btnExport = new QPushButton("Export Skor Sesi");
    btnExport->setStyleSheet(
        "QPushButton { background-color: #3a3a3c; color: white; border-radius: 8px; padding: 8px; }"
        "QPushButton:hover { background-color: #4f8cff; }"
        );

    QPushButton *btnClose = new QPushButton("Tutup");
    btnClose->setStyleSheet(
        "QPushButton { background-color: #3a3a3c; color: white; border-radius: 8px; padding: 8px; }"
        "QPushButton:hover { background-color: #4f8cff; }"
        );
    connect(btnClose, &QPushButton::clicked, &dialog, &QDialog::accept);

    connect(btnExport, &QPushButton::clicked, [&]() {
        btnExport->hide();
        btnClose->hide();
        QPixmap pixmap = dialog.grab();
        btnExport->show();
        btnClose->show();
        QString fileName = QFileDialog::getSaveFileName(
            &dialog,
            "Simpan Skor",
            QDir::homePath() + "/char4char_score.png",
            "Images (*.png *.jpg)"
            );
        if (!fileName.isEmpty()) {
            pixmap.save(fileName);
        }
    });

    mainLayout->addWidget(btnExport);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(btnClose);

    dialog.exec();
    setFocus();
}