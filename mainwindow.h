#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include "core/user.h"
#include "core/customer.h"
#include "core/merchant.h"
#include "core/product.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();  // 登录按钮点击时触发的槽函数
    void on_registerButton_clicked();
    void on_publishButton_clicked();

private:
    Ui::MainWindow *ui;  // GUI 组件
    QSqlDatabase db;     // 数据库连接
    User *currentUser;    // 当前登录的用户
    void loadProducts();
};

#endif // MAINWINDOW_H
