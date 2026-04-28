#include <iostream>
#include <ctype.h>
#include <iomanip>
#define N 1024
#define COL1_WIDTH 10
#define COL2_WIDTH 40
#define FILE_NAME "pochta_data_base.txt"
using namespace std;

struct fio {
    char* fam; 		//Фамилия 
    char* name;		// Имя 
};

struct date {
    int day; 	// день
    int month;	// месяц
    int year;	// год
};

date curent_date; // глобальная текущая дата

struct pochta {
    char* adr1; //адрес получателя 
    char* adr2; // адрес отправителя 
    fio fio1;
    fio fio2;
    date date;
    int control_days; // контрольное время вручения (в днях)
    double cost; // стоимость письма
};

//##############
void add_new(struct pochta** base, int* size);
void print_base(struct pochta* base, int size);
void print_letter(struct pochta* base, int i);
void find_letter_fam_name_transmit(struct pochta* base, int size);
double dateToUnixTimestamp(int year, int month, int day); // вспомогательная 
void find_expired_letters(struct pochta* base, int size); // для лабы 7
void find_letter_price(struct pochta* base, int size);
void sort_cost_low_first(struct pochta* base, int size);
void exit(struct pochta* base, int size);
//##############


int main() {
    setlocale(0, "Russian");

    struct pochta* base = NULL;
    int size = 0;
    char buff[N];

    FILE* fp;
    if (fopen_s(&fp, FILE_NAME, "r") != 0)
        // выводит строку символов с сообщением об ошибке
        perror("ошибка при открытии файла");
    else {
        cout << "open  OK!" << endl;

        while (fgets(buff, N, fp) != 0) {
            base = (pochta*)realloc(base, (size + 1) * sizeof(struct pochta));

            int len = 0;
            char* next_token;

            buff[strlen(buff)-1] = 0; // зануляем \n

            // strtok_s аналог split из питона
            char* token = strtok_s(buff, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].adr1 = (char*)malloc(len);
            strcpy_s(base[size].adr1, len, token);

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].adr2 = (char*)malloc(len);
            strcpy_s(base[size].adr2, len, token);

            //
            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].fio1.fam = (char*)malloc(len);
            strcpy_s(base[size].fio1.fam, len, token);

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].fio1.name = (char*)malloc(len);
            strcpy_s(base[size].fio1.name, len, token);
            //
          
            //
            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].fio2.fam = (char*)malloc(len);
            strcpy_s(base[size].fio2.fam, len, token);

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].fio2.name = (char*)malloc(len);
            strcpy_s(base[size].fio2.name, len, token);
            //

            //
            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].date.day = atoi(token); //преобразуем строку в int

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].date.month = atoi(token);

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].date.year = atoi(token);
            //

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].control_days = atoi(token);

            token = strtok_s(NULL, "\t", &next_token);
            len = strlen(token) + 1;
            base[size].cost = atof(token); //преобразуем строку в double

            size++;
        }
        fclose(fp);
    }

    cout << "Введите текущую дату (ДД.ММ.ГГГГ): " << endl;
    scanf_s("%d.%d.%d", &curent_date.day, &curent_date.month, &curent_date.year);
    getchar(); // Очищаем буфер ввода


    while (true) {
        cout << "***********************************************\n"
            << "\tБаза данных: Почта (ценное письмо)\t\n" << endl;
        //Печать меню
        cout << "1) Добавить новый элемент"
            << "\n2) Распечатать базу писем"
            << "\n3) Поиск письма по фамилии и имени отправителя"
            << "\n4) Поиск клиентов, которые вовремя не получили письмо"
            << "\n5) Найти все письма со стоимостью более заданного значения"
            << "\n6) Сортировать базу по убыванию стоимости"
            << "\n7) Выход из программы"
            << "\n***********************************************" << endl;

        int number;
        cout << "\nВведите номер функции: ";
        scanf_s("%d", &number);
        getchar();

        // Выбор позиции меню
        switch (number) {
        case 1:
            add_new(&base, &size);
            break;

        case 2:
            print_base(base, size);
            break;

        case 3:
            find_letter_fam_name_transmit(base, size);
            break;

        case 4:
            find_expired_letters(base, size);
            break;

        case 5:
            find_letter_price(base, size);
            break;

        case 6:
            sort_cost_low_first(base, size);
            break;

        case 7:
            exit(base, size);
            return 0;

        default:
            cout << "Неверный ввод!!\n" << endl;
        }
    }
    return 0;
}



//#########################################
void add_new(struct pochta** base, int* size) {
    struct pochta* new_base = NULL;
    char buff[N];

    new_base = (pochta*)realloc(*base, (*size + 1) * sizeof(struct pochta));
    if (new_base == NULL) {
        cout << "Ошибка!" << endl;
        return;
    }
    *base = new_base;

    cout << "Адресс получателя: ";
    gets_s(buff, N); // вводстроки cклавиатуры в переменную buff
    int len = strlen(buff) + 1;
    (*base)[*size].adr1 = (char*)malloc(len); //выделить динамическую память под строку
    strcpy_s((*base)[*size].adr1, len, buff); // копировать buff в динамическую память

    cout << "Фамилия получателя: ";
    gets_s(buff, N);
    len = strlen(buff) + 1;
    (*base)[*size].fio1.fam = (char*)malloc(len);
    strcpy_s((*base)[*size].fio1.fam, len, buff);

    cout << "Имя получателя: ";
    gets_s(buff, N);
    len = strlen(buff) + 1;
    (*base)[*size].fio1.name = (char*)malloc(len);
    strcpy_s((*base)[*size].fio1.name, len, buff);

    cout << "Адресс отправителя: ";
    gets_s(buff, N);
    len = strlen(buff) + 1;
    (*base)[*size].adr2 = (char*)malloc(len);
    strcpy_s((*base)[*size].adr2, len, buff);

    cout << "Фамилия отправителя: ";
    gets_s(buff, N);
    len = strlen(buff) + 1;
    (*base)[*size].fio2.fam = (char*)malloc(len);
    strcpy_s((*base)[*size].fio2.fam, len, buff);

    cout << "Имя отправителя: ";
    gets_s(buff, N);
    len = strlen(buff) + 1;
    (*base)[*size].fio2.name = (char*)malloc(len);
    strcpy_s((*base)[*size].fio2.name, len, buff);

    cout << "Стоимость письма: ";
    cin >> (*base)[*size].cost;

    cout << "Дата отправки письма (ДД.ММ.ГГГГ): ";
    scanf_s("%d.%d.%d", &(*base)[*size].date.day, &(*base)[*size].date.month, &(*base)[*size].date.year);
    getchar(); // Очищаем буфер ввода

    cout << "Контрольное время вручения (в днях): ";
    cin >> (*base)[*size].control_days;

    (*size)++;
    cout << "Добавлено письмо\n\n" << endl;
}


void print_letter(struct pochta* base, int i) {
    cout << '|' << setw(COL1_WIDTH) << setfill(' ') << left << "Адрес" << '|' << setw(COL2_WIDTH) << left << base[i].adr1 << '|' << setw(COL2_WIDTH) << base[i].adr2 << '|' << endl;
    cout << '|' << setw(COL1_WIDTH) << left << "Фамилия" << '|' << setw(COL2_WIDTH) << left << base[i].fio1.fam << '|' << setw(COL2_WIDTH) << left << base[i].fio2.fam << '|' << endl;
    cout << '|' << setw(COL1_WIDTH) << left << "Имя" << '|' << setw(COL2_WIDTH) << left << base[i].fio1.name << '|' << setw(COL2_WIDTH) << left << base[i].fio2.name << '|' << endl;
    cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;
    cout << '|' << setw(COL1_WIDTH) << setfill(' ') << left << "Цена (руб)" << '|' << setw(COL2_WIDTH * 2 + 1) << left << base[i].cost << '|' << endl;

    char buff[N];
    sprintf_s(buff, N, "%02d.%02d.%04d (контрольное время вручения %d дней)", base[i].date.day, base[i].date.month, base[i].date.year, base[i].control_days); // вывод даты в буфер
    cout << '|' << setw(COL1_WIDTH) << setfill(' ') << left << "Дата отпр" << '|' << setw(COL2_WIDTH * 2 + 1) << left << buff << '|' << endl;
    cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;
}


void print_base(struct pochta* base, int size) {
    if (size == 0) {
        printf("База данных пуста!\n");
        return;
    }
    cout << "\n\t\t\tБаза данных: Почта (ценное письмо)\t" << endl;
    cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;
    cout << '|' << setw(COL1_WIDTH) << setfill(' ') << " " << '|' << setw(COL2_WIDTH) << left << "Получатель" << '|' << setw(COL2_WIDTH) << left << "Отправитель" << '|' << endl;
    cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;

    for (int i = 0; i < size; i++) {
        print_letter(base, i);
    }
    printf("\nКоличество записей в базе: %d\n\n", size);
}


void find_letter_fam_name_transmit(struct pochta* base, int size) {
    if (size == 0) {
        printf("База данных пуста!\n");
        return;
    }

    char buff_name[N];
    char buff_fam[N];

    cout << "Ведите фамилию отправителя: ";
    gets_s(buff_fam, N); // вводстроки cклавиатуры в переменную buff

    cout << "Ведите имя отправителя: ";
    gets_s(buff_name, N); // вводстроки cклавиатуры в переменную buff

    for (int i = 0; i < size; i++) {
        if (strcmp(buff_name, base[i].fio2.name) == 0 && strcmp(buff_fam, base[i].fio2.fam) == 0) {
            cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;
            print_letter(base, i);
            // TO DO если только первое письмо то ставим break;
            return;
        }
    }
    cout << "\nПисьма с задаными данными не найдены!\n" << endl;
}


double dateToUnixTimestamp(int day, int month, int year) { // переводим дату в юникс (кол-во секунд до даты)
    tm t{};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    time_t tt = mktime(&t);
    return static_cast < double > (tt);
}


void find_expired_letters(struct pochta* base, int size) {
    if (size == 0) {
        printf("База данных пуста!\n");
        return;
    }

    int flag = 0;
    for (int i = 0; i < size; i++) {
        if (dateToUnixTimestamp(curent_date.day, curent_date.month, curent_date.year) - dateToUnixTimestamp(base[i].date.day, base[i].date.month, base[i].date.year) > base[i].control_days * 24 * 60 * 60) {
            cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;
            print_letter(base, i);
            flag++;
        }
    }
    if (!flag)
        cout << "\nПисьма с задаными данными не найдены!\n" << endl;
    else
        printf("\nКоличество найденых записей в базе: %d\n\n", flag);
}


void find_letter_price(struct pochta* base, int size) {
    if (size == 0) {
        printf("База данных пуста!\n");
        return;
    }

    double price;
    int flag = 0;

    cout << "Ведите стоимость письма: ";
    cin >> price;

    for (int i = 0; i < size; i++) {
        if (price < base[i].cost) {
            cout << '+' << setw(COL1_WIDTH + COL2_WIDTH * 2 + 2) << setfill('=') << "=" << '+' << endl;
            print_letter(base, i);
            flag++;
        }
    }
    if (flag == 0) {
        cout << "\nПисьма с задаными данными не найдены!\n" << endl;
    }
}


void sort_cost_low_first(struct pochta* base, int size) {
    if (size == 0) {
        printf("База данных пуста!\n");
        return;
    }

    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (base[j].cost < base[j + 1].cost) {
                struct pochta holder = base[j];
                base[j] = base[j + 1];
                base[j + 1] = holder;
            }
        }
    }
    cout << "База отсортирована по убыванию стоимости!\n" << endl;
}


void exit(struct pochta* base, int size) {
    char buff[N];
    FILE* fp;
    if (fopen_s(&fp, FILE_NAME, "w") != 0) // открываем файл на запись - w
        perror("Ошибка при открытии файла");
    else {
        cout << "open  OK!" << endl;

        for (int i = 0; i < size; i++) {
            sprintf_s(buff, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%f\n",
                base[i].adr1, base[i].adr2,
                base[i].fio1.fam, base[i].fio1.name,
                base[i].fio2.fam, base[i].fio2.name,
                base[i].date.day, base[i].date.month, base[i].date.year,
                base[i].control_days, base[i].cost);
            fputs(buff, fp);
        }
        fclose(fp);
    }

    for (int i = 0; i < size; i++) {
        free(base[i].adr1);
        free(base[i].adr2);
        free(base[i].fio1.fam);
        free(base[i].fio2.fam);
        free(base[i].fio1.name);
        free(base[i].fio2.name);
    }
    free(base);
}
//#########################################