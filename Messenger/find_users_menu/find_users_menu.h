//  Widget describes frontend of menu where user can find
//  Another users to chatting

#pragma once

#include <QLabel>
#include <vector>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QTreeWidgetItem>

class find_users_menu : public QWidget
{
    Q_OBJECT

private:

    QPointer <QLabel> info_label;
    QPointer <QLineEdit> enter_login_line_edit;
    QPointer <QPushButton> back_button,choise_button,find_button;
    QPointer <QTreeWidget> users_tree_widget;
    QPointer <QVBoxLayout> widget_layout;
    QPointer <QHBoxLayout> bottom_layout, upper_layout;

    QTreeWidgetItem *choise_tree_item;

    QString another_user_login;

private slots:

    void back_button_slot()noexcept;
    void find_users_slot();
    void text_line_changed_slot();
    void user_choise_slot(QTreeWidgetItem* user,int pos);
    void choise_button_slot();

public:
    find_users_menu(QWidget *parrent = 0);

public slots:
    void update_user_list_tree(std::vector<QString>login_list);

signals:

    void back_button_signal();
    void choise_button_signal();

    void find_user_signal(QString);

    void choise_is_done_signal(QString);

    //  Frontend fails signals
    void empty_line_edit_signal(QString);

    void print_basic_logins_signal();
};
