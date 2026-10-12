#include "crow_all.h"
#include "calculator.h"
#include "greeter.h"

struct CORSRules {
    struct context {};
    void before_handle(crow::request&, crow::response&, context&) {}
    void after_handle(crow::request&, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

int main() {
    crow::App<CORSRules> app;

    CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::OPTIONS)
    ([](crow::response& res, std::string) {
        res.code = 204;
        res.end();
    });

    CROW_ROUTE(app, "/api/greet").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) {
            return crow::response(400, "{\"error\": \"Missing name field\"}");
        }

        crow::json::wvalue res;
        // Call function defined in greeter.cpp
        res["greeting"] = createGreeting(body["name"].s());

        crow::response response(res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    CROW_ROUTE(app, "/api/calculate").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("numberone") || !body.has("op") || !body.has("numbertwo")) {
            return crow::response(400, "{\"error\": \"Missing fields\"}");
        }

        int n1 = body["numberone"].i();
        std::string op_str = body["op"].s();
        char op = op_str.empty() ? ' ' : op_str[0];
        int n2 = body["numbertwo"].i();

        if (!checkIfValidOperator(op)) {
            crow::json::wvalue res;
            res["error"] = op_str + " is not a valid operator";
            return crow::response(400, res);
        }

        if (op == '/' && n2 == 0) {
            crow::json::wvalue res;
            res["error"] = "Cannot divide by 0";
            return crow::response(400, res);
        }

        crow::json::wvalue res;
        // Call functions defined in calculator.cpp
        res["Calculation"] = std::to_string(n1) + " " + op + " " + std::to_string(n2) + 
                             " = " + std::to_string(calc(n1, op, n2));

        crow::response response(res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    app.port(18080).run();
}