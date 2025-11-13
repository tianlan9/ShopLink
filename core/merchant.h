#ifndef MERCHANT_H
#define MERCHANT_H

#include "user.h"
#include "product.h"
#include <QList>

class Merchant : public User {
public:
    Merchant(int id, QString uname, QString pass, QString mail)
        : User(id, uname, pass, mail, "merchant") {}

    // 发布产品
    void publishProduct(QSqlDatabase &db, const Product &product);

    // 移除产品
    void removeProduct(QSqlDatabase &db, int productId);

    // 查看销售数据
    void viewSalesData(QSqlDatabase &db);

    // 注册用户
    void registerUser(QSqlDatabase &db) override;

    // 登录用户
    bool login(QSqlDatabase &db, const QString &inputPassword) override;

    // 登出
    void logout() override;
};

#endif // MERCHANT_H
