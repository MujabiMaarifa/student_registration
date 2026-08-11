#include "registration.hpp"

namespace routes
{
    crow::response register_user(pqxx::connection& cx, const crow::request& req)
    {
        json body;
        auto err = utils::validate_json_body(req, body, {
                        {"student_id", json::value_t::string},
                        {"first_name", json::value_t::string},
                        {"last_name",  json::value_t::string},
                        {"email",      json::value_t::string},
                        {"password",   json::value_t::string},
                        {"year_of_study", json::value_t::number_unsigned},
                        {"dept_code",  json::value_t::string},
                    });
        if (err) return std::move(err.value());
        pqxx::work tx{cx};
        char password_hash[crypto_pwhash_STRBYTES];
        std::string password = body["password"].get<std::string>();
        if (crypto_pwhash_str(password_hash, password.c_str(), password.size(),
                    crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
            return crow::response(503, json{{"error", "Server out of resources"}}.dump());
        try
        {
            tx.exec(
                "INSERT INTO students "
                "(student_id, first_name, last_name, email, password_hash, year_of_study, dept_code) "
                "VALUES ($1, $2, $3, $4, $5, $6, $7)",
                pqxx::params{
                    body["student_id"].get<std::string>(),
                    body["first_name"].get<std::string>(),
                    body["last_name"].get<std::string>(),
                    body["email"].get<std::string>(),
                    password_hash,
                    body["year_of_study"].get<int>(),
                    body["dept_code"].get<std::string>()
                }
            );
            tx.commit();
        }
        catch (pqxx::unique_violation const &e)
        {
            return crow::response(409, json{{"error", "Student already exists"}}.dump());
        }
        catch (pqxx::foreign_key_violation const &e)
        {
            return crow::response(400, json{{"error", "Department does not exist"}}.dump());
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
        return crow::response(201, json{{"success", true}}.dump());
    }
}
