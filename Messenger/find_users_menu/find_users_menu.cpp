#include "find_users_menu.h"

find_users_menu::find_users_menu(QWidget * parrent):QWidget(parrent)
{
    info_label = new QLabel("Enter user login");
    info_label->setAlignment(Qt::AlignCenter);

    enter_login_line_edit = new QLineEdit;

    users_tree_widget = new QTreeWidget;
    users_tree_widget->setHeaderLabel("Users");
    users_tree_widget->resize(200,100);

    choise_tree_item = new QTreeWidgetItem(users_tree_widget);

    choise_button = new QPushButton("Choise user");
    back_button = new QPushButton("Back");
    find_button = new QPushButton("Find");

    upper_layout = new QHBoxLayout;
    upper_layout->addWidget(enter_login_line_edit);
    upper_layout->addWidget(find_button);

    bottom_layout = new QHBoxLayout;
    bottom_layout->addWidget(back_button);
    bottom_layout->addWidget(choise_button);

    widget_layout = new QVBoxLayout;
    widget_layout->addLayout(upper_layout);
    widget_layout->addWidget(users_tree_widget);
    widget_layout->addLayout(bottom_layout);

    setLayout(widget_layout);

    connect(back_button,SIGNAL(clicked()),this,SLOT(back_button_slot()));
    connect(choise_button,SIGNAL(clicked()),this,SLOT(choise_button_slot()));
    connect(find_button,SIGNAL(clicked()),this,SLOT(find_users_slot()));

    connect(enter_login_line_edit,SIGNAL(textChanged(QString)),this,SLOT(text_line_changed_slot()));

    connect(users_tree_widget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(user_choise_slot(QTreeWidgetItem*,int)));
}

void find_users_menu::back_button_slot()noexcept{
    emit back_button_signal();
}

void find_users_menu::text_line_changed_slot(){

    another_user_login.clear();

    if(enter_login_line_edit->text().size()==0){
        users_tree_widget->clear();
        emit print_basic_logins_signal();
        return;
    }

    if(enter_login_line_edit->text().size()%2==0){
        emit find_user_signal(enter_login_line_edit->text());
    }
}

void find_users_menu::find_users_slot(){
    if(enter_login_line_edit->text().size()==0){
        emit empty_line_edit_signal("Incorrect login format!");
        return;
    }
    emit find_user_signal(enter_login_line_edit->text());
    enter_login_line_edit->clear();
}

void find_users_menu::update_user_list_tree(std::vector<QString>login_list){
    users_tree_widget->clear();
    for(const QString &w: login_list){
        choise_tree_item = new QTreeWidgetItem(users_tree_widget);
        choise_tree_item->setText(0,w);
    }
}

void find_users_menu::user_choise_slot(QTreeWidgetItem* user,int pos){
    another_user_login=user->text(pos);
}

void find_users_menu::choise_button_slot(){
    if(another_user_login.size()!=0){
        emit choise_is_done_signal(std::move(another_user_login));
    }
}
