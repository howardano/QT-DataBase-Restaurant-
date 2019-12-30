#ifndef ADMINACTIONS_H
#define ADMINACTIONS_H

#include <QWidget>
#include <QtWidgets>
#include <QtSql>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QStringListModel>
#include <QScrollArea>
#include <QCheckBox>
#include <QSqlRecord>
#include <QMessageBox>

#include "myQLabel.h"
#include "myQLineEdit.h"
#include "myQSpinBox.h"
#include "myQPushButton.h"
#include "myQComboBox.h"
#include "myQScrollArea.h"
#include "myQCheckBox.h"

class editPrice;
//###############################################     EDIT    RECIPIE    WINDOW ################################################
class editRecepie : public QWidget
{
    Q_OBJECT
public:
    explicit editRecepie(QSqlDatabase database, QWidget *parent = nullptr);

private:
    QSqlDatabase db;
    QStringList ingridients;
    QStringList recipies;
    QLineEdit *nameLine; //название рецепта
    QComboBox *recB;    //рецепт выбор
    QWidget *ingrGridW; //виджет ингр
    QGridLayout *ingrGridL; //ингридиенты
    QComboBox *lastIngr = nullptr;
    QSpinBox *lastCount = nullptr;
    QSqlQuery myQ;
    QMessageBox* msg;
    editPrice *editPri = nullptr;
    int rowNumber = 0;
private slots:
    void makeRowSlot();
    void deleteRowSlot();
    void updateRows();
    void uploadRecepieSlot(const QString &text);
    void saveRecepieSlot();
    void deleteMsgSlot();
    void deleteRecepieSlot();
    void openPriceEditSlot();
    void disconnectMsgSlot();

signals:

};
//###############################################     NEW    INGRIDIENT    WINDOW ################################################
class newIngridient : public QWidget
{
    Q_OBJECT
public:
    explicit newIngridient(QSqlDatabase database, QWidget *parent = nullptr);

signals:

private:
    QSqlDatabase db;
    QStringList ingredientsList;
    QLineEdit *nameLine;    //название ингридиента
    QSpinBox *minCountB = nullptr;  //минимум
    QSpinBox *remainB = nullptr;   //остаток
    QSpinBox *countBox; //кол-во
    QSqlQueryModel *model;  //модель таблицы
    QMessageBox *msg;
    QSqlQuery myQ;

private slots:
    void addIngridientSlot();
};
//###############################################     NEW    RECEPIE    WINDOW ################################################
class newRecepie : public QWidget
{
    Q_OBJECT
public:
    explicit newRecepie(QSqlDatabase database, QWidget *parent = nullptr);

signals:

private:
    QSqlDatabase db;
    QStringList ingridients;    //доступные ингридиенты
    QWidget *ingrGridW; //виджет ингр
    QGridLayout *ingrGridL; //ингридиенты
    QLineEdit *nameLine;    //название шавы

    QStringList recepies;   //названия шав
    QStringList sizes;  //доступные размеры
    QWidget *sizeGridW; //виджет цен
    QGridLayout *sizeGridL; //цены слой

    QComboBox *lastIngr = nullptr;
    QSpinBox *lastCount = nullptr;
    QSqlQuery myQ;
    QMessageBox* msg;
    int rowNumber = 0;
private slots:
    void makeRowSlot();
    void deleteRowSlot();
    void updateRows();
    void saveRecepieSlot();
};

#endif // ADMINACTIONS_H
//###############################################     NEW    PRICE    WINDOW ################################################
class editPrice : public QWidget
{
    Q_OBJECT
public:
    explicit editPrice(QSqlDatabase database, QWidget *parent = nullptr);

signals:

private:
    QSqlDatabase db;
    QStringList recepies;   //названия шав
    QStringList sizes;  //доступные размеры
    QStringList costs;
    QComboBox *nameB;   //шава
    QScrollArea *scrollArea;
    QWidget *sizeGridW; //виджет цен
    QGridLayout *sizeGridL; //цены слой
    QSqlQuery myQ;
    QMessageBox* msg;
    int rowNumber = 0;

public slots:
    void loadPricesSlot(const QString &name);
    void setDisabledSlot();
private slots:
    void savePricesSlot();
};
