#include "adminActions.h"


//###############################################     EDIT    RECIPIE    WINDOW ################################################
editRecepie::editRecepie(QSqlDatabase database, QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Recepie editing");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 3, availableSize.height() / 3);
    db = database;
    db.open();
    myQ = QSqlQuery(db);

    //область изменения ингридиентов
    myQ.exec("SELECT shawarmas FROM myschema.recepies");
    while (myQ.next())
        recipies.append(myQ.record().value(0).toString());

    QLabel *editRecL = new MyQLabel("Edit Recepie");
    editRecL->setAlignment(Qt::AlignHCenter);
    QLabel *recL = new MyQLabel("Recepie");
    recB = new MyQComboBox;
    recB->addItem("...");
    recB->addItems(recipies);

    QCheckBox *nameB = new MyQCheckBox("Edit name");
    nameLine = new MyQLineEdit;
    nameLine->setDisabled(1);
    connect(nameB, QOverload<bool>::of(&QCheckBox::toggled), [=](bool state){if (state) nameLine->setDisabled(0); else nameLine->setDisabled(1);});

    ingrGridW = new QWidget;
    ingrGridL = new QGridLayout;
    ingrGridL->setColumnStretch(0,4);
    ingrGridL->setColumnStretch(1,4);
    ingrGridL->setColumnStretch(2,1);

    ingrGridW->setLayout(ingrGridL);
    ingrGridW->setFixedHeight(10);
    ingrGridW->setContentsMargins(0,0,10,0);
    QScrollArea *scrollArea = new MyQScrollArea();
    scrollArea->setWidget(ingrGridW);

    QPushButton *addIngB = new MyQPushButton("Add ingridient");
    QPushButton *editCostB = new MyQPushButton("💰 Edit costs  ");
    QPushButton *addRecB = new MyQPushButton("✅ Save Recepie  ");
    QPushButton *delRecB = new MyQPushButton("🚫 Delete Recepie  ");

    //компоновка на главный слой
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(editRecL,0,0,1,2);
    mainLayout->addWidget(recL,1,0,1,1);
    mainLayout->addWidget(recB,1,1,1,1);

    mainLayout->addWidget(nameB,2,0,1,1);
    mainLayout->addWidget(nameLine,2,1,1,1);

    mainLayout->addWidget(scrollArea,3,0,6,2);
    mainLayout->addWidget(addIngB,9,0,1,2);
    mainLayout->addWidget(editCostB,10,0,1,2);
    QHBoxLayout *addDelL = new QHBoxLayout;
    addDelL->setContentsMargins(0,0,0,0);
    addDelL->addWidget(addRecB);
    addDelL->addWidget(delRecB);
    mainLayout->addLayout(addDelL,11,0,1,2);

    setLayout(mainLayout);
    mainLayout->setColumnStretch(1,10);
    msg = new QMessageBox;
    connect(addIngB, SIGNAL(clicked()), SLOT(makeRowSlot()));
    connect(recB, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [=](const QString &text){ emit uploadRecepieSlot(text);});
    connect(addRecB, SIGNAL(clicked()), SLOT(saveRecepieSlot()));
    connect(delRecB, SIGNAL(clicked()), SLOT(deleteMsgSlot()));
    connect(editCostB, SIGNAL(clicked()), SLOT(openPriceEditSlot()));
}

void editRecepie::openPriceEditSlot()
{
    if (recB->currentText() == "...")
    {
        msg->setText("Select shawarma first.");
        msg->show();
        return;
    }
    if (editPri) delete editPri;
    editPri = new editPrice(db);
    editPri->loadPricesSlot(recB->currentText());
    editPri->setDisabledSlot();
    editPri->show();
}

void editRecepie::saveRecepieSlot()
{
    if (recB->currentText() == "...")
    {
        msg->setText("Select shawarma first.");
        msg->show();
        return;
    }
    if (nameLine->text() != "")
    {
        myQ.exec("UPDATE myschema.recepies SET shawarmas = '"+nameLine->text()+"' WHERE shawarmas = '"+recB->currentText()+"';");
        recB->setItemText(recB->currentIndex(), nameLine->text());
        qDebug() << "row count = " <<ingrGridL->rowCount();

        //сохранение ингридиентов
        for(int i = 0; i < ingrGridL->rowCount(); i++)
        {
            QComboBox *cb = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
            QSpinBox *sb = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
            QString ingridient = cb->currentText();
            int amount = sb->value();
            qDebug() << ingridient <<" "<<amount;
            myQ.exec("UPDATE myschema.recepies SET "+ingridient.toLower()+" = "+QString::number(amount)+" WHERE shawarmas = '"+nameLine->text()+"';");
        }
        QStringList nullIngr;
        for (auto ingr: ingridients) nullIngr.append(ingr);
        nullIngr.removeOne(lastIngr->currentText());
        for (auto ingr: nullIngr)
            myQ.exec("UPDATE myschema.recepies SET "+ingr.toLower()+" = 0 WHERE shawarmas = '"+nameLine->text()+"';");
        msg->setText("✅ Shawarma saved.");
        msg->show();
        close();
    }
    else
    {
        msg->setText("Choose shawarma name please.");
        msg->show();
        return;
    }

}

void editRecepie::deleteMsgSlot()
{
    if (recB->currentText() == "...")
    {
        msg->setText("Select shawarma first.");
        msg->show();
        return;
    }
    msg->setText("Do you want to remove "+nameLine->text()+" recepie?");
    connect(msg, SIGNAL(accepted()),  SLOT(deleteRecepieSlot()));
    connect(msg, SIGNAL(rejected()),  SLOT(disconnectMsgSlot()));
    msg->show();
}

void editRecepie::deleteRecepieSlot()
{
    //delete
    qDebug()<< recB->currentText();
    myQ.exec("DELETE FROM myschema.recepies WHERE shawarmas = '"+recB->currentText()+"';");
    recB->removeItem(recB->currentIndex());
    nameLine->setText(recB->currentText());
    disconnect(msg, SIGNAL(accepted()), nullptr, nullptr);
    if (recB->count()==1) close();
}

void editRecepie::disconnectMsgSlot()
{
    disconnect(msg, SIGNAL(accepted()), nullptr, nullptr);
}

void editRecepie::uploadRecepieSlot(const QString &recepie)
{
    qDebug() << "row count = " <<ingrGridL->rowCount();
    if (ingrGridL->rowCount() != 1)
    {
        for (int i=0; i<ingrGridL->rowCount(); i++)
        {
            if (ingrGridL->itemAtPosition(i,0) != nullptr)
            {
                QComboBox *cb = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
                cb->hide();
                delete cb;
                QSpinBox *sb = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
                sb->hide();
                delete sb;
                QPushButton *pb = qobject_cast<QPushButton*>(ingrGridL->itemAtPosition(i,2)->widget());
                pb->hide();
                delete pb;
            }
        }
    }
    ingridients.clear();
    lastIngr = nullptr;
    lastCount = nullptr;
    rowNumber = 0;
    ingrGridW->setFixedHeight(10);

    nameLine->setText(recepie);
    myQ.exec("SELECT ingridients FROM myschema.base");
    while (myQ.next())
        ingridients.append(myQ.record().value(0).toString());

    myQ.exec("SELECT * FROM myschema.recepies WHERE shawarmas = '"+recepie+"'");
    QSqlRecord R = myQ.record();
    myQ.next();
    for (int i = 0; i < myQ.record().count(); i++)
    {
        qDebug() << R.fieldName(i) << " - " << myQ.record().value(i).toString();
        if (i>0 && myQ.record().value(i).toInt() != 0)
        {
            QString ingr = R.fieldName(i).at(0).toUpper()+R.fieldName(i).remove(0,1);
            qDebug() << ingr;
            makeRowSlot();
            lastIngr->setCurrentText(ingr);
            lastCount->setValue(myQ.record().value(i).toInt());
        }
    }
}

void editRecepie::makeRowSlot()
{
    if (recB->currentText() == "...")
    {
        msg->setText("Select shawarma first.");
        msg->show();
        return;
    }
    if (ingridients.count()== 1) {lastIngr->setDisabled(1); return;}
    if (lastIngr && lastIngr->currentText()!="ingridient..." && lastCount->value()!=0)
    {
        ingridients.removeOne(lastIngr->currentText());
        lastIngr->setDisabled(1);
    }
    else if (lastIngr)
    {
        msg->setText("Choose ingridient and amount first.");
        msg->show();
        return;
    }
    for (auto b: ingridients) qDebug() << b;

    qDebug() << ingridients.count();
    QComboBox *ingridientB = new MyQComboBox;
    ingridientB->setMaximumHeight(50);
    if (ingridients.count()>1) ingridientB->addItem("ingridient...");
    ingridientB->addItems(ingridients);
    lastIngr = ingridientB;
    QSpinBox *countB = new MyQSpinBox;
    QPushButton *deleteB = new MyQPushButton("✕");
    countB->setMaximumHeight(50);
    lastCount = countB;
    deleteB->setMaximumHeight(50);
    ingrGridW->setFixedHeight(ingrGridW->height()+50);

    ingrGridL->addWidget(ingridientB,rowNumber,0,1,1);
    ingrGridL->addWidget(countB,rowNumber,1,1,1);
    ingrGridL->addWidget(deleteB,rowNumber,2,1,1);
    rowNumber++;
    qDebug() << "row num = " <<rowNumber;

    if (ingridients.count()== 1) lastIngr->setDisabled(1);
    connect(ingridientB, SIGNAL(activated(int index)), SLOT(resetListSlot()));
    connect(deleteB, SIGNAL(clicked()), SLOT(deleteRowSlot()));
}

void editRecepie::deleteRowSlot() {
    //qDebug() <<  sender();
    if (rowNumber == 1) return;
    for(int i = 0; i < ingrGridL->count(); i++){
        if (ingrGridL->itemAtPosition(i,2)==nullptr)
            continue;
        QPushButton *pb = qobject_cast<QPushButton*>(ingrGridL->itemAtPosition(i,2)->widget());
        if  (pb==sender())
        {
            bool key = 0;
            pb->hide();
            delete pb;
            QComboBox *cb = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
            if (i != ingrGridL->rowCount()-1) ingridients.append(cb->currentText());
            else key=1;
            cb->hide();
            delete cb;
            QSpinBox *sb = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
            sb->hide();
            delete sb;

            rowNumber--;
            updateRows();

            for(int i = 0; i < ingrGridL->count(); i++)
                if (ingrGridL->itemAtPosition(i,0)!=nullptr)
                {
                    lastIngr = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
                    lastCount = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
                }
            lastIngr->setDisabled(0);
            QString curI = lastIngr->currentText(); lastIngr->clear();
            if (ingridients.count()>1) lastIngr->addItem("ingridient...");
            if (key) ingridients.append(curI);
            lastIngr->addItems(ingridients);
            lastIngr->setCurrentText(curI);

            ingrGridW->setFixedHeight(ingrGridW->height()-50);
            for (auto b: ingridients) qDebug() << b;

            ingrGridL->setColumnStretch(0,4);
            ingrGridL->setColumnStretch(1,4);
            ingrGridL->setColumnStretch(2,1);

            return;
        }
    }
}

void editRecepie::updateRows()
{
    QGridLayout* lay = new QGridLayout;
    int j = 0;
    for (int i = 0; i < ingrGridL->rowCount(); ++i) {
        if (ingrGridL->itemAtPosition(i, 0) == nullptr) {
            continue;
        }
        lay->addWidget(ingrGridL->itemAtPosition(i, 0)->widget(), j, 0);
        lay->addWidget(ingrGridL->itemAtPosition(i, 1)->widget(), j, 1);
        lay->addWidget(ingrGridL->itemAtPosition(i, 2)->widget(), j, 2);
        ++j;
    }

    delete ingrGridL;
    ingrGridL = new QGridLayout(this);
    ingrGridL = lay;
    ingrGridW->setLayout(ingrGridL);
    //setCentralWidget(main_widget);
}
//###############################################     NEW    INGRIDIENT    WINDOW ################################################
newIngridient::newIngridient(QSqlDatabase database, QWidget *parent) : QWidget(parent)
{
    setWindowTitle("New ingridient adding");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 6, availableSize.height() / 6);
    db = database;
    db.open();
    myQ = QSqlQuery(db);
    QLabel *newIngL = new MyQLabel("New Ingridient");
    newIngL->setAlignment(Qt::AlignHCenter);
    QLabel *nameL = new MyQLabel("Name");
    nameLine = new MyQLineEdit;

    QLabel *minCountL = new MyQLabel("Min count");
    minCountB = new MyQSpinBox;

    QLabel *remainL = new MyQLabel("Remaining");
    remainB = new MyQSpinBox;

    QPushButton *addB = new MyQPushButton("Add Ingridient");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(newIngL,0,0,1,2);
    mainLayout->addWidget(nameL,1,0,1,1);
    mainLayout->addWidget(nameLine,1,1,1,1);

    mainLayout->addWidget(minCountL,2,0,1,1);
    mainLayout->addWidget(minCountB,2,1,1,1);

    mainLayout->addWidget(remainL,3,0,1,1);
    mainLayout->addWidget(remainB,3,1,1,1);

    mainLayout->addWidget(addB,4,0,1,2);

    mainLayout->setColumnStretch(1,10);
    setLayout(mainLayout);
    setMinimumSize(QSize(250,300));
    msg = new QMessageBox(this);

    connect(addB, SIGNAL(clicked()), SLOT(addIngridientSlot()));
}

void newIngridient::addIngridientSlot()
{
    if (minCountB->value() == 0)
    {
        msg->setText("The Minimum value cant be zero.");
        msg->show();
        return;
    }
    if (remainB->value() < minCountB->value()+51)
    {
        msg->setText("Not enougth in remaining. Put more.");
        msg->show();
        return;
    }
    qDebug() << nameLine->text() <<" "<< minCountB->value() << " " <<  remainB->value();
    myQ.exec("ALTER TABLE myschema.recepies ADD "+nameLine->text()+" int NOT NULL DEFAULT 0");
    myQ.exec("ALTER TABLE ADD CONSTRAINT "+nameLine->text()+"_check CHECK('"+nameLine->text()+"' >= "+QString::number(minCountB->value())+")");
    myQ.exec("INSERT INTO myschema.base VALUES ('"+nameLine->text()+"', "+QString::number(minCountB->value())+", "+QString::number(remainB->value())+");");

    msg->setText("✅ Ingridient successfully added.");
    msg->show();
    close();
}
//###############################################     NEW    RECEPIE    WINDOW ################################################
newRecepie::newRecepie(QSqlDatabase database, QWidget *parent) : QWidget(parent)
{
    setWindowTitle("New recepie adding");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 3, availableSize.height() / 3);
    db = database;
    db.open();
    myQ = QSqlQuery(db);

    //ингридиенты
    myQ.exec("SELECT ingridients FROM myschema.base");
    while (myQ.next())
        ingridients.append(myQ.record().value(0).toString());

    QLabel *newRecL = new MyQLabel("New Recepie");
    newRecL->setAlignment(Qt::AlignHCenter);
    QLabel *nameL = new MyQLabel("Shawarma name");
    nameL->setAlignment(Qt::AlignCenter);
    nameLine = new MyQLineEdit;

    ingrGridW = new QWidget;
    ingrGridL = new QGridLayout;
    ingrGridL->setColumnStretch(0,4);
    ingrGridL->setColumnStretch(1,4);
    ingrGridL->setColumnStretch(2,1);

    ingrGridW->setLayout(ingrGridL);
    ingrGridW->setFixedHeight(10);
    QScrollArea *scrollArea = new MyQScrollArea();
    scrollArea->setWidget(ingrGridW);
    makeRowSlot();

    QPushButton *addIngB = new MyQPushButton("Add ingridient");
    QPushButton *addRecB = new MyQPushButton("✅ Save Recepie  ");

    //цены
    myQ.exec("SELECT shawarmas FROM myschema.recepies");
    while (myQ.next())
        recepies.append(myQ.record().value(0).toString());
    for (auto b: recepies) qDebug()<<b;

    myQ.exec("SELECT * FROM myschema.price");
    QSqlRecord R = myQ.record();
    for (int i = 1; i < R.count(); i++)
        sizes.append(R.fieldName(i).toUpper());
    for (auto b: sizes) qDebug()<<b;

    QLabel *sizeCostL = new MyQLabel("Size/Cost");
    sizeCostL->setAlignment(Qt::AlignCenter);

    sizeGridW = new QWidget;
    sizeGridW->setContentsMargins(0,0,10,0);
    sizeGridL = new QGridLayout;
    sizeGridL->setColumnStretch(1,10);

    for (int i=0; i<sizes.count(); i++)
    {
        QLabel *sizeL = new MyQLabel(sizes.at(i));
        sizeL->setMaximumHeight(40);
        QSpinBox *costB = new MyQSpinBox();
        costB->setMaximumHeight(40);
        costB->setValue(100);
        sizeGridL->addWidget(sizeL,i,0,1,1);
        sizeGridL->addWidget(costB,i,1,1,1);
        sizeGridW->setFixedHeight(sizeGridW->height()+40);
    }
    sizeGridW->setLayout(sizeGridL);
    sizeGridW->setFixedHeight(200);
    QScrollArea *sizeScrollArea = new MyQScrollArea();
    sizeScrollArea->setWidget(sizeGridW);
    sizeScrollArea->setContentsMargins(0,0,0,0);


    //компоновка в главный слой
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(newRecL,0,0,1,3);
    mainLayout->addWidget(nameL,1,0,1,1);
    mainLayout->addWidget(nameLine,1,1,1,1);
    mainLayout->addWidget(sizeCostL,1,2,1,1);

    mainLayout->addWidget(scrollArea,2,0,6,2);
    mainLayout->addWidget(sizeScrollArea,2,2,6,1);

    mainLayout->addWidget(addIngB,8,0,1,2);
    mainLayout->addWidget(addRecB,8,2,1,1);

    setLayout(mainLayout);
    mainLayout->setColumnStretch(1,10);
    mainLayout->setColumnStretch(2,10);
    msg = new QMessageBox;
    connect(addIngB, SIGNAL(clicked()), SLOT(makeRowSlot()));
    connect(addRecB, SIGNAL(clicked()), SLOT(saveRecepieSlot()));
}

void newRecepie::saveRecepieSlot()
{
    if (nameLine->text() != "")
    {
        myQ.exec("SELECT shawarmas FROM myschema.recepies WHERE shawarmas = '"+nameLine->text()+"';");
        myQ.next(); if (myQ.record().value(0).toString() == nameLine->text())
        {
            qDebug()<<myQ.record().value(0).toString();
            msg->setText("Recepie of "+nameLine->text()+" allready exists.");
            msg->show();
            return;
        }

        //сохранение ингридиентов
        myQ.exec("INSERT INTO myschema.recepies DEFAULT VALUES;");
        myQ.exec("UPDATE myschema.recepies SET shawarmas = '"+nameLine->text()+"' WHERE shawarmas = '0';");
        myQ.exec("INSERT INTO myschema.popular VALUES ('"+nameLine->text()+"', DEFAULT)");
        qDebug() << "row count = " <<ingrGridL->rowCount();
        //msg->setText("Do you want to save "+nameLine->text()+" recepie?");
        for(int i = 0; i < ingrGridL->rowCount(); i++)
        {
            QComboBox *cb = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
            QSpinBox *sb = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
            QString ingridient = cb->currentText();
            int amount = sb->value();
            qDebug() << ingridient <<" "<<amount;
            myQ.exec("UPDATE myschema.recepies SET "+ingridient.toLower()+" = "+QString::number(amount)+" WHERE shawarmas = '"+nameLine->text()+"';");
        }

        //cохранение цен
        QStringList x;
        for (int i=0; i<sizeGridL->rowCount(); i++)
        {
            QSpinBox *sb = qobject_cast<QSpinBox*>(sizeGridL->itemAtPosition(i,1)->widget());
            x.append(QString::number(sb->value()));
            qDebug() << x;
        }
        myQ.exec("INSERT INTO myschema.price VALUES ('"+nameLine->text()+"', "+x.at(0)+", "+x.at(1)+", "+x.at(2)+");");

        msg->setText("✅ Recepie & Pricelist saved.");
        msg->show();
        close();
    }
    else
    {
        msg->setText("Choose shawarma name please.");
        msg->show();
        return;
    }
}

void newRecepie::makeRowSlot()
{
    if (ingridients.count()== 1) {lastIngr->setDisabled(1); return;}
    if (lastIngr && lastIngr->currentText()!="ingridient..." && lastCount->value()!=0)
    {
        ingridients.removeOne(lastIngr->currentText());
        lastIngr->setDisabled(1);
    }
    else if (lastIngr)
    {
        msg->setText("Choose ingridient and amount first.");
        msg->show();
        return;
    }
    for (auto b: ingridients) qDebug() << b;

    qDebug() << ingridients.count();
    QComboBox *ingridientB = new MyQComboBox;
    ingridientB->setMaximumHeight(50);
    if (ingridients.count()>1) ingridientB->addItem("ingridient...");
    ingridientB->addItems(ingridients);
    lastIngr = ingridientB;
    QSpinBox *countB = new MyQSpinBox;
    QPushButton *deleteB = new MyQPushButton("✕");
    countB->setMaximumHeight(50);
    lastCount = countB;
    deleteB->setMaximumHeight(50);
    ingrGridW->setFixedHeight(ingrGridW->height()+50);

    ingrGridL->addWidget(ingridientB,rowNumber,0,1,1);
    ingrGridL->addWidget(countB,rowNumber,1,1,1);
    ingrGridL->addWidget(deleteB,rowNumber,2,1,1);
    rowNumber++;
    qDebug() << "row num = " <<rowNumber;

    if (ingridients.count()== 1) lastIngr->setDisabled(1);
    connect(ingridientB, SIGNAL(activated(int index)), SLOT(resetListSlot()));
    connect(deleteB, SIGNAL(clicked()), SLOT(deleteRowSlot()));
}

void newRecepie::deleteRowSlot() {
    //qDebug() <<  sender();
    if (rowNumber == 1) return;
    for(int i = 0; i < ingrGridL->count(); i++){
        if (ingrGridL->itemAtPosition(i,2)==nullptr)
            continue;
        QPushButton *pb = qobject_cast<QPushButton*>(ingrGridL->itemAtPosition(i,2)->widget());
        if  (pb==sender())
        {
            bool key = 0;
            pb->hide();
            delete pb;
            QComboBox *cb = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
            if (i != ingrGridL->rowCount()-1) ingridients.append(cb->currentText());
            else key=1;
            cb->hide();
            delete cb;
            QSpinBox *sb = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
            sb->hide();
            delete sb;

            rowNumber--;
            updateRows();

            for(int i = 0; i < ingrGridL->count(); i++)
                if (ingrGridL->itemAtPosition(i,0)!=nullptr)
                {
                    lastIngr = qobject_cast<QComboBox*>(ingrGridL->itemAtPosition(i,0)->widget());
                    lastCount = qobject_cast<QSpinBox*>(ingrGridL->itemAtPosition(i,1)->widget());
                }
            lastIngr->setDisabled(0);
            QString curI = lastIngr->currentText(); lastIngr->clear();
            if (ingridients.count()>1) lastIngr->addItem("ingridient...");
            if (key) ingridients.append(curI);
            lastIngr->addItems(ingridients);
            lastIngr->setCurrentText(curI);

            ingrGridW->setFixedHeight(ingrGridW->height()-50);
            for (auto b: ingridients) qDebug() << b;
            return;
        }
    }
}

void newRecepie::updateRows()
{
    QGridLayout* lay = new QGridLayout;
    int j = 0;
    for (int i = 0; i < ingrGridL->rowCount(); ++i) {
        if (ingrGridL->itemAtPosition(i, 0) == nullptr) {
            continue;
        }
        lay->addWidget(ingrGridL->itemAtPosition(i, 0)->widget(), j, 0);
        lay->addWidget(ingrGridL->itemAtPosition(i, 1)->widget(), j, 1);
        lay->addWidget(ingrGridL->itemAtPosition(i, 2)->widget(), j, 2);
        ++j;
    }

    delete ingrGridL;
    ingrGridL = new QGridLayout(this);
    ingrGridL = lay;
    ingrGridW->setLayout(ingrGridL);
    //setCentralWidget(main_widget);
}
//###############################################     NEW    PRICE    WINDOW ################################################
editPrice::editPrice(QSqlDatabase database, QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Price editing");
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 6, availableSize.height() / 3);
    db = database;
    db.open();
    myQ = QSqlQuery(db);

    myQ.exec("SELECT shawarmas FROM myschema.recepies");
    while (myQ.next())
        recepies.append(myQ.record().value(0).toString());
    for (auto b: recepies) qDebug()<<b;

    myQ.exec("SELECT * FROM myschema.price");
    QSqlRecord R = myQ.record();
    for (int i = 1; i < R.count(); i++)
        sizes.append(R.fieldName(i).toUpper());
    for (auto b: sizes) qDebug()<<b;

    QLabel *editRecL = new MyQLabel("Edit Price");
    editRecL->setAlignment(Qt::AlignHCenter);
    QLabel *nameL = new MyQLabel("Shawarma");
    nameB = new MyQComboBox;
    nameB->addItem("...");
    nameB->addItems(recepies);

    sizeGridW = new QWidget;
    sizeGridL = new QGridLayout;
    sizeGridL->setColumnStretch(1,10);

    for (int i=0; i<sizes.count(); i++)
    {
        QLabel *sizeL = new MyQLabel(sizes.at(i));
        sizeL->setMaximumHeight(40);
        QSpinBox *costB = new MyQSpinBox();
        costB->setMaximumHeight(40);
        sizeGridL->addWidget(sizeL,i,0,1,1);
        sizeGridL->addWidget(costB,i,1,1,1);
        sizeGridW->setFixedHeight(sizeGridW->height()+40);
    }

    sizeGridW->setLayout(sizeGridL);
    sizeGridW->setFixedHeight(200);
    scrollArea = new MyQScrollArea();
    scrollArea->setWidget(sizeGridW);

    QPushButton *saveB = new MyQPushButton("Save prices");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(editRecL,0,0,1,2);
    mainLayout->addWidget(nameL,1,0,1,1);
    mainLayout->addWidget(nameB,1,1,1,1);
    mainLayout->addWidget(scrollArea,2,0,6,2);
    mainLayout->addWidget(saveB,8,0,1,2);

    setLayout(mainLayout);
    mainLayout->setColumnStretch(1,10);
    msg = new QMessageBox;
    connect(saveB, SIGNAL(clicked()), SLOT(savePricesSlot()));
    connect(nameB, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            [=](const QString &text){ emit loadPricesSlot(text);});
}

void editPrice::loadPricesSlot(const QString &name)
{
    if (name == "...")
    {
        for (int i=0; i<sizeGridL->rowCount(); i++)
        {
            QSpinBox *sb = qobject_cast<QSpinBox*>(sizeGridL->itemAtPosition(i,1)->widget());
            sb->setValue(0);
        }
        return;
    }
    nameB->setCurrentText(name);
    costs.clear();
    myQ.exec("SELECT * FROM myschema.price WHERE shawarma = '"+name+"';");
    myQ.next();
    for (int i=1; i<4; i++)
        costs.append(myQ.record().value(i).toString());

    for (int i=0; i<sizeGridL->rowCount(); i++)
    {
        QSpinBox *sb = qobject_cast<QSpinBox*>(sizeGridL->itemAtPosition(i,1)->widget());
        sb->setValue(costs.at(i).toInt());
    }
}

void editPrice::savePricesSlot()
{
    if (nameB->currentText() == "...")
    {
        msg->setText("Choose shawarma first");
        msg->show();
        return;
    }
    else
    {
        QStringList x;
        for (int i=0; i<sizeGridL->rowCount(); i++)
        {
            QSpinBox *sb = qobject_cast<QSpinBox*>(sizeGridL->itemAtPosition(i,1)->widget());
            x.append(QString::number(sb->value()));
        }
        myQ.exec("UPDATE myschema.price SET x = "+x.at(0)+", xx = "+x.at(1)+", xxx = "+x.at(2)+" WHERE shawarma = '"+nameB->currentText()+"';");

        msg->setText("✅ Pricelist saved.");
        msg->show();
        close();
    }
}

void editPrice::setDisabledSlot()
{
    nameB->setDisabled(1);
}
