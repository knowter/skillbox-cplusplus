#include <iostream>
#include <string>
#include <map>
#include <regex>

using namespace std;

// База вопросов и ответов
map<string, string> questions_answer = {
        //Ключ значение
        {"Hello",              "Hi!"},
        {"How are you",        "Normalno!!!"},
        {"What is your name",  "My nameSkillChatC++ 5000"},
        {"What are you doing", "Answering stupid questions"},
};

// На вход: текст пользователя (string question)
// На выход: текст ответа (string)

string bot_reply(string question) {
    //Цикл обработки вопросов и ответова
    // для каждого элемента внутри questions_asnewers
    for (auto item: questions_answer) {
        // item.first - вопрос (ключ)
        // item.second - ответ (значение)

        // .* - означает любое кол-во любых символов
        auto pattern = regex(".*" + item.first + ".*", regex_constants::icase);

        // Совпадает ли пользовательский текст с шаблоном?
        if (regex_match(question, pattern)) {
            return item.second;
        }
    }
    return "Can't understand you. cunt!";
}

int main() {
    cout << "Hello, welcome to SkillChat C++ 5000";
    string question;
    while (question != "exit"){
            getline(cin, question);
            cout << bot_reply(question);
        }
    return 0;
}
