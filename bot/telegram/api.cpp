#include "api.h"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/JSON/Parser.h>

void CheckStatus(const Poco::Net::HTTPResponse& response) {
    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR) {
        throw ServerError{};
    }

    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED) {
        throw UnauthorizedError{};
    }

    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        throw BadStatus{response.getStatus()};
    }
}

User GetUser(const Poco::JSON::Object::Ptr ptr) {
    try {
        int64_t id = ptr->getValue<int64_t>("id");
        bool is_bot = ptr->getValue<bool>("is_bot");
        auto first_name = ptr->getValue<std::string>("first_name");
        User result{is_bot, first_name, id};
        if (ptr->has("username")) {
            result.username = ptr->getValue<std::string>("username");
        }
        if (ptr->has("language_code")) {
            result.language_code = ptr->getValue<std::string>("language_code");
        }
        return result;
    } catch (...) {
        throw GetUserError{};
    }
}

Chat GetChat(const Poco::JSON::Object::Ptr ptr) {
    try {
        int64_t id = ptr->getValue<int64_t>("id");
        auto type = ptr->getValue<std::string>("type");
        Chat result{type, id};
        if (ptr->has("username")) {
            result.username = ptr->getValue<std::string>("username");
        }
        if (ptr->has("first_name")) {
            result.first_name = ptr->getValue<std::string>("first_name");
        }
        if (ptr->has("title")) {
            result.title = ptr->getValue<std::string>("title");
        }
        if (ptr->has("all_members_are_administrators")) {
            result.all_members_are_administrators =
                ptr->getValue<bool>("all_members_are_administrators");
        }
        return result;
    } catch (...) {
        throw GetChatError{};
    }
}

MessageEntity GetMessageEntity(const Poco::JSON::Object::Ptr ptr) {
    try {
        return MessageEntity{ptr->getValue<int>("offset"), ptr->getValue<std::string>("type"),
                             ptr->getValue<int>("length")};
    } catch (...) {
        throw GetMessageEntityError{};
    }
}

std::vector<MessageEntity> GetEntities(const Poco::JSON::Array::Ptr ptr) {
    try {
        std::vector<MessageEntity> result;
        auto entities_vector_in = *ptr;
        for (auto ent : entities_vector_in) {
            auto ent_ptr = ent.extract<Poco::JSON::Object::Ptr>();
            result.push_back(GetMessageEntity(ent_ptr));
        }
        return result;
    } catch (const GetMessageEntityError& ex) {
        throw;
    } catch (const GetEntitiesError& ex) {
        throw GetEntitiesError{};
    }
}

Message GetMessage(const Poco::JSON::Object::Ptr ptr) {
    try {
        Message result{ptr->getValue<int64_t>("date"), GetChat(ptr->getObject("chat")),
                       ptr->getValue<int64_t>("message_id")};
        if (ptr->has("entities")) {
            result.entities = GetEntities(ptr->getArray("entities"));
        }
        if (ptr->has("from")) {
            result.from = GetUser(ptr->getObject("from"));
        }
        if (ptr->has("text")) {
            result.text = ptr->getValue<std::string>("text");
        }
        if (ptr->has("reply_to_message")) {
            result.reply_to_message =
                std::make_unique<Message>(GetMessage(ptr->getObject("reply_to_message")));
        }
        if (ptr->has("group_chat_created")) {
            result.group_chat_created = ptr->getValue<bool>("group_chat_created");
        }
        return result;
    } catch (const GetMessageEntityError& ex) {
        throw;
    } catch (const GetEntitiesError& ex) {
        throw;
    } catch (const GetChatError& ex) {
        throw;
    } catch (const GetUserError& ex) {
        throw;
    } catch (...) {
        throw GetMessageError{};
    }
}

std::vector<Updates> GetVectorUpdates(const Poco::JSON::Array::Ptr ptr) {
    try {
        std::vector<Updates> result;
        auto updates = *ptr;
        for (auto upd : updates) {
            auto upd_ptr = upd.extract<Poco::JSON::Object::Ptr>();
            result.emplace_back(upd_ptr->getValue<int64_t>("update_id"),
                                GetMessage(upd_ptr->getObject("message")));
        }
        return result;
    } catch (...) {
        throw;
    }
}

class BotAPI::ApiImpl {
public:
    ApiImpl(std::string token, std::string host_name)
        : token_(std::move(token)), host_name_(std::move(host_name)) {
    }
    ~ApiImpl() = default;
    User GetMe() const {
        Poco::URI uri{host_name_ + "bot" + token_ + "/getMe"};

        Poco::Net::HTTPRequest request{"GET", uri.getPathAndQuery()};

        std::unique_ptr<Poco::Net::HTTPClientSession> session_ptr;
        if (host_name_ == "http://localhost:8080/") {
            session_ptr =
                std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
        } else {
            session_ptr =
                std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort());
        }

        session_ptr->sendRequest(request);

        try {
            Poco::Net::HTTPResponse response;
            auto& in = session_ptr->receiveResponse(response);

            CheckStatus(response);

            Poco::JSON::Parser parser;
            auto json = parser.parse(in);
            auto get_me_ptr = json.extract<Poco::JSON::Object::Ptr>();

            auto result = get_me_ptr->getObject("result");

            return GetUser(result);

        } catch (...) {
            throw;
        }
    }
    std::vector<Updates> GetUpdates(int64_t timeout, int64_t offset) {

        Poco::URI uri{host_name_ + "bot" + token_ + "/getUpdates"};

        if (offset) {
            uri.addQueryParameter("offset", std::to_string(offset));
        }
        if (timeout) {
            uri.addQueryParameter("timeout", std::to_string(timeout));
        }

        Poco::Net::HTTPRequest request{"GET", uri.getPathAndQuery()};

        std::unique_ptr<Poco::Net::HTTPClientSession> session_ptr;
        if (host_name_ == "http://localhost:8080/") {
            session_ptr =
                std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
        } else {
            session_ptr =
                std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort());
        }
        session_ptr->sendRequest(request);

        try {

            Poco::Net::HTTPResponse response;
            auto& in = session_ptr->receiveResponse(response);

            CheckStatus(response);

            Poco::JSON::Parser parser;
            auto json = parser.parse(in);
            auto updates_ptr = json.extract<Poco::JSON::Object::Ptr>();

            auto updates = GetVectorUpdates(updates_ptr->getArray("result"));

            return updates;

        } catch (...) {
            throw;
        }
    }
    void SendMessage(int64_t chat_id, std::string text, int64_t reply_to_message_id) {
        Poco::URI uri(host_name_ + "bot" + token_ + "/sendMessage");

        Poco::Net::HTTPRequest request{"POST", uri.getPathAndQuery()};
        request.setContentType("application/json");

        std::unique_ptr<Poco::Net::HTTPClientSession> session_ptr;
        if (host_name_ == "http://localhost:8080/") {
            session_ptr =
                std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
        } else {
            session_ptr =
                std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort());
        }

        Poco::JSON::Object object;

        Message message{Chat{chat_id}};
        message.text = text;
        if (reply_to_message_id) {
            message.reply_to_message = std::make_unique<Message>(reply_to_message_id);
        }

        object.set("chat_id", std::to_string(message.chat.id));
        object.set("text", message.text.value());
        if (message.reply_to_message) {
            object.set("reply_to_message_id", std::to_string(message.reply_to_message->message_id));
        }

        Poco::JSON::Stringifier stringfier;
        std::stringstream stream;
        stringfier.stringify(object, stream);
        auto msg = stream.str();

        request.add("Content-Length", std::to_string(msg.size()));

        auto& out = session_ptr->sendRequest(request);
        out << msg;

        try {

            Poco::Net::HTTPResponse response;

            CheckStatus(response);

            session_ptr->receiveResponse(response);

        } catch (...) {
            throw;
        }
    }

private:
    std::string token_;
    std::string host_name_;
};

BotAPI::BotAPI(const std::string& api_key, std::string_view api_endpoint) {
    impl_ = std::make_unique<ApiImpl>(api_key, std::move(std::string{api_endpoint}));
}

BotAPI::~BotAPI() = default;

User BotAPI::GetMe() const {
    if (impl_) {
        return impl_->GetMe();
    } else {
        throw std::runtime_error{""};
    }
}

std::vector<Updates> BotAPI::GetUpdates(int64_t timeout, int64_t offset) {
    if (impl_) {
        return impl_->GetUpdates(timeout, offset);
    } else {
        throw std::runtime_error{""};
    }
}

void BotAPI::SendMessage(int64_t chat_id, std::string text, int64_t reply_to_message_id) {
    if (impl_) {
        return impl_->SendMessage(chat_id, text, reply_to_message_id);
    } else {
        throw std::runtime_error{""};
    }
}