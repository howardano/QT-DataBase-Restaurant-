#ifndef MYQLINEEDIT_H
#define MYQLINEEDIT_H
#include <QLineEdit>

class MyQLineEdit : public QLineEdit{
    Q_OBJECT
public:
    QFont font;

    MyQLineEdit(QLineEdit* parent = nullptr) :
        QLineEdit(parent)
    {
        Q_UNUSED(parent)
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        setMinimumHeight(35);
        setFont(font);
    }

    void resizeEvent(QResizeEvent *event)
    {
        Q_UNUSED(event)
        font.setPixelSize(int(this->size().height() / 2));
        setFont(font);
    }

};
#endif // MYQLINEEDIT_H
