#include "bot.h"
#include <memory>

int main() {
    std::string token, host_name = "https://api.telegram.org/";
    // std::cin >> token;
    token = "5906126884:AAGNC5S8_-KE-oZj64rIMzGvKJ4ag2zZczU";
    Bot bot(std::make_unique<BotAPI>(token, host_name));
    bot.Run();
}
