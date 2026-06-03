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

        QLabel* label = cells[currentRow][currentCol];
        if (label) {
            label->setText("");

            label->setStyleSheet(
                "border: 2px solid #3a3a3c;"
                "background-color: transparent;"
                "border-radius: 6px;"

                "color: white;"
                "font-size: 24px;"
                "font-weight: bold;"
                "qproperty-alignment: AlignCenter;"
                );
        }
    }
}

void WordleGrid::colorCell(int col, const QString &colorHex) {
    if (currentRow >= 0 && currentRow < cells.size() && col >= 0 && col < 5) {
        QLabel* label = cells[currentRow][col];
        if (label) {
            label->setStyleSheet(QString(
                                     "background-color: %1;"
                                     "color: white;"
                                     "border: none;"
                                     "font-size: 24px;"
                                     "font-weight: bold;"
                                     "border-radius: 6px;"
                                     ).arg(colorHex));
        }
    }
}

void WordleGrid::moveToNextRow() {
    currentRow++;
    currentCol = 0;
}

void WordleGrid::clearGrid() {
    currentRow = 0;
    currentCol = 0;

    for (int row = 0; row < cells.size(); ++row) {
        for (int col = 0; col < cells[row].size(); ++col) {
            QLabel* label = cells[row][col];
            if (label) {
                label->clear();
                label->setStyleSheet(
                    "border: 2px solid #3a3a3c;"
                    "color: white;"
                    "font-size: 24px;"
                    "font-weight: bold;"
                    "border-radius: 6px;"
                    "background-color: transparent;"
                    );
            }
        }
    }
}
void WordleGrid::glowRowRed() {
    if (currentRow >= 0 && currentRow < cells.size()) {
        for (int col = 0; col < 5; ++col) {
            QLabel* label = cells[currentRow][col];
            if (label) {
                label->setStyleSheet(
                    "background-color: #a62b2b;"
                    "color: white;"
                    "font-size: 24px;"
                    "font-weight: bold;"
                    "border-radius: 6px;"
                    "border: 2px solid #bd3a3a;"
                    );
            }
        }
    }
}