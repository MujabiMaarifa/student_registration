#pragma once
using json = nlohmann::json;
namespace utils
{

    struct FieldRule
    {
        std::string name;
        json::value_t type;
    };

    std::optional<crow::response> validate_json_body(
            const crow::request& req,
            json& out,
            const std::vector<FieldRule>& rules
     );

     //TODO: replace `secret` with an actual secret
    template <typename T>
    void jwt_verify(const jwt::decoded_jwt<T>& decoded_token)
    {
         auto verifier = jwt::verify()
                             .with_issuer("auth0")
                             .allow_algorithm(jwt::algorithm::hs256{"secret"});
         verifier.verify(decoded_token);
    }

    std::string jwt_create(const std::string& id, const std::string& role);

    std::optional<std::string> get_jwt_claim(const crow::request& req, const std::string& claim);

    std::vector<std::string_view> split_string(const std::string& s1, const std::string& del);
}
