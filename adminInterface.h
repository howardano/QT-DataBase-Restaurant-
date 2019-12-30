#ifndef ADMININTERFACE_H
#define ADMININTERFACE_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql>
#include <QTableView>
#include <QCheckBox>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "myQComboBox.h"
#include "myQLabel.h"
#include "myQPushButton.h"
#include "myQTableView.h"

#include "chefInterface.h"
#include "storekeeperInterface.h"
#include "myQCheckBox.h"
#include "adminActions.h"

class adminInterface : public QWidget
{
    Q_OBJECT
public:
    explicit adminInterface(QSqlDatabase database, QWidget *parent = nullptr);

private slots:
    void tableSelectSlot(const QString &text);
    void actionSelectSlot(const QString &text);
    void updateTableSlot();
    void onCustomContextMenu(const QPoint &point);

private:
    QTableView *table;  //центральная таблица
    QSqlTableModel *model; //модель центральной таблицы
    QSqlQueryModel *Qmodel;
    QMap<QString, QString> map; //названия таблиц

    QLabel *nameL;  //название таблицы
    QComboBox *tableBox;   //выбор таблицы
    QComboBox *editBox;   //выбор окна редактирования
    QCheckBox *editableBox; //включение/выкл. редактирования
    QCheckBox *autoupBox;   //автообновление таблицы
    QTimer *updateTimer;

    chefInterface *chefIn = nullptr;
    storekeeperInterface *storeIn = nullptr;
    newIngridient *newIng = nullptr;
    newRecepie *newRec = nullptr;
    editRecepie *editRec = nullptr;
    editPrice *editPri = nullptr;

    QString currentTableName;
    QMenu *contextMenu = nullptr;
    QAction *removeAction;
    QAction *removeIngAction;
    QSqlDatabase db;
    QSqlQuery myQ;
    QMessageBox* msg;
};

#endif // ADMININTERFACE_H
