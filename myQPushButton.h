#pragma once

#include <QtCore>
#include <QtGui>
#include <QPushButton>

class MyQPushButton : public QPushButton{
    Q_OBJECT
public:
    QFont font;

    MyQPushButton(QString const &str, QPushButton* parent = nullptr) :
        QPushButton(parent)
    {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        setFont(font);
        setText(str);
    }

    void resizeEvent(QResizeEvent *event)
    {
        Q_UNUSED(event)
        font.setPixelSize(int(this->size().height() / 2));
        setMinimumHeight(35);
        setFont(font);
    }
};
