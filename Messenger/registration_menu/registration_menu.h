#pragma once

#include <QFont>
#include <QLabel>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>


class registration_menu : public QWidget
{
    Q_OBJECT
private:

    QPointer <QGridLayout> widget_layout;
    QPointer <QPushButton> register_button,back_button;
    QPointer <QLineEdit> login_line_edit,password_line_edit,repeat_password_line_edit;
    QPointer <QLabel> info_label,enter_login_label,enter_password_label;

private slots:

    void back_button_slot();
    void register_button_slot();

public:
    registration_menu(QWidget *parrent = 0);

signals:

    void back_button_signal();
    void register_button_signal(QString,QString,QString);


    void register_frontend_problem_signal(QString);

};
