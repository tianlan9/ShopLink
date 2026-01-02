#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

void createTables(QSqlDatabase &db) {
    // 检查数据库是否打开
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return;
    }

    // 创建 Users 表
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS Users ("
               "userId INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT NOT NULL, "
               "password TEXT NOT NULL, "
               "email TEXT NOT NULL, "
               "role TEXT NOT NULL)");

    if (query.lastError().isValid()) {
        qDebug() << "Error creating Users table:" << query.lastError().text();
    } else {
        qDebug() << "Users table created successfully.";
    }

    // Ensure backward compatibility: add `salt` column if it doesn't exist (for older DBs)
    bool hasSalt = false;
    query.exec("PRAGMA table_info(Users)");
    while (query.next()) {
        QString colName = query.value(1).toString(); // column name is at index 1
        if (colName == "salt") {
            hasSalt = true;
            break;
        }
    }
    if (!hasSalt) {
        qDebug() << "Adding missing 'salt' column to Users table.";
        if (!query.exec("ALTER TABLE Users ADD COLUMN salt TEXT")) {
            qDebug() << "Error adding salt column:" << query.lastError().text();
        } else {
            qDebug() << "Added 'salt' column to Users table.";
        }
    }

    // 创建 Products 表
    query.exec("CREATE TABLE IF NOT EXISTS Products ("
               "productId INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "description TEXT, "
               "price REAL NOT NULL, "
               "image TEXT)");

    if (query.lastError().isValid()) {
        qDebug() << "Error creating Products table:" << query.lastError().text();
    } else {
        qDebug() << "Products table created successfully.";
    }

    // 创建 Orders 表
    query.exec("CREATE TABLE IF NOT EXISTS Orders ("
               "orderId INTEGER PRIMARY KEY AUTOINCREMENT, "
               "customerId INTEGER NOT NULL, "
               "productId INTEGER NOT NULL, "
               "quantity INTEGER NOT NULL, "
               "orderDate TEXT NOT NULL, "
               "FOREIGN KEY (customerId) REFERENCES Users(userId), "
               "FOREIGN KEY (productId) REFERENCES Products(productId))");

    if (query.lastError().isValid()) {
        qDebug() << "Error creating Orders table:" << query.lastError().text();
    } else {
        qDebug() << "Orders table created successfully.";
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化数据库连接
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ShopLink.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database.");
    }
    createTables(db);

    currentUser = nullptr;  // 默认没有用户登录
}

MainWindow::~MainWindow()
{
    delete ui;
    db.close();  // 关闭数据库
}

void MainWindow::loadProducts()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM Products");

    // 清除现有商品列表
    ui->productListWidget->clear();

    // 遍历查询结果并添加到商品列表中
    while (query.next()) {
        QString productName = query.value("name").toString();
        QString productDescription = query.value("description").toString();
        float productPrice = query.value("price").toFloat();

        QString productInfo = QString("Name: %1\nDescription: %2\nPrice: $%3")
                                  .arg(productName)
                                  .arg(productDescription)
                                  .arg(productPrice);
        ui->productListWidget->addItem(productInfo);
    }
}
// 当点击登录按钮时触发
void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();  // 获取用户名
    QString password = ui->passwordLineEdit->text();  // 获取密码
    QString role = ui->roleLineEdit_4->text();
    if(role == "customer"){
        currentUser.reset(new Customer(0, username, password, ""));
    }
    else if(role == "merchant"){
        currentUser.reset(new Merchant(0, username, password, ""));
    } else {
        QMessageBox::warning(this, "Login Failed", "Role must be 'customer' or 'merchant'.");
        return;
    }
    if (currentUser && currentUser->login(db, password)) {
        QMessageBox::information(this, "Login Successful", "Welcome, " + username);
        if(role == "customer"){
            ui->stackedWidget->setCurrentIndex(1);
            loadProducts();
        }
        else if(role == "merchant"){
            ui->stackedWidget->setCurrentIndex(2);
        }
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}
void MainWindow::on_registerButton_clicked()
{
    QString username = ui->usernameLineEdit_2->text();
    QString password = ui->passwordLineEdit_2->text();
    QString email = ui->emailLineEdit_3->text();
    QString role = ui->roleLineEdit_3->text();
    if(role == "customer"){
        currentUser.reset(new Customer(0, username, password, email));
    }
    else if(role == "merchant"){
        currentUser.reset(new Merchant(0, username, password, email));
    } else {
        QMessageBox::warning(this, "Registration Failed", "Role must be 'customer' or 'merchant'.");
        return;
    }


    // 调用 User::registerUser() 进行注册
    if (currentUser) currentUser->registerUser(db);

    QMessageBox::information(this, "Registration", "User registered successfully!");
}
void MainWindow::on_publishButton_clicked()
{
    QString name = ui->nameLineEdit->text();
    QString description = ui->descriptionLineEdit->text();
    bool ok;
    float price = ui->priceLineEdit->text().toFloat(&ok);
    QString image = ui->imageLineEdit->text();

    Product product(0, name, description, price, image);
    if (!currentUser) {
        QMessageBox::warning(this, "Error", "No user logged in.");
        return;
    }
    Merchant *merchant = dynamic_cast<Merchant *>(currentUser.get());
    if (!merchant) {
        QMessageBox::warning(this, "Error", "Only merchants can publish products.");
        return;
    }
    merchant->publishProduct(db, product);
}
