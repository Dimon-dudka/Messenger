//  Widget describes chating menu

#pragma once

#include <QFont>
#include <QLabel>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QTextBrowser>

class messenger_main_menu : public QWidget
{
    Q_OBJECT
private:

    QString last_data_message;

    QPointer <QLabel> talker_login_label;
    QPointer <QPushButton> send_button,another_user_button;
    QPointer <QLineEdit> message_line_edit;
    QPointer <QTextBrowser> messages_text_browser;
    QPointer <QGridLayout> widget_layout;

private slots:

    void another_user_button_slot();
    void send_button_slot();

public:
    messenger_main_menu(QWidget *parrent = 0);

public slots:

    void change_another_user_label_slot(const QString &login);

signals:

    void find_users_signal();
    void send_message_signal(QString,QString);

};
