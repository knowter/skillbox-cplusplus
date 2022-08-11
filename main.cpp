#include <iostream>
#include <string>
#include <map>
#include <regex> // Regular Expression - регулярные выражения
using namespace std;

// База вопросы и ответов
map<string, string> questions_answers = {
        // Ключ и значение
        {"Hello", "Hi!"},
        {"How are you", "Normalno!!!"},
        {"What.*(is)?.*your.*name", "My name SkillChatC++ 5000"},
        {"What.*(are)?.*you.*doing", "Answering stupid questions"},
        {"(exit|bye|goodbye)", "Thanks, bye!"}
};

// На вход: текст пользователя (string question)
// На выход: текст ответа (string)
string bot_reply (string question) {
    // Цикл обработки вопросов и ответов
    // "для каждого элемента внутри questions_answers"
    string reply = "";
    for (auto item : questions_answers) {
        // item.first - вопрос (ключ)
        // item.second - ответ (значение)
        // if (item.first == question) {
        //    cout << item.second;
        // }
        // .* - означает любое кол-во любых символов
        // regex_constants::icase - регистр не имеет значения
        regex pattern = regex(".*" + item.first + ".*", regex_constants::icase);

        // Совпадает ли пользовательский текст с шаблоном?
        if (regex_match(question, pattern)) {
            reply += item.second + " "; // Ответ найден!
        }
    }
    // Если ответ пустой
    if (reply.length() == 0) {
        return "Can't understand you, cunt!";
    }
    else {
        return reply;
    }

}

int main()
{
    cout << "Hello, welcome to SkillChat C++ 5000\n";

    // cout - console out - выводить информацию на экран
    // cin - console in - получать информацию от пользователя
    string question;  // Пользовательский ввод

    regex exit_pattern = regex(".*(exit|bye).*", regex_constants::icase);
    // ! - "НЕ"
    while (!regex_match(question, exit_pattern)) {
        cout << "[USER]: ";
        getline(cin, question);  // Получить строчку текста из cin, и положить в question
        cout << "[BOT]: " << bot_reply(question) << endl; // Получить ответ и вывести на экран
    }

}