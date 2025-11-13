#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "user.h"
#include "product.h"
#include <QList>

class Customer : public User {
public:
    Customer(int id, QString uname, QString pass, QString mail)
        : User(id, uname, pass, mail, "customer") {}

    // 浏览产品
    void browseProducts(QSqlDatabase &db);

    // 购买产品
    void purchaseProduct(QSqlDatabase &db, int productId);

    // 注册用户
    void registerUser(QSqlDatabase &db) override;

    // 登录用户
    bool login(QSqlDatabase &db, const QString &inputPassword) override;

    // 登出
    void logout() override;
};

#endif // CUSTOMER_H
