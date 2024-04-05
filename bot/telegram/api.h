#include <memory>
#include <string>
#include <optional>
#include <vector>
#include "api_exceptions.h"

struct User {
    bool is_bot;
    std::string first_name;
    int64_t id;
    User(bool is_bot, std::string first_name, int64_t id)
        : is_bot(is_bot), first_name(first_name), id(id) {
    }
    std::optional<std::string> username;
    std::optional<std::string> language_code;
};

struct MessageEntity {
    int offset;
    std::string type;
    int length;
};

struct Chat {
    Chat(std::string type, int64_t id) : type(type), id(id) {
    }
    Chat(int64_t id) : id(id) {
    }
    Chat() = default;
    std::string type;
    std::optional<std::string> username;
    std::optional<std::string> first_name;
    int64_t id;
    std::optional<std::string> title;
    std::optional<bool> all_members_are_administrators;
};

struct Message {
    Message(int64_t date, Chat chat, int64_t message_id)
        : date(date), chat(chat), message_id(message_id) {
    }
    Message(Chat chat) : chat(chat) {
    }
    Message(int64_t message_id) : message_id(message_id) {
    }
    int64_t date;
    std::optional<std::vector<MessageEntity>> entities;
    std::optional<User> from;
    Chat chat;
    std::optional<std::string> text;
    int64_t message_id;
    std::optional<bool> group_chat_created;
    std::unique_ptr<Message> reply_to_message;
};

struct Updates {
    int64_t update_id;
    std::optional<Message> message;
};

class BotAPI {
public:
    BotAPI(const std::string& api_key, std::string_view api_endpoint);
    ~BotAPI();

    User GetMe() const;
    std::vector<Updates> GetUpdates(int64_t timeout = 0, int64_t offset = 0);
    void SendMessage(int64_t chat_id, std::string text, int64_t reply_to_message_id = 0);

private:
    class ApiImpl;
    std::unique_ptr<ApiImpl> impl_;
};