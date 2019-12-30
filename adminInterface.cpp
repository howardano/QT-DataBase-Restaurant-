#include "adminInterface.h"
#include <QtWidgets>

adminInterface::adminInterface(QSqlDatabase database, QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Administrator");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 3, availableSize.height() / 3);
    db = database;
    db.open();
    model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    myQ = QSqlQuery(db);

    nameL = new MyQLabel("Table: ");
    nameL->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //список таблиц
    tableBox = new MyQComboBox;
    tableBox->addItem("Select table...");
    tableBox->addItem("Product base");
    tableBox->addItem("Prices");
    tableBox->addItem("Recepies");
    tableBox->addItem("Served orders");
    tableBox->addItem("Current order");
    tableBox->addItem("Proceeds");

    map["Product base"] = "myschema.base";
    map["Prices"] = "myschema.price";
    map["Recepies"] = "myschema.recepies";
    map["Served orders"] = "myschema.orders";
    map["Current order"] = "myschema.shawarmasordered";
    map["Proceeds"] = "myschema.proceeds";

    editableBox = new MyQCheckBox("Enable editing");
    autoupBox = new MyQCheckBox("Auto update");
    updateTimer = new QTimer;

    editBox = new MyQComboBox;
    editBox->addItem("Select action...");
    editBox->addItem("Edit base");
    editBox->addItem("Edit recepies");
    editBox->addItem("Edit price");
    editBox->addItem("New recepie");
    editBox->addItem("New ingridient");
    editBox->addItem("Clear proceeds");
    editBox->addItem("Wipe database");
    editBox->addItem("Reset database");

    table = new MyQTableView;
    table->setSortingEnabled(true);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(nameL,0,0,1,2);
    mainLayout->addWidget(table,1,0,7,2);
    mainLayout->addWidget(tableBox, 8,0,1,1);
    mainLayout->addWidget(editBox, 8,1,1,1);
    mainLayout->addWidget(editableBox, 9,0,1,1);
    mainLayout->addWidget(autoupBox,9,1,1,1);

    setLayout(mainLayout);
    connect(tableBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            [=](const QString &text){ emit tableSelectSlot(text);});
    connect(editableBox, QOverload<bool>::of(&QCheckBox::toggled), [this](bool state)
    { if (state==1)
        {
            autoupBox->setCheckState(Qt::Unchecked);
            table->setEditTriggers(QAbstractItemView::QAbstractItemView::DoubleClicked);
        }
        else
            table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    });
    connect(autoupBox, QOverload<bool>::of(&QCheckBox::toggled), [this](bool state)
    { if (state==1)
        {
            editableBox->setCheckState(Qt::Unchecked);
            connect(updateTimer, SIGNAL(timeout()), SLOT(updateTableSlot()));
            updateTimer->start(500);
        }
        else
        {
            disconnect(updateTimer, SIGNAL(timeout()),nullptr,nullptr);
        }

    });
    connect(editBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            [=](const QString &text){ emit actionSelectSlot(text);});

    tableBox->setCurrentIndex(3);
}

void adminInterface::updateTableSlot()
{
    model->setTable(currentTableName);
    model->select();
    table->setModel(model);
}

void adminInterface::tableSelectSlot(const QString &text)
{
    if (text == "Select table...") return;
    nameL->setText("Table: " + text);
    currentTableName = map.value(text);
    model->setTable(map.value(text));
    model->select();
    table->setModel(model);
    table->resizeColumnsToContents();
    tableBox->setCurrentIndex(0);
}

void adminInterface::actionSelectSlot(const QString &text)
{
    if (text == "Edit base")
    {
        if (storeIn) delete storeIn;
        storeIn = new storekeeperInterface(db);
        storeIn->show();
    }
    if (text == "New ingridient")
    {
        if (newIng) delete newIng;
        newIng = new newIngridient(db);
        newIng->show();
    }
    if (text == "New recepie")
    {
        if (newRec) delete newRec;
        newRec = new newRecepie(db);
        newRec->show();
    }
    if (text == "Edit recepies")
    {
        if (editRec) delete editRec;
        editRec = new editRecepie(db);
        editRec->show();
    }
    if (text == "Edit price")
    {
        if (editPri) delete editPri;
        editPri = new editPrice(db);
        editPri->show();
    }
    if (text == "Clear proceeds")
    {
        QDialog dlg(this);
        QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        QFormLayout *layout = new QFormLayout();
        QLabel *text = new QLabel("Clear proceeds?");
        layout->addWidget(text);
        layout->addWidget(btn_box);
        dlg.setLayout(layout);
        if(dlg.exec() == QDialog::Accepted) {
            myQ.exec("DELETE FROM myschema.proceeds");
        }


    }
    if (text == "Wipe database")
    {
        QDialog dlg(this);
        QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        QFormLayout *layout = new QFormLayout();
        QLabel *text = new QLabel("⚠ Wipe database? ⚠");
        layout->addWidget(text);
        layout->addWidget(btn_box);
        dlg.setLayout(layout);
        if(dlg.exec() == QDialog::Accepted) {
            myQ.exec("DROP TABLE myschema.proceeds");
            myQ.exec("DROP TABLE myschema.shawarmasordered");
            myQ.exec("DROP TABLE myschema.orders");
            myQ.exec("DROP TABLE myschema.price");
            myQ.exec("DROP TABLE myschema.base");
            myQ.exec("DROP TABLE myschema.popular");
            myQ.exec("DROP TABLE myschema.recepies");
        }
    }
    if (text == "Reset database")
    {
        QDialog dlg(this);
        QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        QFormLayout *layout = new QFormLayout();
        QLabel *text = new QLabel("⚠ Reset database? ⚠");
        layout->addWidget(text);
        layout->addWidget(btn_box);
        dlg.setLayout(layout);
        if(dlg.exec() == QDialog::Accepted) {
            myQ.exec("DELETE FROM myschema.price");
            myQ.exec("DELETE FROM myschema.base");
            myQ.exec("DELETE FROM myschema.proceeds");
            myQ.exec("DELETE FROM myschema.shawarmasordered");
            myQ.exec("DELETE FROM myschema.orders");
            myQ.exec("DELETE FROM myschema.popular");
            myQ.exec("DELETE FROM myschema.recepies");
            myQ.exec("CREATE TABLE IF NOT EXISTS myschema.recepies (shawarmas VARCHAR(20) NOT NULL DEFAULT '0' PRIMARY KEY, tomatoes int DEFAULT 0, cabbage int DEFAULT 0, chicken int DEFAULT 0, sauce int DEFAULT 0, cheese int DEFAULT 0, CONSTRAINT shawarmas_check UNIQUE (shawarmas), CONSTRAINT tomatoes_check CHECK(tomatoes >=0), CONSTRAINT cabbage_check CHECK(cabbage >= 0), CONSTRAINT chicken_check CHECK(chicken >=0), CONSTRAINT sauce_check CHECK(sauce >=0), CONSTRAINT cheese_check CHECK(cheese >=0))");
            myQ.exec("INSERT INTO myschema.recepies VALUES ('BBQ', 210, 230, 80, 50, 40), ('Meety', 100, 0, 400, 80, 0), ('Florida', 200, 100, 50, 50, 50)");
            myQ.exec("CREATE TABLE IF NOT EXISTS myschema.shawarmasordered (shawarma VARCHAR(20) NOT NULL DEFAULT '0', size VARCHAR(10) NOT NULL, amount int NOT NULL, sum int NOT NULL, CONSTRAINT size_check CHECK(size = 'x' OR size = 'xx'OR size = 'xxx'), CONSTRAINT amount_check CHECK(amount > 0), CONSTRAINT sum_check CHECK(sum >= 0))");
            myQ.exec("CREATE TABLE IF NOT EXISTS myschema.orders (number int NOT NULL, sum int, time VARCHAR(80) NOT NULL, CONSTRAINT number_check CHECK(number > 0), CONSTRAINT sum_check CHECK(sum >= 0))");
            myQ.exec("CREATE TABLE IF NOT EXISTS myschema.price (shawarma VARCHAR(20) NOT NULL DEFAULT '0' REFERENCES myschema.recepies(shawarmas) ON DELETE CASCADE ON UPDATE CASCADE, x int DEFAULT 0, xx int DEFAULT 0, xxx int DEFAULT 0, CONSTRAINT shawarma_check UNIQUE(shawarma))");
            myQ.exec("CREATE TABLE IF NOT EXISTS myschema.proceeds (date VARCHAR(30) NOT NULL, sum int, popular VARCHAR(30))");
            myQ.exec("INSERT INTO myschema.price VALUES ('BBQ', 200, 220, 250), ('Meety', 300, 400, 500), ('Florida', 250, 300, 350)");
            myQ.exec("CREATE TABLE IF NOT EXISTS myschema.base (ingridients VARCHAR(20) NOT NULL, minimum int, remaining int, CONSTRAINT ingridients_check UNIQUE(ingridients), CONSTRAINT minimum_check CHECK(minimum > 0), CONSTRAINT remaining_check CHECK (remaining > minimum +50))");
            myQ.exec("INSERT INTO myschema.base VALUES('Chicken', 100, 500), ('Tomatoes', 400, 800), ('Cabbage', 300, 500), ('Sauce', 700, 900), ('Cheese', 50, 400)");
            myQ.exec("CREATE TABLE myschema.popular (shawarma VARCHAR(30) NOT NULL DEFAULT '0' REFERENCES myschema.recepies(shawarmas) ON DELETE CASCADE ON UPDATE CASCADE, counter int DEFAULT 0)");
            myQ.exec("INSERT INTO myschema.popular VALUES ('BBQ', 0), ('Meety', 0), ('Florida', 0)");
        }
    }

    editBox->setCurrentIndex(0);

}

void adminInterface::onCustomContextMenu(const QPoint &point)
{
    disconnect(updateTimer, SIGNAL(timeout()),nullptr,nullptr);

    QModelIndex index = table->indexAt(point);
    qDebug() << index.row();
    table->selectRow(index.row());

    if (!index.isValid())
        return;

    if (!contextMenu)
    {
        removeAction = new QAction("Delete row", contextMenu);
        removeIngAction = new QAction("Delete Ingridient", contextMenu);
    }
    else delete contextMenu;

    contextMenu = new QMenu;
    contextMenu->addAction(removeAction);

    QString ingrid;
    if (index.column()>0)
    {
        QString colName = model->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
        QString firstSimb =  colName.at(0).toUpper();
        QString nolmalColName = firstSimb+colName.remove(0,1);
        qDebug() <<nolmalColName;
        myQ.exec("SELECT ingridients FROM myschema.base WHERE ingridients = '"+nolmalColName+"';");
        myQ.next(); ingrid = myQ.record().value(0).toString(); qDebug() <<ingrid;
        if (nolmalColName==ingrid)
            contextMenu->addAction(removeIngAction);
    }

    QPoint globalPos = table->mapToGlobal(point);
    QAction* selectedItem = contextMenu->exec(globalPos);
    if (selectedItem == removeAction)
    {
        qDebug()<< index.data(Qt::DisplayRole).toString();
        myQ.exec("SELECT * FROM " +currentTableName);
        QString firstColumName = myQ.record().fieldName(0);
        QString secondColName = myQ.record().fieldName(1);
        myQ.exec("DELETE FROM "+currentTableName+" WHERE "+firstColumName+" = '"+model->index(index.row(),0).data(Qt::DisplayRole).toString()+"' AND "+secondColName+" = '"+model->index(index.row(),1).data(Qt::DisplayRole).toString()+"';");
        contextMenu->hide();
    }
    if (selectedItem == removeIngAction)
    {
        myQ.exec("ALTER TABLE myschema.recepies DROP COLUMN " + ingrid + " CASCADE;");
        myQ.exec("DELETE FROM myschema.base WHERE ingridients = '" + ingrid + "'");
    }
    model->setTable(currentTableName);
    model->select();
    table->setModel(model);
    connect(updateTimer, SIGNAL(timeout()), SLOT(updateTableSlot()));
}
