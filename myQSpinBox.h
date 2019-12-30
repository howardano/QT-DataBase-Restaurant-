#ifndef MYQSPINBOX_H
#define MYQSPINBOX_H
#include <QSpinBox>
#include <QResizeEvent>
#include <QFont>
#include <QDebug>

class MyQSpinBox : public QSpinBox{
    Q_OBJECT
public:
    explicit MyQSpinBox(QSpinBox* parent = nullptr) : QSpinBox(parent)
    {
        Q_UNUSED(parent)
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        setButtonSymbols(QAbstractSpinBox::PlusMinus);
        setMaximum(100000);
    }

protected:
    void resizeEvent(QResizeEvent * event)
       {
           QFont f= font();
           f.setPixelSize(event->size().height() / 2);
           setFont(f);
           QSpinBox::resizeEvent(event);
           setStyleSheet(QString ("QSpinBox { padding-right: %1px; }"
                                  "QSpinBox::up-button { image: url(:/images/plus-r.png); width: %1px;  height: %1px; subcontrol-position: right; right: %2px}"
                                  "QSpinBox::down-button { image: url(:/images/minus-r.png); width: %1px;  height: %1px; subcontrol-position: right; right: -2px}").arg(geometry().height()).arg(geometry().height()-2));
       }
};
#endif // MYQSPINBOX_H
