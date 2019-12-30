#ifndef MYQLABEL_H
#define MYQLABEL_H
#include <QLabel>

class MyQLabel : public QLabel{
    Q_OBJECT
public:
    QFont font;

    MyQLabel(QString const &str, QLabel* parent = nullptr) :
        QLabel(str, parent)
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
#endif // MYQLABEL_H
