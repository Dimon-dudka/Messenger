#include "fails_menu.h"

fails_menu::fails_menu(QWidget *parrent):QWidget(parrent)
{
    widget_layout = new QVBoxLayout;
    fail_label = new QLabel("NETWORK FAIL!");

    try_again_button = new QPushButton("Try again");

    QFont fail_label_font=fail_label->font();
    fail_label_font.setBold(true);
    fail_label_font.setPointSize(15);

    fail_label->setFont(fail_label_font);
    fail_label->setAlignment(Qt::AlignCenter);

    widget_layout->addWidget(fail_label);
    widget_layout->addWidget(try_again_button);

    this->setLayout(widget_layout);

    connect(try_again_button,SIGNAL(clicked()),this,SLOT(try_again_button_slot()));
}

void fails_menu::try_again_button_slot()noexcept{
    emit try_again_signal();
}

void fails_menu::open_fails_menu_slot(const QString &problem_type){
    fail_label->setText(problem_type);
}
