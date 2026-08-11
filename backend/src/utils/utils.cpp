#include "utils.hpp"
namespace utils
{
    std::optional<crow::response> validate_json_body(
            const crow::request& req,
            json& out,
            const std::vector<FieldRule>& rules
            )
    {
        if (req.body.empty())
            return crow::response(400, json{{"error", "request body is empty"}}.dump());

        try
        {
            out = json::parse(req.body);
        }
        catch (const json::parse_error& e)
        {
            return crow::response(400, json{{"error", e.what()}}.dump());
        }

        for (const auto& rule : rules) 
        {
            if (!out.contains(rule.name))
                return crow::response(400, json{{"error", "missing field: " + rule.name}}.dump());
            if (out[rule.name].type() != rule.type)
                return crow::response(400, json{{"error", "invalid type for field: " + rule.name}}.dump());
            if (rule.type == json::value_t::string && out[rule.name].get<std::string>().empty())
                return crow::response(400, json{{"error", "field cannot be empty: " + rule.name}}.dump());
        }
        return std::nullopt;
    }

    std::string jwt_create(const std::string& id, const std::string& role)
    {
        std::string claim_name = role == "lecturer" ? "lecturer_id" : "student_id";
        auto token = jwt::create()
                         .set_issuer("auth0")
                         .set_type("JWS")
                         .set_payload_claim(claim_name, jwt::claim(id))
                         .set_payload_claim("role", jwt::claim(role))
                         .sign(jwt::algorithm::hs256{"secret"});
        return token;
    }

    std::optional<std::string> get_jwt_claim(const crow::request& req, const std::string& claim)
    {
        std::string myauth = req.get_header_value("Authorization");
        if (myauth.size() < 7) return std::nullopt;
        std::string token = myauth.substr(7);
        auto decoded = jwt::decode(token);
        if (!decoded.has_payload_claim(claim)) return std::nullopt;
        return decoded.get_payload_claim(claim).as_string();
    }

    std::vector<std::string_view> split_string(const std::string& s1, const std::string& del)
    {
        return std::views::split(s1, del)
            | std::views::transform([](const auto& x) { return std::string_view{x.begin(), x.end()};})
            | std::ranges::to<std::vector<std::string_view>>();
    }
}
