#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <vector>
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

    setWindowTitle("char4char");
    setMinimumSize(550,750);

    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    judulGame = new QLabel(this);
    judulGame->setText("📄char4char");
    judulGame->setGeometry(280, 25, 340, 40);
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

    connect(ui->btnTutorial, &QPushButton::clicked,
            this, &MainWindow::tampilkanTutorial);

    ui->comboDifficulty->clear();
    ui->comboDifficulty->addItem("Normal");
    ui->comboDifficulty->addItem("Hard");
    ui->comboDifficulty->addItem("Extreme");

    connect(ui->comboDifficulty, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::ubahDifficulty);

    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(30, 20, 30, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(judulGame, 0, Qt::AlignHCenter);

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

void MainWindow::ubahDifficulty(int index)
{
    int jumlahHuruf = 5;

    if (index == 0) {
        jumlahHuruf = 5;
    } else if (index == 1) {
        jumlahHuruf = 6;
    } else if (index == 2) {
        jumlahHuruf = 7;
    }

    if (gameGrid) {
        gameGrid->setWordLength(jumlahHuruf);
    }

    qDebug() << "Difficulty diubah. Jumlah huruf:" << jumlahHuruf;
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

    qDebug() << "🎯 Game Dimulai! Kata rahasia game ini adalah:" << targetWord;
}

void MainWindow::resetGame() {
    gameGrid->clearGrid();
    gameKeyboard->clearKeyboard();

    currentGuess = "";
    currentLetterIndex = 0;

    startNewGame();

    labelInfo->setText("🎉 Main lagi! Tebak kata baru.");
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
        } else if (letterStatus[i] == 1) {
            gameGrid->colorCell(i, "#d4a93f");
            gameKeyboard->updateKeyColor(currentLetter, "#d4a93f");
        } else {
            gameGrid->colorCell(i, "#5b6270");
            gameKeyboard->updateKeyColor(currentLetter, "#5b6270");
        }
    }

    if (guess == target) {
        QDialog *winDialog = new QDialog(this);
        winDialog->setWindowTitle("🎉 KAMU MENANG! 🎉");
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
        btnMainLagi->setStyleSheet("background-color: #3fa66b; color: white; padding: 10px; font-weight: bold; border-radius: 5px;");
        connect(btnMainLagi, &QPushButton::clicked, winDialog, &QDialog::accept);

        layout->addWidget(winLabel);
        layout->addWidget(btnMainLagi);

        winDialog->exec();
        delete winDialog;

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

        resetGame();
        return;
    }
}