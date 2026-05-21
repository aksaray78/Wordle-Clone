#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "keyboard.h"
#include "wordlegrid.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->btnTutorial, &QPushButton::clicked,
            this, &MainWindow::tampilkanTutorial);

    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);

    layout->addStretch();

    gameGrid = new WordleGrid(this);

    layout->addWidget(gameGrid, 0, Qt::AlignCenter);

    layout->addStretch();

    gameKeyboard = new Keyboard(this);
    layout->addWidget(gameKeyboard);

    layout->addSpacing(20);

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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tampilkanTutorial()
{
    QMessageBox::information(
        this,
        "Tutorial Wordle",
        "Cara Bermain Wordle:\n\n"
        "1. Pemain harus menebak kata tersembunyi dalam 6 percobaan.\n"
        "2. Masukkan kata sesuai jumlah huruf yang ditentukan.\n"
        "3. Setiap tebakan harus berupa kata yang valid.\n"
        "4. Tekan Enter untuk mengirim jawaban.\n\n"
        "Arti Warna Kotak:\n"
        "- Hijau: huruf benar dan posisinya benar.\n"
        "- Kuning: huruf ada di dalam kata, tetapi posisinya salah.\n"
        "- Abu-abu: huruf tidak ada di dalam kata."
        );
}