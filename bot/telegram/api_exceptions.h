#include <string>
#include <stdexcept>

struct BadStatus : std::runtime_error {
    BadStatus(int code)
        : std::runtime_error{"BadStatus of response. Code: " + std::to_string(code)} {
    }
};

struct GetUserError : std::runtime_error {
    GetUserError() : std::runtime_error{"GetUser exception"} {
    }
};

struct GetChatError : std::runtime_error {
    GetChatError() : std::runtime_error{"GetChat exception"} {
    }
};

struct GetMessageEntityError : std::runtime_error {
    GetMessageEntityError() : std::runtime_error{"GetMessageEntity exception"} {
    }
};

struct GetEntitiesError : std::runtime_error {
    GetEntitiesError() : std::runtime_error{"GetEntities exception"} {
    }
};

struct GetMessageError : std::runtime_error {
    GetMessageError() : std::runtime_error{"GetMessage exception"} {
    }
};

struct ServerError : std::runtime_error {
    ServerError() : std::runtime_error{"Internal server error"} {
    }
};

struct UnauthorizedError : std::exception {
    std::string description = "Unauthorized";
    bool ok = false;
    int error_code = 401;
};