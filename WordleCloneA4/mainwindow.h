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
    void tampilkanSettings();
    void tampilkanStatistik();

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
    bool cekHardMode(const QString &guess, QString &pesanError);

    bool hardMode = false;
    bool darkTheme = true;
    QString lockedPattern = "_____";
    QString requiredLetters = "";

    bool gameSudahMulai() const;
    void applyTheme();

    int statPlayed = 0;
    int statWins = 0;
    int statCurrentStreak = 0;
    int statMaxStreak = 0;
    int statGuessDistribution[6] = {0, 0, 0, 0, 0, 0};
};

#endif // MAINWINDOW_H
