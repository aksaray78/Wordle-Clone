#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QPushButton>
#include <QMap>

class Keyboard : public QWidget {
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = nullptr);

    void animateKeyPress(const QString &key);
    void updateKeyColor(const QString &key, const QString &colorHex);
    void clearKeyboard();

signals:
    void keyTyped(const QString &text);

private:
    void setupLayout();
    QMap<QString, QPushButton*> keys;
    QMap<QString, QString> keyColors;
};

#endif