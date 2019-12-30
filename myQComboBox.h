#ifndef MYQCOMBOBOX_H
#define MYQCOMBOBOX_H
#include <QComboBox>

class MyQComboBox : public QComboBox{
    Q_OBJECT
public:
    QFont font;

    MyQComboBox(QComboBox* parent = nullptr) :
        QComboBox(parent)
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
#endif // MYQCOMBOBOX_H
