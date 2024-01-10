//  This widget describes the login menu

#pragma once

#include <QFont>
#include <QLabel>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

class login_menu : public QWidget
{
    Q_OBJECT
private:

    QPointer <QVBoxLayout> login_menu_layout;
    QPointer <QPushButton> login_button;
    QPointer <QPushButton> register_button;
    QPointer <QLineEdit> login_line_edit;
    QPointer <QLineEdit> password_line_edit;
    QPointer <QLabel> messenger_label;

private slots:
    void login_button_slot();
    void registration_button_slot();

public:
    login_menu(QWidget * parrent = 0);

signals:

    void login_button_signal(QString ,QString , QString);
    void registration_button_signal();

    void login_frontend_problem_signal(QString);

};
