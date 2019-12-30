#ifndef CHEFINTERFACE_H
#define CHEFINTERFACE_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql>
#include <QSpinBox>
#include <QTimeEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTableView>
#include <QLayout>
#include <QLabel>
#include <QMenu>

#include "myQLabel.h"
#include "myQComboBox.h"
#include "myQSpinBox.h"
#include "myQPushButton.h"
#include "myQTableView.h"

class chefInterface : public QWidget
{
    Q_OBJECT
public:
    explicit chefInterface(QSqlDatabase database, QWidget *parent = nullptr);

    void loadTable();

private slots:
    void addButtonClicked();
    void newOrderClicked();
    void cleanOrderClicked();
    void calcProfClicked();
    void onCustomContextMenu(const QPoint &point);

private:
    QTableView *todayHystTable;  //таблица с историей заказов
    QTableView *orderTable;  //таблица текущего заказа
    QSqlQueryModel *hystoryModel;   //модель с историей заказов
    QSqlQueryModel *orderModel; //модель текущего заказа
    QMenu *contextMenu = nullptr;
    QAction *removeAction;
    QLabel *sumPriceL;

    QComboBox *shavaBox;   //тип шавы
    QComboBox *countBox; //кол-во шав
    QComboBox *sizeBox;   //размер шавы

    QSqlDatabase db;
    QSqlQuery myQ;
    QMessageBox* msg;

};

#endif // CHEFINTERFACE_H
