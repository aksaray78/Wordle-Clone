#include "wordlegrid.h"

WordleGrid::WordleGrid(QWidget *parent) : QWidget(parent) {
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(5);
    gridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    createGrid();
}

void WordleGrid::createGrid() {
    while (QLayoutItem *item = gridLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            delete widget;
        }
        delete item;
    }

    cells.clear();

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
                "background-color: transparent;"
                "border-radius: 6px;"
                "color: white;"
                "font-size: 24px;"
                "font-weight: bold;"
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
    if (currentRow >= 0 && currentRow < cells.size()
        && currentCol >= 0 && currentCol < cells[currentRow].size()) {

        QLabel *label = cells[currentRow][currentCol];
        if (label) {
            label->setText(letter);
            label->setStyleSheet(QString(
                "border: 2px solid %1;"
                "background-color: transparent;"
                "border-radius: 6px;"
                "color: %2;"
                "font-size: 24px;"
                "font-weight: bold;"
            ).arg(m_borderColor, m_textColor));
        }

        currentCol++;
    }
}

void WordleGrid::removeLetter() {
    if (currentRow >= 0 && currentRow < cells.size() && currentCol > 0) {
        currentCol--;

        QLabel *label = cells[currentRow][currentCol];
        if (label) {
            label->setText("");
            label->setStyleSheet(QString(
                "border: 2px solid %1;"
                "background-color: transparent;"
                "border-radius: 6px;"
                "color: %2;"
                "font-size: 24px;"
                "font-weight: bold;"
            ).arg(m_borderColor, m_textColor));
        }
    }
}

void WordleGrid::clearGrid() {
    currentRow = 0;
    currentCol = 0;

    for (int row = 0; row < cells.size(); ++row) {
        for (int col = 0; col < cells[row].size(); ++col) {
            QLabel *label = cells[row][col];
            if (label) {
                label->clear();
                label->setStyleSheet(QString(
                    "border: 2px solid #3a3a3c;"
                    "background-color: transparent;"
                    "border-radius: 6px;"
                    "color: white;"
                    "font-size: 24px;"
                    "font-weight: bold;"
                ).arg(m_borderColor, m_textColor));
            }
        }
    }
}

void WordleGrid::colorCell(int col, const QString &colorHex) {
    if (currentRow >= 0 && currentRow < cells.size()
        && col >= 0 && col < cells[currentRow].size()) {

        QLabel *label = cells[currentRow][col];
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
    if (currentRow < cells.size() - 1) {
        currentRow++;
        currentCol = 0;
    }
}

void WordleGrid::glowRowRed() {
    if (currentRow >= 0 && currentRow < cells.size()) {
        for (int col = 0; col < cells[currentRow].size(); ++col) {
            QLabel *label = cells[currentRow][col];
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

void WordleGrid::updateEmptyCells(const QString &textColor, const QString &borderColor) {
    for (int row = 0; row < cells.size(); ++row) {
        for (int col = 0; col < cells[row].size(); ++col) {
            QLabel *label = cells[row][col];
            if (label && label->styleSheet().contains("background-color: transparent")) {
                label->setStyleSheet(QString(
                    "border: 2px solid %1;"
                    "background-color: transparent;"
                    "border-radius: 6px;"
                    "color: %2;"
                    "font-size: 24px;"
                    "font-weight: bold;"
                ).arg(borderColor, textColor));
            }
        }
    }
}

void WordleGrid::setThemeColors(const QString &textColor, const QString &borderColor) {
    m_textColor = textColor;
    m_borderColor = borderColor;
}