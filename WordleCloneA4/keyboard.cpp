#include "keyboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

Keyboard::Keyboard(QWidget *parent) : QWidget(parent) {
    setupLayout();
}

void Keyboard::setupLayout() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QVector<QStringList> rows = {
        {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
        {"A", "S", "D", "F", "G", "H", "J", "K", "L"},
        {"ENTER", "Z", "X", "C", "V", "B", "N", "M", "BACK"}
    };

    for (const QStringList &row : rows) {
        QHBoxLayout *rowLayout = new QHBoxLayout();
        for (const QString &key : row) {
            QPushButton *btn = new QPushButton(key);
            btn->setMinimumSize(40, 50);
            btn->setFocusPolicy(Qt::NoFocus);

            connect(btn, &QPushButton::clicked, [this, key]() {
                emit keyTyped(key);
                animateKeyPress(key);
            });

            rowLayout->addWidget(btn);
            keys.insert(key, btn);
        }
        mainLayout->addLayout(rowLayout);
    }

    this->setStyleSheet(
        "QPushButton { background-color: #818384; color: white; border-radius: 4px; "
        "font-weight: bold; padding: 10px; border: none; font-size: 14px; }"
        "QPushButton:hover { background-color: #acadae; }"
        );
}

void Keyboard::animateKeyPress(const QString &key) {
    if (keys.contains(key)) {
        QPushButton *btn = keys[key];

        btn->setStyleSheet("background-color: #565758; color: white; border-radius: 4px; font-weight: bold; padding: 10px;");

        QTimer::singleShot(100, [this, btn]() {
            btn->setStyleSheet("background-color: #818384; color: white; border-radius: 4px; font-weight: bold; padding: 10px;");
        });
    }
}