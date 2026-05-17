#include "wordlegrid.h"

WordleGrid::WordleGrid(QWidget *parent) : QWidget(parent) {
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(5);
    gridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    cells.resize(6, QVector<QLabel*>(5));

    for (int r = 0; r < 6; ++r) {
        for (int c = 0; c < 5; ++c) {
            QLabel *label = new QLabel("");
            label->setFixedSize(60, 60);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("border: 2px solid #3a3a3c; font-size: 30px; "
                                 "font-weight: bold; color: white;");
            gridLayout->addWidget(label, r, c);
            cells[r][c] = label;
        }
    }
}

void WordleGrid::insertLetter(const QString &letter) {
    if (currentCol < 5 && currentRow < 6) {
        cells[currentRow][currentCol]->setText(letter);
        currentCol++;
    }
}

void WordleGrid::removeLetter() {
    if (currentCol > 0) {
        currentCol--;
        cells[currentRow][currentCol]->setText("");
    }
}