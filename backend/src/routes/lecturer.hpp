#pragma once

namespace routes
{
    crow::response register_lecturer(pqxx::connection& cx, const crow::request& req);
    crow::response create_course(pqxx::connection& cx, const crow::request& req);
    crow::response get_lecturer_courses(pqxx::connection& cx, const crow::request& req);
}
