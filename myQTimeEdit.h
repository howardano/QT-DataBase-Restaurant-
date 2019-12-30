#ifndef MYQTIMEEDIT_H
#define MYQTIMEEDIT_H
#include <QTimeEdit>

class MyQTimeEdit : public QTimeEdit{
    Q_OBJECT
public:
    QFont font;

    MyQTimeEdit(QTimeEdit* parent = nullptr) :
        QTimeEdit(parent)
    {
        Q_UNUSED(parent)
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        setFont(font);
    }

    void resizeEvent(QResizeEvent *event)
    {
        Q_UNUSED(event)
        //font.setPixelSize();
        setMinimumHeight(35);
        setFont(font);
    }
};
#endif // MYQTIMEEDIT_H
