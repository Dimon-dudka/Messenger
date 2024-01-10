#include "messenger_main_menu.h"

messenger_main_menu::messenger_main_menu(QWidget *parrent):QWidget(parrent)
{
    talker_login_label = new QLabel("");

    send_button = new QPushButton("Send");
    another_user_button = new QPushButton("Find another user");

    message_line_edit = new QLineEdit;

    messages_text_browser = new QTextBrowser;

    widget_layout = new QGridLayout;

    widget_layout->addWidget(talker_login_label,0,0);
    widget_layout->addWidget(another_user_button,0,2);

    widget_layout->addWidget(messages_text_browser,1,0);

    widget_layout->addWidget(message_line_edit,3,0);
    widget_layout->addWidget(send_button,3,2);

    setLayout(widget_layout);

    connect(another_user_button,SIGNAL(clicked()),this,SLOT(another_user_button_slot()));

}

void messenger_main_menu::another_user_button_slot(){
    emit find_users_signal();
}

void messenger_main_menu::change_another_user_label_slot(const QString &login){
    talker_login_label->setText(std::move(login));
}
