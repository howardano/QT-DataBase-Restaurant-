#include "chefInterface.h"
#include <QtWidgets>

chefInterface::chefInterface(QSqlDatabase database, QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Chef");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 3, availableSize.height() / 3);
    db = database;
    db.open();
    myQ = QSqlQuery(db);

    //название
    QLabel *shavaL = new MyQLabel("Shawarma");
    shavaBox = new MyQComboBox;
    shavaBox->addItem("Select...");
    myQ.exec("SELECT shawarma FROM myschema.price");
    while (myQ.next())
        shavaBox->addItem(myQ.record().value(0).toString());
    //кол-во
    QLabel *amountL = new MyQLabel("Amount");
    countBox = new MyQComboBox;
    for (int i=1; i<11; i++)
        countBox->addItem(QString::number(i));
    //размер
    QLabel *sizeL = new MyQLabel("Size");
    sizeBox = new MyQComboBox;
    myQ.exec("SELECT * FROM myschema.price");
    QSqlRecord R = myQ.record();
    for (int i = 1; i < R.count(); i++)
        sizeBox->addItem(R.fieldName(i).toUpper());

    QPushButton *addB = new MyQPushButton("  Add ▶");
    QPushButton *newOrderB = new MyQPushButton("◀ New order  ");
    sumPriceL = new MyQLabel("Price: 0");
    sumPriceL->setAlignment(Qt::AlignRight);
    QPushButton *cleanOrderB = new MyQPushButton("Clean order");
    QLabel *calcProfL = new MyQLabel("Close restaurant");
    calcProfL->setAlignment(Qt::AlignCenter);
    QPushButton *calcProfB = new MyQPushButton("🏁 End the day ");
    //таблица всех заказов
    hystoryModel = new QSqlQueryModel();
    myQ.exec("SELECT * FROM myschema.orders");
    hystoryModel->setQuery(myQ);
    todayHystTable = new QTableView;
    todayHystTable->setModel(hystoryModel);
    //todayHystTable->resizeColumnsToContents();
    todayHystTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    todayHystTable->verticalHeader()->hide();
    //столбец с текущим заказом и инфой о нем
    orderModel = new QSqlQueryModel();
    myQ.exec("SELECT * FROM myschema.shawarmasordered");
    orderModel->setQuery(myQ);
    orderTable = new MyQTableView;
    orderTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(orderTable, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    orderTable->setModel(orderModel);
    orderTable->resizeColumnsToContents();
    //orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    //главвный слой
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(todayHystTable,0,0,10,1);
    mainLayout->addWidget(shavaL,0,1,1,1);
    mainLayout->addWidget(shavaBox,1,1,1,1);
    mainLayout->addWidget(amountL,2,1,1,1);
    mainLayout->addWidget(countBox,3,1,1,1);
    mainLayout->addWidget(sizeL,4,1,1,1);
    mainLayout->addWidget(sizeBox,5,1,1,1);
    mainLayout->addWidget(addB,6,1,1,1);
    mainLayout->addWidget(newOrderB,7,1,1,1);
    mainLayout->addWidget(calcProfL,8,1,1,1);
    mainLayout->addWidget(calcProfB,9,1,1,1);

    mainLayout->addWidget(orderTable,0,2,8,1);
    mainLayout->addWidget(sumPriceL,8,2,1,1);
    mainLayout->addWidget(cleanOrderB,9,2,1,1);

    mainLayout->setColumnStretch(0,2);
    mainLayout->setColumnStretch(1,2);
    mainLayout->setColumnStretch(2,2);

    setLayout(mainLayout);
    msg= new QMessageBox;

    connect(addB, SIGNAL(clicked()), SLOT(addButtonClicked()));
    connect(newOrderB, SIGNAL(clicked()), SLOT(newOrderClicked()));
    connect(cleanOrderB, SIGNAL(clicked()), SLOT(cleanOrderClicked()));
    connect(calcProfB, SIGNAL(clicked()), SLOT(calcProfClicked()));
}


void chefInterface::addButtonClicked() {
    if (shavaBox->currentText()=="Select...") { msg->setText("Choose shawarma name first"); msg->show(); return;}
    QStringList ingedients;
    myQ.exec("SELECT ingridients FROM myschema.base");
    while (myQ.next())
        ingedients.append(myQ.record().value(0).toString());
    QString name = shavaBox->currentText();  // - название
    QString size = sizeBox->currentText().toLower();  // - размер
    QString count = countBox->currentText();  //  - кол-во
    myQ.exec("SELECT * FROM myschema.shawarmasordered WHERE shawarma = '"+name+"' AND size = '"+size+"';");
    myQ.next();
    qDebug()<< "founded! - " << myQ.record().value(0).toString();
     if (myQ.record().value(0).toString()!= "")
    {
        myQ.exec("UPDATE myschema.shawarmasordered SET amount = (SELECT amount FROM myschema.shawarmasordered WHERE shawarma = '"+name+"' AND size = '"+size+"') + " +count+ ", sum = (SELECT sum FROM myschema.shawarmasordered WHERE shawarma = '"+name+"' AND size = '"+size+"') + " +count+ "*(SELECT " +size+ " FROM myschema.price WHERE shawarma = '"+name+"')  WHERE shawarma = '"+name+"' AND size = '"+size+"'");

    }
    else
        myQ.exec("INSERT INTO myschema.shawarmasordered VALUES ('" +name+ "', '" +size+ "', " +count+ ", ((SELECT " +size+ " FROM myschema.price WHERE shawarma = '" +name+ "') * " +count+ "));");
    for (auto ingr: ingedients)
    {
        qDebug() <<ingr<<" ";
        myQ.exec("UPDATE myschema.Base SET Remaining = (SELECT Remaining FROM Base WHERE Ingridients = '"+ingr+"') - ((SELECT "+ingr+" FROM Recepies WHERE Shawarmas = '" +name+"') * " +count+ ") WHERE Ingridients = '"+ingr+"';");
    }
    myQ.exec("UPDATE myschema.popular SET counter = (SELECT counter FROM myschema.popular WHERE shawarma = '" +name+ "') + " +count+ " WHERE shawarma = '" +name+ "'");
    loadTable();
}


void chefInterface::newOrderClicked()
{
    if (sumPriceL->text() == "Price: 0")
    {
        msg->setText("Unable to add empty order.");
        msg->show();
        return;
    }
    myQ.exec("INSERT INTO myschema.orders (number, sum, time) VALUES ((SELECT COUNT(*) FROM myschema.orders) + 1, (SELECT SUM(sum) FROM myschema.shawarmasordered), (SELECT LOCALTIME(0)));");
    QSqlQueryModel * model = new QSqlQueryModel();
    myQ.exec("SELECT * FROM myschema.orders");
    model->setQuery(myQ);
    todayHystTable->setModel(model);
    todayHystTable->show();
    myQ.exec("DELETE FROM myschema.shawarmasordered");
    loadTable();
}

void chefInterface::cleanOrderClicked()
{
    myQ.exec("DELETE FROM myschema.shawarmasOrdered;");
    loadTable();
}

void chefInterface::calcProfClicked()
{
    myQ.exec("INSERT INTO myschema.proceeds VALUES ((SELECT CURRENT_DATE), (SELECT SUM(sum) FROM myschema.orders), (SELECT shawarma FROM myschema.popular WHERE counter = (SELECT max(counter) FROM myschema.popular)));");
    myQ.exec("DELETE FROM myschema.orders");
    myQ.exec("DELETE FROM myschema.popular");
    myQ.exec("INSERT INTO myschema.popular VALUES ('BBQ', 0), ('Meety', 0), ('Florida', 0);");
    msg->setText("Closed succefully. Revenue calculated");
    close();
}

void chefInterface::loadTable() {
    myQ.exec("SELECT * FROM myschema.shawarmasordered");
    orderModel->setQuery(myQ);
    orderTable->setModel(orderModel);
    myQ.exec("SELECT SUM(sum) FROM myschema.shawarmasordered");
    myQ.next(); sumPriceL->setText("Price: " + myQ.record().value(0).toString());
    orderTable->resizeColumnsToContents();
}

void chefInterface::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = orderTable->indexAt(point);
    qDebug() << index.row();
    orderTable->selectRow(index.row());

    if (!index.isValid())
        return;

    if (!contextMenu)
    {
        contextMenu = new QMenu;
        removeAction = new QAction("Delete", contextMenu);
        contextMenu->addAction(removeAction);
    }

    QPoint globalPos = orderTable->mapToGlobal(point);
    QAction* selectedItem = contextMenu->exec(globalPos);
    if (selectedItem == removeAction)
    {
        qDebug()<< index.data(Qt::DisplayRole).toString();
        myQ.exec("DELETE FROM myschema.shawarmasordered WHERE shawarma = '"+orderModel->index(index.row(),0).data(Qt::DisplayRole).toString()+"' AND size = '"+orderModel->index(index.row(),1).data(Qt::DisplayRole).toString()+"';");
        loadTable();
        contextMenu->hide();
    }
}
