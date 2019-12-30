#ifndef STOREKEEPERINTERFACE_H
#define STOREKEEPERINTERFACE_H

#include <QWidget>
#include <QtSql>
#include <QMessageBox>
#include <QTableView>
#include <QLayout>
#include <QComboBox>
#include <QSpinBox>

#include "myQPushButton.h"
#include "myQComboBox.h"
#include "myQSpinBox.h"

class storekeeperInterface : public QWidget
{
    Q_OBJECT
public:
    explicit storekeeperInterface(QSqlDatabase database, QWidget *parent = nullptr);

private slots:
    void findButtonClicked();
    void allButtonClicked();
    void addIngridients();
    void takeIngridients();
    void updateModel();
private:
    QTableView *tableView;  //таблица
    QSqlDatabase db;
    QComboBox *ingridientBox;   //Выбор
    QSpinBox *countBox; //кол-во
    QSqlQueryModel *model;  //модель таблицы
    QSqlQuery myQ;
    //QMessageBox *msg;
};

#endif // STOREKEEPERINTERFACE_H
