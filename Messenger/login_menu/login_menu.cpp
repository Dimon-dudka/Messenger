
#include "login_menu.h"

login_menu::login_menu(QWidget * parrent) : QWidget(parrent)
{

    login_menu_layout = new QVBoxLayout;
    messenger_label = new QLabel("Capybara Messenger");
    messenger_label->setAlignment(Qt::AlignCenter);

    QFont messenger_label_font=messenger_label->font();
    messenger_label_font.setBold(true);
    messenger_label_font.setPointSize(15);

    messenger_label->setFont(messenger_label_font);

    login_line_edit = new QLineEdit;
    password_line_edit = new QLineEdit;
    login_button = new QPushButton("LogIn");
    register_button = new QPushButton("Registration");

    password_line_edit->setEchoMode(QLineEdit::Password);

    login_menu_layout->addWidget(messenger_label);
    login_menu_layout->addWidget(login_line_edit);
    login_menu_layout->addWidget(password_line_edit);
    login_menu_layout->addWidget(login_button);
    login_menu_layout->addWidget(register_button);

    connect(login_button,SIGNAL(clicked()),this,SLOT(login_button_slot()));
    connect(register_button,SIGNAL(clicked()),this,SLOT(registration_button_slot()));

    setLayout(login_menu_layout);
}

void login_menu::login_button_slot(){

    if(login_line_edit->text().size()<6){
        emit login_frontend_problem_signal("Incorrect login!");
        return;
    }

    if(password_line_edit->text().size()<6){
        emit login_frontend_problem_signal("Password is less then 6!");
        return;
    }

    emit login_button_signal(login_line_edit->text(),password_line_edit->text(),"login");

    password_line_edit->clear();

}

void login_menu::registration_button_slot(){
    emit registration_button_signal();
}
