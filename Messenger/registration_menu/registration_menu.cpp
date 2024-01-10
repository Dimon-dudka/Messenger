
#include "registration_menu.h"

registration_menu::registration_menu(QWidget *parrent):QWidget(parrent)
{
    widget_layout = new QGridLayout;

    back_button = new QPushButton("Back");
    register_button = new QPushButton("Registration");

    login_line_edit = new QLineEdit;
    password_line_edit = new QLineEdit;
    repeat_password_line_edit = new QLineEdit;

    password_line_edit->setEchoMode(QLineEdit::Password);
    repeat_password_line_edit->setEchoMode(QLineEdit::Password);

    info_label = new QLabel("Login must contain english letters."
                            "\nPassword must contatinsat least 6 charachters");
    enter_login_label = new QLabel("Enter login");
    enter_password_label = new QLabel("Enter password");

    widget_layout->addWidget(info_label,0,1);
    widget_layout->addWidget(enter_login_label,1,0);
    widget_layout->addWidget(login_line_edit,1,1);
    widget_layout->addWidget(enter_password_label,2,0);
    widget_layout->addWidget(password_line_edit,2,1);
    widget_layout->addWidget(repeat_password_line_edit,3,1);
    widget_layout->addWidget(register_button,4,1);
    widget_layout->addWidget(back_button,4,2);

    connect(back_button,SIGNAL(clicked()),this,SLOT(back_button_slot()));
    connect(register_button,SIGNAL(clicked()),this,SLOT(register_button_slot()));

    setLayout(widget_layout);

}

void registration_menu::back_button_slot(){
    emit back_button_signal();
}

void registration_menu::register_button_slot(){

    // login
    if(login_line_edit->text().size()<6){
        emit register_frontend_problem_signal("Login is less then 6");
        return;
    }

    //password
    if(password_line_edit->text().size()<6){
        emit register_frontend_problem_signal("Password is less then 6!");
        return;
    }
    if(password_line_edit->text()!=repeat_password_line_edit->text()){
        emit register_frontend_problem_signal("Passwords are not equal!");
        return;
    }
    emit register_button_signal(login_line_edit->text(),password_line_edit->text(),"register");

    login_line_edit->clear();
    password_line_edit->clear();
    repeat_password_line_edit->clear();
}
