#ifndef MYQTABLEVIEW_H
#define MYQTABLEVIEW_H
#include <QTableView>

class MyQTableView : public QTableView{
    Q_OBJECT
public:
    QFont font;

    MyQTableView(QTableView* parent = nullptr) :
        QTableView(parent)
    {
        Q_UNUSED(parent)
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        //if (horizontalHeader()) horizontalOffset()
        font.setPixelSize(16);
        setFont(font);
    }

//    void resizeEvent(QResizeEvent *event)
//    {
//        Q_UNUSED(event)
//        this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//        font.setPixelSize(int(this->size().width() / ));
//        setFont(font);
//    }

};
#endif // MYQTABLEVIEW_H
