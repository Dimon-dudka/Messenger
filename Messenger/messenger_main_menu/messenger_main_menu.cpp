#include "messenger_main_menu.h"

messenger_main_menu::messenger_main_menu(QWidget *parrent):QWidget(parrent)
{
    last_data_message="9999-99-99  99:99:99";

    talker_login_label = new QLabel("");

    send_button = new QPushButton("Send");
    another_user_button = new QPushButton("Find another user");

    message_line_edit = new QLineEdit;

    messages_text_browser = new QTextBrowser;
    messages_text_browser->setOpenExternalLinks(true);
    messages_text_browser->setOpenLinks(true);
    messages_text_browser->setHtml("");
    messages_text_browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    widget_layout = new QGridLayout;

    widget_layout->addWidget(talker_login_label,0,0);
    widget_layout->addWidget(another_user_button,0,2);

    widget_layout->addWidget(messages_text_browser,1,0);

    widget_layout->addWidget(message_line_edit,3,0);
    widget_layout->addWidget(send_button,3,2);

    setLayout(widget_layout);

    connect(another_user_button,SIGNAL(clicked()),this,SLOT(another_user_button_slot()));
    connect(send_button,SIGNAL(clicked()),this,SLOT(send_button_slot()));

    connect(messages_text_browser->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(scroll_value_changed(int)));

}

void messenger_main_menu::scroll_value_changed(int value){
    if(value==messages_text_browser->verticalScrollBar()->minimum()){
        emit become_message_history(last_data_message,talker_login_label->text());
    }
}

void messenger_main_menu::another_user_button_slot(){
    emit find_users_signal();
}

void messenger_main_menu::change_another_user_label_slot(const QString &login){
    talker_login_label->setText(std::move(login));
    messages_text_browser->clear();
    last_data_message="9999-99-99  99:99:99";

    emit become_message_history(last_data_message,talker_login_label->text());
}

void messenger_main_menu::send_button_slot(){
    if(message_line_edit->text().size()==0||talker_login_label->text().size()==0){
        return;
    }

    bool flag_empty_message{true};

    for(const auto&w:message_line_edit->text()){
        if(w!=' '){
            flag_empty_message=false;
            break;
        }
    }

    if(flag_empty_message){
        return;
    }

    emit send_message_signal(talker_login_label->text(),message_line_edit->text());

    messages_text_browser->append("You: "+message_line_edit->text());

    message_line_edit->clear();
}

void messenger_main_menu::push_new_message_slot(const QString &login_from,const QString &message){

    if(talker_login_label->text()!=login_from){
        return;
    }

    messages_text_browser->append(login_from+": "+message);
}

void messenger_main_menu::print_message_story_slot(const std::vector<std::tuple<QString,QString,QString>>&data){
    int scroll_position_start = messages_text_browser->verticalScrollBar()->value();
    for(const auto&w:data){
        messages_text_browser->moveCursor(QTextCursor::Start);

        //  Message output
        if(get<0>(w)!=talker_login_label->text()){

            messages_text_browser->insertPlainText("You: " +get<2>(w)+"\n" );
        }else{
            messages_text_browser->insertPlainText(get<0>(w)+": "+get<2>(w)+"\n");
        }

        if(get<1>(w)<last_data_message){
            last_data_message=get<1>(w);
        }
        scroll_position_start+=1;
    }
    scroll_position_start*=11.2;
    messages_text_browser->verticalScrollBar()->setValue(messages_text_browser->verticalScrollBar()->minimum()
                                                         +scroll_position_start);
}
