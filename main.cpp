#include <iostream>
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

const string PRIVATE_MSG = "PRIVATE_MSG";
const string PUBLIC_MSG = "PUBLIC_MSG";
const string STATUS = "STATUS";
const string SET_NAME = "SET_NAME";
const string PUBLIC_CHANNEL = "PUBLIC_CHANNEL";

// Структура, где мы будем хранить информацию о каждом подключившемся
struct UserData {
	int id;
	string name;
};

// Чтобы записывать типы покороче
typedef uWS::WebSocket<false, true, UserData>* websock;
string personal_channel(UserData* data);
// Карта, ключ = айди пользователя, значения = данные пользователя
map<int, UserData*> all_users;

// "отправить приватное сообщение" = PRIVATE_MSG
// JSON {"command": "PRIVATE_MSG", "user_to": 11, "text" : "Эй привет 11ый!"}
void process_private_msg(websock ws, json parsed_data) {
	UserData* data = ws->getUserData(); // Получаем информацию о текущему пользовале

	json payload;
	payload["command"] = PRIVATE_MSG;
	payload["user_from"] = data->id;
	payload["text"] = parsed_data["text"];
	
	//ToDO check if user exists
	UserData* recepient = all_users[parsed_data["user_to"]];

	ws->publish(personal_channel(recepient), payload.dump());
}

// JSON {"command": "PUBLIC_MSG", "text" : "Всем приветы в этом чате!"}
void process_public_msg(websock ws, json parsed_data) {
	UserData* data = ws->getUserData(); 

	json payload = {
		{"command", PUBLIC_MSG },
		{"text", parsed_data["text"]},
		{"user_from", data->id}
	};

	ws->publish(PUBLIC_CHANNEL, payload.dump());
}

// Кто в онлайне, а кто в оффлайне?
// {"command": "STATUS", "online": True/False, "user_id": 10}
string user_status(UserData* data, bool online) {
	json payload = {
		{"command", STATUS},
		{"online", online},
		{"user_id", data->id},
		{"name", data->name}
	};
	return payload.dump();
}

// {"command":"SET_NAME", "name": "AKOKO"}
void process_set_name(websock ws, json parsed_data) {
	UserData* data = ws->getUserData();
	data->name = parsed_data["name"]; // Обновляем имя

	ws->publish(PUBLIC_CHANNEL, user_status(data, true)); // Всем сообщаем
}

string personal_channel(UserData* data) {
	return "USER_" + to_string(data->id);
}




int main()
{
	int latest_user_id = 10; // ID последнего подключившегося
	uWS::App app = uWS::App(); // Создали приложение
	
	// Создали раздел "/" на сайте
	app.get("/", [](auto *response, auto *request) {
		response->writeHeader("Content-Type", "text/html; charset=utf-8"); // Отдаем ХТМЛ
		response->end("Hello, this is <b>SkillChat</b> Server C++ 5000"); // <- Вот этот ХТМЛ
	});
	// [0-1000] [ 1001-3000]
	app.ws<UserData>("/*", {
		// "[]() {}" - Лямбда - функция
		// открытие соединения
		// Назначить ID
		// Сообщить всем что человек (такой-то) подключился
		.open = [&latest_user_id](websock ws) {
			UserData* data = ws->getUserData(); // Получаем данные пользователя
			data->id = latest_user_id++; // Каждому следующему подключив. будет назначен следующий АЙди
			
			data->name = "UnnamedUser#" + to_string(data->id); // Даем временное имя
			
			all_users[data->id] = data; // Положить информацию о пользователе в карту

			cout << "New user connected: " << data->id << endl;

			ws->subscribe(personal_channel(data)); // Каждого пользователя подписываем на личный канал

			ws->publish(PUBLIC_CHANNEL, user_status(data, true)); // Уведомляем всех что "новичок" подключился

			ws->subscribe(PUBLIC_CHANNEL); // Всех-всех-всех подписываем на публичный канал

			for (auto item : all_users) {
				//item.second UserData*
				//item.first int == id
				ws->send(user_status(item.second, true), uWS::OpCode::TEXT);
			}
			// ToDo: уведомим новичка о всех "старичках" в чате
		},
		// при получении информации от клиента
		// "отправить публичное сообщение" = PUBLIC_MSG
		// "представиться" = SET_NAME

		.message = [](websock ws, string_view message, uWS::OpCode opcode) {
			
			json parsed_data;
			try {
				parsed_data = json::parse(message); // Распарсить JSON
			}
			catch (const json::parse_error& err) {
				cout << err.what() << endl;
				return; 
			}
			string command = parsed_data["command"];

			cout << "Got command " << command << endl;

			if (command == PRIVATE_MSG) {
				process_private_msg(ws, parsed_data);
			}

			if (command == PUBLIC_MSG) {
				process_public_msg(ws, parsed_data);
			}

			if (command == SET_NAME) {
				process_set_name(ws, parsed_data);
			}
			// ToDo: command "SET_NAME"
		},
		// при закрытии соединения
		// Сообщить всем что человек (такой-то) отключился
		.close = [&app](websock ws, int, string_view) {
			UserData* data = ws->getUserData();
			// ToDo: Всем сообщить что пользователь отключился
			app.publish(PUBLIC_CHANNEL, user_status(data, false), uWS::OpCode::TEXT);
			all_users.erase(data->id); // Стираем пользователя
		},
	});
	
	app.listen(9001, [](auto *) {});
	app.run();
}