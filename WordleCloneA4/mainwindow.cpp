#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "keyboard.h"
#include "wordlegrid.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

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