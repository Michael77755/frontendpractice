#include "crow_all.h"

using namespace std;

// Define the CORS middleware
struct CORSRules {
    struct context {};

    void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) {}

    void after_handle(crow::request& req, crow::response& res, context& /*ctx*/) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

bool checkIfValidOperator(char op)
{
    switch(op)
    {
        case '+': return true;
        break;
        case '-': return true;
        break;
        case '/': return true;
        break;
        case '*': return true;
        break;
        default: return false;
    }
};

double calc(int numberone, char op, int numbertwo)
{
    switch(op)
    {
        case '+': return numberone + numbertwo;
        break;
        case '-': return numberone - numbertwo;
        break;
        case '/': return static_cast<double>(numberone) / numbertwo;
        break;
        case '*': return numberone * numbertwo;;
        break;
        default: return 0.0;
    }
};


int main() {
    // Attach the middleware to the app
    crow::App<CORSRules> app;

    // 1. Universal handler for browser preflight (OPTIONS) checks
    CROW_ROUTE(app, "/<path>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](crow::response& res, std::string /*path*/) {
        res.code = 204;
        res.end();
    });

    // 2. GET route
    CROW_ROUTE(app, "/api/message")
    ([]() {
        crow::json::wvalue res;
        res["message"] = "Please enter an op and two numbers for me to do calculations on.";
        return crow::response(res);
    });

    // 3. POST route
    CROW_ROUTE(app, "/api/greet")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) {
            return crow::response(400, "{\"error\": \"Missing name field\"}");
        }

        std::string user_name = body["name"].s();

        crow::json::wvalue res;
        res["greeting"] = "Nice to meet you, " + user_name + " (from C++)!";

        crow::response response(res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    CROW_ROUTE(app, "/api/calculate")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("numberone")) {
            return crow::response(400, "{\"error\": \"Missing number one field\"}");
        }

        if (!body || !body.has("op")) {
            return crow::response(400, "{\"error\": \"Missing op field\"}");
        }

        if (!body || !body.has("numbertwo")) {
            return crow::response(400, "{\"error\": \"Missing number two field\"}");
        }
        

        int numberone = body["numberone"].i();
        std::string op_str = body["op"].s();
        char op = op_str.empty() ? ' ' : op_str[0];
        int numbertwo = body["numbertwo"].i();
        

        if(!checkIfValidOperator(op))
        {    
            crow::json::wvalue res;
            res["error"] = op_str + " is not a valid op! Please try again";
            crow::response response(400, res);
            return response;
        }
        else if(op == '/' && numbertwo == 0)
        {    
            crow::json::wvalue res;
            res["error"] = "You can't divide by 0";
            crow::response response(400, res);
            return response;
        }


        crow::json::wvalue res;
        res["calculation"] = std::to_string(numberone) + " " + op_str + " " + std::to_string(numbertwo) + " = " + std::to_string(calc(numberone, op, numbertwo));

        crow::response response(res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    app.port(18080).run();
}