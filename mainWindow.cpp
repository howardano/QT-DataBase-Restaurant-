#include "mainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Main menu");
    QWidget *centralWidget = new QWidget;
    const QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
    resize(availableSize.width() / 3, availableSize.height() / 3);


    QPushButton *admiB = new MyQPushButton("⚙ Administrator  ");
    QPushButton *chefB = new MyQPushButton("🍔 Chef  ");//🍳🍗
    QPushButton *storB = new MyQPushButton("🏃 Storekeeper  ");
    QPushButton *quitB = new MyQPushButton("❌ Quit  ");

    QGridLayout *butLayout = new QGridLayout;
    QWidget *left = new QWidget; QWidget *right = new QWidget;
    butLayout->addWidget(left,0,0,10,1);
    butLayout->setSpacing(20);
    butLayout->addWidget(right,0,6,10,1);
    butLayout->addWidget(admiB,1,1,2,4);
    butLayout->addWidget(chefB,3,1,2,4);
    butLayout->addWidget(storB,5,1,2,4);
    butLayout->addWidget(quitB,7,1,2,4);
    centralWidget->setLayout(butLayout);
    setCentralWidget(centralWidget);

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("fn1132_2019");
    db.setHostName("195.19.32.74");
    db.setPort(5432);
    db.setUserName("student");
    db.setPassword("bmstu");
    msg = new QMessageBox;
    if (db.open()) {
        msg->setText("Connected");

        QSqlQuery myQ = QSqlQuery(db);
        /*myQ.exec("CREATE TABLE myschema.proceeds (number int, date VARCHAR(20) NOT NULL, sum int, CONSTRAINT number_check CHECK(number > 0), CONSTRAINT sum_check CHECK(sum > 0));");
        myQ.exec("CREATE TABLE myschema.shawarmasordered (shawarma VARCHAR(20) NOT NULL, size VARCHAR(10) NOT NULL, amount int NOT NULL, sum int,  CONSTRAINT size_check CHECK(size = 'x' OR size = 'xx'OR size = 'xxx'), CONSTRAINT amount_check CHECK(amount > 0), CONSTRAINT sum_check CHECK(sum >= 0));");
        myQ.exec("IF NOT EXIST CREATE TABLE myschema.orders (number int NOT NULL, sum int, time VARCHAR(80) NOT NULL, CONSTRAINT number_check CHECK(number > 0), CONSTRAINT sum_check CHECK(sum >= 0));");
        myQ.exec("IF NOT EXIST CREATE TABLE myschema.price (shawarma VARCHAR(20) NOT NULL REFERENCES myschema.recepies(shawarmas) ON DELETE CASCADE, x int, xx int, xxx int, CONSTRAINT shawarma_check UNIQUE(shawarma));");
        myQ.exec("IF NOT EXIST CREATE TABLE myschema.recepies (shawarmas VARCHAR(20) PRIMARY KEY, tomatoes int, cabbage int, chicken int, sauce int, cheese int, CONSTRAINT shawarmas_check UNIQUE (shawarmas), CONSTRAINT tomatoes_check CHECK(tomatoes >=0), CONSTRAINT cabbage_check CHECK(cabbage >= 0), CONSTRAINT chicken_check CHECK(chicken >=0), CONSTRAINT sauce_check CHECK(sauce >=0), CONSTRAINT cheese_check CHECK(cheese >=0));");
        myQ.exec("IF NOT EXIST CREATE TABLE myschema.base (ingridients VARCHAR(20) NOT NULL, minimum int, remaining int, CONSTRAINT ingridients_check UNIQUE(ingridients), CONSTRAINT minimum_check CHECK(minimum > 0), CONSTRAINT remaining_check CHECK (remaining > minimum +50));");
    */
    }
    else {
        msg->setText("Can not connect");
    }
    //msg->show();
    setMinimumSize(QSize(250,300));

    connect(admiB, SIGNAL(clicked()), SLOT(adminButtonClicked()));
    connect(chefB, SIGNAL(clicked()), SLOT(shefButtonClicked()));
    connect(storB, SIGNAL(clicked()), SLOT(storekeeperButtonClicked()));
    connect(quitB, SIGNAL(clicked()), SLOT(quitButtonClicked()));
}

MainWindow::~MainWindow() {
}

void MainWindow::adminButtonClicked() {
    if (adIn) delete adIn;
    adIn = new adminInterface(db);
    adIn->show();
}
void MainWindow::shefButtonClicked() {
    if (chefIn) delete chefIn;
    chefIn = new chefInterface(db);
    chefIn->show();
}
void MainWindow::storekeeperButtonClicked() {
    if (storeIn) delete storeIn;
    storeIn = new storekeeperInterface(db);
    storeIn->show();
}
void MainWindow::quitButtonClicked() {
    QApplication::quit();
}
