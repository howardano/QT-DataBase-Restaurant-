#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QtWidgets>
#include <QMessageBox>
#include <QtSql>
#include <QSqlQuery>
#include <QLayout>

#include "adminInterface.h"
#include "chefInterface.h"
#include "storekeeperInterface.h"
#include "myQPushButton.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void adminButtonClicked();
    void shefButtonClicked();
    void storekeeperButtonClicked();
    void quitButtonClicked();

private:
    adminInterface *adIn = nullptr;
    chefInterface *chefIn = nullptr;
    storekeeperInterface *storeIn = nullptr;
    QSqlDatabase db;
    QMessageBox *msg;
};
#endif // MAINWINDOW_H
