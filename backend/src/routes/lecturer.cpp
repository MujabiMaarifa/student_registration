#include "lecturer.hpp"

static std::optional<std::string> get_lecturer_id(const crow::request& req)
{
    auto role = utils::get_jwt_claim(req, "role");
    if (!role || *role != "lecturer") return std::nullopt;
    return utils::get_jwt_claim(req, "lecturer_id");
}

static json course_to_json(const pqxx::row_ref& row)
{
    json course;
    course["course_id"]      = row["course_id"].as<std::string>();
    course["dept_code"]      = row["dept_code"].as<std::string>();
    course["course_name"]    = row["course_name"].as<std::string>();
    course["description"]    = row["description"].is_null() ? "" : row["description"].as<std::string>();
    course["capacity"]       = row["capacity"].as<int>();
    course["enrolled_count"] = row["enrolled_count"].as<int>();
    course["semester"]       = row["semester"].as<std::string>();
    course["day_of_week"]    = row["day_of_week"].as<std::string>();
    course["start_time"]     = row["start_time"].as<std::string>();
    course["end_time"]       = row["end_time"].as<std::string>();
    course["room"]           = row["room"].is_null() ? "" : row["room"].as<std::string>();
    course["lecturer_id"]    = row["lecturer_id"].is_null() ? "" : row["lecturer_id"].as<std::string>();
    course["is_active"]      = row["is_active"].as<bool>();
    return course;
}

namespace routes
{
    crow::response register_lecturer(pqxx::connection& cx, const crow::request& req)
    {
        json body;
        auto err = utils::validate_json_body(req, body, {
                        {"lecturer_id",  json::value_t::string},
                        {"first_name",   json::value_t::string},
                        {"last_name",    json::value_t::string},
                        {"email",        json::value_t::string},
                        {"password",     json::value_t::string},
                        {"dept_code",    json::value_t::string},
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
            std::string dept_code = utils::normalize_dept_code(body["dept_code"].get<std::string>());
            tx.exec(
                "INSERT INTO lecturers "
                "(lecturer_id, first_name, last_name, email, password_hash, dept_code) "
                "VALUES ($1, $2, $3, $4, $5, $6)",
                pqxx::params{
                    body["lecturer_id"].get<std::string>(),
                    body["first_name"].get<std::string>(),
                    body["last_name"].get<std::string>(),
                    body["email"].get<std::string>(),
                    password_hash,
                    dept_code
                }
            );
            tx.commit();
        }
        catch (pqxx::unique_violation const &e)
        {
            return crow::response(409, json{{"error", "Lecturer already exists"}}.dump());
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

    crow::response create_course(pqxx::connection& cx, const crow::request& req)
    {
        auto lecturer_id = get_lecturer_id(req);
        if (!lecturer_id)
            return crow::response(403, json{{"error", "Only lecturers can create courses"}}.dump());

        json body;
        auto err = utils::validate_json_body(req, body, {
                        {"course_id",    json::value_t::string},
                        {"course_name",  json::value_t::string},
                        {"capacity",     json::value_t::number_unsigned},
                        {"semester",     json::value_t::string},
                        {"day_of_week",  json::value_t::string},
                        {"start_time",   json::value_t::string},
                        {"end_time",     json::value_t::string},
                    });
        if (err) return std::move(err.value());

        std::string description = body.value("description", "");
        std::string room        = body.value("room", "");

        pqxx::work tx{cx};
        try
        {
            pqxx::result dept{tx.exec(
                "SELECT dept_code FROM lecturers WHERE lecturer_id = $1",
                pqxx::params{*lecturer_id}
            )};
            if (dept.empty())
                return crow::response(404, json{{"error", "Lecturer not found"}}.dump());
            std::string dept_code = dept[0]["dept_code"].as<std::string>();

            pqxx::result rows{tx.exec(
                "INSERT INTO courses "
                "(course_id, dept_code, course_name, description, capacity, semester, "
                " day_of_week, start_time, end_time, room, lecturer_id) "
                "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11) "
                "RETURNING *",
                pqxx::params{
                    body["course_id"].get<std::string>(),
                    dept_code,
                    body["course_name"].get<std::string>(),
                    description,
                    body["capacity"].get<int>(),
                    body["semester"].get<std::string>(),
                    body["day_of_week"].get<std::string>(),
                    body["start_time"].get<std::string>(),
                    body["end_time"].get<std::string>(),
                    room,
                    *lecturer_id
                }
            )};
            tx.commit();
            return crow::response(201, json(course_to_json(rows[0])).dump());
        }
        catch (pqxx::unique_violation const &e)
        {
            return crow::response(409, json{{"error", "Course already exists"}}.dump());
        }
        catch (pqxx::failure const &e)
        {
            std::cerr << "SQL error: " << e.what() << '\n';
            return crow::response(500, json{{"error", "Internal Server Error"}}.dump());
        }
        catch (std::exception const &e)
        {
            return crow::response(400, json{{"error", e.what()}}.dump());
        }
    }

    crow::response get_lecturer_courses(pqxx::connection& cx, const crow::request& req)
    {
        auto lecturer_id = get_lecturer_id(req);
        if (!lecturer_id)
            return crow::response(403, json{{"error", "Only lecturers can view their courses"}}.dump());

        pqxx::work tx{cx};
        std::vector<json> courses;
        try
        {
            pqxx::result rows{tx.exec(
                "SELECT * FROM courses WHERE lecturer_id = $1 ORDER BY course_id",
                pqxx::params{*lecturer_id}
            )};
            for (const auto& row : rows)
                courses.emplace_back(course_to_json(row));
            return crow::response(200, json(courses).dump());
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
