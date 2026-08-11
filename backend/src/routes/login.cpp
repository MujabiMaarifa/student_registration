#include "login.hpp"

namespace routes
{
    crow::response login(pqxx::connection& cx, const crow::request& req)
    {
        json body;
        if (auto err = utils::validate_json_body(req, body, {
                        {"email",      json::value_t::string},
                        {"password",   json::value_t::string}})
            ) return std::move(err.value());
        pqxx::work tx{cx};
        try
        {
            std::string email    = body["email"].get<std::string>();
            std::string password = body["password"].get<std::string>();

            pqxx::result result{tx.exec(
                "select * from students where email = $1",
                pqxx::params{email}
            )};

            if (!result.empty())
            {
                auto row = result.front();
                auto user_password = row["password_hash"].c_str();

                if (crypto_pwhash_str_verify(user_password, password.c_str(), password.size()) != 0)
                    return crow::response(401, json{{"error", "wrong password"}}.dump());

                auto token = utils::jwt_create(row["student_id"].c_str(), "student");
                return crow::response(200, json{{"success", true}, {"token", token}, {"role", "student"}}.dump());
            }

            pqxx::result lec_result{tx.exec(
                "select * from lecturers where email = $1",
                pqxx::params{email}
            )};

            if (!lec_result.empty())
            {
                auto row = lec_result.front();
                auto user_password = row["password_hash"].c_str();

                if (crypto_pwhash_str_verify(user_password, password.c_str(), password.size()) != 0)
                    return crow::response(401, json{{"error", "wrong password"}}.dump());

                auto token = utils::jwt_create(row["lecturer_id"].c_str(), "lecturer");
                return crow::response(200, json{{"success", true}, {"token", token}, {"role", "lecturer"}}.dump());
            }

            return crow::response(404, json{{"error", "user not found"}}.dump());
        }
        catch (pqxx::failure const &e)
        {
            std::cerr << "SQL error: " << e.what() << '\n';
            return crow::response(500, json{{"error", "Internal Server Error"}}.dump());
        }
        catch (std::exception const &e)
        {
            return crow::response(409, json{{"error", e.what()}}.dump());
        }
    }
}
