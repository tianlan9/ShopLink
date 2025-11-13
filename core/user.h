#ifndef USER_H
#define USER_H

#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

class User {
protected:
    int userId;
    QString username;
    QString password;  // 存储哈希后的密码
    QString email;
    QString role; // 'customer' or 'merchant'

public:
    User(int id, QString uname, QString pass, QString mail, QString r)
        : userId(id), username(uname), password(pass), email(mail), role(r) {}

    // Getter and Setter Methods
    int getUserId() const { return userId; }
    QString getUsername() const { return username; }
    QString getEmail() const { return email; }
    QString getRole() const { return role; }

    void setUsername(const QString &uname) { username = uname; }
    void setPassword(const QString &pass) { password = pass; }
    void setEmail(const QString &mail) { email = mail; }
    void setRole(const QString &r) { role = r; }

    // 用户注册
    virtual void registerUser(QSqlDatabase &db) = 0;

    // 用户登录
    virtual bool login(QSqlDatabase &db, const QString &inputPassword) = 0;

    // 用户登出
    virtual void logout() = 0;

    virtual ~User() {}
};

#endif // USER_H
