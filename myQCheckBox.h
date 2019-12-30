#ifndef MYQCHECKBOX_H
#define MYQCHECKBOX_H
#include <QCheckBox>

class MyQCheckBox : public QCheckBox{
    Q_OBJECT
public:
    QFont font;

    MyQCheckBox(QString const &str, QCheckBox* parent = nullptr) :
        QCheckBox(str, parent)
    {
        Q_UNUSED(parent)
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        setMinimumHeight(35);
    }

    void resizeEvent(QResizeEvent *event)
    {
        Q_UNUSED(event)
        font.setPixelSize(int(this->size().height() / 2));
        setFont(font);
        setStyleSheet(QString ("QCheckBox::indicator { width: %1px; height: %1px; }").arg(geometry().height()/2));
    }
};

#endif // MYQCHECKBOX_H
