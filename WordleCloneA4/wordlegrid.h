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
    void colorCell(int col, const QString &colorHex);
    void moveToNextRow();
    void clearGrid();
    void glowRowRed();

private:
    QGridLayout *gridLayout;
    QVector<QVector<QLabel*>> cells;
    int currentRow = 0;
    int currentCol = 0;
};

#endif