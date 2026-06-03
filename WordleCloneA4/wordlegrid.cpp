#include "wordlegrid.h"

WordleGrid::WordleGrid(QWidget *parent) : QWidget(parent) {
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(5);
    gridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    createGrid();
}

void WordleGrid::clearGrid() {
    while (QLayoutItem *item = gridLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    cells.clear();
}

void WordleGrid::createGrid() {
    clearGrid();

    currentRow = 0;
    currentCol = 0;

    cells.resize(maxRows);

    for (int r = 0; r < maxRows; ++r) {
        cells[r].resize(wordLength);

        for (int c = 0; c < wordLength; ++c) {
            QLabel *label = new QLabel("");
            label->setFixedSize(60, 60);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet(
                "border: 2px solid #3a3a3c;"
                "font-size: 30px;"
                "font-weight: bold;"
                "color: white;"
                );

            gridLayout->addWidget(label, r, c);
            cells[r][c] = label;
        }
    }
}

void WordleGrid::setWordLength(int length) {
    if (length < 5 || length > 7) {
        return;
    }

    wordLength = length;
    createGrid();
}

void WordleGrid::insertLetter(const QString &letter) {
    if (currentCol < wordLength && currentRow < maxRows) {
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