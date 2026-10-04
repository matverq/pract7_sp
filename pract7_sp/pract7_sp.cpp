#define DEBUG_MODE
#define FILE_NAME "clients.txt"

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;

struct Client {
    string name;
    int number;
    string type;
    int bought;
    int used;

    int ostatok() { return bought - used; }
};

int main() {
    setlocale(LC_ALL, "Russian");

    const int MAX = 100;
    Client clients[MAX];
    int count = 0;

    try {
        ifstream in(FILE_NAME);
        if (!in.is_open()) {
            throw invalid_argument("файл не найден или недоступен");
        }

        string line;
        int lineNo = 0;
        while (getline(in, line)) {
            lineNo++;
            if (line == "") continue;
            if (count >= MAX) {
                cout << "Достигнут лимит записей " << MAX << ", остальные строки пропущены\n";
                break;
            }
            try {
                int pos1 = line.find(';');
                int pos2 = line.find(';', pos1 + 1);
                int pos3 = line.find(';', pos2 + 1);
                int pos4 = line.find(';', pos3 + 1);

                if (pos1 == string::npos || pos2 == string::npos ||
                    pos3 == string::npos || pos4 == string::npos) {
                    throw invalid_argument("недостаточно полей в строке");
                }

                Client c;
                c.name = (line.substr(0, pos1));
                c.number = stoi((line.substr(pos1 + 1, pos2 - pos1 - 1)));
                c.type = (line.substr(pos2 + 1, pos3 - pos2 - 1));
                c.bought = stoi((line.substr(pos3 + 1, pos4 - pos3 - 1)));
                c.used = stoi((line.substr(pos4 + 1)));

                if (c.name == "") throw invalid_argument("пустое имя клиента");
                if (c.number <= 0 || c.number > 999999) throw invalid_argument("номер абонемента вне диапазона");
                if (c.bought < 0 || c.bought > 10000) throw invalid_argument("количество купленных посещений вне диапазона");
                if (c.used < 0 || c.used > 10000) throw invalid_argument("количество использованных посещений вне диапазона");
                if (c.used > c.bought) throw invalid_argument("использовано больше, чем куплено");
                clients[count] = c;
                count++;
            }
            catch (exception e) {
                cout << "Строка " << lineNo << " пропущена: " << e.what() << "\n";
            }
        }
        in.close();

#ifdef DEBUG_MODE
        cout << "Загружено записей: " << count << "\n";
#endif
        if (count == 0) {
            cout << "список клиентов пуст\n";
        }
    }
    catch (exception e) {
        cout << "Ошибка загрузки: " << e.what() << "\n";
        cout << "Список пуст.\n";
    }

    int menu = -1;
    while (menu != 0) {
        cout << "\nМеню:\n";
        cout << "1. Поиск клиента\n";
        cout << "2. Просмотр оставшихся посещений\n";
        cout << "3. Регистрация посещения\n";
        cout << "4. Вывод абонементов без посещений\n";
        cout << "5. Продление абонемента\n";
        cout << "6. Сохранить изменения в файл\n";
        cout << "0. Выход\n";
        cout << "\nВведите пункт: ";

        string input;
        cin >> input;
        bool isNumber = true;
        for (int i = 0; i < input.length(); i++) {
            if (input[i] < '0' || input[i] > '9') isNumber = false;
        }
        if (!isNumber) {
            cout << "Введен неподходящий символ, введите число\n";
            menu = -1;
            continue;
        }
        menu = stoi(input);

        if (menu == 1 || menu == 2 || menu == 3 || menu == 5) {
            isNumber = true;
            cout << "Введите номер абонемента: ";
            cin >> input;
            for (int i = 0; i < input.length(); i++) {
                if (input[i] < '0' || input[i] > '9') isNumber = false;
            }
            if (!isNumber) {
                cout << "Введен неподходящий символ, введите число\n";
                continue;
            }
            int num = stoi(input);
            int index = -1;
            for (int i = 0; i < count; i++) {
                if (clients[i].number == num) {
                    index = i;
                    break;
                }
            }

            if (index == -1) {
                cout << "Клиент не найден.\n";
                continue;
            }

            if (menu == 1) {
                cout << "Имя: " << clients[index].name << "\n";
                cout << "Тип: " << clients[index].type << "\n";
                cout << "Осталось: " << clients[index].ostatok() << "\n";
            }
            else if (menu == 2) {
                cout << "Осталось посещений: " << clients[index].ostatok() << "\n";
            }
            else if (menu == 3) {
                try {
                    if (clients[index].ostatok() <= 0) {
                        throw invalid_argument("у клиента не осталось посещений");
                    }
                    clients[index].used++;
                    cout << "Посещение зарегистрировано, осталось: " << clients[index].ostatok() << "\n";
                }
                catch (exception e) {
                    cout << "Ошибка: " << e.what() << "\n";
                }
            }
            else if (menu == 5) {
                isNumber = true;
                cout << "Сколько посещений добавить? ";
                cin >> input;
                for (int i = 0; i < input.length(); i++) {
                    if (input[i] < '0' || input[i] > '9') isNumber = false;
                }
                if (!isNumber) {
                    cout << "Введен неподходящий символ, введите число\n";
                    continue;
                }
                int extra = stoi(input);
                if (extra <= 0) {
                    cout << "Число должно быть больше 0\n";
                }
                else if (clients[index].bought + extra > 10000) {
                    cout << "Превышен максимум посещений\n";
                }
                else {
                    clients[index].bought += extra;
                    cout << "Абонемент продлен, осталось: " << clients[index].ostatok() << "\n";
                }
            }
        }
        else if (menu == 4) {
            bool found = false;
            for (int i = 0; i < count; i++) {
                if (clients[i].ostatok() == 0) {
                    cout << clients[i].name << " номер " << clients[i].number << "\n";
                    found = true;
                }
            }
            if (!found) cout << "Исчерпанных абонементов нет\n";
        }
        else if (menu == 6) {
            try {
                ofstream out(FILE_NAME);
                if (!out.is_open()) throw invalid_argument("не удалось открыть файл для записи");

                for (int i = 0; i < count; i++) {
                    out << clients[i].name << ";" << clients[i].number << ";"
                        << clients[i].type << ";" << clients[i].bought << ";" << clients[i].used << "\n";
                }
                out.close();
#ifdef DEBUG_MODE
                cout << "Сохранено записей: " << count << "\n";
#endif
                cout << "Данные сохранены в файл\n";
            }
            catch (exception e) {
                cout << "Ошибка сохранения: " << e.what() << "\n";
            }
        }
        else if (menu == 0) {
            cout << "Выход из программы\n";
        }
        else {
            cout << "Такого пункта нет\n";
        }
    }
}