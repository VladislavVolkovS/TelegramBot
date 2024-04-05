#include "api.h"
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iostream>

class Bot {
public:
    Bot(std::unique_ptr<BotAPI>&& api) {
        api_ = std::move(api);
        GetUpdateID();
    }
    void Run() {
        while (true) {
            for (const auto& upd : api_->GetUpdates(5, update_id_)) {
                if (upd.message && upd.message.value().text) {
                    update_id_ = upd.update_id + 1;
                    WriteUpdateID();
                    if (auto text = upd.message.value().text.value(); text == "/random") {
                        Random(upd.message.value().chat.id);
                    } else if (text == "/weather") {
                        SayWeather(upd.message.value().chat.id, upd.message.value().message_id);
                    } else if (text == "/styleguide") {
                        SayJoke(upd.message.value().chat.id);
                    } else if (text == "/stop") {
                        return;
                    } else if (text == "/crash") {
                        std::abort();
                    }
                }
            }
        }
    }
    void Random(int64_t chat_id, int64_t reply_id = 0) {
        std::srand(time(nullptr));
        api_->SendMessage(chat_id, std::to_string(std::rand()), reply_id);
    }
    void SayWeather(int64_t chat_id, int64_t reply_id = 0) {
        api_->SendMessage(chat_id, "Winter Is Coming", reply_id);
    }
    void SayJoke(int64_t chat_id, int64_t reply_id = 0) {
        api_->SendMessage(
            chat_id,
            "10 lines of code = 10 issues\n500 lines of code = \"looks fine\"\nCode reviews.",
            reply_id);
    }

private:
    std::unique_ptr<BotAPI> api_;
    int64_t update_id_;
    void GetUpdateID() {
        std::ifstream in("offset.txt");
        if (!in.is_open()) {
            update_id_ = 0;
            return;
        }
        if (in >> update_id_) {
            in.close();
        } else {
            throw std::runtime_error{"Failed read offset"};
        }
    }
    void WriteUpdateID() {
        std::ofstream out("offset.txt");
        if (out << update_id_) {
            out.close();
        } else {
            throw std::runtime_error{"Failed read offset"};
        }
    }
};