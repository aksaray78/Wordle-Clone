#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QKeyEvent>
#include <QStringList>
#include "keyboard.h"
#include "wordlegrid.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void simpanNamaUser();
    void tampilkanTutorial();
    void ubahDifficulty(int index);

private:
    Ui::MainWindow *ui;

    QLabel *judulGame;
    QLabel *subJudul;
    QLineEdit *inputNama;
    QPushButton *btnOK;
    QLabel *labelInfo;
    QString namaPemain;

    Keyboard *gameKeyboard;
    WordleGrid *gameGrid;
    void processInput(const QString &key);

    QStringList targetWordsList;
    QStringList validWordsList;
    QString targetWord;
    int currentAttempt = 0;
    int currentLetterIndex = 0;
    QString currentGuess = "";

    void loadWordLists();
    void startNewGame();
    void checkGuess();
    void resetGame();
};

#endif // MAINWINDOW_H
