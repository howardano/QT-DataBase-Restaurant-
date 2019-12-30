#include "storekeeperInterface.h"
#include <QtWidgets>

storekeeperInterface::storekeeperInterface(QSqlDatabase database, QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Storekeeper");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 3, availableSize.height() / 3);
    db = database;
    db.open();
    myQ = QSqlQuery(db);

    //список
    ingridientBox = new MyQComboBox;
    ingridientBox->addItem("Pick ingridient...");
    myQ.exec("SELECT ingridients FROM myschema.base");
    while (myQ.next())
        ingridientBox->addItem(myQ.record().value(0).toString());
    countBox = new MyQSpinBox;
    countBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    countBox->setFont(QFont());
    //таблица
    model = new QSqlQueryModel();
    myQ.exec("SELECT * FROM myschema.base");
    model->setQuery(myQ);
    tableView = new QTableView;
    tableView->setModel(model);
    tableView->resizeColumnsToContents();

    //основной слой (вертикальный)
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(tableView,0,0,7,3);

    //строка с выбором и поиском
    mainLayout->addWidget(ingridientBox,8,0,1,1);
    QPushButton *findB = new MyQPushButton("🔍 Find  ");
    QPushButton *allB = new MyQPushButton("Show all");
    mainLayout->addWidget(findB,8,1,1,1);
    mainLayout->addWidget(allB,8,2,1,1);

    //строка с выбором кол-ва и добавлением/удалением
    mainLayout->addWidget(countBox,9,0,1,1);
    QPushButton *addB = new MyQPushButton("Add");
    QPushButton *takeB = new MyQPushButton("Take away");
    mainLayout->addWidget(addB,9,1,1,1);
    mainLayout->addWidget(takeB,9,2,1,1);

    setLayout(mainLayout);

    connect(findB, SIGNAL(clicked()), SLOT(findButtonClicked()));
    connect(allB, SIGNAL(clicked()), SLOT(allButtonClicked()));
    connect(addB, SIGNAL(clicked()), SLOT(addIngridients()));
    connect(takeB, SIGNAL(clicked()), SLOT(takeIngridients()));

    QTimer *updateTimer = new QTimer;
    connect(updateTimer, SIGNAL(timeout()), SLOT(updateModel()));
    updateTimer->start(2000);
}

void storekeeperInterface::findButtonClicked()
{
    if (ingridientBox->currentIndex() == 0) return;
    myQ.exec("SELECT * FROM myschema.base WHERE ingridients = '"+ ingridientBox->currentText() +"'");
    model->setQuery(myQ);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();

}

void storekeeperInterface::allButtonClicked()
{
    myQ.exec("SELECT * FROM myschema.base");
    model->setQuery(myQ);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->show();
}

void storekeeperInterface::addIngridients()
{
    if (ingridientBox->currentIndex() == 0) return;
    QSqlQuery newQ(db);
    int count = countBox->value();
    qDebug() << count;

    QString ingridient = ingridientBox->currentText();
    newQ.exec("UPDATE myschema.base SET remaining = (SELECT remaining FROM myschema.base WHERE ingridients = '" +
              ingridient + "') + " + QString::number(count) +
              " WHERE ingridients = '" + ingridient + "';");
    myQ.exec(myQ.lastQuery());
    model->setQuery(myQ);
    tableView->resizeColumnsToContents();
}

void storekeeperInterface::takeIngridients()
{
    if (ingridientBox->currentIndex() == 0) return;
    QSqlQuery newQ(db);
    int count = 0-countBox->value();
    qDebug() << count;

    QString ingridient = ingridientBox->currentText();
    newQ.exec("UPDATE myschema.base SET Remaining = (SELECT Remaining FROM myschema.base WHERE ingridients = '" +
              ingridient + "') + " + QString::number(count) +
              " WHERE ingridients = '" + ingridient + "';");
    myQ.exec(myQ.lastQuery());
    model->setQuery(myQ);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
}

void storekeeperInterface::updateModel()
{
    myQ.exec(myQ.lastQuery());
    model->setQuery(myQ);
}
