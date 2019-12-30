#ifndef MYQSCROLLAREA_H
#define MYQSCROLLAREA_H
#include <QScrollArea>
#include <QLayout>
#include <QEvent>

class MyQScrollArea : public QScrollArea{
    Q_OBJECT
public:
    explicit MyQScrollArea(QWidget *parent=nullptr) : QScrollArea(parent)
    {
        setWidgetResizable(true);
        setFrameStyle(QFrame::NoFrame);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    void resizeEvent(QResizeEvent *event)
    {
        widget()->setFixedWidth(width());
        QScrollArea::resizeEvent(event);
    }
};
#endif // MYQSCROLLAREA_H
