#ifndef WORDLEGRID_H
#define WORDLEGRID_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVector>

class WordleGrid : public QWidget {
    Q_OBJECT
public:
    explicit WordleGrid(QWidget *parent = nullptr);

    void insertLetter(const QString &letter);
    void removeLetter();
    void setWordLength(int length);

    void clearGrid();
    void colorCell(int col, const QString &colorHex);
    void moveToNextRow();
    void glowRowRed();
    void createGrid();
    void updateEmptyCells(const QString &textColor, const QString &borderColor);
    void setThemeColors(const QString &textColor, const QString &borderColor);

private:
    QGridLayout *gridLayout;
    QVector<QVector<QLabel*>> cells;
    int maxRows = 6;
    int wordLength = 5;
    int currentRow = 0;
    int currentCol = 0;
    QString m_textColor = "white";
    QString m_borderColor = "#3a3a3c";


};

#endif