#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

class Product {
private:
    int productId;          // 商品ID
    QString name;           // 商品名称
    QString description;    // 商品描述
    float price;            // 商品价格
    QString image;          // 商品图片路径

public:
    // 构造函数
    Product(int id, QString productName, QString productDescription, float productPrice, QString productImage)
        : productId(id), name(productName), description(productDescription), price(productPrice), image(productImage) {}

    // Getter 和 Setter 方法
    int getProductId() const { return productId; }
    void setProductId(int id) { productId = id; }

    QString getName() const { return name; }
    void setName(const QString &productName) { name = productName; }

    QString getDescription() const { return description; }
    void setDescription(const QString &productDescription) { description = productDescription; }

    float getPrice() const { return price; }
    void setPrice(float productPrice) { price = productPrice; }

    QString getImage() const { return image; }
    void setImage(const QString &productImage) { image = productImage; }

    // 商品的数据库操作
    void insertProductToDB(QSqlDatabase &db) const;
    static Product getProductFromDB(QSqlDatabase &db, int productId);
    static void deleteProductFromDB(QSqlDatabase &db, int productId);

    // 显示商品信息
    void displayProduct() const;
};

// Safer parser variant: parses and outputs the payload (without prefix).
// Returns true on success; in case of truncation sets error message but still returns true.
bool parseProductDescriptionToQString(const char* input, QString &out, QString &errorMsg, int maxLen = 1024);

#endif // PRODUCT_H
