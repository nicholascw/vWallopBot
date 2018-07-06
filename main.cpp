#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>
#include <csignal>
#include <sqlite3.h>
#include <tgbot/tgbot.h>
#include <ctime>

using namespace std;
using namespace TgBot;
bool sigintGot = false;
time_t birthtime(time(nullptr));

int main() {
    Bot bot("611832030:AAGXfzQMKVDFlpdUkWGkYXqrHy9YcTeWKwo");

    bot.getEvents().onCommand("listadm", [&bot](Message::Ptr message) {
        if (message->date < birthtime)return;
        if (message->chat->type == Chat::Type::Group
            || message->chat->type == Chat::Type::Supergroup) {
            vector<ChatMember::Ptr> admList(bot.getApi().getChatAdministrators(message->chat->id));
            string txtList;
            for (auto const i : admList)
                txtList +=
                        "[" + i->user->firstName + " " + i->user->lastName + "](tg://user?id=" +
                        to_string(i->user->id) + ")\n";
            bot.getApi().sendMessage(message->chat->id, txtList, false, message->messageId,
                                     std::make_shared<GenericReply>(), "markdown", false);
        } else {
            bot.getApi().sendMessage(message->chat->id, "This isn't a *group chat*!", false, message->messageId,
                                     std::make_shared<GenericReply>(), "markdown", false);
        }
    });
    bot.getEvents().onCommand("wallop", [&bot](Message::Ptr message) {
        if (message->date < birthtime)return;
        if (message->chat->type == Chat::Type::Group
            || message->chat->type == Chat::Type::Supergroup) {
            vector<ChatMember::Ptr> admList(bot.getApi().getChatAdministrators(message->chat->id));
            string txtList;
            for (auto const i : admList)
                txtList += i->user->username != "" ? "@" + i->user->username + "\n" :
                           "[" + i->user->firstName + " " + i->user->lastName + "](tg://user?id=" +
                           to_string(i->user->id) + ")\n";
            string strFrom = (message->from->username != "" ? "@" + message->from->username :
                              "[" + message->from->firstName + " " + message->from->lastName +
                              "](tg://user?id=" +
                              to_string(message->from->id) + ")");
            string strSuspect = (message->replyToMessage != nullptr ? (
                    (message->replyToMessage->from->username != "" ? "@" +
                                                                     message->replyToMessage->from->username
                                                                   : "[" +
                                                                     message->replyToMessage->from->firstName +
                                                                     " " +
                                                                     message->replyToMessage->from->lastName +
                                                                     "](tg://user?id=" + to_string(
                                    message->replyToMessage->from->id) + ")") +
                    " sent restricted content") : "some recent messages are restricted content");
            string strComments =
                    !StringTools::endsWith(message->text, "/wallop") &&
                    !StringTools::endsWith(message->text, "/wallop@vWallopBot") ?
                    ", with comment: *" + message->text.substr(message->text[7] == '@' ? 19 : 8) + "*" : "";

            bot.getApi().sendMessage(message->chat->id,
                                     "User " + strFrom + " reported " + strSuspect + strComments +
                                     "!\nCalling *administrators*:\n" + txtList, false,
                                     (message->replyToMessage != nullptr ? message->replyToMessage->messageId
                                                                         : message->messageId),
                                     std::make_shared<GenericReply>(), "markdown", false);
            cout << message->text << endl;
        } else {
            bot.getApi().sendMessage(message->chat->id, "This isn't a *group chat*!", false, message->messageId,
                                     std::make_shared<GenericReply>(), "markdown", false);
        }
    });
    signal(SIGINT, [](int s) {
        if (!sigintGot) {
            printf("SIGINT received, exiting...\n");
            sigintGot = true;
        }
    });

    try {
        cout << "Bot: " << bot.getApi().getMe()->username.c_str() << endl
             << "Author: Nicholas Wang <me@nicho1as.wang>" << endl
             << "Build: " << __DATE__ << " " << __TIME__ << endl
             << "----------" << endl
             << "\033[1m[" << put_time(localtime(&birthtime), "%h %d %H:%M:%S") << "]\033[0m\t" << "Birth." << endl;

        TgLongPoll longPoll(bot);
        while (!sigintGot) {
            auto tm = time(nullptr);
            cout << "\033[1m[" << put_time(localtime(&tm), "%h %d %H:%M:%S") << "]\033[0m\t" << "longPoll started.\n";
            longPoll.start();
            tm = time(nullptr);
            cout << "\033[1m[" << put_time(localtime(&tm), "%h %d %H:%M:%S") << "]\033[0m\t" << "longPoll ended.\n";
            cin.ignore(0);
        }
    } catch (exception &e) {
        cout << "\033[1;31mErr: \033[0m" << e.what();
    }

    return 0;
}