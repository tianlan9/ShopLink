#include "merchant.h"
#include <QDebug>

// 发布产品
void Merchant::publishProduct(QSqlDatabase &db, const Product &product) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO Products (name, description, price, image) "
                  "VALUES (:name, :description, :price, :image)");
    query.bindValue(":name", product.getName());
    query.bindValue(":description", product.getDescription());
    query.bindValue(":price", product.getPrice());
    query.bindValue(":image", product.getImage());

    if (!query.exec()) {
        qDebug() << "Error publishing product:" << query.lastError().text();
    } else {
        qDebug() << "Product published successfully!";
    }
}

// 移除产品
void Merchant::removeProduct(QSqlDatabase &db, int productId) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM Products WHERE productId = :productId");
    query.bindValue(":productId", productId);

    if (!query.exec()) {
        qDebug() << "Error removing product:" << query.lastError().text();
    } else {
        qDebug() << "Product removed successfully!";
    }
}

// 查看销售数据
void Merchant::viewSalesData(QSqlDatabase &db) {
    // 这是一个简单示例，实际可能需要更复杂的查询
    QSqlQuery query(db);
    query.exec("SELECT * FROM Sales");

    while (query.next()) {
        int saleId = query.value(0).toInt();
        QString productName = query.value(1).toString();
        int quantity = query.value(2).toInt();
        qDebug() << "Sale ID:" << saleId
                 << ", Product Name:" << productName
                 << ", Quantity Sold:" << quantity;
    }
}

// 注册用户
void Merchant::registerUser(QSqlDatabase &db) {
    User::registerUser(db);  // 调用基类的注册函数
}

// 登录用户
bool Merchant::login(QSqlDatabase &db, const QString &inputPassword) {
    return User::login(db, inputPassword);  // 调用基类的登录函数
}

// 登出
void Merchant::logout() {
    User::logout();  // 调用基类的登出函数
}
