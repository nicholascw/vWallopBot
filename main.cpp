#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <exception>
#include <csignal>
#include <sqlite3.h>
#include <tgbot/tgbot.h>
#include <ctime>
#include <thread>

#include "bot_token.h"

using namespace std;
using namespace TgBot;
bool sigintGot = false;
time_t birthtime(time(nullptr));

void lsadm(Bot &bot, const Message::Ptr &message) {
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
}

void wallop(Bot &bot, const Message::Ptr &message) {
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
                                                                     : message->messageId), make_shared<GenericReply>(),
                                 "markdown", false);
        cout << message->text << endl;
    } else {
        bot.getApi().sendMessage(message->chat->id, "This isn't a *group chat*!", false, message->messageId,
                                 std::make_shared<GenericReply>(), "markdown", false);
    }
}

void mute(Bot &bot, const Message::Ptr &message) {
    if (message->chat->type == Chat::Type::Private) {

    } else {
        if (!StringTools::startsWith(message->text, "/start@" + string(bot.getApi().getMe()->username.c_str())))return;
        bot.getApi().sendMessage(message->chat->id, "Please use /start command in the private chat!", false,
                                 message->messageId,
                                 std::make_shared<GenericReply>(), "markdown", false);
    }

}

void unmute(Bot &bot, const Message::Ptr &message) {
    if (message->chat->type == Chat::Type::Private) {

    } else {
        if (!StringTools::startsWith(message->text, "/start@" + string(bot.getApi().getMe()->username.c_str())))return;
        bot.getApi().sendMessage(message->chat->id, "Please use /start command in the private chat!", false,
                                 message->messageId,
                                 std::make_shared<GenericReply>(), "markdown", false);
    }
}

void start(Bot &bot, const Message::Ptr &message) {
    if (message->chat->type == Chat::Type::Private) {
        bot.getApi().sendMessage(message->chat->id,
                                 "Hi~! This private chat section is mainly for the administrators to change settings. "
                                 "If you are interested in this bot, please visit this "
                                 "[GitHub Repo](https://github.com/nicholascw/vWallopBot).", false, 0,
                                 make_shared<GenericReply>(), "markdown", false);
    } else {
        if (!StringTools::startsWith(message->text, "/start@" + string(bot.getApi().getMe()->username.c_str())))return;
        bot.getApi().sendMessage(message->chat->id, "Please use /start command in the private chat!", false,
                                 message->messageId,
                                 std::make_shared<GenericReply>(), "markdown", false);
    }
}

int main(int argc, char *argv[]) {
    cout << "\033[1mvWallopBot\033[0m" << endl
         << "Author: Nicholas Wang <me@nicho1as.wang>" << endl
         << "Copyright (C) 2018  Licensed with GPLv3, for details, see: https://www.gnu.org/licenses/gpl-3.0.en.html"
         << endl << endl;

    string bot_token = BOT_TOKEN_;
    if (argc > 1) {
        bot_token = argv[1];
        regex token_verify("\\d{9}:[0-9A-Za-z_-]{35}");
        cmatch m;
        int i = 0;
        do {
            if (regex_match(argv[1], m, token_verify)) {
                bot_token = string(argv[1]);
                break;
            } else {
                cout << "\033[1;31mErr: Invalid Bot token!\033[0m" << endl << "Please re-enter: ";
                cin >> argv[1];
            }
        } while (i++ < 3);
    }

    Bot bot(bot_token);
    bot.getEvents().onCommand("lsadm", [&bot](const Message::Ptr &message) {
        if (message->date < birthtime)return;
        lsadm(bot, message);
    });
    bot.getEvents().onCommand("wallop", [&bot](const Message::Ptr &message) {
        if (message->date < birthtime)return;
        wallop(bot, message);
    });

    bot.getEvents().onCommand("mute", [&bot](const Message::Ptr &message) {
        if (message->date < birthtime)return;
        mute(bot, message);
    });
    bot.getEvents().onCommand("unmute", [&bot](const Message::Ptr &message) {
        if (message->date < birthtime)return;
        unmute(bot, message);
    });
    bot.getEvents().onCommand("start", [&bot](const Message::Ptr &message) {
        if (message->date < birthtime)return;
        start(bot, message);
    });
    signal(SIGINT, [](int s) {
        if (!sigintGot) {
            printf("SIGINT received, exiting...\n");
            sigintGot = true;
        }
    });

    try {
        cout << "Bot: " + string(bot.getApi().getMe()->username.c_str()) << endl
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
        cout << "\033[1;31mErr\033[0m " << e.what();
    }
    return 0;
}
