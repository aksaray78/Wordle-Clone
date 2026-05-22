#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "keyboard.h"
#include "wordlegrid.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QInputDialog>  // ✅ TAMBAH INI

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);

    layout->addSpacing(70);

    gameGrid = new WordleGrid(this);

    layout->addWidget(gameGrid, 0, Qt::AlignCenter);

    layout->addStretch();

    gameKeyboard = new Keyboard(this);
    layout->addWidget(gameKeyboard);

    layout->addSpacing(20);

    connect(gameKeyboard, &Keyboard::keyTyped, this, [this](const QString &key){
        processInput(key);
    });

    // ✅ TAMBAH DI SINI (paling bawah constructor)
    bool ok;
    QString nama = QInputDialog::getText(this,
                                         "Selamat Datang!",
                                         "Masukkan nama kamu:",
                                         QLineEdit::Normal, "", &ok);

    if (ok && !nama.isEmpty()) {
        ui->lblSalam->setText("👋 Selamat datang, " + nama + "! Tebak kata hari ini.");
        ui->lblSalam->setStyleSheet("color: #2e7d32; font-size: 14px; font-weight: bold;");
    } else {
        ui->lblSalam->setText("👋 Selamat datang, Pemain Misterius!");
        ui->lblSalam->setStyleSheet("color: #1565c0; font-size: 14px; font-style: italic;");
    }
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

