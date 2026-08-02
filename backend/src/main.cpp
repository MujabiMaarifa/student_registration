#include <mutex>
#include "routes/courses.hpp"
#include "routes/login.hpp"
#include "routes/registration.hpp"
#include "utils/utils.hpp"
#include "crow/middlewares/cors.h"

struct JWTMiddleWare: crow::ILocalMiddleware
{
     struct context{};
     void before_handle(crow::request& req, crow::response& res, context&)
     {
         std::string myauth = req.get_header_value("Authorization");
         if (myauth.size() < 7)
         {
                 res.code = 400;
                 res.write("Bad Request: malformed authorization header");
                 res.end();
                 return;
         }
         std::string token  = myauth.substr(7);
         try
         {
             auto decoded = jwt::decode(token);
             utils::jwt_verify(decoded);
             auto student_id = decoded.get_payload_claim("student_id").as_string();
             if (student_id.empty())
             {
                 res.code = 400;
                 res.write("Bad Request: malformed jwt");
                 res.end();
             }
         }
         catch (const jwt::error::token_verification_exception& e) 
         {
             res.code = 401;
             res.write("Unauthorized: " + std::string(e.what()));
             res.end();
         }
         catch (const std::exception& e) 
         {
             res.code = 400;
             res.write("Bad Request: " + std::string(e.what()));
             res.end();
         }
     }
     void after_handle(crow::request&, crow::response&, context&){}
};

int main()
{
    crow::App<crow::CORSHandler, JWTMiddleWare> app;
    //TODO: put this in a dotenv
    pqxx::connection cx{"postgresql://srg_user:student_reg@localhost/student_reg"};
    std::mutex db_mutex;

    CROW_ROUTE(app, "/")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    ([](){
        return "Hello world";
    });

    CROW_ROUTE(app, "/register")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        std::lock_guard lock(db_mutex);
        return routes::register_user(cx, req);
    });

    CROW_ROUTE(app, "/login")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        std::lock_guard lock(db_mutex);
        return routes::login(cx, req);
    });

    CROW_ROUTE(app, "/courses/registered")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    .methods("GET"_method)
    ([&](const crow::request& req) {
        std::lock_guard lock(db_mutex);
        return routes::get_registered_courses(cx, req);
    });

    CROW_ROUTE(app, "/courses")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    .methods("GET"_method)
    ([&](const crow::request& req) {
        std::lock_guard lock(db_mutex);
        return routes::get_courses(cx, req);
    });

    CROW_ROUTE(app, "/courses/<string>/register")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    .methods("POST"_method)
    ([&](const crow::request& req, std::string course_id) {
        std::lock_guard lock(db_mutex);
        return routes::register_course(cx, req, course_id);
    });

    CROW_ROUTE(app, "/courses/<string>/drop")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    .methods("POST"_method)
    ([&](const crow::request& req, std::string course_id) {
        std::lock_guard lock(db_mutex);
        return routes::drop_course(cx, req, course_id);
    });

    CROW_ROUTE(app, "/courses/<string>")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    .methods("GET"_method)
    ([&](const crow::request&, std::string course_id) {
        std::lock_guard lock(db_mutex);
        return routes::get_course_by_id(cx, course_id);
    });

    app.port(1234).multithreaded().run();

}
