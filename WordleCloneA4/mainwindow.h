#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
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
    void tampilkanTutorial();

private:
    Ui::MainWindow *ui;

    Keyboard *gameKeyboard;
    WordleGrid *gameGrid;

    void processInput(const QString &key);
};
#endif