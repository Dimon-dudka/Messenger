//  Widget describes frontend of chating menu

#pragma once

#include <QFont>
#include <QLabel>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBrowser>

class messenger_main_menu : public QWidget
{
    Q_OBJECT
private:

    QString last_data_message;

    QPointer <QLabel> talker_login_label;
    QPointer <QPushButton> send_button,another_user_button,settings_button;
    QPointer <QLineEdit> message_line_edit;
    QPointer <QTextBrowser> messages_text_browser;
    QPointer <QGridLayout> widget_layout;

private slots:

    void another_user_button_slot()noexcept;
    void send_button_slot();
    void scroll_value_changed(int value);

public:
    messenger_main_menu(QWidget *parrent = 0);

public slots:

    void change_another_user_label_slot(const QString &login);
    void push_new_message_slot(const QString &login_from,const QString &message);
    void print_message_story_slot(const std::vector<std::tuple<QString,QString,QString>>& data);

signals:

    void find_users_signal();
    void send_message_signal(QString,QString);
    void become_message_history(QString,QString);
};
