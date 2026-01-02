#include "product.h"
#include <QDebug>
#include <cstring>


// 插入商品到数据库
bool parseProductDescriptionToQString(const char* input, QString &out, QString &errorMsg, int maxLen) {
    if (!input) {
        errorMsg = "Invalid input (null).";
        return false;
    }
    const char prefix[] = "DESC:";
    if (std::strncmp(input, prefix, 5) != 0) {
        errorMsg = "Invalid format.";
        return false;
    }
    const char* payload = input + 5;
    int payload_len = static_cast<int>(std::strlen(payload));
    if (payload_len == 0) {
        errorMsg = "Empty product description.";
        return false;
    }
    if (payload_len > maxLen) {
        errorMsg = QString("Product description too long (max %1). Truncating.").arg(maxLen);
        out = QString::fromUtf8(payload, maxLen);
        return true;
    }
    out = QString::fromUtf8(payload);
    return true;
}

void Product::insertProductToDB(QSqlDatabase &db) const {
    QString descToStore = description;
    QString err;
    if (description.startsWith("DESC:")) {
        bool ok = parseProductDescriptionToQString(description.toUtf8().constData(), descToStore, err);
        if (!ok) {
            qDebug() << "Product description invalid:" << err;
            // Decide: reject insertion or store empty description. Here we reject insertion to prevent bad data.
            return;
        }
        if (!err.isEmpty()) {
            qDebug() << err;
        }
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO Products (name, description, price, image) "
                  "VALUES (:name, :description, :price, :image)");
    query.bindValue(":name", name);
    query.bindValue(":description", descToStore);
    query.bindValue(":price", price);
    query.bindValue(":image", image);

    if (!query.exec()) {
        qDebug() << "Error inserting product:" << query.lastError().text();
    } else {
        qDebug() << "Product inserted successfully!";
    }
}

// 从数据库中获取商品信息
Product Product::getProductFromDB(QSqlDatabase &db, int productId) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Products WHERE productId = :productId");
    query.bindValue(":productId", productId);

    if (!query.exec()) {
        qDebug() << "Error fetching product:" << query.lastError().text();
        return Product(-1, "", "", 0.0, "");  // 返回一个空的 Product 对象表示未找到
    }

    if (query.next()) {
        QString name = query.value("name").toString();
        QString description = query.value("description").toString();
        float price = query.value("price").toFloat();
        QString image = query.value("image").toString();

        return Product(productId, name, description, price, image);
    }

    return Product(-1, "", "", 0.0, "");  // 返回一个空的 Product 对象
}

// 从数据库中删除商品
void Product::deleteProductFromDB(QSqlDatabase &db, int productId) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM Products WHERE productId = :productId");
    query.bindValue(":productId", productId);

    if (!query.exec()) {
        qDebug() << "Error deleting product:" << query.lastError().text();
    } else {
        qDebug() << "Product deleted successfully!";
    }
}

// 显示商品信息
void Product::displayProduct() const {
    qDebug() << "Product ID:" << productId
             << ", Name:" << name
             << ", Description:" << description
             << ", Price:" << price
             << ", Image:" << image;
}
