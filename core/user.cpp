#include "user.h"
#include <QDebug>
#include <QCryptographicHash>  // 用于密码哈希
#include <QRandomGenerator>

// Helper: generate a per-user random salt (hex)
QString User::generateSalt(int length) {
    QByteArray bytes;
    bytes.resize(length);
    for (int i = 0; i < length; ++i) {
        bytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(0, 256));
    }
    return bytes.toHex();
}

// Helper: derive password hash using iterative SHA-256 (PBKDF2-like)
QString User::hashPassword(const QString &password, const QString &salt, int iterations) {
    QByteArray saltBytes = QByteArray::fromHex(salt.toUtf8());
    QByteArray result = saltBytes + password.toUtf8();
    result = QCryptographicHash::hash(result, QCryptographicHash::Sha256);
    for (int i = 1; i < iterations; ++i) {
        result = QCryptographicHash::hash(result, QCryptographicHash::Sha256);
    }
    return result.toHex();
}

// 用户注册函数
void User::registerUser(QSqlDatabase &db) {

    qDebug() << "username=" << username;
    qDebug() << "email=" << email;
    qDebug() << "role=" << role;
    // Generate per-user salt and derive password hash using iterative SHA-256
    QString newSalt = User::generateSalt();
    QString derivedHash = User::hashPassword(password, newSalt, User::DEFAULT_PBKDF2_ITERATIONS);
    // Store salt along with the derived hash
    QSqlQuery query(db);
    query.prepare("INSERT INTO Users (username, password, salt, email, role) "
                  "VALUES (:username, :password, :salt, :email, :role)");
    query.bindValue(":username", username);
    query.bindValue(":password", derivedHash);
    query.bindValue(":salt", newSalt);
    query.bindValue(":email", email);
    query.bindValue(":role", role);
    // keep salt in instance
    salt = newSalt;

    if (!query.exec()) {
        qDebug() << "Error registering user:" << query.lastError().text();
    } else {
        qDebug() << "User registered successfully!";
    }
}

// 用户登录
bool User::login(QSqlDatabase &db, const QString &inputPassword) {
    QSqlQuery query(db);
    query.prepare("SELECT password, salt FROM Users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Error during login check:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString storedPassword = query.value(0).toString();
        QString storedSalt = query.value(1).toString();

        if (storedSalt.isEmpty()) {
            // Legacy record: unsalted SHA-256(password). Verify and migrate to salted hash on successful login.
            QString legacy = QCryptographicHash::hash(inputPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
            if (storedPassword == legacy) {
                // Migrate: generate a salt and store a derived hash
                QString newSalt = User::generateSalt();
                QString newDerived = User::hashPassword(inputPassword, newSalt, User::DEFAULT_PBKDF2_ITERATIONS);
                QSqlQuery updateQuery(db);
                updateQuery.prepare("UPDATE Users SET password = :password, salt = :salt WHERE username = :username");
                updateQuery.bindValue(":password", newDerived);
                updateQuery.bindValue(":salt", newSalt);
                updateQuery.bindValue(":username", username);
                if (!updateQuery.exec()) {
                    qDebug() << "Error migrating user password to salted hash:" << updateQuery.lastError().text();
                }
                return true;
            }
            return false;
        } else {
            QString derived = User::hashPassword(inputPassword, storedSalt, User::DEFAULT_PBKDF2_ITERATIONS);
            return (storedPassword == derived);
        }
    }
    return false;
}

// 用户登出
void User::logout() {
    qDebug() << username << " logged out successfully!";
}
