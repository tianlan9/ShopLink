#include "user.h"
#include <QDebug>
#include <QCryptographicHash>  // 用于密码哈希

// 用户注册函数
void User::registerUser(QSqlDatabase &db) {

    qDebug() << "username=" << username;
    qDebug() << "password=" << password;
    qDebug() << "email=" << email;
    qDebug() << "role=" << role;
    QSqlQuery query(db);
    query.prepare("INSERT INTO Users (username, password, email, role) "
                  "VALUES (:username, :password, :email, :role)");
    query.bindValue(":username", username);
    query.bindValue(":password", QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());  // 存储哈希后的密码
    query.bindValue(":email", email);
    query.bindValue(":role", role);

    if (!query.exec()) {
        qDebug() << "Error registering user:" << query.lastError().text();
    } else {
        qDebug() << "User registered successfully!";
    }
}

// 用户登录
bool User::login(QSqlDatabase &db, const QString &inputPassword) {
    QSqlQuery query(db);
    query.prepare("SELECT password FROM Users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Error during login check:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString storedPassword = query.value(0).toString();
        // 验证哈希后的密码
        return (storedPassword == QCryptographicHash::hash(inputPassword.toUtf8(), QCryptographicHash::Sha256).toHex());
    }
    return false;
}

// 用户登出
void User::logout() {
    qDebug() << username << " logged out successfully!";
}
