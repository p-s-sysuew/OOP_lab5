#include "include/functions.h"

#include <iostream>
#include <string>


// ПРОИЗВОЛЬНЫЙ СЛОЖНЫЙ ТИП
struct Person {
    int id;
    std::string name;

    Person(int i, std::string n) : id(i), name(std::move(n)) {}
};

int main() {


    // Создаём память размером 256 КБ
    FixedBlockResource memory(256 * 1024);

    // Создаём контейнеры
    PmrList<int> intList(&memory);
    PmrList<Person> personList(&memory);

    std::cout << "Добавляем числа:\n";
    for (int i = 1; i <= 5; ++i)
        intList.push_back(i * 10);

    std::cout << "Список чисел: ";
    for (auto x : intList)
        std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\nДобавление в начало: 5\n";
    intList.push_front(5);

    std::cout << "Список чисел: ";
    for (auto x : intList)
        std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\nУдаляем последний элемент\n";
    intList.pop_back();

    std::cout << "Список чисел: ";
    for (auto x : intList)
        std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\nРазмер списка: " << intList.size() << "\n";

    ///////////////////////////////////////////////////////////////////////
    //                Демонстрация для struct
    ///////////////////////////////////////////////////////////////////////

    std::cout << "\nДобавляем пользователей:\n";

    personList.push_back(Person(1, "Павел"));
    personList.push_back(Person(2, "Анна"));
    personList.push_front(Person(0, "Администратор"));

    for (auto& p : personList) {
        std::cout << "ID: " << p.id
                  << " | Имя: " << p.name << "\n";
    }

    // Очистка
    intList.clear();
    personList.clear();

    std::cout << "\nПрограмма завершена.\n";
    return 0;
}
