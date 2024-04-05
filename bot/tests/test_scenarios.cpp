#include <tests/test_scenarios.h>

#include <catch2/catch_test_macros.hpp>

#include <telegram/api.h>

void CheckUser(const User& expected, const User& result) {
    REQUIRE(expected.is_bot == result.is_bot);
    REQUIRE(expected.first_name == result.first_name);
    REQUIRE(expected.id == result.id);
    if (expected.username) {
        REQUIRE(result.username.has_value());
        REQUIRE(expected.username.value() == result.username.value());
    }
    if (expected.language_code) {
        REQUIRE(result.language_code.has_value());
        REQUIRE(expected.language_code.value() == result.language_code.value());
    }
}

void CheckEntities(const std::vector<MessageEntity>& expected,
                   const std::vector<MessageEntity>& result) {
    REQUIRE(expected.size() == result.size());
    for (size_t ind = 0; ind < expected.size(); ++ind) {
        REQUIRE(expected[ind].length == result[ind].length);
        REQUIRE(expected[ind].offset == result[ind].offset);
        REQUIRE(expected[ind].type == result[ind].type);
    }
}

void CheckMessage(const Message& expected, const Message& result) {
    REQUIRE(expected.date == result.date);
    REQUIRE(expected.message_id == result.message_id);
    REQUIRE(expected.chat.type == result.chat.type);
    REQUIRE(expected.chat.id == result.chat.id);
    if (expected.entities) {
        REQUIRE(result.entities.has_value());
        CheckEntities(expected.entities.value(), result.entities.value());
    }
    if (expected.from) {
        REQUIRE(result.from.has_value());
        CheckUser(expected.from.value(), result.from.value());
    }
    if (expected.text) {
        REQUIRE(result.text.has_value());
        REQUIRE(expected.text.value() == result.text.value());
    }
    if (expected.group_chat_created) {
        REQUIRE(result.group_chat_created.has_value());
        REQUIRE(expected.group_chat_created.value() == result.group_chat_created.value());
    }
    if (expected.reply_to_message) {
        REQUIRE(expected.reply_to_message->message_id == result.reply_to_message->message_id);
    }
}

void CheckUpdates(const Updates& expected, const Updates& result) {
    REQUIRE(expected.update_id == result.update_id);
    if (expected.message) {
        REQUIRE(result.message.has_value());
        CheckMessage(expected.message.value(), result.message.value());
    }
}

void CheckVectorsUpdates(const std::vector<Updates>& expected, const std::vector<Updates>& result) {
    REQUIRE(expected.size() == result.size());
    for (size_t ind = 0; ind < expected.size(); ++ind) {
        CheckUpdates(expected[ind], result[ind]);
    }
}

std::vector<Updates> MakeUpdates() {
    std::vector<Updates> result;
    // 1 msg
    result.emplace_back(851793506, Message{1510493105, Chat{"private", 104519755}, 1});
    result.back().message.value().entities = {MessageEntity{0, "bot_command", 6}};
    result.back().message.value().chat.username = "darth_slon";
    result.back().message.value().chat.first_name = "Fedor";
    result.back().message.value().text = "/start";
    result.back().message.value().from = User{false, "Fedor", 104519755};
    result.back().message.value().from.value().username = "darth_slon";

    // 2 msg
    result.emplace_back(851793507, Message{1510493105, Chat{"private", 104519755}, 2});
    result.back().message.value().entities = {MessageEntity{0, "bot_command", 6}};
    result.back().message.value().chat.username = "darth_slon";
    result.back().message.value().chat.first_name = "Fedor";
    result.back().message.value().text = "/end";
    result.back().message.value().from = User{false, "Fedor", 104519755};
    result.back().message.value().from.value().username = "darth_slon";

    // 3 msg
    result.emplace_back(851793507, Message{1510519971, Chat{"group", -274574250}, 10});
    result.back().message.value().chat.title = "bottest";
    result.back().message.value().chat.all_members_are_administrators = true;
    result.back().message.value().group_chat_created = true;
    result.back().message.value().from = User{false, "Fedor", 104519755};
    result.back().message.value().from.value().username = "darth_slon";
    result.back().message.value().from.value().language_code = "en-US";

    // 4 msg
    result.emplace_back(851793508, Message{1510520023, Chat{"group", -274574250}, 11});
    result.back().message.value().entities = {MessageEntity{0, "bot_command", 5}};
    result.back().message.value().chat.all_members_are_administrators = true;
    result.back().message.value().chat.title = "bottest";
    result.back().message.value().text = "/1234";
    result.back().message.value().from = User{false, "Fedor", 104519755};
    result.back().message.value().from.value().username = "darth_slon";
    result.back().message.value().from.value().language_code = "en-US";
    return result;
}

std::vector<Updates> TimeoutUpdates() {
    auto expected = MakeUpdates();
    expected.pop_back();
    expected.pop_back();
    return expected;
}

std::vector<Updates> LastUpdates() {
    std::vector<Updates> result;
    result.emplace_back(851793508, Message{1510520023, Chat{"group", -274574250}, 11});
    result.back().message.value().entities = {MessageEntity{0, "bot_command", 5}};
    result.back().message.value().chat.all_members_are_administrators = true;
    result.back().message.value().chat.title = "bottest";
    result.back().message.value().text = "/1234";
    result.back().message.value().from = User{false, "Fedor", 104519755};
    result.back().message.value().from.value().username = "darth_slon";
    result.back().message.value().from.value().language_code = "en-US";
    return result;
}

void TestSingleGetMe(std::string_view url) {
    auto api = BotAPI("123", url);
    auto me = api.GetMe();
    REQUIRE(me.id == 1234567);
    REQUIRE(me.is_bot);
    REQUIRE(me.username == "test_bot");
    REQUIRE(me.first_name == "Test Bot");
}

void TestGetMeErrorHandling(std::string_view url) {
    auto api = BotAPI("123", url);
    REQUIRE_THROWS_AS(api.GetMe(), ServerError);
    REQUIRE_THROWS_AS(api.GetMe(), UnauthorizedError);
}

void TestSingleGetUpdatesAndSendMessages(std::string_view url) {
    auto api = BotAPI("123", url);
    auto updates = api.GetUpdates();
    auto expected = MakeUpdates();
    CheckVectorsUpdates(expected, updates);
    api.SendMessage(104519755, "Hi!");
    api.SendMessage(104519755, "Reply", 2);
    api.SendMessage(104519755, "Reply", 2);
}

void TestHandleGetUpdatesOffset(std::string_view url) {
    auto api = BotAPI("123", url);
    auto updates = api.GetUpdates(5);
    auto expected = TimeoutUpdates();
    CheckVectorsUpdates(expected, updates);
    REQUIRE(updates.size() == 2);
    auto max_update_id = std::max(updates[0].update_id, updates[1].update_id);
    updates = api.GetUpdates(5, max_update_id + 1);
    REQUIRE(updates.empty());
    updates = api.GetUpdates(5, max_update_id + 1);
    expected = LastUpdates();
    CheckVectorsUpdates(expected, updates);
}
