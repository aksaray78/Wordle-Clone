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
    QString upperKey = key.toUpper();
    if (keys.contains(upperKey)) {
        QPushButton *btn = keys[upperKey];

        btn->setStyleSheet("background-color: #565758; color: white; border-radius: 4px; font-weight: bold; padding: 10px;");

        QTimer::singleShot(100, [this, upperKey, btn]() {
            QString colorToRestore = keyColors.value(upperKey, "#818384");

            btn->setStyleSheet(QString(
                                   "background-color: %1;"
                                   "color: white;"
                                   "font-weight: bold;"
                                   "border-radius: 6px;"
                                   "border: none;"
                                   "padding: 10px;"
                                   ).arg(colorToRestore));
        });
    }
}

void Keyboard::updateKeyColor(const QString &key, const QString &colorHex) {
    QString upperKey = key.toUpper();
    if (keys.contains(upperKey)) {
        QPushButton* button = keys[upperKey];
        if (button) {
            QString currentColor = keyColors.value(upperKey, "");

            if (currentColor == "#3fa66b") {
                return;
            }

            if (currentColor == "#d4a93f" && colorHex != "#3fa66b") {
                return;
            }

            keyColors[upperKey] = colorHex;

            button->setStyleSheet(QString(
                                      "background-color: %1;"
                                      "color: white;"
                                      "font-weight: bold;"
                                      "border-radius: 6px;"
                                      "border: none;"
                                      "padding: 10px;"
                                      ).arg(colorHex));
        }
    }
}

void Keyboard::clearKeyboard() {
    keyColors.clear();

    for (auto btn : keys.values()) {
        if (btn) {
            btn->setStyleSheet(
                "background-color: #818384; color: white; border-radius: 4px; "
                "font-weight: bold; padding: 10px; border: none; font-size: 14px;"
                );
        }
    }
}