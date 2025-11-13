#include "customer.h"
#include <QDebug>

// 浏览产品
void Customer::browseProducts(QSqlDatabase &db) {
    QSqlQuery query(db);
    query.exec("SELECT * FROM Products");

    while (query.next()) {
        int productId = query.value(0).toInt();
        QString productName = query.value(1).toString();
        float productPrice = query.value(3).toFloat();
        qDebug() << "Product ID:" << productId
                 << ", Product Name:" << productName
                 << ", Price:" << productPrice;
    }
}

// 购买产品
void Customer::purchaseProduct(QSqlDatabase &db, int productId) {
    QSqlQuery query(db);
    query.prepare("SELECT name FROM Products WHERE productId = :productId");
    query.bindValue(":productId", productId);

    if (!query.exec()) {
        qDebug() << "Error purchasing product:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        QString productName = query.value(0).toString();
        qDebug() << username << " purchased product:" << productName;
    }
}

// 注册用户
void Customer::registerUser(QSqlDatabase &db) {
    User::registerUser(db);  // 调用基类的注册函数
}

// 登录用户
bool Customer::login(QSqlDatabase &db, const QString &inputPassword) {
    return User::login(db, inputPassword);  // 调用基类的登录函数
}

// 登出
void Customer::logout() {
    User::logout();  // 调用基类的登出函数
}
