#include <stdio.h>
#include <iostream>
#include <utility>
#include <string>
#include <cctype>
#include <cstring> // strstr
#include <map> /* Ассоциативные массивы */
#include <array>
#include <ctime>
#include <algorithm> // Количество символов в строке
#include <bitset> // Перевод десятичных чисел к двоичному виду
#include <boost/dynamic_bitset.hpp>
#include <cmath> // Возведение в степень
#include <unistd.h> // sleep
//#include <sqlite3.h>
#include <sys/file.h> // umask
#include <cstdlib>

#include "inc/csv.h"

#include "inc/json.hpp"
#include <random>
#include <sys/time.h>
#include "inc/md5.h"
#include <regex> // Регулярные выражения замена символов во входящей строке

#include <iomanip>
#include <ctime> // std::put_time Функции времени
#include <sys/time.h> // timeval Функции времени
#include <experimental/filesystem> // Права доступа к файлу
#include <string.h> // strpos
#include <thread>
#include <mutex>
#include <future> // Асинхноррное выполение потоков
//#include <vector>
//#include <cctype>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <stack>

//#include <unistd.h>
//#include <termios.h> // Функции нажатий клавиш консоли
//#include <curses.h>
//#include <curses.h>
//#include <ncurses.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <termios.h>
#include <memory>

//#include <db_cxx.h>
//#include <dbstl_map.h>
#include <pthread.h>
#include <unistd.h>

#include <mariadb/mysql.h>
#include <hiredis/hiredis.h>

//#define CL_HPP_MINIMUM_OPENCL_VERSION 110
//#define CL_HPP_TARGET_OPENCL_VERSION 210
//#define CL_HPP_CL_1_2_DEFAULT_BUILD
//	#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
//#include "CL/cl2.hpp"

#include "version.txt"

#define pre(x) printf("%d.%s\n", __LINE__, x);
#define err(x) printf("%d.\x1b[37;41m ОШИБКА \x1b[0m %s\n", __LINE__, x); exit(1); // \x1b[37;41m \x1b[0m

//using json = nlohmann::json; json in;

using namespace std; //setlocale(LC_ALL, "ru_RU");
namespace fs = std::experimental::filesystem;


//struct comp{ bool operator()(const std::string& lhs, const std::string& rhs) const{ size_t ls = lhs.size(); size_t rs = rhs.size(); return std::tie(ls, lhs) > std::tie(rs, rhs); }};

typedef std::map<string ,string> TMs;
typedef std::map<int ,string> TMi;
typedef std::map<string ,TMs> TMMs;
typedef std::map<int ,TMs> TMMi;
typedef std::map<string ,TMMi> TM3i;
typedef std::map<string ,TMMs> TM3s;
typedef std::map<string ,boost::dynamic_bitset<>> TMMb;
typedef std::map<string ,TMMb> TM3b;

//typedef dbstl::db_map<string, TMs> TMMt;
//typedef std::map<std::string, TMMt*> TMMg;

std::recursive_mutex mu;

#include "bmf.c"


TM3i BMF_INDEX_EX, BMF_DANO_EX, BMF_ITOG_EX;
TMMi BMF_CLUMP, DATABASES;//, BMF_INDEX, BMF_DANO, _BMF_DANO, _BMF_ITOG, BMF_ITOG;
TM3i BMF_DANO_VALUES_EX, BMF_ITOG_VALUES_EX; // Значения позиций морфов
TM3i BMF_DANO_TITLES_EX, BMF_ITOG_TITLES_EX; // Списки справочников
TM3i BMF_DATASET_EX; // Список наборов данных

namespace bmf{ // Глобальные переменные
	int loop = 0; // Количество итераций скрипта
	string clump_id = ""; // Скопление если не указано то данные из php кода
	TMs bmf_clump; // Текущее скопление
	TMs dataset; // Текущее скопление
	int values_length = 1024; // Ограничение размера истории
	int change_sum = 0; // Счетчик расчетов
	int size_max = 2e9; // Максимально допустимое количество морфов в расчете
	float perc = 0; // Процент правильных прогнозов в примере
	float pips_perc = -1; // Процент правильных прогнозов в бите
	float pips_first = -1; // Процент правильных прогнозов в бите
	float pips_last = -1; // Процент правильных прогнозов в бите

//	TMs CACHE; // Кеш результатов расчета
//	TMs COUNT; // Кеш результатов расчета
//	TMs CALCULATE; // Список формул для быстрых расчетов для каждого из итогов
//	std::map<int, boost::dynamic_bitset<>> DANO; // Кеш результатов исходников
//	std::map<int, boost::dynamic_bitset<>> ITOG; // Кеш результатов расчета
//	std::map<int, boost::dynamic_bitset<>> MAPS; // Список битовых результатов расчета для каждого морфа


	TMs databases; // Список баз данных
	int group_level = 16; // Уровень групп кешей
	MYSQL *mysql = NULL; // БД mysql
	redisContext *redis = NULL; // БД редис

	string dbtype; //Тип БД
	string dbname; //Имя БД
	string dbuser = "root"; //Пользователь БД
	string dbpassword = ""; //Пароль БД
	string dbhost; //Хост БД
	int dbport; //Порт БД

	TMs ARGV; // Массив параметров консоли
	int timestamp =  time(0); // Расчет выполнения участков кода
	auto microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9;

//	DbEnv *env; //Окружение БД
//	TMMt *TEST; // Справочнк морфов

	std::string INDEX = "index"; // Справочнк морфов
	std::string INDEX_GROUP = "index_group"; // Промежуточные значения
	std::string INDEX_SHIFT = "index_shift"; // Количество изменений
	std::string DATASET = "dataset"; // Наборы данных
	std::string DATASET_MAP = "dataset_map"; // Карты
	std::string DANO = "dano"; // Исходники
	std::string DANO_VALUES = "dano_values"; // Значения исходников
	std::string DANO_TITLES = "dano_titles"; // Заголовки исходников
	std::string ITOG = "itog"; // Итоги
	std::string ITOG_VALUES = "itog_values"; // Значения итогов
	std::string ITOG_TITLES = "itog_titles"; // Заголовки итогов

	std::function<std::string(TMs)> Calculate; // Расчет строки для расчета польской нотации
	std::function<std::string(std::string,TM3i&)> Calc; // Расчет строки польской нотации
	std::function<TMs(TMs)> Value; // История морфа
	std::function<TMs(string,int,string,TMMs&,TMMb&,int)> Choice; // Расчет истории
	std::function<double(string)> Bin2dec; // Перерасчет размерности из двоичной в десятеричную
	std::function<string(double)> Dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
	std::function<string(TMs,string,int)> Learn; // Рерасчет морфа

	std::function<void(string,float,int)> Progress; // Отрисовка полосы выполнения
	std::function<TMs(TMs&,std::string,int)> Maps; // Расчет карт результатов

//	std::function<Db*(string)> Open; // Список баз данных
	std::function<bool(int)> Close; // Закрытие соединения с БД
	std::function<TMMs(TMs,string,int,string,string)> Vals; // Обучение
	std::function<TMs(string,TMs,TMs,TMs,int)> Up; // Обучение
	std::function<TMs(string,TMs,TMs,TMs,int)> Up_mysql; // Обучение
	std::function<TMs(string,TMs,TMs,TMs,int)> Up_redis; // Обучение
	std::function<TMMs(string,TMs,int)> List; // Список выборки
	std::function<TMMs(string,TMs,string,int)> List_mysql; // Список выборки
	std::function<TMMs(string,TMs,int)> List_redis; // Список выборки
	std::function<TMMs(string,TMMs&,int)> Scan; // Список выборки
	std::function<TMMs(string,TMMs&,int)> Scan_mysql; // Список выборки
	std::function<TMMs(string,TMMs&,int)> Scan_redis; // Список выборки
	std::function<string(string)> Id; // Закрытие соединения с БД
	//std::function<TMMs(TMs,int)> Shift; // Список изменений
	//std::function<TMMs(TMs,TMMs&,int)> Shift_mysql; // Список изменений
	//std::function<TMMs(TMs,TMMs&,int)> Shift_redis; // Список изменений
	//std::function<TMMs(TMs,int)> Group; // Список групп
	//std::function<TMMs(TMs,TMMs&,int)> Group_mysql; // Список групп
	//std::function<TMMs(TMs,TMMs&,int)> Group_redis; // Список групп
	std::function<string(TMs&,int,TMMs&,TMMs&,TMMb&,TMMb&)> Grp; // РРасчет группы
}

int main(int argc, char **argv){
	if(setlocale(LC_ALL, "LC_ALL=C.UTF-8"); false){ mpre("ОШИБКА установки кирилицы", __LINE__);
	}else if(bmf::ARGV = [&](TMs ARGV = {}){ for(int i = 0; i < argc; i++){ // Перебор всех параметров
		if(0 >= i){ //mpre("Имя файла", __LINE__);
		}else if(string str = argv[i]; (0 >= str.length())){ mpre("ОШИБКА получения аргумента консоли", __LINE__);
		}else if(int npos = str.find("="); false){ mpre("Расчет позиции равно в строке аргументов", __LINE__);
		}else if(std::string arg = (std::string::npos == npos ? str : str.substr(0, npos)); false){ mpre("ОШИБКА получения имени аргумента", __LINE__);
		}else if(std::string val = (std::string::npos == npos ? "" : str.substr(npos +1, str.length())); false){ mpre("ОШИБКА получения значения аргумента", __LINE__);
		}else if([&](bool skip = false ){ // Параметры консоли
			if(std::string::npos != npos){ //mpre("Задан аргумент", __LINE__);
			}else if("-" != arg){ //mpre("Не признак загрузки", __LINE__);
			}else if(ARGV.insert(make_pair("-", "")); ARGV.empty()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
			}else if(!(skip = true)){ mpre("Установка значения пропуска", __LINE__);
			}return skip; }()){ // Признак загрузки
		}else if([&](bool skip = false){ // Установка БД
			if(std::string::npos != npos){ //mpre("Задан аргумент", __LINE__);
			}else if(ARGV.end() != ARGV.find("db")){ //mpre("ОШИБКА дублирование аргументов командной строки `" +ARGV.at("db") + "` " +arg, __LINE__);
			}else if(ARGV.insert(make_pair("db", arg)); ARGV.empty()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
			}else if(!(skip = true)){ mpre("Установка значения пропуска", __LINE__);
			}else{ //mpre("Установка БД " +arg, __LINE__);
			}return skip; }()){ //mpre("ОШИБКА установки БД", __LINE__);
		}else if(ARGV.insert(make_pair(arg, val)); ARGV.empty()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
		}else{ //mpre(ARGV ,"Аргументы консоли "+ arg + "=" +val , __LINE__);
		}}return ARGV; }(); bmf::ARGV.empty()){ err("Параметры консоли");
	}else if(nlohmann::json in = [&](nlohmann::json in = {}){ // Входной поток
		if(bmf::ARGV.end() == bmf::ARGV.find("-")){ //mpre("Данные для обучения не установлены", __LINE__);
		}else if([&](string str = ""){ while(getline(std::cin, str)){ bmf::ARGV.at("-") += ("\n"+ str); }; return (0 >= bmf::ARGV.at("-").length()); }()){ mpre("ОШИБКА входящий параметр не задан", __LINE__);
		}else if(in = nlohmann::json::parse(bmf::ARGV.at("-")); false){ mpre("ОШИБКА Входящий параметр с обучающими данными пуст", __LINE__);
		}else{ //mpre("Данные из потока std::cin "+ in.dump(), __LINE__);
		}return in; }(); false){ mpre("ОШИБКА получения входного потока", __LINE__);
	}else if([&](bool skip = false){ // Остановки из консоли
		if([&](bool skip = false){ // Версия программы
			if(bmf::ARGV.end() == bmf::ARGV.find("-v")){ //mpre("Пропускаем отображение версии", __LINE__);
			}else if(skip = !skip; false){ mpre("Условия выхода", __LINE__);
			}else{ std::cout << endl;
				std::cout << "bimorph " << version << endl;
				std::cout << "Copyright (C) 2021 биморф.рф" << endl;
				std::cout << "Нет НИКАКИХ ГАРАНТИЙ до степени, разрешённой законом." << endl << endl;
				std::cout << "Лицензия freemium https://ru.wikipedia.org/wiki/Freemium" << endl;
				std::cout << "Данная версия является условной бесплатной с некоторыми граничениями" << endl;
				std::cout << "Морф — наименьшая осмысленная единица языка (логики)" << endl << endl;

				std::cout << "Авторы программы -- Кривошлыков Евгений Павлович +79618063797" << endl << endl;
			}return skip; }()){ exit(mpre("Информация о версии", __LINE__));
		}else if([&](){ // Раздел помощи
			if(bmf::ARGV.end() == bmf::ARGV.find("-h")){ return false; //mpre("Пропускаем раздел помощи", __LINE__);
			}else if(string file = (1 <= argc ? argv[0] : "./bimorph"); (0 >= file.length())){ mpre("ОШИБКА получения имени файла", __LINE__);
			}else if(skip = !skip; false){ mpre("Условие выхода", __LINE__);
			}else{ std::cout << endl;
				std::cout << "Использование: "+ file+ " [КЛЮЧИ]… [SQLITE ФАЙЛ]… [КЛЮЧИ]…" << endl;
				std::cout << "epoch		количество эпох выполнения при полном совпадении процесс обучения останавливается" << endl;
				std::cout << "db		файл sqlite базы данных в которых хранится модель" << endl;
				std::cout << "itog		номер итога для обучения" << endl;
				std::cout << "-v		показать информацию о версии" << endl;
				std::cout << "-h		показать эту справку и выйти" << endl;
				std::cout << "dano		параметры расчета в формате json при пустом значении берет из стандартного ввода -" << endl;
				//std::cout << "-thread		количество потоков обучения" << endl;
				//std::cout << "-tt	установка тестовых данных из выборки" << endl;
				std::cout << "-c		обнулить результаты предыдущих обучений" << endl;
				std::cout << "-a		отображение атрибутов параметров консоли" << endl << endl;

				std::cout << "Пример загрузки набора данных $ cat iris.json | "+ file+ " iris -" << endl;
				std::cout << "Пример обучения набора данных $ "+ file+ " iris ds=1 epoch=10" << endl;
				std::cout << "Пример расчета $"+ file+ " iris dano=[{\"dano\":{\"Параметр_0\":\"0\",\"Параметр_1\":\"1\"}}]" << endl << endl;

				std::cout << "Об ошибках «биморф» сообщайте по адресу: <bimorph@mpak.su>" << endl;
				std::cout << "Дополнительная инморфация: http://биморф.рф/" << endl << endl;
			} return true; return false; }()){ mpre("Раздел помощи", __LINE__);
		}else if([&](){ // Отображение атрибутов командной строки
			if(bmf::ARGV.end() == bmf::ARGV.find("-a")){ return false; //mpre("Пропускаем отображение версии", __LINE__);
			}else{ mpre(bmf::ARGV, __LINE__, "Атрибуты командной строки");
			} return true; }()){ mpre("Атрибуты параметров командной строки", __LINE__);
		}else{
		}return skip; }()){ exit(mpre("Остановка выполнения", __LINE__));
	}else if(bmf::mysql = [&](MYSQL *mysql = NULL){ // Подключение БД
		if(std::string db = [&](std::string db = ""){ // Имя базы данных
			if(bmf::ARGV.end() == bmf::ARGV.find("db")){ mpre("ОШИБКА БД для сохранения не задана -db", __LINE__);
			}else if(0 >= bmf::ARGV.at("db").length()){ mpre("База данных для сохранения не указана", __LINE__);
			}else if(db = bmf::ARGV.at("db"); db.empty()){ mpre("ОШИБКА имя файла для БД не задано", __LINE__);
			}else{ //mpre("База данных "+ bmf::dbname, __LINE__);
			}return db; }(); db.empty()){ err("Строка подключения не установлена");
		}else if(bmf::dbtype = [&](std::string type = ""){ // Получение БД
			if(size_t npos = db.find_first_of("://"); (std::string::npos == npos)){ err("Тип БД не задан");
			}else if(type = db.substr(0, npos); type.empty()){ err("Выборка типа БД");
			}else if(db = db.substr(npos+3, db.length()); db.empty()){ err("Остаток атрибутов подключения");
			}else{ //mpre("База данных type=" +type +" db=" +db +" npos=" +to_string(npos), __LINE__);
			}return type; }(); bmf::dbtype.empty()){ err("Тип БД не задан");
		}else if(std::string auth = [&](std::string auth = ""){ // Получение БД
			if(size_t npos = db.find_first_of("@"); (std::string::npos == npos)){ //mpre("Аутентификация не задана", __LINE__);
			}else if(auth = db.substr(0, npos); auth.empty()){ err("Выборка аутентификации БД");
			}else if(db = db.substr(npos+1, db.length()); db.empty()){ err("Остаток атрибутов подключения");
			}else{ //mpre("База данных auth=" +auth +" db=" +db +" npos=" +to_string(npos), __LINE__);
			}return auth; }(); false){ err("Тип БД не задан");
		}else if(bmf::dbuser = [&](std::string dbuser){ // Пользователь БД
			if(auth.empty()){ //mpre("Аутентификация не задана", __LINE__);
			}else if(size_t npos = auth.find_first_of(":"); (std::string::npos == npos)){ //mpre("Аутентификация не задана", __LINE__);
			}else if(dbuser = auth.substr(0, npos); dbuser.empty()){ err("Остаток атрибутов подключения");
			}else{ //mpre("База данных dbuser=" +dbuser +" auth=" +auth +" npos=" +to_string(npos), __LINE__);
			}return dbuser; }(bmf::dbuser); false){ err("Получение пользователя БД");
		}else if(bmf::dbpassword = [&](std::string dbpassword){ // Пользователь БД
			if(auth.empty()){ //mpre("Аутентификация не задана", __LINE__);
			}else if(size_t npos = auth.find_first_of(":"); (std::string::npos == npos)){ //mpre("Аутентификация не задана", __LINE__);
			}else if(dbpassword = auth.substr(npos+1, auth.length()); false){ err("Остаток атрибутов подключения");
			}else{ //mpre("База данных dbpassword=" +dbpassword +" auth=" +auth +" npos=" +to_string(npos), __LINE__);
			}return dbpassword; }(bmf::dbpassword); false){ err("Получение пароля БД");
		}else if(std::string host = [&](std::string host = ""){ // Получение БД
			if(size_t npos = db.find_first_of("/"); (std::string::npos == npos)){ //mpre("Аутентификация не задана", __LINE__);
			}else if(host = db.substr(0, npos); host.empty()){ err("Выборка аутентификации БД");
			}else if(db = db.substr(npos+1, db.length()); db.empty()){ err("База данных не указана");
			}else{ //mpre("База данных host=" +host +" db=" +db +" npos=" +to_string(npos), __LINE__);
			}return host; }(); host.empty()){ err("Хост БД не задан");
		}else if(bmf::dbhost = [&](std::string dbhost){ // Хост БД
			if(size_t npos = host.find_first_of(":"); (std::string::npos == npos)){ //mpre("Порт не задан", __LINE__);
			}else if(dbhost = host.substr(0, npos); dbhost.empty()){ err("Остаток атрибутов подключения");
			}else{ //mpre("База данных dbhost=" +dbhost +" host=" +host +" npos=" +to_string(npos), __LINE__);
			}return dbhost; }(host); bmf::dbhost.empty()){ err("Получение пользователя БД");
		}else if(bmf::dbport = [&](int dbport = 0){ // Хост БД
			if(size_t npos = host.find_first_of(":"); false){ //mpre("Порт не задан", __LINE__);
			}else if([&](){ // Значение порта по умолчанию	
				if(std::string::npos != npos){ pre("Порт задан настройками");
				}else if("mysql" == bmf::dbtype){ dbport = 3306;
				}else if("redis" == bmf::dbtype){ dbport = 6379;
				}else{ mpre("Порт для типа " +bmf::dbtype +" данных не задан", __LINE__);
				}return (std::string::npos == npos); }()){ //pre("Порт не задан. Устанавливаем по умолчанию");
			}else if(std::string _dport = host.substr(npos+1, host.length()); _dport.empty()){ err("Выборка порта");
			}else if(dbport = atoi(_dport.c_str()); !dbport){ mpre("host=" +host +" _dport=" +_dport, __LINE__); err("Остаток атрибутов подключения");
			}else{ //mpre("База данных dbport=" +std::to_string(dbport) +" host=" +host +" npos=" +to_string(npos), __LINE__);
			}return dbport; }(); !bmf::dbport){ err("Получение порта БД");
		}else if(bmf::dbname = db; bmf::dbname.empty()){ err("Имя БД не задано");
		}else if([&](){ // Подключение к БД mysql
			if("mysql" != bmf::dbtype){ //mpre("Используем не mysql", __LINE__);
			}else if(bmf::dbname.empty()){ err("Имя БД не задано");
			}else if(mysql = mysql_init(NULL); (nullptr == mysql)){ err("ОШИБКА создания соединения");
			}else if(mysql = mysql_real_connect(mysql, bmf::dbhost.c_str(), bmf::dbuser.c_str(), bmf::dbpassword.c_str(), bmf::dbname.c_str(), bmf::dbport, NULL, 0); !mysql){ mpre("Параметры подключения dbhost=" +bmf::dbhost +" dbport=" +to_string(bmf::dbport) +" dbuser=" +bmf::dbuser +" dbpassword=" +bmf::dbpassword +" database=" +bmf::dbname, __LINE__); err("Подключение БД"); 
			}else if(mysql_query(mysql, std::string("SET NAMES utf8").c_str()); !mysql){ err("Установка кодировки");
			}else{ //mpre("ОШИБКА: " +string(mysql_error(mysql)), __LINE__);
			}return mysql; }(); false){ err("Подключение к БД mysql");
		}else if([&](){ // Подключение к БД redis
			if("redis" != bmf::dbtype){ //mpre("Используем не mysql", __LINE__);
			}else if(bmf::redis = redisConnect(bmf::dbhost.c_str() ,bmf::dbport); (bmf::redis == NULL)){ err("Подключение к redis");
			}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,string("SELECT " +bmf::dbname).c_str()); ("OK" != string(reply->str))){ mpre("ОШИБКА Выбор БД " +bmf::dbname +" (В redis доступны базы от 0 до 15)", __LINE__);
			}else{ //pre("Подключение к redis");
			}return false; }()){ err("Подключение к БД redis");
		}else{ //mpre("Подключение БД " +bmf::dbname, __LINE__);
		}return mysql; }(); false){ mpre("ОШИБКА подключения БД", __LINE__);
	}else if([&](){ // Функции БД
		if(bmf::Up = ([&](std::string ROWS, TMs where, TMs insert, TMs update, int line){
			TMs _row; // Возвращаем обновленное значение
			if([&](){ // Проверка условия
				if(where.end() == where.find("id")){ mpre(where, "Условие", __LINE__); mpre("ОШИБКА У поля не задан идентификатор id line=" +to_string(line), __LINE__);
				}else if(where.at("id").empty()){ mpre("ОШИБКА Пустой идентификатор line=" +to_string(line), __LINE__);
				}else if(1 != where.size()){ mpre("ОШИБКА условий выборки больше одного", __LINE__);
				}else{ //mpre("Условие корректно", __LINE__);
				}return false; }()){ err("Проверка условия выборки");
			}else if("mysql" == bmf::dbtype){ _row = bmf::Up_mysql(ROWS, where, insert, update, line);
			}else if("redis" == bmf::dbtype){ _row = bmf::Up_redis(ROWS, where, insert, update, line);
			}else{
			}return _row; }); false){ err("Функция обновления");
		}else if(bmf::Up_redis = ([&](std::string ROWS, TMs where, TMs insert, TMs update, int line){
			TMs _row; // Возвращаем обновленное значение
			if(!bmf::redis){ err("Соединение БД");
			//}else if(mpre(where, "Проверка1", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if([&](){ // Выборка
				if(where.empty()){ //mpre("Не заданы условия выборки", __LINE__);
				//}else if(!update.empty()){ mpre("Обновление", __LINE__);
				}else if(where.end() == where.find("id")){ err("Идентийикатор не указан");
				}else if(std::string id = bmf::dbname +":" +ROWS +":" +where.at("id"); id.empty()){ err("Идентификатор записи");
				}else if(std::string cmd = "HGETALL " +id +""; cmd.empty()){ err("Составление запроса");
				//}else if(mpre("Выборка " +cmd, __LINE__); false){ err("Уведомление");
				}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,cmd.c_str()); (reply->type != REDIS_REPLY_ARRAY)){ err("Выборка");
				//}else if(reply->type != REDIS_REPLY_ARRAY){ err("Проверка типа данных");
				//}else if(mpre("Количество элементов " +to_string(reply->elements), __LINE__); false){ err("Уведомление");
				}else if(_row = [&](TMs _row = {}){ for(int i =0; i <reply->elements; i =i +2){
					if(std::string first = reply->element[i]->str; first.empty()){ err("Ключ");
					}else if(std::string second = reply->element[i+1]->str; false){ err("Значение");
					}else if(second = ("\"\"" == second ? "" : second); false){ err("Пустое значение");
					}else if(_row.insert(make_pair(first, second)); _row.empty()){ err("Добавление поля/значение");
					}else{
					}}return _row; }(); false){ err("Выборка значения");
				}else if(freeReplyObject(reply); false){ err("Освобождение ресурсов");
				}else{ //mpre(_row, "Выборка "+cmd + " (" +to_string(reply->elements) +")", __LINE__); //mpre("ОШИБКА Выборка по условию " +sql, __LINE__);
				}return false; }()){ err("Выборка");
			}else if([&](){ // Обновление
				if(update.empty()){ //mpre("Не обновляем значение", __LINE__);
				}else if(_row.empty()){ //mpre("Элемент по условию не найден", __LINE__);
				}else if(where.end() == where.find("id")){ err("Не заданы условия");
				}else if(std::string id = bmf::dbname +":" +ROWS +":" +where.at("id"); id.empty()){ err("Идентификатор записи");
				}else if([&](){
					if(where.end() == where.find("id")){ //mpre("Не заданы условия", __LINE__);
					}else if(update["id"] = where.at("id"); false){ err("Установка идентификатора");
					//}else if(update == _row){
					}else if(std::string values = [&](std::string values = ""){ for(auto update_itr:update){ // Строка значений
						if(std::string key = update_itr.first; key.empty()){ err("Ключ не найден");
						}else if(std::string val = update_itr.second; false){ //pre("Пустое значение");
						}else if(values += string(values.empty() ? "" : " ") +key +" " +(val.empty() ? "\"\"" : val) +""; values.empty()){ err("Добавление значения");
						}else{
						}}return values; }(); values.empty()){ err("Значения списка");
					}else if(std::string cmd = "HMSET " +id +" " +values; cmd.empty()){ err("Запрос на выборку");
					}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,cmd.c_str()); ("OK" != string(reply->str))){ mpre("ОШИБКА добавления line=" +to_string(line) +" HMSET " +id +" " +values +" " +reply->str, __LINE__);
					}else if(_row = update; _row.empty()){ err("Возвращаемое значение");
					}else if(freeReplyObject(reply); false){ err("Освобождение ресурсов");
					}else{ //mpre(update, "Обновляем значения в базе " +cmd, __LINE__); //mpre("Проверка на совпадения", __LINE__);
					}return _row.empty(); }()){ err("Добавление");
				}else{ //mpre(where, "Условие", __LINE__); mpre(update, "Обновление", __LINE__);
				}return false; }()){ err("Редактирование строка " +line);
			}else if([&](){ // Добавление
				if(insert.empty()){ //mpre("Не добавляем в базу", __LINE__);
				}else if(!_row.empty()){ //mpre("Элемент по условию найден", __LINE__);
				}else if(std::string id = bmf::dbname +":" +ROWS +":" +where.at("id"); id.empty()){ err("Идентификатор записи");
				}else if([&](){
					if(where.end() == where.find("id")){ //mpre("Не заданы условия", __LINE__);
					}else if(insert["id"] = where.at("id"); false){ err("Установка идентификатора");
					}else if(std::string values = [&](std::string values = ""){ for(auto insert_itr:insert){ // Строка значений
						if(std::string key = insert_itr.first; key.empty()){ err("Ключ не найден");
						}else if(std::string val = insert_itr.second; false){ //pre("Пустое значение");
						}else if(values += string(values.empty() ? "" : " ") +key +" " +(val.empty() ? "\"\"" : val) +""; values.empty()){ err("Добавление значения");
						}}return values; }(); values.empty()){ err("Значения списка");
					}else if(std::string cmd = "HMSET " +id +" " +values; cmd.empty()){ err("Запрос на выборку");
					}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,cmd.c_str()); ("OK" != string(reply->str))){ mpre("ОШИБКА добавления line=" +to_string(line) +" HMSET " +id +" " +values +" " +reply->str, __LINE__);
					}else if(freeReplyObject(reply); false){ err("Освобождение ресурсов");
					}else if(_row = insert; _row.empty()){ err("Возвращаемое значение");
					}else{ //mpre(insert, "Добавление значения в базу " +cmd, __LINE__); //mpre("Проверка на совпадения", __LINE__);
					}return _row.empty(); }()){ err("Добавление");
				}else if([&](std::string table){ // Добавление значения в список
					if("dataset_map" == ROWS){ //mpre("Только для небольших списков", __LINE__);
					}else if(std::transform(table.begin() ,table.end() ,table.begin(), ::toupper); table.empty()){ err("ОШИБКА получения первого заглавного символа");
					}else if(std::string list_id = bmf::dbname +":" +table; list_id.empty()){ err("Идентификатор записи");
					}else if(std::string cmd = "SADD " +list_id +" " +where.at("id") +""; cmd.empty()){ err("Составление запроса");
					}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,cmd.c_str()); (0 != reply->str)){ mpre("ОШИБКА добавления line=" +to_string(line) +" " +cmd +" " +reply->str, __LINE__);
					}else if(freeReplyObject(reply); false){ err("Освобождение ресурсов");
					}else{ //mpre("Добавление нового значения в список " +cmd, __LINE__);
					}return false; }(ROWS)){ err("Список значения");
				}else{ //mpre("Добавление значения HMSET " +id +" " +values, __LINE__);
				}return false; }()){ err("Обновление");
			//}else if(mpre(where, "Проверка2", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else{ //mpre(where, "Условие", __LINE__); mpre(insert, "Вставка", __LINE__); mpre(update, "Обновление", __LINE__); mpre("Запрос `" +ROWS +"` строка:" +std::to_string(line), __LINE__); //mpre("Добавление заголовока в базу `" +row.at("id") +"` ", __LINE__);
			}return _row; }); false){ err("Функция обновления");
		}else if(bmf::Up_mysql = ([&](std::string table, TMs where, TMs insert, TMs update, int line){
			TMs _row; // Возвращаем обновленное значение
			if(!bmf::mysql){ err("Соединение БД");
			}else if([&](){ // Выборка
				if(where.empty()){ //mpre("Не заданы условия выборки", __LINE__);
				//}else if(!update.empty()){ mpre("Обновление", __LINE__);
				}else if(std::string _where = [&](std::string _where = ""){ // Условия выборки
					if([&](){ for(auto where_itr:where){ // Строка условий
						_where += string(_where.empty() ? "" : " AND ") +"`" +where_itr.first +"`='" +where_itr.second +"'";
						}return false; }()){ err("Строка условий");
					}else{ //mpre("Условия выборки "+ _where, __LINE__);
					}return _where; }(); false){ err("Составления условий выборки");
				}else if(std::string sql = "SELECT * FROM `" + table+"` WHERE " +_where +";"; sql.empty()){ err("Запрос к БД");
				}else if(_row = [&](TMs _row = {}){ // Получение значения
					if(mysql_query(bmf::mysql, sql.c_str()); !bmf::mysql){ mpre("Выбор данных из таблицы " +sql, __LINE__);
					}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); !result){ mpre("ОШИБКА выполнения запроса\n" +sql +"\n" +string(mysql_error(bmf::mysql)) ,__LINE__);
					}else if(int count = mysql_num_fields(result); !count){ err("Получение количества полей");
					}else if(std::string FIELDS[count]; false){ err("Массив списка полей");
					}else if([&](int i = 0){ while(MYSQL_FIELD *field = mysql_fetch_field(result)){ FIELDS[i++] = field->name; }return false; }()){ mpre("ОШИБКА получения полей", __LINE__);
					}else if([&](){ while(MYSQL_ROW row = mysql_fetch_row(result)){ for(int i = 0; i < count; i++){ //printf("%s  ", row[i] ? row[i] : "NULL");
						if(std::string field = FIELDS[i]; field.empty()){ err("Выборки имени поля");
						}else if(std::string value = (row[i] ? row[i] : ""); false){ err("Получение значения поля");
						}else if(_row.insert(make_pair(field, value)); _row.empty()){ err("Добавление значение в список");
						}else{ //mpre("Поле значение " +field +" => " +value, __LINE__);
						}}}return _row; }(); false){ err("Получение значений полей");
					}else if(mysql_free_result(result); false){ err("Очистка ресурсов");
					}else{ //mpre(_row, "Выборка " +sql, __LINE__);
					}return _row; }(); false){ err("Значение поля");
				}else{ //mpre(_row, "Результат", __LINE__); //mpre("ОШИБКА Выборка по условию " +sql, __LINE__);
				}return false; }()){ err("Выборка");
			}else if([&](){ // Редактирование
				if(update.empty()){ //mpre("Не обновляем значение", __LINE__);
				}else if(_row.empty()){ //mpre("Элемент по условию не найден", __LINE__);
				}else if(where.end() == where.find("id")){ err("Не заданы условия");
				}else if(update["id"] = where.at("id"); false){ err("Установка идентификатора");
				}else if(update == _row){ //mpre("При редактировании значения совпали", __LINE__);
				}else if(std::string values = [&](std::string values = ""){ for(auto update_itr:update){ // Составление запроса
					if("id" == update_itr.first){ //mpre("Не редактируем идентификатор", __LINE__);
					}else if(values += string(values.empty() ? "" : ", ") +"`" +update_itr.first + "`='" +update_itr.second +"'"; values.empty()){ err("Подстановка значения");
					}else{ //mpre("Список значений " +values, __LINE__);
					}}return values; }(); values.empty()){ err("Установка значений");
				}else if(std::string sql = "UPDATE `" +table + "` SET " +values +" WHERE `id`='" +where["id"] +"'"; sql.empty()){ err("Составление запроса");
				}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Выборка из таблицы " +sql, __LINE__); mpre("ОШИБКА " +string(mysql_error(bmf::mysql)), __LINE__);
				}else if(_row = update; false){ mpre("ОШИБКА установки результата", __LINE__);
				}else{ //mpre(where, "Условие", __LINE__); mpre(insert, "Вставка", __LINE__); mpre(update, "Обновление", __LINE__); mpre("ОШИБКА Обновление значения " +sql, __LINE__);
				}return false; }()){ err("Редактирование строка " +line);
			}else if([&](){ // Добавление
				if(insert.empty()){ //mpre("Не добавляем в базу", __LINE__);
				}else if(!_row.empty()){ //mpre("Элемент по условию найден", __LINE__);
				}else if([&](){
					if(where.end() == where.find("id")){ //mpre("Не заданы условия", __LINE__);
					}else if(insert["id"] = where.at("id"); false){ err("Установка идентификатора");
					}else{ //mpre("Проверка на совпадения", __LINE__);
					}return (insert == _row); }()){ mpre("Значения при обновлении совпали", __LINE__);
				}else if(_row = [&](){ // Добавление значения
					if(std::string VALUES = [&](string VALUES = "", string fields = "", string values = "", string duplicate = ""){ for(auto insert_itr:insert){
						if(fields += string(fields.empty() ? "" : " ,") +"`" +insert_itr.first +"`"; fields.empty()){ err("Поле значения");
						}else if(values += string(values.empty() ? "" : " ,") +"'" +insert_itr.second +"'"; values.empty()){ err("Значение");
						//}else if("id" == insert_itr.first){ //mpre("Не дублируем идентификатор" ,__LINE__);
						//}else if(duplicate += string(duplicate.empty() ? "" : " ,") + "`" +insert_itr.first +"` = VALUES(`" +insert_itr.first +"`)"; duplicate.empty()){ err("Установка дубликата");
						}else{ //mpre("Список полей значений " +insert_itr.first +" => " +insert_itr.second, __LINE__);
						}}return "(" +fields +") VALUES (" +values +")" /*+" ON DUPLICATE KEY UPDATE " +duplicate*/; }(); VALUES.empty()){ err("Получение пар полей значений запроса");
					}else if(std::string sql = "INSERT INTO `" +table +"` " +VALUES +";"; sql.empty()){ err("Запрос на добавление");
					}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Запрос таблица `" +table +"` line=" +to_string(line) +" " +sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__); //err("Результат выполнения запроса");
					}else if(std::string id = to_string(mysql_insert_id(bmf::mysql)); id.empty()){ err("Идентификатор новой записи");
					}else if(insert.insert(make_pair("id", id)); insert.empty()){ err("Установка идентификатора");
					//}else if((_row = insert).empty()){ mpre("ОШИБКА Обновленное значение", __LINE__);
					}else{ //mpre("Добавление " +sql, __LINE__);
					}return insert; }(); _row.empty()){ err("Добавление значения");
				//}else if(mpre("ОШИБКА добавления значения", __LINE__); false){ err("Уведомление");
				}else{ //mpre("Добавление значения в базу", __LINE__);
				}return false; }()){ err("Обновление");
			}else{ //mpre(where, "Условие", __LINE__); mpre(insert, "Вставка", __LINE__); mpre(update, "Обновление", __LINE__); mpre("Запрос `" +ROWS +"` строка:" +std::to_string(line), __LINE__); //mpre("Добавление заголовока в базу `" +row.at("id") +"` ", __LINE__);
			}return _row; }); false){ err("Функция обновления");
		}else if(bmf::List = [&](std::string table, TMs where, int line){ // Список значений из базы
			TMMs LIST = {};
			if(false){ err("Выборка");
			}else if("mysql" == bmf::dbtype){ LIST = bmf::List_mysql(table, where, "", line);
			}else if("redis" == bmf::dbtype){ LIST = bmf::List_redis(table, where, line);
			}else{ err("Тип БД не установлен");
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::List_redis = [&](std::string table, TMs where, int line){ // Список значений из базы
			TMMs LIST = {};
			if(std::string _table = table; _table.empty()){ err("Копитя таблицы для списка");
			}else if(std::transform(_table.begin() ,_table.end() ,_table.begin(), ::toupper); table.empty()){ err("ОШИБКА получения первого заглавного символа");
			}else if(std::string id = bmf::dbname +":" +_table; id.empty()){ err("Идентификатор записи");
			}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis,"SMEMBERS %s", id.c_str()); (NULL == reply)){ err("Выборка списка");
			}else if(reply->type != REDIS_REPLY_ARRAY){ err("Тип данных");
			//}else if(mpre("bmf::List_redis " +id +" " +to_string(reply->elements), __LINE__); false){ err("Выборка");
			}else if(LIST = [&](TMMs LIST = {}){ for(int i =0; i <reply->elements; i++){
				if(std::string key = reply->element[i]->str; false){ err("Значение");
				}else if(std::string id = bmf::dbname +":" +table +":" +key; id.empty()){ err("Получения ключа redis");
				}else if(TMs index = bmf::Up_redis(table, {{"id", key}}, {}, {}, __LINE__); index.empty()){ mpre("ОШИБКА Выборка элемента списка " +key, __LINE__);
				}else if(LIST.insert(make_pair(key, index)); LIST.empty()){ err("Добавление поля/значение");
				}else{
				}}return LIST; }(); false){ err("Выборка значения");
			}else if(freeReplyObject(reply); false){ err("Освобождение ресурсов");
			}else{ //mpre(LIST, "Запрос `" +table +"` списка redis строка:" +to_string(line) +" id=" +id +" количество элементов:" +to_string(LIST.size()), __LINE__);
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::List_mysql = [&](std::string table, TMs where, string sql = "", int line){ // Список значений из базы
			TMMs LIST = {};
			if(false){ err("Выборка");
			}else if(string _where = [&](string _where = "1"){ // Условия выборки
				if(where.empty()){ //mpre("Нет условий на выборку", __LINE__);
				}else if([&](){ for(auto where_itr:where){ // Список условий
					_where += string(_where.empty() ? "" : " AND ") +"`" +where_itr.first +"`='" +where_itr.second +"'";
					}return false; }()){ err("Список условий");
				}else{ //mpre("Условия выборки " +_where, __LINE__);
				}return _where; }(); false){ err("Условия выборки");
			}else if(string _sql = (sql.empty() ? "SELECT * FROM `" +table +"` WHERE " +_where : sql); _sql.empty()){ err("Запрос на выборку");
			}else if(mysql_query(bmf::mysql, _sql.c_str())){ mpre("Выбор данных из таблицы line=" +to_string(line) +" " +_sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__);
			}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); false){ err("ОШИБКА выполнения запроса");
			}else if([&](TMs titles = {}){ while(MYSQL_ROW row = mysql_fetch_row(result)){
				if(TMs ROW = [&](TMs ROW = {}){ for(int i = 0; i < mysql_num_fields(result); i++){
					if([&](){ // Список заголовоков полей
						if(0 < i){ //mpre("Получаем заголовки только на нулевой итерации", __LINE__);
						}else{ while(MYSQL_FIELD *field = mysql_fetch_field(result)){
							titles.insert(make_pair(to_string(titles.size()), field->name));
						}}return titles.empty(); }()){ err("Получения заголвоков");
					}else if(titles.end() == titles.find(to_string(i))){ mpre("ОШИБКА заголовок не найден " +to_string(i), __LINE__);
					}else if(std::string key = titles.at(to_string(i)); key.empty()){ err("Выборки заголовока");
					}else if(std::string val = (row[i] ? string(row[i]) : ""); false){ err("Получения значения");
					}else if(ROW.end() != ROW.find(key)){ ROW[key] = val;
					}else if(ROW.insert(make_pair(key, val)); ROW.empty()){ err("Заполнение списка");
					}else{ //mpre("Поле " +key +" значение " +val, __LINE__);
					}}return ROW; }(); ROW.empty()){ err("Получение полей записи");
				}else if(ROW.end() == ROW.find("id")){ mpre("ОШИБКА Поле идентификатора line=" +to_string(line) ,__LINE__);
				}else if(LIST.insert(make_pair(ROW.at("id"), ROW)); LIST.empty()){ err("Инкремент списка");
				}else{ //mpre(ROW, "Элемент списка", __LINE__);
				}}return false; }()){ err("ОШИБКА перебора значений");
			}else if(mysql_free_result(result); false){ err("Очистка ресурсов");
			}else{ //mpre("Запрос `" +table +"` списка:" +to_string(line) +" количество элементов:" +to_string(LIST.size()), __LINE__);
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Scan = [&](std::string table, TMMs LIST, int line){ // Список значений из базы
			if(false){ err("Выборка");
			}else if("mysql" == bmf::dbtype){ LIST = bmf::Scan_mysql(table, LIST, line);
			}else if("redis" == bmf::dbtype){ LIST = bmf::Scan_redis(table, LIST, line);
			}else{ err("Тип БД не установлен");
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Scan_mysql = [&](std::string table, TMMs LIST, int line){ // Список значений из базы
			TMMs _LIST;
			if(static std::string timestamp = ""; false){ err("Уведомление");
			}else if(string WHERE = (timestamp.empty() ? "1" : "timestamp>='" +timestamp +"'"); WHERE.empty()){ err("Условия выборки");
			}else if(string sql = "SELECT * FROM `" +table +"` WHERE " +WHERE; sql.empty()){ err("Запрос на выборку обновлений");
			//}else if(mpre("Запрос на обновление " +sql ,__LINE__); false){ err("Уведомлениение");
			}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Выбор данных из таблицы" +sql +"\n" +string(mysql_error(bmf::mysql)) ,__LINE__);
			}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); false){ err("ОШИБКА выполнения запроса");
			}else if([&](TMs titles = {}){ while(MYSQL_ROW row = mysql_fetch_row(result)){
				if(TMs ROW = [&](TMs ROW = {}){ for(int i = 0; i < mysql_num_fields(result); i++){
					if([&](){ // Список заголовоков полей
						if(0 < i){ //mpre("Получаем заголовки только на нулевой итерации", __LINE__);
						}else{ while(MYSQL_FIELD *field = mysql_fetch_field(result)){
							titles.insert(make_pair(to_string(titles.size()), field->name));
						}}return titles.empty(); }()){ err("Получения заголвоков");
					}else if(titles.end() == titles.find(to_string(i))){ mpre("ОШИБКА заголовок не найден " +to_string(i), __LINE__);
					}else if(std::string key = titles.at(to_string(i)); key.empty()){ err("Выборки заголовока");
					}else if(std::string val = (row[i] ? string(row[i]) : ""); false){ err("Получения значения");
					}else if(ROW.insert(make_pair(key, val)); ROW.empty()){ err("Заполнение списка");
					}else{
					}}return ROW; }(); ROW.empty()){ err("Получение полей записи");
				}else if(ROW.end() == ROW.find("id")){ err("Поле идентификатора");
				}else if(_LIST.insert(make_pair(ROW.at("id"), ROW)); _LIST.empty()){ err("Инкремент списка");
				}else if(ROW.end() == ROW.find("timestamp")){ err("Поле с временем создания");
				}else if(timestamp = (ROW.at("timestamp") > timestamp ? ROW.at("timestamp") : timestamp); timestamp.empty()){ err("Расчет максимальной даты обновления");
				}else{ //mpre("Обновленный элемент списка " +ROW.at("id"), __LINE__);
				}}return false; }()){ err("ОШИБКА перебора значений");
			}else if(mysql_free_result(result); false){ err("Очистка ресурсов");
			}else{ //mpre("Обновление в БД для mysql timestamp=" +timestamp, __LINE__);
			}return _LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Scan_redis = [&](std::string table, TMMs LIST, int line){ // Список значений из базы
			TMMs _LIST;
			if(std::string _table = table; _table.empty()){ err("Копитя таблицы для списка");
			}else if(std::transform(_table.begin() ,_table.end() ,_table.begin(), ::toupper); table.empty()){ err("ОШИБКА получения первого заглавного символа");
			}else if(std::string id = bmf::dbname +":" +_table; id.empty()){ err("Идентификатор записи");
			}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis,"SSCAN %s 0 count 1000000000", id.c_str()); (NULL == reply)){ err("Выборка списка");
			}else if(reply->type != REDIS_REPLY_ARRAY){ err("Тип данных");
			}else if(reply->element[1]->type != REDIS_REPLY_ARRAY){ err("Тип данных списока");
			}else if([&](){ for(int i =0; i <reply->element[1]->elements; i++){ // Проверка в списке новых элементов
				if(std::string key = reply->element[1]->element[i]->str; false){ err("Значение");
				}else if(LIST.end() != LIST.find(key)){ //mpre("Элемент уже в списоке", __LINE__);
				}else if(TMs index = bmf::Up(table, {{"id", key}}, {}, {}, __LINE__); index.empty()){ err("Выборка элемента");
				}else if(_LIST.insert(make_pair(key, index)); LIST.empty()){ err("Добалвение нового элемента в список");
				}else{ //mpre("Новый элемент i=" +to_string(i) +" key=" +key, __LINE__);
				}}return false; }()){ err("Выборка значения");
			}else{ //mpre("Проверка обновлений " +to_string(reply->elements), __LINE__);
			}return _LIST; }; false){ err("ОШИБКА выборки из базы");
		/*}else if(bmf::Shift = [&](TMs where, int line){ // Список значений из базы
			TMMs LIST;
			if(false){ err("Выборка");
			}else if("mysql" == bmf::dbtype){ LIST = bmf::Shift_mysql(where, LIST, line);
			}else if("redis" == bmf::dbtype){ LIST = bmf::Shift_redis(where, LIST, line);
			}else{ err("Тип БД не установлен");
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Shift_mysql = [&](TMs where, TMMs &LIST, int line){ // Список значений из базы
			if([&](){ // Список
				if(2 != where.size()){ //mpre("Список изменений не формируем", __LINE__);
				}else if(where.end() == where.find("dataset_id")){ err("Поле групы не установлено");
				}else if(std::string dataset_id = where.at("dataset_id"); dataset_id.empty()){ err("Значение группы");
				}else if(where.end() == where.find("key")){ mpre("Поле ключа не установлено line=" +to_string(line), __LINE__);
				}else if(std::string key = where.at("key"); key.empty()){ err("Значение группы");
				}else if(LIST = bmf::List(bmf::INDEX_SHIFT, {}, __LINE__); false){ err("Выборка изменений");
				}else{ //mpre("Выборка списка dataset_id=" +dataset_id +" key=" +key +" size=" +to_string(LIST.size()), __LINE__);
				}return false; }()){ err("Список изменений");
			}else if([&](){ // Изменения
				if(2 == where.size()){ //mpre("Изменения не устанавливаем", __LINE__);
				}else if(where.end() == where.find("group")){ mpre(where, "Условие line=" +to_string(line), __LINE__); err("Поле групы не установлено");
				}else if(std::string group = where.at("group"); group.empty()){ err("Значение группы");
				}else if(where.end() == where.find("dataset_id")){ mpre("Поле набора данных не установлено line=" +to_string(line), __LINE__);
				}else if(std::string dataset_id = where.at("dataset_id"); dataset_id.empty()){ err("Значение группы");
				}else if(where.end() == where.find("key")){ err("Поле ключа не установлено");
				}else if(std::string key = where.at("key"); key.empty()){ err("Значение ключа");
				}else if(string index_shift_id = group; index_shift_id.empty()){ err("Идентификатор изменений");
				//}else if(std::string sql = "INSERT INTO `index_shift` SET `id`='" +index_shift_id +"', `dataset_id`='" +dataset_id +"', `key`='" +key +"', `group`='" +group +"', `shift`=1 ON DUPLICATE KEY UPDATE `shift`=`shift`+1;"; sql.empty()){ err("Запрос на изменение");
				//}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Установка изменений " +sql +"\n" +mysql_error(bmf::mysql), __LINE__);
				}else if(TMs _index_shift = {{"group", group}, {"dataset_id", dataset_id}, {"key", key}, {"shift", "0"}}; _index_shift.empty()){ err("Первоначальные изменения");
				}else if(TMs index_shift = bmf::Up(bmf::INDEX_SHIFT, {{"id", index_shift_id}}, _index_shift, {}, __LINE__); index_shift.empty()){ err("Добавление нового изменения");
				//}else if(std::string sql = "INSERT INTO `index_shift` SET `id`=MD5('" +index_shift_id +"'), `group`=MD5('" +group +"'), `dataset_id`='" +dataset_id +"', `key`='" +key +"', `shift`='0';"; sql.empty()){ err("Запрос на изменение");
				//}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Установка изменений " +sql +"\n" +mysql_error(bmf::mysql), __LINE__);
				}else{ //mpre(where, "Установка изменения " +sql, __LINE__);
				}return false; }()){ err("Изменения");
			}else{ //mpre("Изменения");
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Shift_redis = [&](TMs where, TMMs &LIST, int line){ // Список значений из базы
			err("Изменения redis");
			return LIST; }; false){ err("ОШИБКА выборки из базы");*/
		/*}else if(bmf::Group = [&](TMs where, int line){ // Список значений из базы
			TMMs LIST;
			if(false){ err("Выборка");
			}else if("mysql" == bmf::dbtype){ LIST = bmf::Group_mysql(where, LIST, line);
			}else if("redis" == bmf::dbtype){ LIST = bmf::Group_redis(where, LIST, line);
			}else{ err("Тип БД не установлен");
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Group_mysql = [&](TMs where, TMMs LIST, int line){ // Список значений из базы
			if([&](){ // Список
				if(2 != where.size()){ //mpre("Список изменений не формируем", __LINE__);
				}else if(where.end() == where.find("key")){ err("Ключ не найден");
				}else if(std::string key = where.at("key"); key.empty()){ err("Значение ключа");
				}else if(where.end() == where.find("dataset_id")){ err("Ключ не найден");
				}else if(std::string dataset_id = where.at("dataset_id"); dataset_id.empty()){ err("Значение ключа");
				}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
				}else if(string sql = "SELECT grp.* FROM `index_group` AS `grp` LEFT JOIN `index_shift` as `shift` ON grp.`group`=`shift`.`group` AND grp.`shift`=`shift`.`shift` WHERE `shift`.`id` IS NOT NULL AND grp.`key`='" +key +"' AND grp.`dataset_id`='" +dataset_id +"'"; sql.empty()){ err("Запрос на выборку актуальных групп");
				}else if(mysql_query(bmf::mysql, sql.c_str())){ err("Выбор данных из таблицы");
				}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); false){ err("ОШИБКА выполнения запроса");
				}else if([&](TMs titles = {}){ while(MYSQL_ROW row = mysql_fetch_row(result)){
					if(TMs ROW = [&](TMs ROW = {}){ for(int i = 0; i < mysql_num_fields(result); i++){
						if([&](){ // Список заголовоков полей
							if(0 < i){ //mpre("Получаем заголовки только на нулевой итерации", __LINE__);
							}else{ while(MYSQL_FIELD *field = mysql_fetch_field(result)){
								titles.insert(make_pair(to_string(titles.size()), field->name));
							}}return titles.empty(); }()){ err("Получения заголвоков");
						}else if(titles.end() == titles.find(to_string(i))){ mpre("ОШИБКА заголовок не найден " +to_string(i), __LINE__);
						}else if(std::string key = titles.at(to_string(i)); key.empty()){ err("Выборки заголовока");
						}else if(std::string val = (row[i] ? string(row[i]) : ""); false){ err("Получения значения");
						}else if(ROW.insert(make_pair(key, val)); ROW.empty()){ err("Заполнение списка");
						}else{
						}}return ROW; }(); ROW.empty()){ err("Получение полей записи");
					}else if(ROW.end() == ROW.find("id")){ err("Поле идентификатора");
					}else if(LIST.insert(make_pair(ROW.at("id"), ROW)); LIST.empty()){ err("Инкремент списка");
					}else{ //mpre("Обновленный элемент списка " +ROW.at("id"), __LINE__);
					}}return false; }()){ err("ОШИБКА перебора значений");
				}else if([&](){ // Информация о запроса
					if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
					}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Выборка значений групп size=" +to_string(LIST.size()), __LINE__);
					}return false; }()){ err("Вывод инморфации о запросе");
				}else if(mysql_free_result(result); false){ err("Очистка ресурсов");
				}else{ //mpre(LIST, "Выборка списка групп key=" +key +" " +sql, __LINE__);
				}return false; }()){ err("Список изменений");
			}else if([&](){ // Изменения
				if(2 == where.size()){ //mpre("Изменения не устанавливаем", __LINE__);
				}else if(where.end() == where.find("group")){ err("Поле групы не установлено");
				}else if(std::string group = where.at("group"); group.empty()){ err("Значение группы");
				}else if(where.end() == where.find("shift")){ err("Поле изменения не задано");
				}else if(std::string shift = where.at("shift"); shift.empty()){ err("Значение изменения");
				}else if(where.end() == where.find("grp")){ err("Поле значения группы");
				}else if(std::string grp = where.at("grp"); grp.empty()){ err("Значение группы");
				}else if(where.end() == where.find("key")){ err("Поле ключа");
				}else if(std::string key = where.at("key"); key.empty()){ err("Значение ключа");
				}else if(where.end() == where.find("dataset_id")){ err("Поле набора данных");
				}else if(std::string dataset_id = where.at("dataset_id"); dataset_id.empty()){ err("Значение набора данных");
				}else if(std::string index_group_id = group +"," +dataset_id +"," +key; index_group_id.empty()){ err("Идентификатор записи");
				}else if(std::string sql = "INSERT INTO `index_group` SET `id`=" +index_group_id +", `group`=" +group +"', `dataset_id`='" +dataset_id +"', `key`='" +key +"', `shift`='" +shift +"', `grp`='" +grp +"' ON DUPLICATE KEY UPDATE `shift`='" +shift +"', `grp`='" +grp +"';"; sql.empty()){ err("Запрос на изменение");
				}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Установка изменений " +sql +"\n" +mysql_error(bmf::mysql), __LINE__);
				}else{ //mpre(where, "Установка значения группы " +sql, __LINE__);
				}return false; }()){ err("Изменения");
			}else{ //mpre("Изменения");
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else if(bmf::Group_redis = [&](TMs where, TMMs LIST, int line){ // Список значений из базы
			err("Группа redis");
			return LIST; }; false){ err("ОШИБКА выборки из базы");*/
		}else if( bmf::Progress = [&](string mess, float val, int line, int pos = 0){
				fprintf(stderr, "\r%d.[", line);
				for(pos=0; pos<val*20; pos++) fprintf(stderr, "%s", string("#").c_str());
				for(;pos<20; pos++) fprintf(stderr, "%s", string(" ").c_str());
				fprintf(stderr, "] (%.1f %%) %s\r", val*100, mess.c_str());
				std::cerr.flush();
			}; false){ mpre("ОШИБКА создания функции отображения прогресса", __LINE__);
		}else{ //mpre("Загрузка функций БД", __LINE__);
		}return false; }()){ mpre("ОШИБКА функции БД", __LINE__);
	}else if([&](){ // Сервис
		if([&](){ // Очистка БД
			if(bmf::ARGV.end() == bmf::ARGV.find("-c")){ //mpre("Не очищаем БД", __LINE__);
			}else if("redis" != bmf::dbtype){ //mpre("Используем не редис", __LINE__);
			}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,"FLUSHDB"); ("OK" != string(reply->str))){ err("Очистка FLUSHDB БД redis");
			}else{ mpre("Очистка FLUSHDB базы данных " +bmf::dbname, __LINE__);
			}return false; }()){ err("ОШИБКА очистки БД");
		}else if([&](){ // Очистка БД
			if(bmf::ARGV.end() == bmf::ARGV.find("-c")){ //mpre("Не очищаем БД", __LINE__);
			}else if("mysql" != bmf::dbtype){ //mpre("Используем не mysql", __LINE__);
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `index`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `index_grp`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset_map`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano_values`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano_titles`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog_values`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog_titles`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `epoch`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if([&](){ // Шард таблицы
				if(string sql = "SHOW TABLES FROM " +bmf::dbname +";"; sql.empty()){ err("Запрос списка всех таблиц базы данных");
				}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Выбор данных из таблицы " +sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__);
				}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); false){ err("ОШИБКА выполнения запроса");
				}else if(TMMs TABLES = [&](TMMs TABLES = {} ,TMs titles = {}){ while(MYSQL_ROW row = mysql_fetch_row(result)){
					if(TMs ROW = [&](TMs ROW = {}){ for(int i = 0; i < mysql_num_fields(result); i++){
						if([&](){ // Список заголовоков полей
							if(0 < i){ //mpre("Получаем заголовки только на нулевой итерации", __LINE__);
							}else{ while(MYSQL_FIELD *field = mysql_fetch_field(result)){
								titles.insert(make_pair(to_string(titles.size()), field->name));
							}}return titles.empty(); }()){ err("Получения заголвоков");
						}else if(titles.end() == titles.find(to_string(i))){ mpre("ОШИБКА заголовок не найден " +to_string(i), __LINE__);
						}else if(std::string key = titles.at(to_string(i)); key.empty()){ err("Выборки заголовока");
						}else if(std::string val = (row[i] ? string(row[i]) : ""); false){ err("Получения значения");
						}else if(ROW.end() != ROW.find(key)){ ROW[key] = val;
						}else if(ROW.insert(make_pair(key, val)); ROW.empty()){ err("Заполнение списка");
						}else{ //mpre("Поле " +key +" значение " +val, __LINE__);
						}}return ROW; }(); ROW.empty()){ err("Получение полей записи");
					}else if(TABLES.insert(make_pair(to_string(TABLES.size()), ROW)); TABLES.empty()){ err("Инкремент списка");
					}else{ //mpre(ROW, "Элемент списка", __LINE__);
					}}return TABLES; }(); false){ err("ОШИБКА перебора значений");
				}else if(mysql_free_result(result); false){ err("Очистка ресурсов");
				}else if(mpre("Очистка БД " +to_string(TABLES.size()) +" таблиц" ,__LINE__); false){ err("Уведомление");
				}else if([&](string field, int cnt = 0){ for(auto tables_itr:TABLES){ // Удаление таблицы
					if(bmf::Progress("Удаление осколков " +to_string(TABLES.size()), (float)(++cnt)/TABLES.size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
					}else if(TMs tables = tables_itr.second; tables.empty()){ err("Поле списка таблиц");
					}else if(string table = (tables.end() == tables.find(field) ? "" : tables.at(field)); table.empty()){ err("Имя таблицы");
					}else if(string::npos == table.find("index_vals_")){ //mpre("", __LINE__); mpre("Неизвестная таблица `" +table +"` пропуск автоматического удаления",__LINE__);
					}else if(mysql_query(bmf::mysql, string("DROP TABLE " +table).c_str())){ mpre("Выбор данных из таблицы " +sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__);
					}else{ //mpre("Удаление шард таблицы " +table ,__LINE__);
					}} std::cerr << endl; return false; }("Tables_in_" +bmf::dbname)){ err("Удаление таблицы");
				}else{ //mpre(TABLES ,"Список таблиц" ,__LINE__);
				}return false; }()){ err("Удаление таблиц шардинга");
			}else{ //mpre("Очистка базы данных mysql", __LINE__);
			}return false; }()){ err("ОШИБКА очистки БД");
		}else if([&](){ // Очистка кеша
			if(bmf::ARGV.end() == bmf::ARGV.find("cache")){ //mpre("Не очищаем БД", __LINE__);
			}else if(string cache = bmf::ARGV.at("cache"); "clean" != cache){ mpre("Комманда не распознана", __LINE__);
			}else if(TMMs SHIFT = bmf::List(bmf::INDEX_SHIFT, {}, __LINE__); false){ err("Выборка изменений");
			}else if([&](){ for(auto index_shift_itr:SHIFT){ // Перебор текущих значений
				if(TMs index_shift = index_shift_itr.second; index_shift.empty()){ err("Значение изменений");
				}else if(index_shift.end() == index_shift.find("group")){ err("Набор данных");
				}else if(string group = index_shift.at("group"); group.empty()){ err("Не указан набор данных");
				}else if(index_shift.end() == index_shift.find("shift")){ err("Набор данных");
				}else if(string shift = index_shift.at("shift"); shift.empty()){ err("Не указан набор данных");
				}else if(string sql = "DELETE FROM `index_group` WHERE `group`='" +group +"' AND `shift`<" +shift +" LIMIT 1000;"; sql.empty()){ err("Запрос на удаление");
				}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Вставка данных " +sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__); //err("Список морфов"); 
				}else if(int affected = mysql_affected_rows(bmf::mysql); false){ err("Количество затронутых значений");
				}else if(!affected){ //mpre("Не отображаем информацию об удалении пустых строк", __LINE__);
				}else{ mpre("Очистка кеша:" +to_string(affected) +" shift=" +shift +" group=" +group, __LINE__);
				}}return false; }()){ err("Перебор текущих значений");
			}else{ mpre("Очистка кеша", __LINE__);
			}return false; }()){ err("Очистка кеша");
		}else if([&](string sql = ""){ // Создание таблиц 
			if("mysql" != bmf::dbtype){ //mpre("Используем не mysql", __LINE__);
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `index` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY, grp INT NOT NULL ,itog_id INT NOT NULL , addr TEXT ,md5 VARCHAR(32) NOT NULL ,`timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, `dano_id` INT NOT NULL, UNIQUE KEY index_md5(md5), KEY `itog_md5`(itog_id ,md5))").c_str())){ err("Список морфов"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `index_grp` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY ,index_grp_id INT DEFAULT NULL ,itog_id INT NOT NULL ,dataset_id INT NOT NULL ,`index_id` INT NOT NULL ,count INT(1) DEFAULT NULL ,`key` INT NOT NULL ,list TEXT ,`grow` INT DEFAULT NULL, vals BIT(64) DEFAULT NULL ,exist BIT(64) DEFAULT NULL ,dano BIT(64) DEFAULT NULL, UNIQUE KEY `index_vals_unique` (dataset_id ,itog_id ,`key` ,`index_id`) ,KEY exist(exist) ,FOREIGN KEY (index_grp_id) REFERENCES index_grp(id))").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dataset` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY, `name` TEXT, md5 VARCHAR(32) NOT NULL, `count` INT, `key` INT DEFAULT 0, `epoch` INT DEFAULT 0, `err` INT DEFAULT 0, `microtime` BIGINT(20) DEFAULT 0)").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dataset_map` (`id` VARCHAR(750) NOT NULL UNIQUE, `dataset_id` INT, `alias` VARCHAR(750), `alias_id` VARCHAR(750), `itog_id` VARCHAR(750), `map` TEXT, KEY `dataset_id`(`dataset_id`), KEY `alias`(`alias`))").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY, `dano_values_id` VARCHAR(750), `name` VARCHAR(750), `val` VARCHAR(750))").c_str())){ err("Дано"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano_values` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY, `name` TEXT)").c_str())){ err("Дано значения"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano_titles` (`id` VARCHAR(750) NOT NULL UNIQUE, `dano_values_id` VARCHAR(750), `name` VARCHAR(750), `value` VARCHAR(750) UNIQUE)").c_str())){ err("Дано заголовки"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY ,`itog_values_id` VARCHAR(750), `name` VARCHAR(750), `val` VARCHAR(750), `index_id` VARCHAR(32))").c_str())){ err("Итог"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog_values` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY, `name` VARCHAR(750))").c_str())){ err("Итоги значения"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog_titles` (`id` VARCHAR(750) NOT NULL UNIQUE, `itog_values_id` VARCHAR(750), `name` VARCHAR(750), `value` VARCHAR(750))").c_str())){ err("Итоги заголовки"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `epoch` (`id` INT AUTO_INCREMENT NOT NULL PRIMARY KEY, `epoch` INT NOT NULL, `dataset_id` INT NOT NULL, `err` INT NOT NULL, `microtime` FLOAT NOT NULL)").c_str())){ err("Вставка данных"); 
			}else{ //mpre("Создание таблиц", __LINE__); 
			}return false; }()){ err("Создание таблиц");		
		}else if([&](){ // Уровень групп
			if(bmf::ARGV.end() == bmf::ARGV.find("group_level")){ //mpre(bmf::ARGV, "Значения консоли", __LINE__); mpre("Уровень групп не указан", __LINE__);
			}else if(bmf::group_level = atoi(bmf::ARGV.at("group_level").c_str()); !bmf::group_level){ err("Не верный уровень групп");
			}else{ mpre("Изменение уровня гурпп bmf::group_level=" +to_string(bmf::group_level), __LINE__);
			}return false; }()){ err("Уровень групп");
		}else{
		}return false; }()){ err("Сервисные комманды");
	}else if([&](bool skip = true){ // Подключение базы
		if(bmf::Dec2bin = ([&](double decimal, string binary = ""){ // Получаем число возвращаем двойное число
			if(std::function<string(unsigned long long)> dec2bin = ([&](long long n){ // Получаем число возвращаем двойное число
				std::string r; long long i = n;
				do{ r=(i%2==0 ?"0":"1")+r;
				}while(0<(i/=2)); return (0 == n ? "0" : r); }); false){ mpre("ОШИБКА создания функции перевода из целого числа в двоичное", __LINE__);
			}else if(string  _decimal = to_string(decimal); false){ mpre("ОШИБКА приведение к строке", __LINE__);
			}else if(_decimal = ("-" == _decimal.substr(0, 1) ? _decimal.substr(1, _decimal.length()) : _decimal); (0 >= _decimal.length())){ mpre("ОШИБКА удаления знака отрицания", __LINE__);
			}else if(int pos = _decimal.rfind("."); (0 > pos)){ mpre("Число без дробной части", __LINE__);
			}else if(string dec = [&](string dec = ""){ // Получение целой части числа
				if(dec = _decimal.substr(0, pos); (0 >= dec.length())){ mpre("ОШИБКА нахождения целой части числа", __LINE__);
				}else{ //mpre("Число с дробной частью в позиции "+ to_string(pos), __LINE__);
				} return dec; }(); false){ mpre("ОШИБКА получения целой части числа "+ dec, __LINE__);
			}else if(string _dec = [&](string _dec = ""){ // Дробная часть
				if(_dec = _decimal.substr(pos+1, _decimal.length()); (0 >= _dec.length())){ mpre("ОШИБКА нахождения дробной части числа", __LINE__);
				}else if(pos = _dec.find_last_not_of('0'); false){ mpre("ОШИБКА Действительных чисел в дробной части не найдено", __LINE__);
				}else if(_dec = (0 > pos ? "0" : _dec.substr(0, pos+1)); (0 >= _dec.length())){ mpre("ОШИБКА удаления лишних пробелов в дробной части", __LINE__);
				}else{
				} return _dec; }(); false){ mpre("ОШИБКА получения дробной части", __LINE__);
			}else if(string bin = dec2bin(stod(dec)); (0 >= bin.length())){ mpre("ОШИБКА перевода в двоичную форму целой части числа", __LINE__);
			}else if(std::reverse(_dec.begin(), _dec.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
			}else if(string _bin = dec2bin(stod(_dec)); (0 >= _bin.length())){ mpre("ОШИБКА перевода в двоичную форму дробной части числа", __LINE__);
			}else if(std::reverse(_bin.begin(), _bin.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
			}else if(binary = bin+ ("0" == _bin ? "" : "."+_bin); (0 >= binary.length())){ mpre("ОШИБКА получения итогового двоичного числа", __LINE__);
			}else if(binary = (0 > decimal ? "-" : "")+ binary; (0 >= binary.length())){ mpre("ОШИБКА установки знака отрицания", __LINE__);
			}else{ //mpre("Исходное число "+ to_string(decimal)+ " "+ dec+ "."+ _dec, __LINE__); //mpre("Результат "+ bin+ "."+ _bin+ " => "+ binary, __LINE__);
			}return binary; }); false){ mpre("ОШИБКА формирования функции пересчета разрядности чисел", __LINE__);
		}else if(bmf::Bin2dec = ([](string binary, double decimal = 0){ // Получаем число возвращаем двойное число
			string _bin, _dec, _binary = binary;
			if(std::function<long long(string)> bin2dec = ([&](string binary, unsigned long long decimal = 0){ for(int i = 0; i < binary.length(); i++){ // Получаем число возвращаем двойное число
				if(int pos = binary.length()-i-1; (0 > pos)){ mpre("ОШИБКА получения позиции символа", __LINE__);
				}else if(int _pow = (unsigned long long)pow(2, i); (0 > _pow)){ mpre("ОШИБКА получения степени числа", __LINE__);
				}else if(string val = binary.substr(pos, 1); (1 != val.length())){ mpre("ОШИБКА получения символа", __LINE__);
				}else if(decimal += ("1" == val ? _pow : 0); (0 > decimal)){ mpre("ОШИБКА получения результата", __LINE__);
				}else{ //mpre("Смещение/степень i="+ to_string(i)+ " pos="+ to_string(pos)+ " val="+ val+ " _pow="+ to_string(_pow)+ " >> "+ to_string(decimal), __LINE__);
				}}return decimal; }); false){ mpre("ОШИБКА создания функции перевода из двоичного числа в десятичное", __LINE__);
			}else if(_binary = ("-" == _binary.substr(0, 1) ? _binary.substr(1, _binary.length()) : _binary); (0 >= _binary.length())){ mpre("ОШИБКА удаления символа отрицания", __LINE__);
			}else if(string bin = [&](string bin = ""){ // Получение целой части числа
				if(int pos = _binary.rfind("."); false){ mpre("Число без дробной части", __LINE__);
				}else if(bin = (-1 == pos ? _binary.substr(0, _binary.length()) : _binary.substr(0, pos)); (0 >= bin.length())){ mpre("ОШИБКА нахождения целой части числа "+ binary+ " "+ _binary+ " "+ to_string(pos), __LINE__);
				}else if(0 > pos){ //mpre("Не расчитываем дробную часть", __LINE__);
				}else if(_bin = _binary.substr(pos+1, _binary.length()); (0 >= _bin.length())){ //mpre("Дробная часть не указана "+ _binary, __LINE__);
				}else if(pos = _bin.find_last_not_of('0'); false){ mpre("ОШИБКА Действительных чисел в дробной части не найдено", __LINE__);
				}else{ //mpre("Число с дробной частью в позиции "+ to_string(pos)+ " >> "+ _bin, __LINE__);
				}return bin; }(); false){ mpre("ОШИБКА получения целой части числа", __LINE__);
			}else if(unsigned long long dec = bin2dec(bin); (0 > dec)){ mpre("ОШИБКА перевода в двоичную форму целой части числа", __LINE__);
			}else if(std::reverse(_bin.begin(), _bin.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
			}else if(_dec = to_string(bin2dec(_bin)); (0 > _dec.length())){ mpre("ОШИБКА перевода в двоичную форму дробной части числа", __LINE__);
			}else if(std::reverse(_dec.begin(), _dec.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
			}else if(decimal = stod(to_string(dec)+ "."+_dec); false){ mpre("ОШИБКА получения итогового двоичного числа", __LINE__);
			}else if(decimal *= ("-" == binary.substr(0, 1) ? -1 : 1); false){ mpre("ОШИБКА установки отрицательного значения", __LINE__);
			}else{ //mpre("Результат расчетов "+ binary+ " "+ bin+ "("+ to_string(dec)+ ")."+ _bin+ "("+ _dec+ ") >> "+ to_string(decimal), __LINE__);
			}return decimal; }); false){ mpre("ОШИБКА Функция перевода из двоичной в десятичную систему", __LINE__);
		}else if(bmf::Calculate = ([&](TMs bmf_index, std::string calculate = ""){ // Функция строки польской нотации
			if(TMMi STAIRS = [&](TMMi STAIRS = {}){ // Заполнение лестницы первоначальным значением
				if(false){ pre("Пропуск первоначального заполнения лестницы");
				}else if(bmf_index.end() == bmf_index.find("id")){ err("Идентификатор морфа не указан");
				}else if(TMs stairs = {{"index_id", bmf_index.at("id")}, {"parent", "bmf-index"}}; stairs.empty()){ err("Получение первоначальной ступени");
				}else if(STAIRS.insert(make_pair(0, stairs)); STAIRS.empty()){ err("Добавление ступени в лестницу");
				}else{ //mpre(bmf_index, "Корневой морф", __LINE__); //pre("Первоначальное заполнение лестницы");
				} return STAIRS; }(); STAIRS.empty()){ err("Создание первоначальной ступени");
			}else if([&](){ do{ // Перебор лестницы
				if(false){ pre("Пропуск расчета лестницы");
				}else if(1 > STAIRS.size()){ err("Пустая лестница");
				}else if(auto stairs_itr = STAIRS.rbegin(); (STAIRS.rend() == stairs_itr)){ err("Выборка итаратора ступени");
				}else if(TMs stairs = stairs_itr->second; stairs.empty()){ err("Выборка ступени");
				}else if(TMs index = [&](TMs index = {}){ // Проверка и выборка морфа ступени
					if(stairs.end() == stairs.find("index_id")){ err("У ступени не найдено поле идентификатора морфа");
					}else if(int index_id = atoi(stairs.at("index_id").c_str()); (0 == index_id)){ err("Выборка идентификатор морфа");
					}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(index_id)){ err("Справочник морфов не содержит морфа итога");
					}else if(index = BMF_INDEX_EX.at("").at(index_id); index.empty()){ err("Выборка морфа из справочника");
					}else{ //mpre(index, "Морф", __LINE__);
					} return index; }(); index.empty()){ err("Выборка морфа");
				//}else if(mpre(index, "Ступень", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(std::string field = [&](std::string field = ""){ // Направление расчета
					if(std::string _field = (stairs.end() == stairs.find("field") ? "" : stairs.at("field")); false){ pre("Определение старого направления");
					}else if(TMs fields = {{"", "index_id"}, {"index_id", "bmf-index"}, {"bmf-index", ""}}; fields.empty()){ err("Установка списка переходов");
					}else if(fields.end() == fields.find(_field)){ err("Значение перехода не задано");
					}else if(field = fields.at(_field); false){ err("Определение нового направления");
					}else if(stairs_itr->second["field"] = stairs["field"] = field; false){ err("Установка направления в ступень");
					}else{ //mpre("Установка направления " +field, __LINE__);
					} return field; }(); false){ err("Установка направления расчета");
				}else if(std::string index_id = [&](std::string index_id =""){ // Дополнение строки расчетов
					if(false){ pre("Пропуск расчета строки");
					}else if(index.end() == index.find(field)){ //mpre("Поле перехода не найден в морфе `" +field +"`", __LINE__);
					}else if(index_id = index.at(field); !index_id.empty()){ //mpre("Нижестоящий морф установлен "+ index.at("id"), __LINE__);
					}else if(calculate += index.at("dano_id"); calculate.empty()){ err("Установка знака");
					}else if(std::string sign = ("index_id" == field ? "/" : "\\"); false){ err("Выборка знака");
					}else if(calculate += sign; calculate.empty()){ err("Установка знака");
					}else{ //mpre("Расчет строки " +index.at("id") +" calculate=" +calculate, __LINE__);
					} return index_id; }(); false){ err("Формирование строки");
				}else if(TMs _stairs = [&](TMs _stairs = {}){ // Добавление ступени
					if(index_id.empty()){ //pre("Нет морфа для добавления");
					}else if(_stairs = {{"index_id", index_id}, {"parent", field}}; _stairs.empty()){ err("Создание следующей ступени");
					}else if(STAIRS.insert(make_pair(STAIRS.size(), _stairs)); STAIRS.empty()){ err("Добавление новой ступени");
					}else{ //mpre("Добавление новой ступени " +index.at("id") +" > " +index_id, __LINE__);
					} return _stairs; }(); false){ err("Добавление ступени");
				}else if([&](){ // Удаление ступени
					if(!_stairs.empty()){ //pre("Уже добавлена нижестоящая");
					}else if("index_id" == field){ //pre("Не удаляем старшее направление");
					//}else if(index.end() == index.find("calc_pos_id")){ err("Выборка позиции");
					//}else if(int calc_pos_id = atoi(index.at("calc_pos_id").c_str()); 0 > calc_pos_id){ err("Выборка позиции морфа");
					}else if(TMs morpths = TMs({{"0", "<"}, {"1", ">"}}); morpths.empty()){ err("Установка знаков");
					}else if(stairs.end() == stairs.find("parent")){ mpre("ОШИБКА поле родительской связи не установлено", __LINE__);
					}else if(std::string pos = ("index_id" == stairs.at("parent") ? "0" : "1"); (1 != pos.length())){ err("Определение позиции знака морфа");
					}else if(morpths.end() == morpths.find(pos)){ mpre("ОШИБКА Позияия морфа не установлена " +pos, __LINE__);
					}else if(std::string morpth = morpths.at(pos); (1 != morpth.length())){ err("Расчет морфа");
					}else if(calculate += index.at("id") +morpth; calculate.empty()){ pre("Установка символа расчета морфа");
					}else if(STAIRS.erase(STAIRS.size()-1); STAIRS.empty()){ //err("Удаление последней ступени");
					}else{ //mpre("Удаление ступени " +index.at("id"), __LINE__);
					} return false; }()){ err("Удаление ступени");
				}else{ //mpre("Расчет лестницы "+ std::to_string(loop), __LINE__);
				}}while(0 < STAIRS.size()); return false; }()){ err("Перебор всех элементов дерева для составления строки расчетов");
			}else{ //Tree(bmf_index, BMF_INDEX_EX); mpre("Расчет строки польской нотации " +calculate, __LINE__);
			}return calculate; }); false){ err("Создание функции расчета польской нотации");
		}else if(bmf::Calc = ([&](std::string calculate, TM3i& _BMF_DANO_EX, std::string calc = ""){ // Расчет калькуляции
			if(std::stack<string> vals = [&](std::stack<string> vals = {}, std::string c = "", std::string num = ""){ for(char c:calculate){ // Перебор значенийa
				if(std::string str(1,c); false){ err("Преобразование символа к строке");
				}else if([&](){
					if(string::npos != str.find_last_not_of("-0123456789")){ //pre("Не число"); //mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);
					}else if(num += str; false){ err("Инкримент числа");
					}else{ //mpre("Чтение числа "+ num, __LINE__);
					}return false; }()){ err("Расчет числа");
				}else if([&](){
					if(string::npos != str.find_last_not_of("/\\")){ //pre("Не значения"); //mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);a
					}else if(num.empty()){ //pre("Расчет ранее сохраненных значений");
					}else if(_BMF_DANO_EX.at("").end() == _BMF_DANO_EX.at("").find(atoi(num.c_str()))){ mpre("ОШИБКА Значение в исходниках не найдено "+ num, __LINE__);
					}else if(TMs dano = _BMF_DANO_EX.at("").at(atoi(num.c_str())); dano.empty()){ err("Выборка дано");
					}else if(dano.end() == dano.find("val")){ err("Значение дано не задано");
					}else if(std::string _val = dano.at("val"); (1 != _val.length())){ err("Выборка значения дано");
					}else if(vals.push(_val); false){ err("Сохранение результата");
					}else if(std::string tmp = num; false){ pre("Для отладки");
					}else if(num = ""; false){ err("Обнуление числа");
					}else{ //mpre("Знак " +tmp +" "+ _val, __LINE__);
					}return false; }()){ err("Сохранение значения");
				}else if([&](){ // Первое значение
					if(string::npos != str.find_last_not_of("<>")){ //pre("Не морфы"); //mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);
					}else if(int index_id = atoi(num.c_str()); (0 == index_id)){ err("Идентификатор морфа не найден");
					}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(index_id)){ err("Морф не найден в справочнике");
					}else if(std::string _v0 = vals.top(); (1 != _v0.length())){ err("Старшее значение");
					}else if(vals.pop(); false){ err("Удаляем младший элемент");
					}else if(std::string _v1 = vals.top(); (1 != _v1.length())){ err("Младшее значение");
					}else if(vals.pop(); false){ err("Удаляем старший элемент");
					}else if(std::string smooth = ("<" == str ? "1" : "0"); (1 != smooth.length())){ err("Расчет прямого значения");
					}else if(std::string reverse = (">" == str ? "1" : "0"); (1 != reverse.length())){ err("Расчет прямого значения");
					}else if(std::string val = ("01" == _v1 +_v0 ? smooth : reverse); (1 != val.length())){ err("ОШИБКА расчета значения");
					}else if(vals.push(val); false){ err("Сохранение результата");
					//}else if(bmf::CACHE[num] = val; bmf::CACHE.empty()){ err("Сохарнение в кеш");
					//}else if(mpre("Расчет морфа " +_v1 + ":"+ _v0  +" " +str + " val=" + val +" CACHE[" +std::to_string(key) +"][" +num +"]", __LINE__); false){ err("Уведомление");
					}else if(num = ""; false){ err("Обнуление числа");
					}else{ //pre("Расчет");
					} return false; }()){ err("Установка первого значения");
				}else{ //mpre("расчет " +str, __LINE__);
				}}return vals; }(); (0 > vals.size())){ mpre("ОШИБКА Расчет значений " +std::to_string(vals.size()), __LINE__);
			}else if(calc = (1 == vals.size() ? vals.top() : ""); false){ err("Получение значения расчетов");
			}else{ //mpre(_BMF_DANO_EX.at(""), "Дано", __LINE__); mpre("Расчет калькуляции "+ calc+ " "+ calculate, __LINE__);
			}return calc; }); false){ err("Расчет калькуляции");
		}else{ return false; //mpre("Функции успешно установлены", __LINE__);
		}return true; }()){ mpre("Не выполнены все условия", __LINE__);
	}else if([&](){ // Основные функции
		if(bmf::Choice = ([&](std::string list, int key ,string itog_id ,TMMs &DANO, TMMb &DANO_BITMAP ,int line){ //mpre("Выбор итога для расширения", __LINE__);
			TMs _dano;
			if(DANO_BITMAP.empty()){ err("Карта бит пуста");
			}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в наборе данных", __LINE__);
			//}else if(static int _key = dataset_count -1; false){ err("Значение последнего ключа");
			}else if(int _key = [&](int _key = 0){ // Значение прошедшего изменения
				if(static TMs KEYS = {} ;false){ err("Статический массив ключей");
				}else if(static TMs _KEYS = {} ;false){ err("Статический массив использованных ключей");
				//}else if(string key_other = (KEYS.end() == KEYS.find(itog_id) ?to_string(dataset_count -1) :KEYS.at(itog_id)); key_other.empty()){ err("Сохраненное значение ключа");
				}else if(string key_other = [&](string key_other = ""){ // Другой ключ
					if(key_other = (KEYS.end() == KEYS.find(itog_id) ?to_string(dataset_count -1) :KEYS.at(itog_id)); to_string(key) != key_other){ //mpre("Нет повторения ключа" ,__LINE__);
					}else if(key_other = (_KEYS.end() == _KEYS.find(itog_id) ?to_string(dataset_count -1) :_KEYS.at(itog_id)); key_other.empty()){ err("Прошлый ключ");
					}else{ //mpre("Повтор ключа key=" +to_string(key) +" key_other=" +key_other ,__LINE__);
					}return key_other; }(); key_other.empty()){ err("Другой ключ");
				}else if(_key = atoi(key_other.c_str()); 0 >_key){ err("Числовое значение ключа");
				}else if(string key_last = (KEYS.end() == KEYS.find(itog_id) ?"" :KEYS.at(itog_id)); to_string(key) == key_last){ //mpre("Не сохраняем повторяющийся ключ" ,__LINE__);
				}else if(KEYS[itog_id] = to_string(key); KEYS.empty()){ err("Сохарнение текущего значения");
				}else if(_KEYS[itog_id] = to_string(_key); _KEYS.empty()){ err("Сохарнение текущего значения");
				}else{ //mpre("Предыдущее значение ключа _key=" +_key_ +" текущее key=" +to_string(key) ,__LINE__);
				}return _key; }(); 0 > _key){ err("Значение прошедшего изменения");
			}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени эпохи", __LINE__);
			}else if(TMMi COUNT = [&](){ // Расчет процентов разнообразия
				static TMMi COUNT; // Считаем только один раз
				if(!COUNT.empty()){ //mpre("Расчет разнообразия создан ранее" ,__LINE__);
				}else if([&](){ for(auto dano_itr:DANO){ // Уровни разнообразия
					if(auto dano_bitmap_itr = DANO_BITMAP.find(dano_itr.first); (DANO_BITMAP.end() == dano_bitmap_itr)){ err("Список значений исходника не найден");
					}else if(int size = dano_bitmap_itr->second.size(); !size){ err("Количество наборов в примере");
					}else if(int count = dano_bitmap_itr->second.count(); false){ err("Количество установленных бит исходника");
					}else if(int res = (size > count *2 ? count : size-count) ; false){ err("Расчет результата");
					}else if(COUNT[res] = [&](TMs cnt = {}){ // Значение в счетчик
						if(cnt = (COUNT.end() == COUNT.find(res) ? TMs({{dano_itr.first ,to_string(count)}}) : cnt); COUNT.end() == COUNT.find(res)){ //mpre("Первоначальное значение" ,__LINE__);
						}else if(cnt = COUNT.at(res); cnt.empty()){ err("Значение списка");
						}else if(cnt.insert(make_pair(dano_itr.first ,to_string(count))); cnt.empty()){ err("Инкремент значения");
						}else{ //mpre("Добавление нового значения res=" +to_string(res) ,__LINE__);
						}return cnt; }(); COUNT.end() == COUNT.find(res)){ err("Добавление значения");
					}else{ //mpre("Исходник " +dano_itr.first +" количество установленных бит " +to_string(count) +" размер " +to_string(dano_bitmap_itr->second.size()) +" результат " +to_string(res) ,__LINE__);
					}}return false; }()){ err("Установка уровня разнообразия");
				}else{ //mpre(COUNT ,"Расчет разнообразия" ,__LINE__);
				}return COUNT; }(); COUNT.empty()){ err("Расчет разнообразия"); //}else if(mpre(COUNT ,"Список расчетов" ,__LINE__); false){ err("Уведомление");
			//}else if(mpre(COUNT ,"Распределение", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(TMs EPT =[&](string list ,TMs EPT ={} ,int pos =0 ,int nn =0 ,int loop =2e9){ while((string::npos != (pos = list.find_last_of(";"))) && (loop-- > 0)){ // Исключение повторяющихся исходников
				if(std::string dano_id =list.substr(pos +1, list.length()); dano_id.empty()){ mpre("ОШИБКА получения идентификатор исходника " +list, __LINE__);
				}else if(list =list.substr(0, pos); false){ mpre("ОШИБКА сокращение длинны списка", __LINE__);
				}else if(EPT.end() != EPT.find(dano_id)){ //mpre("Значение уже в списке");
				}else if(EPT[dano_id] = to_string(++nn); EPT.empty()){ err("Очередное исключение");
				}else{ //mpre("Исключение " +dano_id ,__LINE__);
				}}return EPT; }(list); false){ err("Формирование списка исключений"); //}else if(mpre(EPT ,"Список исключений" ,__LINE__); false){ err("Уведомление");
			}else if(TM3i RES = [&](TM3i RES = {}){ for(auto [res ,LIST]:COUNT){ // Выбор подходящей связи
				if([&](){ for(auto [itog_id ,count]:LIST){ // Исходники разнообразия
					if(DANO_BITMAP.end() == DANO_BITMAP.find(itog_id)){ err("Исходник в списке биткарт не найден");
					}else if(string val_cur = (DANO_BITMAP.at(itog_id).test(key) ? "1" : "0"); val_cur.empty()){ err("Текущее значение");
					}else if(string val_prev = (DANO_BITMAP.at(itog_id).test(_key) ? "1" : "0"); val_prev.empty()){ err("Предыдущее значение");
					}else if(val_cur == val_prev){ //mpre("Значения равны itog_id=" +itog_id ,__LINE__);
					}else if(string grp = val_prev +val_cur; grp.empty()){ err("Группа");
					}else if(bool lock = EPT.end() != EPT.find(itog_id); false){ err("Значение запрета");
					}else if(string nn = (lock ? EPT.at(itog_id) :"0"); false){ err("Порядковый номер в списке list");
					}else if(string _grp = (lock ? "lock_" : "unlock_") +grp; _grp.empty()){ err("Имя группы");
					}else if(int pos = (lock ? atoi(nn.c_str()) : res); false){ err("Важность события");
					}else if(RES[_grp][pos][itog_id] = count; RES.empty()){ err("Установка значения");
					}else{ //mpre("Значение итога res=" +to_string(res) +" [" +_grp +"][" +to_string(pos) +"][" +itog_id +"]=" +count ,__LINE__);
					}}return false; }()){ err("Перебор исходников разнообразия");
				}else{ //mpre("Обзор разнообразия " +to_string(count_itr->first) ,__LINE__);
				}}; return RES; }(); RES.empty()){ mpre("ОШИБКА Выбор подходящей связи itog_id=" +itog_id +" _key=" +to_string(_key) +" key=" +to_string(key) ,__LINE__);
			//}else if(mpre(RES ,"Ресурсы", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(string dano_id = [&](string dano_id = ""){ // Выбор исходника
				if(dano_id = (RES.end() == RES.find("unlock_01") ?"" :RES.at("unlock_01").rbegin()->second.rbegin()->first); !dano_id.empty()){ //mpre(RES.at("unlock_01") ,"Доступны изменения 01" ,__LINE__);
				}else if(dano_id = (RES.end() == RES.find("unlock_10") ?"" :RES.at("unlock_10").rbegin()->second.rbegin()->first); !dano_id.empty()){ //mpre(RES.at("unlock_10") ,"Доступны изменения 10" ,__LINE__);
				}else if(auto [pos_01 ,dano_01_id] = [&](int pos =0 ,string dano_id ="" ,string field = "lock_01"){ // значение изменений
					if(pos = (RES.end() == RES.find(field) ?0 :RES.at(field).begin()->first) ;0 >pos){ err("значение позиции");
					}else if(dano_id = (RES.end() == RES.find(field) ?"" :RES.at(field).begin()->second.rbegin()->first); false){ err("идентификатор дано 01"); //mpre(res.at("unlock_01") ,"доступны изменения 01" ,__line__);
					}else{ //mpre("значение идентификатора " +field ,__LINE__);
					}return make_pair(pos ,dano_id); }() ;0 >pos_01){ err("значение изменений");
				}else if(auto [pos_10 ,dano_10_id] = [&](int pos =0 ,string dano_id ="" ,string field = "lock_10"){ // значение изменений
					if(pos = (RES.end() == RES.find(field) ?0 :RES.at(field).begin()->first) ;0 >pos){ mpre(RES ,"Ресурсы" ,__LINE__); err("значение позиции");
					}else if(dano_id = (RES.end() == RES.find(field) ?"" :RES.at(field).begin()->second.rbegin()->first); false){ mpre(RES ,"Ресурс" ,__LINE__); err("идентификатор дано 01"); //mpre(res.at("unlock_01") ,"доступны изменения 01" ,__line__);
					}else{ //mpre("значение идентификатора " +field ,__LINE__);
					}return make_pair(pos ,dano_id); }() ;0 >pos_01){ err("значение изменений");
				}else if([&](){ // Значение отсутствует
					if(dano_01_id.empty() && dano_10_id.empty()){ err("Значения не найдены");
					}else if(dano_id =(dano_01_id.empty() ?dano_10_id :""); !dano_id.empty()){ //mpre("Отсутствует dano_01_id" ,__LINE__);
					}else if(dano_id =(dano_10_id.empty() ?dano_01_id :""); !dano_id.empty()){ //mpre("Отсутствует dano_10_id" ,__LINE__);
					}else{ //mpre("Оба значения в наличии" ,__LINE__);
					}return false; }(); !dano_id.empty()){ //mpre("Выбор по отсутствующему значению" ,__LINE__);
				}else if(dano_id =(pos_01 > pos_10 ?dano_01_id :dano_10_id); !dano_id.empty()){ //mpre("Расчет разности pos_01=" +to_string(pos_01) +" pos_10=" +to_string(pos_10) ,__LINE__);
				}else{ mpre(RES ,"ОШИБКА нет доступных вариантов pos_01=" +to_string(pos_01) +" dano_01_id=" +dano_01_id +" pos_10=" +to_string(pos_10) +" dano_10_id=" +dano_10_id ,__LINE__);
				}return dano_id; }(); dano_id.empty()){ mpre(RES ,"Ресурсы " +list ,__LINE__); err("Идентификатор исходника");
			}else if(_dano = (DANO.end() == DANO.find(dano_id) ? _dano : DANO.at(dano_id)); _dano.empty()){ err("ОШИБКА установки результата");
			}else{ //mpre(_dano ,"Исходник" ,__LINE__); mpre("ОШИБКА выбор значения" ,__LINE__);
			}return _dano; }); false){ mpre("ОШИБКА установки функции выбора Исходного значения", __LINE__);
		}else if(bmf::Vals = ([&](TMs VALUE, string alias, int key, std::string table, std::string BMF_TITLES){ // Установка входных значений
			TMMs VALS = {};
			for(auto &val_itr:VALUE){ //mpre("VALUE ", __LINE__);
				if(string value = val_itr.second; false){ mpre("ОШИБКА получения значения итератора", __LINE__);
				}else if(TMs values = [&](TMs values = {}){ // Добавление значения
					if(string name = val_itr.first; (0 >= name.length())){ mpre("ОШИБКА получения имени значения", __LINE__);
					}else if(string _val = val_itr.second; false){ mpre("ОШИБКА получения значения из данных", __LINE__);
					}else if(TMs _values = {{"name", name}, /*{"clump_id", bmf::clump_id}, {"value", _val}*/}; _values.empty()){ mpre("ОШИБКА фонмирования нового значения", __LINE__);
					}else if((values = bmf::Up_mysql(table, _values, _values, {}, __LINE__)).empty()){ mpre("ОШИБКА обновления значения", __LINE__);
					}else{ //mpre(values ,"Добавляем новое значение `"+ name+ "` ("+ alias+ ")", __LINE__); //mpre("Добавление значения", __LINE__);
					}return values; }(); values.empty()){ mpre("ОШИБКА добавления значения", __LINE__);
				}else if(TMs bmf_titles = [&](TMs bmf_titles = {}){ // Получение заголовка значения если нет то его создание
					if(regex b("^[0-9|.|-]+$"); regex_match(value, b)){ //mpre("Формат значения `"+ value_itr.first+ "` верен `"+ value_itr.second+ "`", __LINE__);
					}else if(std::string titles_id = [&](std::string titles_id = ""){ // Идентификатор заголовка
						if(values.end() == values.find("id")){ mpre("ОШИБКА заголовок не задан в значении", __LINE__);
						}else if(titles_id = values.at("id") +","+ value; titles_id.empty()){ mpre("ОШИБКА формата заголовока", __LINE__);
						}else{ //mpre("Формирование заголовка "+ titles_id, __LINE__);
						}return titles_id; }(); titles_id.empty()){ err("Формирования идентификатора заголовока");
					}else if(bmf_titles = bmf::Up(BMF_TITLES, {{"id", titles_id}}, {}, {}, __LINE__); !bmf_titles.empty()){ //mpre("Выборка значения записанного ранее", __LINE__);
					}else if([&](){ // Проверка на символ возврата коретки \r
						if(int npos = value.find("\r"); (std::string::npos != npos)){ mpre("ОШИБКА заголовок содержит символ возврат коретки " +val_itr.first +":" + value+"", __LINE__);
						}else if(int npos = value.find("\n"); (std::string::npos != npos)){ mpre("ОШИБКА заголовок содержит символ переноса строки " +val_itr.first +":" + value+"", __LINE__);
						}else{ //mpre("Данные не содержат недопустимых символов", __LINE__);
						} return false; }()){ mpre("ОШИБКА замены переноса строки", __LINE__);							
					}else if(int nn = [&](int nn = 0){ // Установка номера заголовка
						if(TMMi NN; false){ mpre("ОШИБКА создания первоначальной структуры", __LINE__);
						}else if([&](){ for(auto titles_itr:bmf::List(BMF_TITLES, {}, __LINE__)){ //for_each(TITLES.begin(), TITLES.end(), [&](auto &titles_itr){
							if(TMs titles = titles_itr.second; titles.empty()){ mpre("ОШИБКА получения заголовка из списка", __LINE__);
							}else if(titles.end() == titles.find("value")){ mpre(titles, "Заголовок", __LINE__); err("Значение заголовока не установлено");
							}else if(int _nn = atoi(titles.at("value").c_str()); !_nn){ mpre("ОШИБКА расчета значения", __LINE__);
							}else if(NN.insert(make_pair(_nn, titles)); NN.empty()){ mpre("ОШИБКА добавления значения в список", __LINE__);
							}else{ //mpre("Создаем список категорий", __LINE__);
							}}; return false; }()){ mpre("ОШИБКА получени списка по номерам", __LINE__);
						}else if(nn = (NN.empty() ? -1 : NN.begin()->first-1); (0 <= nn)){ mpre("ОШИБКА нахождения следующего номера "+ to_string(nn), __LINE__);
						}else{ //mpre("Определение следующего значения заголовока " +to_string(nn), __LINE__);
						}return nn; }(); !nn){ mpre("ОШИБКА определения номера заголовка", __LINE__);
					}else if(TMs _bmf_titles = {/*{"clump_id", bmf::clump_id},*/ {alias+ "_values_id", values["id"]}, {"value", to_string(nn)}, {"name", value}}; _bmf_titles.empty()){ mpre("ОШИБКА получения нового заголовка", __LINE__);
					}else if((bmf_titles = bmf::Up(BMF_TITLES, {{"id", titles_id}}, _bmf_titles, _bmf_titles, __LINE__)).empty()){ mpre("ОШИБКА обновления значения заголовка", __LINE__);
					}else{ //mpre(bmf_titles, __LINE__, "Заголовок"); //mpre("ОШИБКА Установлено наличие заголовка", __LINE__);
					} return bmf_titles; }(); false){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if(string _value = bmf::Dec2bin(stod(bmf_titles.end() == bmf_titles.find("value") ? value : bmf_titles.at("value"))); (0 >= _value.length())){ mpre("ОШИБКА получения строки сиволов двоичного розультата "+ value, __LINE__);
				}else if(string _value_ = ("-" == _value.substr(0, 1) ? _value : "+"+ _value); (0 >= _value.length())){ mpre("ОШИБКА получения значения без знака", __LINE__);
				}else if(int pos = _value_.rfind("."); false){ mpre("ОШИБКА получения позиции точки", __LINE__);
				}else if([&](){ // //mpre("Добавляем новый знак "+ values["name"]+ " "+ to_string(VALS.size())+ "=>"+ to_string(_value.length())+ " "+ _value+ " ("+ to_string(value)+ ")" , __LINE__); //system("sleep 1");
					for(int i = 0; i < _value_.length(); i++){ // Добавление знаков значению
						if(string val = _value_.substr(i, 1); (1 != val.length())){ mpre("ОШИБКА получения символа "+ to_string(i), __LINE__);
						}else if("." == val){ //mpre("Символ отрицания "+ val+ " не должен попадаться", __LINE__);
						}else if([&](){ // Расчет значения отрицания
							if(0 < i){ //mpre("Вторая и последующие позиции", __LINE__);
							}else if(("-" != val) && ("+" != val)){ mpre("ОШИБКА Симовл не является знаком _value_="+ _value_+ " i="+ to_string(i)+ " val="+ val, __LINE__);
							}else if(val = ("-" == _value.substr(0, 1) ? "1" : ""); false){ mpre("ОШИБКА расчета знака числа", __LINE__);
							}else{ //mpre(vals, "ОШИБКА Расчет знака "+ value, __LINE__);
							}return (1 != val.length()); }()){ //mpre("Отмена создания отрицательного знака", __LINE__);
						}else if("-" == val){ mpre("ОШИБКА Символ отрицания "+ val+ " не должен попадаться", __LINE__);
						}else if("+" == val){ mpre("ОШИБКА Символ отрицания "+ val+ " не должен попадаться", __LINE__);
						}else if(int nn = [&](int nn = 0){ // Расчет номера в списке
							if(0 == i){ nn = 0; //mpre("Знак отрицания "+ to_string(nn), __LINE__);
							}else if(0 > pos){ nn = _value_.length()-i; //mpre("Число без точки "+ to_string(nn), __LINE__);
							}else if(i == pos){ nn = 0; mpre("ОШИБКА Точка "+ to_string(nn), __LINE__);
							}else if(i < pos){ nn = pos-i; //mpre("Целая часть "+ to_string(nn), __LINE__);
							}else if(i > pos){ nn = -i+pos; //mpre("Дробная часть "+ to_string(nn), __LINE__);
							}else{ mpre("ОШИБКА получение значения источника/результата", __LINE__);
							} return nn; }(); false){ mpre("ОШИБКА результат точка", __LINE__);
						}else if(TMs vals = [&](TMs vals = {}){ // Проверка и выборка знака
							if(false){ mpre("Пропуск", __LINE__);
							}else if(std::string vals_id = values.at("id") +"-" +to_string(nn); vals_id.empty()){ mpre("ОШИБКА составления идентификатора значения", __LINE__);
							}else if(TMs _vals = {{alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}, {"val", ""}/*, {"values", ""}*/}; _vals.empty()){ mpre("ОШИБКА создания нового знака", __LINE__);
							}else if((vals = bmf::Up_mysql(alias, {{alias+ "_values_id", values.at("id")} ,{"name" ,to_string(nn)}}, _vals, {}, __LINE__)).empty()){ mpre(_vals, "Знак", __LINE__); mpre("ОШИБКА обновления знака", __LINE__);
							}else{ //mpre(values ,"Добавление нового знака " +to_string(nn) +" в базу " +vals_id, __LINE__); //mpre("ОШИБКА добавление глобального знака", __LINE__); //mpre(*BMF, __LINE__, "Список знаков"); mpre("Добавление глобального знака `"+ values["name"]+ "` ["+ vals.at("id")+ "] "+ to_string(nn), __LINE__); mpre(vals, __LINE__, "Знак"); //mpre("ОШИБКА добавления знака "+ alias+ " "+ vals.at("id"), __LINE__);
							} return vals; }(); false){ mpre("ОШИБКА Добавления нового знака", __LINE__);
						}else if([&](){ // Установка значения
							if(vals.empty()){ //mpre("Отрицательный знак", __LINE__);
							}else if(VALS[ values.at("id") ][ vals.at("id") ] = val; (1 != val.length())){ err("ОШИБКА установки значения знака");
							}else{ //mpre("Установка значения values_id=" +values.at("id") +" vals_id=" +vals.at("id") +" val=" +val, __LINE__);
							}return false; }()){ mpre("ОШИБКА установки значения", __LINE__);
						}else{ //mpre("Установка знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); //mpre(_vals, __LINE__, "Добавление нового знака "+ to_string(i));
						}
					}return false; }()){ mpre("ОШИБКА добавления знаков значению", __LINE__);
				}else{ //mpre(BMF_VALS.at(""), __LINE__, "Список знаков"); //mpre(BMF_VALS, __LINE__, "Знаки"); mpre("ОШИБКА Знака "+ alias, __LINE__); //mpre(values, __LINE__, "Значение"); mpre(VALS, __LINE__, "Значения");
				}
			}return VALS; }); false){ mpre("ОШИБКА установки значений", __LINE__);
		}else if(bmf::Value = ([&](TMs itog_values){ // Расчет итога
			if(TMMi ITOG = rb(BMF_ITOG_EX, {{"itog_values_id", itog_values.at("id")}}); ITOG.empty()){ mpre("ОШИБКА получения списка знаков значения", __LINE__); //mpre(itog_values, __LINE__, "Значение"); mpre(BMF_ITOG_EX.at(""), __LINE__, "ОШИБКА получения списка знаков значения");
			}else if(TMMi INDEX; false){ mpre("ОШИБКА создания списка морфов", __LINE__);
			}else if([&](){ for(auto itog_itr:ITOG){ //for_each(ITOG.begin(), ITOG.end(), [&](auto itog_itr){ // Сортировка списка
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА получения итога", __LINE__);
				}else if(itog.end() == itog.find("index_id")){ mpre("ОШИБКА поле index_id у итога не задано", __LINE__);
				}else if("" == itog.at("index_id")){ mpre(itog, "Итог", __LINE__); mpre("Пустое поле у итога index_id", __LINE__);
				}else if(TMs index = erb(BMF_INDEX_EX, {{"id", itog.at("index_id")}}); index.empty()){ //mpre("ОШИБКА получения морфа итога", __LINE__);
				}else if(TMs dano = erb(BMF_DANO_EX, {{"id", index.at("dano_id")}}); dano.empty()){ mpre("ОШИБКА выборки исходника морфа", __LINE__);
				}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА поле исходника val не устанволено", __LINE__);
				}else if("" == dano.at("val")){ mpre("ОШИБКА значение исходника морфа не задано", __LINE__);
				/*}else if(index["val"] = [&](std::string calc = ""){ // Быстрая проверка
					if(std::string calculate = (bmf::CALCULATE.end() == bmf::CALCULATE.find(itog.at("id")) ? "" : bmf::CALCULATE.at(itog.at("id"))); false){ //pre("Удаленная ранее формула");
					}else if(calculate = ("" == calculate ? bmf::Calculate(index) : calculate); calculate.empty()){ err("Расчет формулы калькуляции");
					}else if(bmf::CALCULATE[itog.at("id")] = calculate; bmf::CALCULATE.empty()){ err("Сохранение значений калькуляции");
					}else if(calc = bmf::Calc(calculate, BMF_DANO_EX); (1 != calc.length())){ mpre("ОШИБКА Расчета Калькуляции " +calc +" " +calculate, __LINE__);
					}else{ //mpre("Быстрая проверка calculate="+ calculate, __LINE__);
					} return calc; }(); (1 != index.at("val").length())){ err("Расчет формулы"); //mpre("Пропуск быстрой проверки calc=" +calc +" itog.at(val)=" +itog.at("val"), __LINE__);*/
				}else if(mpre("Расчет "+ itog.at("id")+ " "+ itog_values.at("name")+ " ("+ itog.at("name")+ ") index["+ index.at("id")+ "]="+ index["val"] , __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(INDEX.insert(make_pair(atoi(itog.at("name").c_str()), index)); INDEX.empty()){ mpre("ОШИБКА добавления итогов к сортированному списку", __LINE__);
				}else{ //mpre(itog, __LINE__, "Итог");
				}}return ITOG.empty(); }()){ mpre("ОШИБКА получения сортированного списка значений", __LINE__);
			}else if(string bin = ""; false){ mpre("ОШИБКА создания двоичной переменной", __LINE__);
			}else if([&](){ for(auto &index_itr:INDEX){ //for_each(INDEX.begin(), INDEX.end(), [&](auto index_itr){ // Установка знаков в значение
				if(TMs index = index_itr.second; index.empty()){ mpre("ОШИБКА получения индекса", __LINE__);
				}else if(int pos = index_itr.first; false){ mpre("ОШИБКА получения позиции знака", __LINE__);
				}else if(int os = INDEX.rbegin()->first-pos; (0 > os)){ mpre("ОШИБКА получения смещения (ноль и больше)", __LINE__);
				}else if(bin += (os >= bin.length() ? std::string(os-bin.length()+1, '-') : ""); (os >= bin.length())){ mpre("ОШИБКА увеличения длинны строки до нужного размера", __LINE__);
				}else if(string _val = (0 == pos ? "." : index.at("val")); (0 >= _val.length())){ mpre("ОШИБКА получения символа знака", __LINE__);
				}else if([&](){ string _bin = bin; bin = bin.substr(0, os)+ _val+ bin.substr(os+1, bin.length()); return (0 >= bin.length()); }()){ mpre("ОШИБКА установки символа знака", __LINE__);
				}else{ //mpre("Расчеты позиции и смещения pos="+ to_string(pos)+ " os="+ to_string(os)+ " length="+ to_string(bin.length())+ " val="+ val, __LINE__);
				}}; return INDEX.empty(); }()){ mpre("Морфы значения не установлены `"+ itog_values["name"]+ "`", __LINE__);
			}else if(bin = ((INDEX.find(0) != INDEX.end()) && (INDEX.at(0).at("val") == "1") ? "-" : "")+ bin; (0 >= bin.length())){ mpre("ОШИБКА установки символа отрицания", __LINE__);
			}else if(double dec = bmf::Bin2dec(bin); false){ mpre("ОШИБКА конвертации двоичной в десятичную систему", __LINE__);
			}else if(dec = bmf::Bin2dec(bin); false){ mpre("ОШИБКА перевода двоичной строки в десятичное число", __LINE__);
			}else if(string value = to_string(dec); false){ mpre("ОШИБКА установки значения расчета", __LINE__);
			}else if(itog_values["value"] = value; itog_values.empty()){ mpre("ОШИБКА получения конвертируемого значения", __LINE__);
			}else{ //mpre("Значение bin="+ bin+ " dec="+ to_string(dec), __LINE__);
			}return itog_values; }); false){ mpre("ОШИБКА создания функции расчета итога", __LINE__);
		}else{
		}return false; }()){ mpre("ОШИБКА создания рабочих функций", __LINE__);
	}else if([&](){ // Расчет исходников
		if(in.empty()){ //mpre("Данные для расчета не указаны", __LINE__);
		}else if(bmf::ARGV.end() == bmf::ARGV.find("-")){ //mpre("Не устанавливаем данные -", __LINE__);
		}else if([&](){ // Перемешивание
			if(1 >= in.size()){ //mpre("Не перемешиваем одно значение", __LINE__);
			}else if(in.is_object()){ mpre("Обьект не перемешиваем", __LINE__);
			}else if(string random = (bmf::ARGV.end() == bmf::ARGV.find("rand") ? "0" : bmf::ARGV.at("rand")); (0 >= random.length())){ mpre("ОШИБКА аргумент перемешивания не задан -rand", __LINE__);
			}else if(string::npos != random.find_last_not_of("0123456789")){ mpre("ОШИБКА формат перемешивания задан неправильно "+ random, __LINE__);
			}else if("0" == random){ //mpre("Перемешивание отключено -rand " +random, __LINE__);
			}else if(std::random_device rd;  false){ mpre("ОШИБКА создания случайного значения", __LINE__);
			}else if(std::mt19937 g(rd()); false){ mpre("ОШИБКА инициации случайного значения", __LINE__);
			}else if(shuffle(in.begin(), in.end(), g); false){ mpre("ОШИБКА перемешивания массива", __LINE__);
			}else{ mpre("Перемешиваем список -rand "+ random, __LINE__);
			} return false; }()){ mpre("ОШИБКА перемешивания списка", __LINE__);
		}else if(TMMi DANO = {}; false){ mpre("ОШИБКА создания временного хранилища результатов исходника", __LINE__);
		}else if(TMMi ITOG = {}; false){ mpre("ОШИБКА создания временного хранилища результатов итогов", __LINE__);
		}else if(auto microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if([&](){ for(auto &js:in.items()){ // Расчет карты
			if(auto el = js.value(); el.empty()){ mpre("ОШИБКА элемент не найден", __LINE__);
			}else if(int key = atoi(js.key().c_str()); (0 > key)){ mpre("ОШИБКА расчета номера обучающего примера", __LINE__);
			}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - microtime; false){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(bmf::Progress("Расчет обучающих примеров " +to_string(key+1) +" (" + to_string(_microtime) +" сек.)", (float)(key+1)/in.size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(TMs dano = TMs(el["dano"]); dano.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
			}else if(TMMs _DANO_ = bmf::Vals(dano, "dano", key, bmf::DANO_VALUES, bmf::DANO_TITLES); false){ mpre("ОШИБКА установки входящих значений", __LINE__);
			//}else if(mpre(dano ,"Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if([&](){ for(auto dano_itr:bmf::List(bmf::DANO, {}, __LINE__)){ // Сохранение результатов
				if(dano_itr.second.end() == dano_itr.second.find("id")){ mpre("ОШИБКА поле идентификатора не найдено", __LINE__);
				}else if(std::string id = dano_itr.second.at("id"); id.empty()){ mpre("ОШИБКА получения идентификатора итога", __LINE__);
				}else if(_DANO_.end() == _DANO_.find(dano_itr.second.at("dano_values_id"))){ err("Результаты значения не найдены");
				}else if(_DANO_.at(dano_itr.second.at("dano_values_id")).end() == _DANO_.at(dano_itr.second.at("dano_values_id")).find(dano_itr.second.at("id"))){ //mpre("Результаты значения не найдены dano_values_id=" +dano_itr.second.at("dano_values_id") + " id=" +dano_itr.second.at("id"), __LINE__);
				}else if(std::string val = _DANO_.at(dano_itr.second.at("dano_values_id")).at(dano_itr.second.at("id")); val.empty()){ mpre("ОШИБКА выборки знака", __LINE__);
				}else if(DANO[key][id] = val; false){ mpre("ОШИБКА сохарнения значения расчета", __LINE__);
				}else{ //mpre("Сохранения сигнала расчета", __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения результатов", __LINE__);
			}else if(TMs itog = TMs(el["itog"]); itog.empty()){ mpre("ОШИБКА получения итоговых знаков", __LINE__);
			}else if(TMMs _ITOG_ = bmf::Vals(itog, "itog", key, bmf::ITOG_VALUES, bmf::ITOG_TITLES); false){ mpre("ОШИБКА установки входящих значений", __LINE__);
			//}else if(mpre("Расчет карты", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if([&](){ for(auto& itog_itr:bmf::List(bmf::ITOG, {}, __LINE__)){ // Сохранение результатов
				if(itog_itr.second.end() == itog_itr.second.find("id")){ mpre("ОШИБКА поле идентификатора не найдено", __LINE__);
				}else if(std::string id = itog_itr.second.at("id"); id.empty()){ mpre("ОШИБКА получения идентификатора итога", __LINE__);
				}else if(_ITOG_.end() == _ITOG_.find(itog_itr.second.at("itog_values_id"))){ err("Результаты значения не найдены");
				}else if(_ITOG_.at(itog_itr.second.at("itog_values_id")).end() == _ITOG_.at(itog_itr.second.at("itog_values_id")).find(itog_itr.second.at("id"))){ //mpre("Результаты значения не найдены", __LINE__);
				}else if(std::string val = _ITOG_.at(itog_itr.second.at("itog_values_id")).at(itog_itr.second.at("id")); val.empty()){ mpre("ОШИБКА выборки знака", __LINE__);
				}else if(ITOG[key][id] = val; false){ mpre("ОШИБКА сохарнения значения расчета", __LINE__);
				}else{ //mpre("Сохранения сигнала расчета", __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения результатов", __LINE__);
			}else{ //mpre("Расчет " +to_string(key), __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА расчета карты", __LINE__);
		}else if(int dataset_count = in.size(); !dataset_count){ err("Размер обучающей выборки");
		}else if([&](){ // Установка набора данных
			if(std::string dataset_id = md5(bmf::ARGV.at("-")); dataset_id.empty()){ mpre("ОШИБКА расчета хеша набора обучающих данных", __LINE__);
			}else if(TMs _dataset = {{"count", to_string(dataset_count)}, {"md5", dataset_id}, {"key", "0"}}; _dataset.empty()){ mpre("ОШИБКА создания записи нового набора данных", __LINE__);
			}else if((bmf::dataset = bmf::Up_mysql(bmf::DATASET, {/*{"id", dataset_id}*/}, _dataset, _dataset, __LINE__)).empty()){ mpre("ОШИБКА добавления набора данных", __LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("id")){ mpre("ОШИБКА набор данных не содержит поля идентификатора", __LINE__);
			}else{ mpre("Набор данных " +bmf::dataset.at("id"), __LINE__);
			}return bmf::dataset.empty(); }()){ mpre("ОШИБКА устанвоки набора данных", __LINE__);
		}else if(TMMs _DANO = bmf::List(bmf::DANO, {}, __LINE__); _DANO.empty()){ err("Список исходников");
		}else if([&](int progress = 0){ for(auto dano_itr:_DANO){ // Двоичная карта исходников
			if(bmf::Progress("Индекс исходных сигналов " +to_string(progress), (float)++progress/_DANO.size(), __LINE__); false){ err("Индикатор прогресса");
			}else if(TMs dano = dano_itr.second; dano.empty()){ err("Выборка исходника");
			}else if(std::string dano_id = dano_itr.second.at("id"); dano_id.empty()){ mpre("ОШИБКА получения идентификатора дано", __LINE__);
			}else if(boost::dynamic_bitset<> bit(dataset_count); false){ mpre("ОШИБКА создания бита исходника", __LINE__);
			}else if([&](){ for(auto &js:in.items()){ // Количество обучающих примеров для исходника
				if(int key = atoi(js.key().c_str()); (0 > key)){ mpre("ОШИБКА расчета номера обучающего примера", __LINE__);
				}else if(DANO.end() == DANO.find(key)){ mpre("ОШИБКА расчет примера не найден", __LINE__);
				}else if(std::string val = (DANO.at(key).end() == DANO.at(key).find(dano_id) ? "0" : DANO.at(key).at(dano_id)); (1 != val.length())){ mpre("ОШИБКА выборки значения дано", __LINE__);
				}else if(bit[key] = atoi(val.c_str()); false){ mpre("ОШИБКА установки бита", __LINE__);
				}else{ //mpre(_dano, "Дано " +val, __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения обучающих примеров", __LINE__);
			}else if(std::string map = [&](std::string map = ""){ boost::to_string(bit, map); return map; }(); (map.length() != dataset_count)){ mpre("ОШИБКА установки карты исходника", __LINE__);
			}else if(std::string dataset_map_id = "dano," +dano.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
			}else if(TMs _dataset_map = {{"id", dataset_map_id}, {"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА создания карты набора данных", __LINE__);
			}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map, __LINE__); dataset_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
			}else{ //mpre("Установка карты морфа "+ dataset_map_id, __LINE__);//mpre("Исходник " +dano_itr.second.at("id") +" " +dano_itr.second.at("map"), __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА составления двоичной карты", __LINE__);
		}else if(TMMs _ITOG = bmf::List(bmf::ITOG, {}, __LINE__); _ITOG.empty()){ err("Список итогов");
		}else if([&](int progress = 0){ for(auto& itog_itr:_ITOG){ // Двоичная карта итогов
			if(bmf::Progress("Индекс итоговых сигналов " +to_string(progress), (float)++progress/_ITOG.size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
			}else if(std::string itog_id = itog_itr.second.at("id"); itog_id.empty()){ mpre("ОШИБКА получения идентификатора итога", __LINE__);
			}else if(boost::dynamic_bitset<> bit(dataset_count); false){ mpre("ОШИБКА создания бита исходника", __LINE__);
			}else if([&](){ for(auto& js:in.items()){ // Количество обучающих примеров для исходника
				if(int key = atoi(js.key().c_str()); (0 > key)){ mpre("ОШИБКА расчета номера обучающего примера", __LINE__);
				}else if(ITOG.end() == ITOG.find(key)){ mpre("ОШИБКА расчет примера не найден", __LINE__);
				}else if(std::string val = (ITOG.at(key).end() == ITOG.at(key).find(itog_id) ? "0" : ITOG.at(key).at(itog_id)); (1 != val.length())){ mpre("ОШИБКА выборки значения дано", __LINE__);
				}else if(bit[key] = atoi(val.c_str()); false){ mpre("ОШИБКА установки бита", __LINE__);
				}else{ //mpre(_dano, "Дано " +val, __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения обучающих примеров", __LINE__);
			}else if(std::string map = [&](std::string map = ""){ boost::to_string(bit, map); return map; }(); (map.length() != dataset_count)){ mpre("ОШИБКА установки карты итога", __LINE__);
			}else if(std::string dataset_map_id = "itog," +itog.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
			}else if(TMs _dataset_map = {{"itog_id", itog.at("id")}, {"dataset_id", bmf::dataset.at("id")}, {"alias", "itog"}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА создания карты набора данных", __LINE__);
			}else if(bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map, __LINE__).empty()){ mpre("ОШИБКА обновления набора данных", __LINE__);
			}else{ //mpre("Итог " +itog_itr.second.at("id") +" " +itog_itr.second.at("map"), __LINE__); //mpre(dano_itr.second, "Исходник", __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА составления двоичной карты", __LINE__);
		}else{ //mpre(DANO, "Расчет итогов", __LINE__);
		}return false; }()){ mpre("ОШИБКА расчета исходников", __LINE__);
	}else if(bmf::dataset = [&](){ // Выборка набора данных для расчета
		if(false){ mpre("Пропуск определения набора данных", __LINE__);
		}else if(bmf::ARGV.end() != bmf::ARGV.find("dano")){ //mpre("Расчетный пример", __LINE__);
		}else if(std::string ds = (bmf::ARGV.end() == bmf::ARGV.find("ds") ? "" : bmf::ARGV.at("ds")); false){ //mpre("ОШИБКА выборки идентификатора набора данных", __LINE__);
		}else if(bmf::dataset = (ds.empty() ? bmf::dataset : bmf::Up(bmf::DATASET, {{"id", ds}}, {}, {}, __LINE__)); false){ //mpre({{"id", ds}}, "Условия выборки набора данных " +to_string((*bmf::DATASET).size()), __LINE__); mpre("ОШИБКА выборки набора данных", __LINE__);
		}else if([&](){ // Установка ключа
			if(bmf::dataset.empty()){ //mpre("Набор данных не пуст", __LINE__);
			}else if(std::string key = (bmf::ARGV.end() == bmf::ARGV.find("key") ? "" : bmf::ARGV.at("key")); key.empty()){ //mpre("Значение ключа в консоли не задано" ,__LINE__);
			}else if(string::npos != key.find_first_not_of("0123456789")){ //mpre("ОШИБКА формат ключа задан неправильно key=" +key ,__LINE__);
			}else if(int _key = atoi(key.c_str()); 0 > _key){ err("Числовое значение ключа");
			}else if(bmf::dataset["key"] = to_string(_key); bmf::dataset.empty()){ err("Установка значения ключа в обучающий набор");
			}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", bmf::dataset.at("id")}}, {}, bmf::dataset, __LINE__); bmf::dataset.empty()){ err("Обновление позиции расчетов");
			}else{ mpre("Обновление позиции расчетов ds=" +bmf::dataset.at("id") +" key=" +bmf::ARGV.at("key"), __LINE__);
			}return false; }()){ err("Установка ключа");
		}else if(!bmf::dataset.empty()){ //mpre("Набор данных не пуст", __LINE__);
		}else if(bmf::ARGV.end() != bmf::ARGV.find("-")){ //mpre("Не выводим статистику на загузке набора данных", __LINE__);
		}else if(!ds.empty()){ mpre("Набора данных не найден в базе "+ ds, __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> ITOG; false){ mpre("Справочник карт", __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> INDEX; false){ mpre("Справочник карт", __LINE__);
		}else if(TMMs DATASET = bmf::List(bmf::DATASET, {}, __LINE__); DATASET.empty()){ mpre("Наборов не найдено" ,__LINE__);
		}else if([&](){ for(auto [id ,dataset]:DATASET){ // Вывод списка набора данных
			//if(TMs dataset = dataset_itr.second; dataset.empty()){ mpre("ОШИБКА выборки набора данных", __LINE__);
			if(dataset.end() == dataset.find("count")){ mpre("ОШИБКА поле количества примеров не установлено", __LINE__);
			}else if(int dataset_count = atoi(dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в наборе данных", __LINE__);
			}else if(TMMs _ITOG_ = bmf::List(bmf::ITOG, {}, __LINE__); _ITOG_.empty()){ err("Выборка списка итогов");
			}else if(DATASET[id]["diff"] = dataset["diff"] = [&](int ers = 0, int count = 0){ for(auto itog_itr:_ITOG_){ // Расчет результата поитогово
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
				}else if(std::string dataset_map_id = "itog," +itog.at("id") +"," +dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
				}else if(TMs itog_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); itog_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
				}else if(boost::dynamic_bitset<> gmap(itog_map.at("map")); gmap.empty()){ mpre("ОШИБКА установки карты итога", __LINE__);
				}else if(ITOG[itog.at("id")] = gmap; ITOG.empty()){ mpre("ОШИБКА сохранения карты итога", __LINE__);
				}else if(dataset.end() == dataset.find("id")){ err("Не найден идентификатор набора данных");
				}else if(std::string index_id = (itog.end() == itog.find("index_id") ? "0" : itog.at("index_id")); false){ err("ОШИБКА расчета идентификатора морфа");
				}else if(std::string dataset_map_id = "index," +index_id +"," +dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
				}else if(TMs index_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
				}else if(std::string map = (index_map.end() == index_map.find("map") ? std::string(dataset_count, '0') : index_map.at("map")); map.empty()){ mpre("ОШИБКА расчета карты", __LINE__);
				}else if(boost::dynamic_bitset<> imap(map); imap.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
				}else if(INDEX[itog.at("id")] = imap; INDEX.empty()){ mpre("ОШИБКА сохраенения карты морфа", __LINE__);
				}else if(auto temp = gmap ^ imap; temp.empty()){ mpre("Получение разнциы результатов", __LINE__);
				}else if(ers += temp.count(); (0 > ers)){ mpre("ОШИБКА инкремента разницы", __LINE__);
				}else if(count += dataset_count; (0 > count)){ mpre("ОШИБКА расчета количества сигналов", __LINE__);
				}else{ //mpre("Карта itog " +itog_map.at("map"), __LINE__); mpre("Карта index " +index_map.at("map"), __LINE__); mpre("Разница count=" +to_string(count) +" ers=" +to_string(ers), __LINE__);
				}}return to_string(!ers ? 1 : (float)(count-ers)/count); }(); dataset.end() == dataset.find("diff")){ mpre("ОШИБКА расчета результата набора данных", __LINE__);
			}else if(float perc = [&](int count = 0){ for(int i = 0; i < dataset_count; i++){ // Совпадение по значениям
				if(int ers = [&](int ers = 0){ for( auto itog_itr:_ITOG_){ // Проверка всех итогов
					if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
					}else if(itog.end() == itog.find("id")){ mpre("ОШИБКА у итога не найдено поле идентификатора", __LINE__);
					}else if(ITOG.end() == ITOG.find(itog.at("id"))){ mpre("ОШИБКА в справочнике карт итог не найден " +itog.at("id"), __LINE__);
					}else if(boost::dynamic_bitset<> gmap = ITOG.at(itog.at("id")); gmap.empty()){ mpre("ОШИБКА восстановления карты итога", __LINE__);
					}else if(INDEX.end() == INDEX.find(itog.at("id"))){ mpre("ОШИБКА В справочнике карт морфа итог не найден", __LINE__);
					}else if(boost::dynamic_bitset<> imap = INDEX.at(itog.at("id")); imap.empty()){ mpre("ОШИБКА восстановления карты морфа итога", __LINE__);
					}else if(gmap.test(i) == imap.test(i)){ //mpre("Значения равны", __LINE__);
					}else if(ers++; (0 >= ers)){ mpre("Инкремент ошибки", __LINE__);
					}else{ //mpre("Не совпадение itog_id=" +itog.at("id") +" i=" +to_string(i), __LINE__);
					}}return ers; }(); (0 > ers)){ mpre("ОШИБКА расчета значения", __LINE__);
				}else if(count += (ers ? 0 : 1); (0 > count)){ mpre("ОШИБКА количество правильных ответов", __LINE__);
				}else{ //mpre("Количество правильных ответов i=" +to_string(i) +" dataset_count=" +to_string(dataset_count) +" count=" +to_string(count) +" ers=" +to_string(ers), __LINE__);
				}}return (float)(count*100)/dataset_count; }(); (0 > perc)){ mpre("ОШИБКА расчета процента полных совпадений по значениям", __LINE__);
			}else if(DATASET[id]["perc"] = dataset["perc"] = [&](string _perc = ""){ char chr[10]; sprintf(chr ,"%.3f" ,perc); return string(chr); }(); dataset.end() ==dataset.find("perc")){ mpre("ОШИБКА расчета строки процента ошибки", __LINE__);
			}else if(DATASET[id]["epoch"] = dataset["epoch"] = [&](float epoch = 0){ // Расчет эпох
				if(dataset.end() == dataset.find("epoch")){ err("Поле эпох не задано");
				}else if(epoch = atoi(dataset.at("epoch").c_str()); 0 > epoch){ err("Целое количество эпох");
				}else if(dataset.end() == dataset.find("key")){ err("Поле позиции не задано");
				}else if(epoch += ((float)atoi(dataset.at("key").c_str())/ dataset_count); 0 > epoch){ err("Дробное количество эпох");
				}else{ //mpre("Эпох dataset.at(epoch)=" +dataset.at("epoch") +" dataset_count=" +to_string(dataset_count) +" key=" +dataset.at("key") +" epoch=" +to_string(epoch), __LINE__);
				}return to_string(epoch); }(); false){ err("Расчет эпох");
			}else if(string epoch = (!atof(dataset["epoch"].c_str()) ?"" :" эпох:" +dataset["epoch"]); false){ err("Строковое значение эпох");
			}else{ mpre("Набор " +dataset["id"] +" количество:"+ dataset["count"] +" точность:"+ dataset["diff"] +" (" +dataset["perc"] +"%)" +epoch, __LINE__);
			}}return false; }()){ mpre("ОШИБКА отображения списка набора данных", __LINE__);
		}else if([&](){ // Полная статистика
			if(string count = [&](int count =0){ for(auto [id ,dataset]:DATASET){ count += atoi((dataset.end() ==dataset.find("count") ?"0" :dataset.at("count")).c_str()); }return to_string(count); }(); count.empty()){ err("Количество примеров в наборе");
			}else if(string diff = [&](float diff =0){ for(auto [id ,dataset]:DATASET){ diff += atof((dataset.end() ==dataset.find("diff") ?"0" :dataset.at("diff")).c_str()); }return to_string((float)diff /DATASET.size()); }(); diff.empty()){ err("Количество двоичных совпадений в наборе");
			}else if(float perc = [&](float perc =0){ for(auto [id ,dataset]:DATASET){ perc += atof((dataset.end() ==dataset.find("perc") ?"0" :dataset.at("perc")).c_str()); }return (float)perc /DATASET.size(); }(); !perc){ err("Количество полных совпадений в наборе");
			}else if(string _perc = [&](string _perc = ""){ char chr[10]; sprintf(chr ,"%.3f" ,perc); return string(chr); }(); _perc.empty()){ mpre("ОШИБКА расчета строки процента ошибки", __LINE__);
			}else if(string epoch = [&](float epoch =0){ for(auto [id ,dataset]:DATASET){ epoch += atof((dataset.end() ==dataset.find("epoch") ?"0" :dataset.at("epoch")).c_str()); }return to_string(epoch); }(); epoch.empty()){ err("Количество эпох в наборе");
			}else{ mpre(string(65 ,'-') ,__LINE__); mpre("Наборов " +to_string(DATASET.size()) +" количество:" +count +" точность:" +diff +" (" +_perc +"%) эпох:" +epoch +" " ,__LINE__);
			}return false; }()){ err("Полная статистика");
		}else{ //mpre("Размер модели ", __LINE__);
		}return bmf::dataset; }(); false){ err("Набор данных не установлен");
	}else if(int loop = [&](int loop = 0){ // Количетсво повторов
		if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); (0 > epoch)){ mpre("ОШИБКА расчета количества эпох", __LINE__);
		}else if(bmf::ARGV.end() == bmf::ARGV.find("epoch")){ //mpre("Для обучения укажите количество эпох epoch", __LINE__);
		}else if(!epoch){ loop = 1; //mpre("Указано нулевое количество эпох расчитываем до полного совпадения", __LINE__);
		}else if(loop = epoch){ //mpre("Набор данных -ds " +bmf::dataset.at("id")+ " количество эпох -epoch " +to_string(loop), __LINE__);
		}else{ mpre("ОШИБКА расчтеа количества повторений", __LINE__);
		}return loop; }(); false){ mpre("ОШИБКА расчета колчиества повторений", __LINE__);
	}else if(int errors = [&](int errors = 0, bool pass = false ,TMMs BMF_INDEX = {}){ do{ // Итерации обучения
		if(bmf::loop++ >= loop){ //mpre("Остановка по максимальному количеству эпох", __LINE__);
		}else if(bmf::dataset.empty()){ mpre("ОШИБКА набор данных не установлен", __LINE__);
		}else if(int dataset_count = atoi(bmf::dataset["count"].c_str()); (0 >= dataset_count)){ mpre("ОШИБКА нулевое количество в наборе данных", __LINE__);
		}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if(TMMs DANO = bmf::List(bmf::DANO, {}, __LINE__); DANO.empty()){ err("Список итогов");
		//}else if(mpre(DANO ,"Исходники" ,__LINE__); false){ err("");
		}else if(TMMb DANO_BITMAP = [&](TMMs DATASET_MAP, TMMb DANO_BITMAP = {}){ for(auto dano_itr:DANO){ // Список карт исходников
			if(TMs dano = dano_itr.second; dano.empty()){ err("Выборка исходника");
			}else if(std::string dataset_map_id = "dano," +dano.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ err("Составления идентификатора карты");
			//}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("Выборка карты итога " +dataset_map_id, __LINE__); err("Выборка карты исходника");
			}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Карта исходника
				if(dataset_map = (DATASET_MAP.end() == DATASET_MAP.find(dataset_map_id) ? dataset_map : DATASET_MAP.at(dataset_map_id)); !dataset_map.empty()){ //mpre("Значение из списка выборки dataset_map_id=" +dataset_map_id, __LINE__);
				}else if(dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ err("Выборка карты исходника");
				}else{ //mpre("Выборки карты", __LINE__);
				}return dataset_map; }(); false){ err("Выборка карты");
			}else if(DANO_BITMAP[dano.at("id")] = boost::dynamic_bitset<>(dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); DANO_BITMAP.empty()){ mpre("ОШИБКА получения результата расчета", __LINE__);
			}else if([&](string alias = "dano"){ // Обнолвение алиаса
				if(dataset_map.empty()){ //mpre("Не устанволенная карта", __LINE__);
				}else if(dataset_map.end() == dataset_map.find("alias")){ err("Не найдено поле");
				}else if(dataset_map.end() == dataset_map.find("dataset_id")){ err("Не найдено поле");
				}else if((alias == dataset_map.at("alias")) && (bmf::dataset.at("id") == dataset_map.at("dataset_id"))){ //mpre("Не обновляем", __LINE__);
				}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {{"dataset_id", bmf::dataset.at("id")}, {"alias", alias}}, __LINE__); false){ mpre("Выборка карты итога " +dataset_map_id, __LINE__); err("Выборка карты исходника");
				}else{ mpre("Обновление алиаса " +alias +" " +dataset_map_id, __LINE__);
				}return false; }()){ err("Обновление алиаса");
			}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
			}else{ //mpre("Исходные сигналы dano_id=" +dano.at("id"), __LINE__);
			}}return DANO_BITMAP; }(bmf::List(bmf::DATASET_MAP, {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}}, __LINE__)); DANO_BITMAP.empty()){ err("Список карт исходников");
		}else if([&](){ // Отображение времени загрузки исходных сигналов
			if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); !verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
			}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Загрузка исходных сигналов size=" +to_string(DANO_BITMAP.size()), __LINE__);
			}return false; }()){ err("Время расчета");
		}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if(TMMb ITOG_BITMAP = [&](TMMs DATASET_MAP, std::map<string,boost::dynamic_bitset<>> ITOG_BITMAP = {}){ for(auto itog_itr:bmf::List(bmf::ITOG, {}, __LINE__)){ // Расчетные результаты
			if(TMs itog = itog_itr.second; itog.empty()){ err("Выборка итога");
			}else if(std::string dataset_map_id = "itog," +itog.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ err("Составления идентификатора карты");
			//}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("Выборка карты итога " +dataset_map_id, __LINE__); err("Выборка карты исходника");
			}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Карта исходника
				if(dataset_map = (DATASET_MAP.end() == DATASET_MAP.find(dataset_map_id) ? dataset_map : DATASET_MAP.at(dataset_map_id)); !dataset_map.empty()){ //mpre("Значение из списка выборки dataset_map_id=" +dataset_map_id, __LINE__);
				}else if(dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ err("Выборка карты исходника");
				}else{ //mpre("Выборки карты", __LINE__);
				}return dataset_map; }(); false){ err("Выборка карты");
			}else if(ITOG_BITMAP[itog.at("id")] = boost::dynamic_bitset<>(dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); ITOG_BITMAP.empty()){ mpre("ОШИБКА получения результата расчета", __LINE__);
			}else if([&](string alias = "itog"){ // Обнолвение алиаса
				if(dataset_map.empty()){ //mpre("Не устанволенная карта", __LINE__);
				}else if(dataset_map.end() == dataset_map.find("alias")){ err("Не найдено поле");
				}else if(dataset_map.end() == dataset_map.find("dataset_id")){ err("Не найдено поле");
				}else if((alias == dataset_map.at("alias")) && (bmf::dataset.at("id") == dataset_map.at("dataset_id"))){ //mpre("Не обновляем", __LINE__);
				}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {{"dataset_id", bmf::dataset.at("id")}, {"alias", alias}}, __LINE__); false){ mpre("Выборка карты итога " +dataset_map_id, __LINE__); err("Выборка карты исходника");
				}else{ mpre("Обновление алиаса " +alias +" " +dataset_map_id, __LINE__);
				}return false; }()){ err("Обновление алиаса");
			}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
			}else{ //mpre("Итоговые сигналы dano_id=" +dano.at("id"), __LINE__);
			}}return ITOG_BITMAP; }(bmf::List(bmf::DATASET_MAP, {{"dataset_id", bmf::dataset.at("id")}, {"alias", "itog"}}, __LINE__)); ITOG_BITMAP.empty()){ err("Список карт исходников");
		}else if([&](){ // Отображение времени загрузки итоговых сигналов
			if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); !verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
			}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Загрузка итоговых сигналов size=" +to_string(ITOG_BITMAP.size()), __LINE__);
			}return false; }()){ err("Время расчета");
		}else if(int key = [&](int key = 0){ // Позиция обучения
			if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Не установлено обучение" ,__LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("key")){ err("Поле ключа не найдено в базе");
			}else if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); !epoch){ //mpre("Локальный ключ" ,__LINE__);
			}else if(key = atoi(bmf::dataset.at("key").c_str()); 0 > key){ err("Расчет значения последнего ключа");
			}else{ //mpre("Текущее состояние ключа key=" +to_string(key), __LINE__);
			}return key; }(); (key >= dataset_count)){ mpre("Выход за пределы значений dataset_count=" +to_string(dataset_count) +" key=" +to_string(key) ,__LINE__);
		}else if(int err = [&](int err = 0){ // Позиция обучения
			if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Не установлено обучение" ,__LINE__);
			}else if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); !epoch){ //mpre("Локальный ключ" ,__LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("err")){ err("Поле ключа не найдено в базе");
			}else if(err = atoi(bmf::dataset.at("err").c_str()); 0 > err){ err("Расчет значения последнего ключа");
			}else{ //mpre("Текущее состояние ключа err=" +to_string(err), __LINE__);
			}return err; }(); false){ err("Расчет позиции обучения");
		}else if(long microtime = (/*bmf::ARGV.end() == bmf::ARGV.find("learn") ? (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 :*/ atol(bmf::dataset.at("microtime").c_str())); false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if([&](bool rep = false, TMMs VALUES = {}){ do{ // Повторение расчетов эпохи
			if(string epoch = [&](std::string epoch = "1"){ // Расчет эпохи
				if(bmf::dataset.end() == bmf::dataset.find("epoch")){ // mpre("Поле эпохи не задано", __LINE__);
				}else if(epoch = bmf::dataset.at("epoch"); false){ err("Выборка значения эпохи");
				}else if(epoch = to_string(atoi(epoch.c_str()) + 1); epoch.empty()){ err("Инкремент эпохи");
				}else{ //mpre("Увеличение значения эпохи " +epoch, __LINE__);
				}return epoch; }(); epoch.empty()){ mpre("ОШИБКА расчета эпохи", __LINE__);
			}else if(key = [&](int key){ // Установка зависимой позиции
				if(string _key = (bmf::ARGV.end() == bmf::ARGV.find("key") ? "" : bmf::ARGV.at("key")); _key.empty()){ //mpre("Пропуск расчета" ,__LINE__);
				}else if(0 != _key.find_first_of("-+")){ mpre("Не установлен знак в ключе key=" +_key +" " ,__LINE__);
				}else if(int _key_ = atoi(_key.c_str()); !_key_){ mpre("Значение для изменения не задано" ,__LINE__);
				}else if(string dataset_id = (bmf::dataset.end() == bmf::dataset.find("id") ? "" : bmf::dataset.at("id")); dataset_id.empty()){ err("Идентификатор набора данных");
				}else if(TMs dataset = bmf::Up(bmf::DATASET, {{"id", dataset_id}}, {}, {}, __LINE__); dataset.empty()){ err("Выборка набора данных");
				}else if(key = (atoi(bmf::dataset.at("key").c_str()) +_key_) %dataset_count; false){ err("Изменение ключа");
				}else{ //mpre("Изменение позиции ключа " +to_string(_key_) +" > " +to_string(key) ,__LINE__);
				}return key; }(key); (0 > key >= dataset_count)){ err("Изменение позиции ключа");
			}else if([&](int repeat = 100){ // Прворка на повторы
				if(string _key = (bmf::ARGV.end() == bmf::ARGV.find("key") ? "" : bmf::ARGV.at("key")); _key.empty()){ //mpre("Пропуск расчета" ,__LINE__);
				}else if(static int key_static =-1 ;false){ err("Установка статического значения");
				}else if(static int cnt =0 ;false){ err("Количество повторений");
				}else if(cnt +=1; false){ err("Расчет количества повторов");
				}else if((key_static == key) && (repeat < cnt)){ exit(mpre("Остановка вслед за основным потоком " +to_string(repeat) +" проверок " ,__LINE__));
				}else if(key_static == key){ key = ((key + cnt) %dataset_count); std::cerr << "\x1b[37;41m " << cnt << " \x1b[0m"; //sleep(1); //system("sleep 3");
				}else if(key_static = key; false){ err("Сохранение значения ключа для дальнейших сравнений");
				}else if(cnt = 0; false){ err("Сохранение значения ключа для дальнейших сравнений");
				}else{ //mpre("Не повтор");
				}return false; }()){ err("Повтор ключа");
			}else if([&](){ // Обнуление позиции
				if(string epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? "" : bmf::ARGV.at("epoch")); "0" != epoch){ //mpre("Только для нулевой эпохи");
				}else if(static int err_current = 0; false){ err("Текущее количество ошибок");
				}else if(err == err_current){ //mpre("Количество ошибок не изменилось", __LINE__);
				}else if(err_current = err; false){ err("Сохранение текущего количества ошибок");
				}else if(key = round(key/2); false){ err("Начинаем с середины от пройденного");
				}else{ //mpre("Обнуление позиции при нулевой эпохе" ,__LINE__);
				}return false; }()){ err("Обнуление позиции расчета при нулевом количестве эпох");
			}else if([&](){ // Выборка обновлений модели
				if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
				}else if(TMMs _INDEX = bmf::Scan(bmf::INDEX, BMF_INDEX, __LINE__); _INDEX.empty()){ //mpre("Нет изменений в структуре", __LINE__);
				}else if([&](){ for(auto index_itr:_INDEX){ // Обнолвение структуры расчетов групп
					if(TMs index = index_itr.second; index.empty()){ err("Выборка обновленного морфа");
					}else if(string index_md5 = (index.end() == index.find("md5") ? "" : index.at("md5")); index_md5.empty()){ err("Хеш адреса морфа");
					}else if(BMF_INDEX.insert(make_pair(index_md5, index)); BMF_INDEX.empty()){ err("Обновление справочника");
					}else{ //mpre(_INDEX ,"Обновление струкруты групп", __LINE__);
					}}return false; }()){ err("Обновление структуры групп и списка морфов");
				}else if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); 3 != verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
				}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Обновление модели " +to_string(_INDEX.size()) +" size=" +to_string(BMF_INDEX.size()) +" ", __LINE__);
				}return false; }()){ err("Обновление модели");
			}else if(string table_vals = [&](string table_vals = ""){ // Создание таблицы значений
				if(table_vals = "index_grp" /*+"_" +bmf::dataset.at("id") +string("_") +to_string(key)*/; table_vals.empty()){ err("Имя таблицы значений");
				}else if("index_grp" == table_vals){ //mpre("Таблица уже создана в качестве образца" ,__LINE__);
				}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `" +table_vals +"` LIKE `index_grp`;").c_str())){ mpre("Вставка данных " +string(mysql_error(bmf::mysql)) ,__LINE__);
				}else{ mpre("Таблица для промежуточных данных" ,__LINE__);
				}return table_vals; }(); table_vals.empty()){ err("Расчет имени таблицы значений");
			}else if(string table_grp = [&](string table_grp = ""){ // Создание таблицы значений
				if(table_grp = "index_grp" /*+"_" +bmf::dataset.at("id") +string("_") +to_string(key)*/; table_grp.empty()){ err("Имя таблицы значений");
				}else if("index_grp" == table_grp){ //mpre("Таблица уже создана в качестве образца" ,__LINE__);
				}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `" +table_grp +"` LIKE `index_vals`;").c_str())){ mpre("Вставка данных " +string(mysql_error(bmf::mysql)) ,__LINE__);
				}else{ mpre("Таблица для промежуточных данных" ,__LINE__);
				}return table_grp; }(); table_grp.empty()){ err("Расчет имени таблицы значений");
			}else if([&](){ for(auto [itog_id ,itog]:bmf::List(bmf::ITOG, {}, __LINE__)){ // Проверка списка итогов
				if(TMs index = [&](TMs index = {}){ // Добавление первоначального морфа
					if(std::string addr = "1"; addr.empty()){ err("Идентификатор первоначального морфа");
					}else if(string index_md5 = md5(itog_id +":" +addr); index_md5.empty()){ err("Хеш морфа");
					}else if(index = (BMF_INDEX.end() == BMF_INDEX.find(index_md5) ? index : BMF_INDEX.at(index_md5)); !index.empty()){ //mpre("Морф уже в базе" ,__LINE__);
					}else if(TMs dano = bmf::Choice("", key, itog_id, DANO, DANO_BITMAP ,__LINE__); dano.empty()){ err("Выборка первоначального исходника");
					}else if(TMs _index = {{"grp", "0"}, {"addr", "1"} ,{"md5" ,index_md5}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}}; _index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
					}else if(_index = bmf::Up_mysql(bmf::INDEX, {/*{"id", md5(addr)}*/}, _index, {}, __LINE__); _index.empty()){ mpre("ОШИБКА добавления корневого морфа в базу", __LINE__);
					}else if(itog["index_id"] = _index.at("id"); itog.empty()){ mpre("ОШИБКА установки свойства связи итога с морфом", __LINE__);
					}else if(TMs _itog = bmf::Up(bmf::ITOG, {{"id", itog.at("id")}}, {}, itog, __LINE__); _itog.empty()){ err("Обновление поля корневого морфа у итога");
					}else if(BMF_INDEX[index_md5] = _index; BMF_INDEX.empty()){ err("Добавление нового морфа в список");
					}else if(string dano_val = (DANO_BITMAP.at(dano.at("id")).test(key) ? "1" : "0"); dano_val.empty()){ err("Значение исходника");
					}else{ //mpre(_index, "Добавление первоначального морфа " +_index.at("id"), __LINE__);
					}return index; }(); index.empty()){ //mpre("Добавление корневого морфа addr=1", __LINE__);
				}else if([&](){ // Группа корневого морфа
					if(string index_id = (index.end() == index.find("id") ? "" : index.at("id")); !atoi(index_id.c_str())){ err("Идентификатор не установлена");
					}else if(string sql_select = "SELECT * ,CONV(hex(exist), 16, 2) AS exist ,CONV(hex(vals), 16, 2) AS vals ,CONV(hex(dano), 16, 2) AS dano FROM index_grp WHERE itog_id=" +itog_id +" AND dataset_id=-1 AND index_id=" +index_id +" AND `key`=-1;"; sql_select.empty()){ err("Запрос на выборку значения группы");
					}else if(TMMs INDEX_GRP = bmf::List_mysql("index_grp", {}, sql_select, __LINE__); !INDEX_GRP.empty()){ //mpre("Выборка значения первоначальной группы " ,__LINE__);
					}else if(string sql_insert = "INSERT INTO index_grp SET itog_id=" +itog_id +" ,index_grp_id=NULL ,dataset_id=-1 ,index_id=" +index_id +" ,count=1 ,`key`=-1 ,exist =0b10;"; sql_insert.empty()){ err("Запрос на создание группы");
					}else if(mysql_query(bmf::mysql, sql_insert.c_str())){ mpre("Вставка данных " +string(mysql_error(bmf::mysql)) ,__LINE__);
					}else if(TMMs INDEX_GRP = bmf::List_mysql("index_grp", {}, sql_select ,__LINE__); 1 != INDEX_GRP.size()){ mpre("ОШИБКА Выборка значения первоначальной группы " ,__LINE__);
					}else if(TMs index_grp = INDEX_GRP.begin()->second; index_grp.empty()){ err("Значение исходного морфа");
					}else{ //mpre(index_grp ,"Идентификатор группы grp_id=" +index_id ,__LINE__);
					}return false; }()){ err("Группа морфа");
				}else if(string learn = [&](string learn = ""){ // Обучающее значение
					if(ITOG_BITMAP.end() == ITOG_BITMAP.find(itog.at("id"))){ err("Битовая карта итога");
					}else if(learn = (ITOG_BITMAP.at(itog.at("id")).test(key) ? "1" : "0"); learn.empty()){ err("Получения бит");
					}else{ //mpre("Выборка обучающего значения learn=" +learn, __LINE__);
					}return learn; }(); 1 != learn.length()){ err("Выборка обучающего значения");
				}else if(std::function Val = [&](string addr ,bool dv ,string ig0_id ,boost::dynamic_bitset<> &exist, boost::dynamic_bitset<> &vals ,TMMs *GRP , int line){ // Расчет значения
					bool val = dv;
					if(string adr = (addr.length() %6 ? addr.substr(0 ,addr.length() %6) : addr.substr(0 ,6 -addr.length() %6)); adr.empty()){ mpre("ОШИБКА Локальный адрес addr=" +addr +" adr=" +adr ,__LINE__);
					}else if(int nn = bmf::Bin2dec(adr); !nn){ mpre("ОШИБКА Смещение адреса addr=" +addr ,__LINE__);
					}else if(1 != nn){ val = (exist.test(nn) ? vals.test(nn) : dv); //mpre("Адрес не выходит за пределы группы addr=" +addr +" adr=" +adr ,__LINE__);
					}else if(string addr_grp = "1" +(addr.length()%6 ?addr.substr(addr.length()%6 ,-1) :addr.substr(6 ,-1)); 1 >= addr_grp.length()){ //mpre("Родительская группа не найдена addr=" +addr, __LINE__);
					}else if(string index_md5 = md5(itog_id +":" +addr_grp); index_md5.empty()){ err("Хеш морфа");
					}else if(TMs index =(BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5)); index.empty()){ //mpre("ОШИБКА Морф не найден в справочнике addr=" +addr +" itog_id=" +itog_id +" addr_grp=" +addr_grp ,__LINE__);
					}else if(string index_id = (index.end() == index.find("id") ? "" : index.at("id")); index_id.empty()){ mpre(index ,"Морф" ,__LINE__); err("Идентификатор дочерней группы");
					}else if(TMs index_grp = [&](TMs index_grp = {}){ // Выборка группы из базы
						if(nullptr != GRP){ //mpre("Список указан" ,__LINE__);
						}else if(string sql = "SELECT * ,CONV(hex(exist), 16, 2) AS exist ,CONV(hex(vals), 16, 2) AS vals FROM index_grp WHERE itog_id=" +itog_id +" AND dataset_id=" +bmf::dataset.at("id") +" AND index_id=" +index_id + " AND `key`=" + to_string(key)+";"; sql.empty()){ err("Запрос на выборку значения группы");
						}else if(TMMs INDEX_GRP = bmf::List_mysql("index_grp", {}, sql, __LINE__); 1 <INDEX_GRP.size()){ //mpre("ОШИБКА Список групп не найден _adr1=" +_adr1 +" " +sql ,__LINE__); // //mpre("Выборка значения первоначальной группы " ,__LINE__);
						}else if(index_grp = INDEX_GRP.begin()->second; index_grp.empty()){ err("Значение группы");
						}else if([&](){ // Проверка устанвоки вышестоящей группы
							if(ig0_id.empty()){ //mpre("Не проверяем правильность установки вышестоящей группы" ,__LINE__);
							}else if(string index_grp_id =(index_grp.end() ==index_grp.find("index_grp_id") ?"" :index_grp.at("index_grp_id")); index_grp_id.empty()){ err("Идентификатор вышестоящей группы не установлен");
							}else if(index_grp_id != ig0_id){ err("Идентификатор не равен вышестоящей группе");
							}else{ //mpre("Проверка правильности установки вышестоящей группы ig0_id=" +ig0_id +" index_grp_id=" +index_grp_id ,__LINE__);
							}return false; }()){ err("Вышестоящая группы установлена не правильно");
						}else{ //mpre(index_grp ,"Выборка группы из базы" ,__LINE__);
						}return index_grp; }() ;false){ //mpre("Группа не найдена" ,__LINE__);
					}else if([&](){ // Выборка из списка дочерних групп
						if(nullptr == GRP){ //mpre("Список не указан __" +to_string(line) +"__" ,__LINE__);
						}else if(GRP->empty()){ err("Список пуст");
						}else if(GRP->end() == GRP->find(index_id)){ mpre("ОШИБКА Группа в списке дочерних не найдена addr=" +addr +" index_id=" +index_id ,__LINE__);
						}else if(index_grp = GRP->at(index_id); index_grp.empty()){ err("Выборка группы из списка дочерних групп");
						}else if([&](){ // Проверка устанвоки вышестоящей группы
							if(ig0_id.empty()){ //mpre("Не проверяем правильность установки вышестоящей группы" ,__LINE__);
							}else if(string index_grp_id =(index_grp.end() ==index_grp.find("index_grp_id") ?"" :index_grp.at("index_grp_id")); index_grp_id.empty()){ mpre(index_grp ,"Группа из списка" ,__LINE__); err("Идентификатор вышестоящей группы не установлен");
							}else if(index_grp_id != ig0_id){ err("Идентификатор не равен вышестоящей группе");
							}else{ //mpre("Проверка правильности установки вышестоящей группы ig0_id=" +ig0_id +" index_grp_id=" +index_grp_id ,__LINE__);
							}return false; }()){ err("Вышестоящая группы установлена не правильно");
						}else{ //mpre(_index_grp ,"Выборка группы из списка дочерних групп" ,__LINE__);
						}return false; }(); index_grp.empty()){ //mpre("Группа не найдена" ,__LINE__);
					}else if(val = [&](bool val = false){ // Значение
						if(string str = (index_grp.end() == index_grp.find("vals") ? "" : index_grp.at("vals")); str.empty()){ mpre(index_grp ,"ОШИБКА Значение общей карты наличия" ,__LINE__);
						}else if(2 > str.length()){ //mpre(index_grp ,"Группа" ,__LINE__); mpre("ОШИБКА Размер строки значений addr=" +addr +" addr_grp=" +addr_grp +" " +str +" " ,__LINE__);
						}else if(val = ("0" == str.substr(str.length()-2, 1) ?false :true); false){ err("Значение из дочерней группы");
						}else{ //mpre("Расчет значения ", __LINE__);
						}return val; }(); false){ err("Значение");
					}else{ //mpre("Val Расчет дочерней группы line=" +to_string(line) +" addr=" +addr +" adr=" +adr +" addr_grp=" +addr_grp +" val=" +string(val ?"1" :"0") +" str_val=" +str_vals ,__LINE__);
					}return val; }; false){ err("Функция расчета значения")
				}else if(string SQL = [&](TMMi INDEX_GRP = {}, TMi PARENTS = {} , TM3s GRP = {} ,string SQL = ""){ do{ //for(auto index_grp_itr = INDEX_GRP.rbegin(); index_grp_itr != INDEX_GRP.rend(); index_grp_itr++){ //for(auto [index_grp_id ,index_grp]:INDEX_GRP){ // Расчет групп
					if([&](){ // Расчетные группы
						if(!INDEX_GRP.empty()){ //mpre("Список содержит группы" ,__LINE__);
						}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени эпохи", __LINE__);
						}else if(auto [on ,where] = [&](string on ="" ,string where ="AND (ig0.exist<>ig1.exist OR ig1.exist IS NULL)"){ // Условие выборки родителей
							if(PARENTS.empty()){ //mpre("Список родителей не задан" ,__LINE__);
							}else if(string in = [&](string in = ""){ for(auto [grp_id ,addr]:PARENTS){ in += "," +to_string(grp_id); }return in.substr(1 ,-1); }(); in.empty()){ err("Список идентификаторов родительских групп");
							}else if(where = "AND ig0.id IN(" +in +")"; where.empty()){ err("Условия выборки родителей");
							}else{ //mpre("Установка условия на выборку групп из списка родителей in=" +in +" parents=" +parents ,__LINE__);
							}return make_pair(on ,where); }(); on.empty() && where.empty()){ err("Родительское условие");
						}else if(PARENTS.clear(); !PARENTS.empty()){ err("Очистка списка групп");
						}else if(string sql = "SELECT ig0.id AS id ,ig0.id AS ig0_id ,ig1.id AS ig1_id ,ig1.itog_id AS ig0_itog_id ,ig0.index_grp_id AS index_grp_id, ig1.index_grp_id AS ig1_index_grp_id ,ig0.index_id AS index_id , CONV(hex(ig0.exist),16,2) AS exist0 ,CONV(hex(ig1.exist),16,2) AS exist1 ,CONV(hex(ig1.exist),16,2) AS exist ,CONV(hex(ig1.vals),16,2) AS vals ,CONV(hex(ig1.dano),16,2) AS dano FROM index_grp AS ig0 LEFT JOIN index_grp AS ig1 ON ig1.dataset_id=" +bmf::dataset.at("id") +" AND ig1.`key`=" +to_string(key) +" AND ig0.index_id=ig1.index_id AND ig0.itog_id=ig1.itog_id " +on +" WHERE ig0.`key`=-1 AND ig0.itog_id=" +itog_id +" AND ig0.dataset_id=-1 " +where +";"; sql.empty()){ err("Запрос на выборку"); // // OR (ig1.index_grp_id IS NULL AND ig1.dataset_id IS NULL) OR (ig0.index_grp_id IS NULL AND ig0.dataset_id IS NULL AND ig1.itog_id=" +itog_id +"))
						}else if(TMMs _INDEX_GRP = bmf::List_mysql("index_grp", {}, sql, __LINE__); _INDEX_GRP.empty()){ //mpre("Пустой список выборки " +sql ,__LINE__);
						}else if(TMs _GRP = [&](TMs _GRP = {}){ for(auto [index_grp_id ,index_grp]:_INDEX_GRP){ // По числовому ключу
							if(string ig0_id = (index_grp.end() == index_grp.find("ig0_id") ? "" : index_grp.at("ig0_id")) ;ig0_id.empty()){ err("Идентификатор группы");
							}else if(int grp_id =atoi(ig0_id.c_str()); !grp_id){ err("Числовое значение идентификатора корневого морфа");
							}else if(INDEX_GRP.end() != INDEX_GRP.find(grp_id)){ mpre(index_grp ,"Группа" ,__LINE__); mpre(INDEX_GRP.at(grp_id) ,"Дубяж" ,__LINE__); mpre("ОШИБКА Группа уже в списке grp_id=" +to_string(grp_id) ,__LINE__);
							}else if(INDEX_GRP[grp_id] = index_grp; INDEX_GRP.empty()){ err("Добавление значения");
							}else if(_GRP[ig0_id] = ig0_id; _GRP.empty()){ err("Список групп сигналов");
							}else{ //mpre(INDEX_GRP ,"Список групп" ,__LINE__);
							}}return _GRP; }(); false){ err("Список групп");
						}else if([&](){ // Уведомление
							if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); 3 != verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
							}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Расчетный список itog_id=" +itog_id +" " +to_string(INDEX_GRP.size()) +" " ,__LINE__);
							}return false; }()){ err("Уведомление");
						}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени эпохи", __LINE__);
						}else if([&](){ // Список сигналов групп
							if(_GRP.empty()){ //mpre("Список групп пуст" ,__LINE__);
							}else if(string in = [&](string in = ""){ for(auto [ig0_id ,val]:_GRP){ in +="," +ig0_id; }return in.substr(1 ,-1); }(); in.empty()){ err("Список идентификаторов групп");
							}else if(string sql = "SELECT id ,index_id ,index_grp_id ,grow ,CONV(hex(vals), 16, 2) AS vals FROM index_grp WHERE dataset_id=" +bmf::dataset.at("id") +" AND itog_id=" +itog_id +" AND `key`=" +to_string(key) +" AND index_grp_id IN(" +in +");"; sql.empty()){ err("Запрос на выборку");
							}else if(TMMs INDEX_GRP = bmf::List_mysql("index_grp" ,{} ,sql ,__LINE__); false){ err("Список групп потомков");
							}else if([&](){ for(auto [id ,index_grp]:INDEX_GRP){ // Список значений
								if(string index_grp_id =(index_grp.end() ==index_grp.find("index_grp_id") ?"" :index_grp.at("index_grp_id")); index_grp_id.empty()){ err("Идентификатор вышестоящей общей группы");
								}else if(string index_id =(index_grp.end() ==index_grp.find("index_id") ?"" :index_grp.at("index_id")); index_id.empty()){ err("Идентификатор морфа группы");
								}else if(GRP[index_grp_id][index_id] = index_grp; GRP.empty()){ err("Добавление группы в список");
								}else{ //mpre("Формирование списка значений дочерних групп GRP[" +index_grp_id +"][" +index_id +"]" ,__LINE__);
								}}return false; }()){ err("Список значений");
							}else{ //mpre(GRP ,"Выборка сигналов групп in=" +in +" " +sql ,__LINE__);
							}return false; }() ;false){ err("Список сигналов групп");
						}else if(INDEX_GRP.size() != _INDEX_GRP.size()){ mpre("ОШИБКА Нарушение количества выборки _INDEX_GRP.size()=" +to_string(_INDEX_GRP.size()) +" INDEX_GRP.size()=" +to_string(INDEX_GRP.size()) ,__LINE__);
						}else if([&](){ // Уведомление
							if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); 3 != verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
							}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Значения групп itog_id=" +itog_id +" " +to_string(GRP.size()) +" " ,__LINE__);
							}return false; }()){ err("Уведомление");
						}else{ //mpre("Формирование списка групп" ,__LINE__);
						}return false; }(); INDEX_GRP.empty()){ //mpre("Нет расчетных групп" ,__LINE__);
					}else if(TMs index_grp = [&](TMs index_grp = {}){ // Группа
						if(auto index_grp_itr = INDEX_GRP.rbegin(); (INDEX_GRP.rend() == index_grp_itr)){ err("Последнее значение в группе");
						}else if(index_grp = index_grp_itr->second; index_grp.empty()){ err("Значения группы");
						}else if(INDEX_GRP.erase(index_grp_itr->first); false){ err("Удаление группы из списка");
						}else if(string _itog_id =(index_grp.end() ==index_grp.find("ig0_itog_id") ?"" :index_grp.at("ig0_itog_id")); (!_itog_id.empty() && _itog_id != itog_id)){ mpre(index_grp ,"ОШИБКА Значения итогов группы itog_id=" +itog_id ,__LINE__);
						}else{ //mpre(index_grp ,"Расчет группы" ,__LINE__);
						}return index_grp; }(); index_grp.empty()){ err("Выборка группы");
					}else if(TMs index = [&](TMs index = {}){ // Морф группы
						if(string index_id = (index_grp.end() == index_grp.find("index_id") ? "" : index_grp.at("index_id")); index_id.empty()){ mpre(index_grp ,"ОШИБКА Идентификатор группового морфа" ,__LINE__);
						}else if(index = bmf::Up("index" ,{{"id" ,index_id}}, {}, {}, __LINE__); index.empty()){ mpre(index_grp ,"Группа" ,__LINE__); mpre("ОШИБКА Групповой морф index_id=" +index_id ,__LINE__);
						}else{ //mpre(index ,"Морф группы" ,__LINE__);
						}return index; }() ;index.empty()){ err("Морф группы");
					}else if(string addr_grp = (index.end() == index.find("addr") ?"" : index.at("addr")); addr_grp.empty()){ err("Адрес группы");
					}else if([&](){ // Сохранение родителя
						if(string index_grp_id = (index_grp.end() == index_grp.find("index_grp_id") ? "" : index_grp.at("index_grp_id")); index_grp_id.empty()){ //mpre("Корневая группа" ,__LINE__);
						}else if(int grp_id = atoi(index_grp_id.c_str()); !grp_id){ err("Идентификатор группы");
						}else if(INDEX_GRP.end() != INDEX_GRP.find(grp_id)){ //mpre(INDEX_GRP ,"Родитель уже в списке групп grp_id=" +to_string(grp_id) ,__LINE__);
						}else if(PARENTS[grp_id] = addr_grp; PARENTS.empty()){ err("Пополнение списка родительских групп");
						}else{ //mpre(PARENTS ,"Список родительских групп addr_grp=" +addr_grp +" index_grp_id=" +index_grp_id ,__LINE__);
						}return false; }()){ err("Увеличение списка родительских гурпп");
					}else if(boost::dynamic_bitset<> exist0 = [&](boost::dynamic_bitset<> exist0 = {}){ //Карта наличия
						if(string str = (index_grp.end() == index_grp.find("exist0") ? "" : index_grp.at("exist0")); str.empty()){ err("Значение общей карты наличия");
						}else if(string map = (string(64 -str.length() ,'0') +str); 64 != map.length()){ err("Полная строка карты");
						}else if(exist0 = boost::dynamic_bitset(map); 64 != exist0.size()){ err("Битовая карта общего наличия");
						}else{ //mpre("Карта общ exist0=" +map ,__LINE__);
						}return exist0; }(); exist0.empty()){ err("Общая карта наличия");
					}else if(boost::dynamic_bitset<> exist = [&](boost::dynamic_bitset<> exist = {}){ //Карта наличия
						if(string str = (index_grp.end() == index_grp.find("exist") ? "" : index_grp.at("exist")); false){ mpre(index_grp ,"ОШИБКА Значение общей карты наличия" ,__LINE__);
						}else if(string map = (string(64 -str.length() ,'0') +str); 64 != map.length()){ err("Полная строка карты");
						}else if(exist = boost::dynamic_bitset(map); 64 != exist.size()){ err("Битовая карта общего налия");
						}else{ //mpre("Карта ключ exist=" +map ,__LINE__);
						}return exist; }(); exist.empty()){ err("Общая карта наличия");
					}else if(boost::dynamic_bitset<> dano = [&](boost::dynamic_bitset<> dano = {}){ //Карта наличия
						if(string str = (index_grp.end() == index_grp.find("dano") ? "" : index_grp.at("dano")); false){ mpre(index_grp ,"ОШИБКА Значение общей карты наличия" ,__LINE__);
						}else if(string map = (string(64 -str.length() ,'0') +str); 64 != map.length()){ err("Полная строка карты");
						}else if(dano = boost::dynamic_bitset(map); 64 != dano.size()){ err("Битовая карта общего налия");
						}else{ //mpre(index_grp ,"Значен dano dano=" +map ,__LINE__);
						}return dano; }(); dano.empty()){ err("Общая карта наличия");
					}else if(boost::dynamic_bitset<> vals = [&](boost::dynamic_bitset<> vals = {}){ //Карта значений
						if(string str = (index_grp.end() == index_grp.find("vals") ? "" : index_grp.at("vals")); false){ mpre(index_grp ,"ОШИБКА Значение общей карты наличия" ,__LINE__);
						}else if(string map = (string(64 -str.length() ,'0') +str); 64 != map.length()){ err("Полная строка карты");
						}else if(vals = boost::dynamic_bitset(map); 64 != vals.size()){ err("Битовая карта общего налия");
						}else{ //mpre("Значен vals vals=" +map ,__LINE__);
						}return vals; }(); vals.empty()){ err("Общая карта значений");
					}else if(string ig0_id =(index_grp.end() ==index_grp.find("ig0_id") ?"" :index_grp.at("ig0_id")); ig0_id.empty()){ //mpre("Не указана родительская группа" ,__LINE__);
					}else if(TMMs SIGN = (GRP.end() == GRP.find(ig0_id) ?SIGN :GRP.at(ig0_id)); false){ err("Список значений");
					}else if([&](){ for(int nn = 63; nn > 0; nn--){ // Расчет группы
						if(bool ext = [&](bool ext = false){ // Наличие морфа
							if(ext = exist0.test(nn)){ //mpre("Морф на месте" ,__LINE__);
							}else if(dano.test(nn)){ mpre("ОШИБКА Установлено исходное значение при отсутствующем морфа nn=" +to_string(nn) ,__LINE__);
							}else{ //mpre("Проверяем исходное занчение nn=" +to_string(nn) ,__LINE__);
							}return ext; }(); !ext){ //mpre("Не установлен морф в общей карте nn=" +to_string(nn) ,__LINE__);
						}else if(string adr = bmf::Dec2bin(nn); adr.empty()){ err("Двоичный локальный адрес");
						}else if([&](){ // Установка значения исходника в группу
							if(exist.test(nn)){ //mpre("Значение исходника уже установлено" ,__LINE__);
							}else if(string index_md5 = md5(itog_id +":" +adr +addr_grp.substr(1, -1)); index_md5.empty()){ err("Хеш адреса");
							//}else if(TMs _index = bmf::Up_mysql("index" ,{{"itog_id", itog_id} ,{"md5" ,index_md5}}, {}, {}, __LINE__); _index.empty()){ mpre(index_grp ,"ОШИБКА Выборка морфа key=" +to_string(key) +" nn=" +to_string(nn) +" adr=" +adr +" addr_grp=" +addr_grp +" " +itog_id +":" +adr +addr_grp +" index_md5=" +index_md5 ,__LINE__);
							}else if(TMs _index = [&](TMs index = {}){ // Выборка морфа
								if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
								}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
								}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
								}return index; }(); _index.empty()){ err("Выборка морфа");
							}else if(string dano_id = (_index.end() == _index.find("dano_id") ? "" : _index.at("dano_id")); dano_id.empty()){ err("Идентификатор исходника");
							}else if(string addr = (_index.end() == _index.find("addr") ? "" : _index.at("addr")); addr.empty()){ err("Адрес морфа");
							}else if(DANO_BITMAP.end() == DANO_BITMAP.find(dano_id)){ err("Исходник не найден в битовой карте");
							}else if(DANO_BITMAP.at(dano_id).test(key) ? dano.set(nn) : dano.reset(nn); false){ err("Установка значения исходника");
							}else{ //mpre("Исходник key=" +to_string(key) +" adr=" +bmf::Dec2bin(nn) +" dano_id=" +dano_id +" dv=" +(dano.test(nn) ? "1" : "0") ,__LINE__); //mpre(BMF_INDEX ,"Справочник" ,__LINE__); //mpre("ОШИБКА выборки локального морфа addr_grp=" +addr_grp +" addr=" +addr ,__LINE__);
							}return false; }()){ err("Морф для установки значения исходника в группу");
						}else if([&](){ // Значение морфа
							if(bool dv = dano.test(nn); false){ err("Значение исходника");
							}else if(bool v1 = [&](bool v1 = false){ // Значение старшего морфа
								if(string addr1 = "11" +adr.substr(1, -1) +addr_grp.substr(1, -1); addr1.empty()){ err("Полный адрес старшего потомка");
								}else if(v1 = Val(addr1 ,dv ,ig0_id ,exist ,vals ,&SIGN ,__LINE__) ;false){ err("Значение страшего морфа");
								}else{ //mpre("Значение Полный адрес страшего морфа addr1=" +addr1 +" v1=" +string(v1 ?"1" :"0") ,__LINE__);
								}return v1; }(); false){ err("Значение старшего морфа");
							}else if(bool v0 = [&](bool v0 = false){ // Значение старшего морфа
								if(string addr0 = "10" +adr.substr(1, -1) +addr_grp.substr(1, -1); addr0.empty()){ err("Полный адрес младшего потомка");
								}else if(v0 = Val(addr0 ,dv ,ig0_id ,exist ,vals ,&SIGN ,__LINE__) ;false){ err("Значение младшего морфа");
								}else{ //mpre("Значение Полный адрес младшего морфа addr0=" +addr0 +" v0=" +string(v0 ?"1" :"0") ,__LINE__);
								}return v0; }(); false){ err("Значение младшего морфа");
							}else if(string link = [&](string link = ""){ // Ссылка
								if(1 < adr.length()){ link = adr.substr(1, 1); //mpre("Ссылка из адреса морфа" ,__LINE__);
								}else if(1 < addr_grp.length()){ link = addr_grp.substr(1, 1); // mpre("Ссылка из родительской группы" ,__LINE__);
								}else if(bool _link = exist0.test(0) ;true){ link = (_link ?"1" :"0"); //mpre("Установка ссылки по наличию adr=" +adr +" _link=" +string(_link ?"1" :"0") +" link=" +link ,__LINE__);
								}else{ //err("ОШИБКА расчета ссылки");
								}return link; }() ;1 != link.length()){ err("Ссылка")
							}else if(bool val = !v1 & v0 ^!("0" != link); false){ err("Значение");
							}else if(val ? vals.set(nn) : vals.reset(nn); false){ err("Установка значения");
							}else if([&](){ // Отображение результатов расчета
								if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); 1 != verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
								}else if(string str_exist = [&](std::string map = ""){ boost::to_string(exist, map); return map; }(); (str_exist.length() != exist.size())){ mpre("ОШИБКА установки карты итога", __LINE__);
								}else if(string str_dano = [&](std::string map = ""){ boost::to_string(dano, map); return map; }(); (str_dano.length() != dano.size())){ mpre("ОШИБКА установки карты итога", __LINE__);
								}else if(string vals = [&](string vals = ""){ for(auto [dano_id ,bitset]:DANO_BITMAP){ vals += (bitset.test(key) ?"1" :"0"); }return vals; }(); vals.empty()){ err("Значения исходников");
								}else{ mpre("Расчет epoch=" +epoch +" key=" +to_string(key) +" itog_id=" +itog_id +" " +adr +" " +addr_grp +" link=" +link +" dv=" +string(dv ? "1" : "0") +" v1=" +string(v1 ? "1" : "0") +" v0=" +string(v0 ? "1" : "0") +" val=" +string(val ? "1" : "0") +" " ,__LINE__);
								}return false; }()){ err("Отображение результатов расчетов"); 
							}else{ //mpre("Расчет морфа key=" +to_string(key) +" itog_id=" +itog_id +" adr=" +adr +" addr_grp=" +addr_grp ,__LINE__);
							}return false; }()){ err("Значение морфа");
						}else if(exist.set(nn) ;false){ err("Установка морфа в ключевую группу");
						}else{ //mpre("Расчет Группы adr=" +adr +" " ,__LINE__);
						}}return false; }()){ err("Перебор значений карты");
					}else if(auto [grow ,list] = [&](string grow ="" ,string list =""){ // Сохрнанение результатов
						if(auto [ADDR ,next] = [&](int nn =1 ,TMi ADDR = {}){ do{ // Значение роста
							if(bool dv = dano.test(nn); false){ err("Значение исходника");
							}else if(string adr = bmf::Dec2bin(nn); adr.empty()){ err("Двоичный локальный адрес");
							}else if(bool v1 = [&](bool v1 = false){ // Значение старшего морфа
								if(string addr1 = "11" +adr.substr(1, -1) +addr_grp.substr(1, -1); addr1.empty()){ err("Полный адрес старшего потомка");
								}else if(v1 = Val(addr1 ,dv ,"" ,exist ,vals ,&SIGN ,__LINE__) ;false){ err("Значение страшего морфа");
								}else{ //mpre("Направление Полный адрес страшего морфа addr1=" +addr1 +" v1=" +string(v1 ?"1" :"0") ,__LINE__);
								}return v1; }(); false){ err("Значение старшего морфа");
							}else if(bool v0 = [&](bool v0 = false){ // Значение старшего морфа
								if(string addr0 = "10" +adr.substr(1, -1) +addr_grp.substr(1, -1); addr0.empty()){ err("Полный адрес старшего потомка");
								}else if(v0 = Val(addr0 ,dv ,"" ,exist ,vals ,&SIGN ,__LINE__) ;false){ err("Значение страшего морфа");
								}else{ //mpre("Направление Полный адрес страшего морфа addr0=" +addr0 +" v0=" +string(v0 ?"1" :"0") ,__LINE__);
								}return v0; }(); false){ err("Значение старшего морфа");
							}else if(bool _link = (dv ?v0 :v1); false){ err("Расчет направления развития");
							}else if(string link = (_link ?"1" : "0"); 1 != link.length()){ err("Строковое значение ссылки");
							}else if(string bin = bmf::Dec2bin(nn) ;bin.empty()){ err("Двоичное представление адреса");
							}else if(string addr = bin +addr_grp.substr(1, -1) ;addr.empty()){ err("Адрес текущего морфа");
							}else if(string index_md5 = md5(itog_id +":" +addr) ;index_md5.empty()){ err("Хеш морфа");
							}else if(TMs _index = [&](TMs index = {}){ // Выборка морфа
								if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
								}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
								}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
								}return index; }(); _index.empty()){ err("Выборка морфа");
							}else if(string dano_id = (_index.end() == _index.find("dano_id") ? "" : _index.at("dano_id")) ;dano_id.empty()){ err("Идентификатор исходника");
							}else if(ADDR[nn] = string(_link ?"" :"-") +dano_id ;ADDR.empty()){ err("Добавление адреса в список");
							}else if(string _adr = "1" +link +adr.substr(1, -1); _adr.empty()){ err("Значение следующей ссылки");
							}else if(nn = bmf::Bin2dec(_adr); !nn){ err("Числовое представление ссылки");
							}else{ //mpre("Расчет направлений роста key=" +to_string(key) +" itog_id=" +itog_id +" link=" +link +" dv=" +string(dv ?"1" :"0") +" v1=" +string(v1 ?"1" :"0") +" v0=" +string(v0 ?"1" :"0") +" adr=" +adr +" addr_grp=" +addr_grp +" addr=" +addr +" " ,__LINE__);
							}}while((64 >nn) && exist.test(nn)); return make_pair(ADDR, nn); }(); ADDR.empty()){ err("Список адресов родителей");
						}else if(list = [&](string list = ""){ for(auto [nn ,dano_id]:ADDR){ list += ";" +("-" == dano_id.substr(0 ,1) ?dano_id.substr(1 ,-1) :dano_id); }return list; }(); list.empty()){ err("Список исходников");
						//}else if(mpre(ADDR ,"Рост " +to_string(next), __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
						}else if(grow = [&](){ // Значение роста
							if(int nn = ADDR.rbegin()->first; !nn){ mpre("Максимальное значение адреса nn=" +to_string(nn) ,__LINE__);
							}else if(string dano_id = ADDR.rbegin()->second; dano_id.empty()){ mpre("Значение нижестоящего перехода dano_id=" +dano_id ,__LINE__);
							}else if(string addr_grow = bmf::Dec2bin(nn) +addr_grp.substr(1 ,-1); addr_grow.empty()){ err("Полный адрес роста");
							//}else if(TMs index = bmf::Up_mysql("index" ,{{"itog_id", itog_id} ,{"md5" ,md5(itog_id +":" +addr_grow)}}, {}, {}, __LINE__); index.empty()){ mpre("ОШИБКА Выборка морфа adr=" +bmf::Dec2bin(nn) +" addr_grow=" +addr_grow ,__LINE__);
							}else if(string index_md5 = md5(itog_id +":" +addr_grow); index_md5.empty()){ err("Формирование идентификаторм морфа");
							}else if(TMs index = [&](TMs index = {}){ // Выборка морфа
								if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
								}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
								}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
								}return index; }(); index.empty()){ err("Выборка морфа");
							}else if(string _grow = (index.end() == index.find("id") ? "" : index.at("id")); _grow.empty()){ mpre(index ,"Морф" ,__LINE__); mpre("ОШИБКА Идентификатор морфа роста addr_grow=" +addr_grow ,__LINE__);
							}else if(grow = string("-" == dano_id.substr(0 ,1) ?"-" :"") +_grow; grow.empty()){ err("Значение роста с учетом направления");
							}else if(32 > nn){ //mpre("Значение роста из локальной группы key=" +to_string(key) +" itog_id=" +itog_id +" addr_grp=" +addr_grp +" grow=" +grow +" adr=" +bmf::Dec2bin(nn) +" list=" +list +" " ,__LINE__);
							}else if(string adr_next = bmf::Dec2bin(next); adr_next.empty()){ err("Расчет адреса морфа нижестоящей группы");
							}else if(string addr_next = adr_next +addr_grp.substr(1, -1); addr_next.empty()){ err("Полный адрес следующей группы");
							}else if(string index_md5 = md5(itog_id +":" +addr_next); index_md5.empty()){ err("Идентификатор родителя нижестоящей группы");
							}else if(TMs _index = (BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?_index :BMF_INDEX.at(index_md5)); _index.empty()){ //mpre("Морф нижестоящей группы еще не создан adr_next=" +adr_next +" adr_grp=" +adr_grp ,__LINE__);
							/*}else if(TMs index_grp = [&](TMs index_grp = {}){ // Значение из базы
								if(string index_id = (_index.end() == _index.find("id") ?"" :_index.at("id")); index_id.empty()){ err("Значение идентификатора морфа роста");
								}else if(string sql = "SELECT * FROM index_grp WHERE itog_id=" +itog_id +" AND dataset_id=" +bmf::dataset.at("id") +" AND index_id=" +index_id +" AND `key`=" +to_string(key) +";"; sql.empty()){ err("Запрос выборки обновлений");
								}else if(TMMs _INDEX_GRP = bmf::List_mysql("index_grp", {}, sql, __LINE__); 1 != _INDEX_GRP.size()){ //mpre("Не создана группа значений adr=" +addr_grow +" " +sql ,__LINE__);
								}else if(index_grp = _INDEX_GRP.begin()->second; index_grp.empty()){ err("Значение исходного морфа");
								}else{
								}return index_grp; }(); index_grp.empty()){ err("Значение из базы");*/
							}else if(TMs index_grp = [&](TMs index_grp = {}){ // Дочерняя группа
								if(auto grp_itr = GRP.find(ig0_id); GRP.end() == grp_itr){ err("Группа исходников не найдена");
								}else if(string index_id = (_index.end() == _index.find("id") ?"" :_index.at("id")); index_id.empty()){ err("Значение идентификатора морфа роста");
								}else if(auto _grp_itr = grp_itr->second.find(index_id); grp_itr->second.end() == _grp_itr){ err("Группа занчений в группе родителя не найдена");
								}else if(index_grp = _grp_itr->second; index_grp.empty()){ err("Группа значений");
								}else{
								}return index_grp; }(); index_grp.empty()){ err("Выборка дочерней группы");
							}else if(grow = (index_grp.end() ==index_grp.find("grow") ?"" :index_grp.at("grow")); grow.empty()){ err("Значение роста нижестоящей группы");
							}else if(TMs index_grow = [&](TMs index_grow = {}){ // Расширяемый морф
								if(string _grow = ("-" == grow.substr(0 ,1) ?grow.substr(1 ,-1) :grow); _grow.empty()){ err("Положительное значение роста");
								}else if(index_grow = bmf::Up("index" ,{{"id", _grow}} ,{} ,{} ,__LINE__); index_grow.empty()){ err("Морф роста");
								}else{ //mpre(index_grow ,"Адрес роста" ,__LINE__);
								}return index_grow; }(); index_grow.empty()){ err("Расширяемый морф");
							}else if(string addr_grow =(index_grow.end() ==index_grow.find("addr") ?"" :index_grow.at("addr")); addr_grow.empty()){ err("Адрес расширяемого морфа");
							}else{ //mpre(index_grp ,"Группа" ,__LINE__); mpre(ADDR ,"Расчет роста key=" +to_string(key) +" nn=" +to_string(nn) +" addr_grp=" +addr_grp +" adr=" +bmf::Dec2bin(nn) +" adr_next=" +adr_next +" grow=" +grow +" addr_grow=" +addr_grow +" list=" +list +" " ,__LINE__);
							}return grow; }(); grow.empty()){ err("Значение роста");
						}else{ //mpre(ADDR ,"Адрес роста key=" +to_string(key) +" addr_grp=" +addr_grp +" grow=" +grow +" list=" +list ,__LINE__);
						}return make_pair(grow, list); }(); grow.empty()){ err("Сохранение результатов расчетов");
					}else if([&](){ // Сохранение значения группы
						if(string str_exist = [&](std::string map = ""){ // Карта наличия
							if(boost::to_string(exist, map); map.empty()){ err("Конвертация в строку");
							}else if(auto npos = map.find_first_of("1"); npos == string::npos){ mpre("Нулевое значение наличия" ,__LINE__);
							}else if(map = map.substr(npos ,-1); map.empty()){ err("Оптимизация строки карты");
							}else{ //mpre("Составление строки карты наличия str_exist=" +map ,__LINE__);
							}return map; }(); str_exist.empty()){ mpre("ОШИБКА установки карты", __LINE__);
						}else if(string str_dano = [&](std::string map = ""){ // Карта исходника
							if(boost::to_string(dano, map); map.empty()){ err("Конвертация в строку");
							}else if(auto npos = map.find_first_of("1"); npos == string::npos){ map ="0"; //mpre("Нулевое значение наличия" ,__LINE__);
							}else if(map = map.substr(npos ,-1); map.empty()){ err("Оптимизация строки карты");
							}else{ //mpre("Составление строки карты наличия str_exist=" +map ,__LINE__);
							}return map; }(); str_dano.empty()){ mpre("ОШИБКА установки карты", __LINE__);
						}else if(string str_vals = [&](std::string map = ""){ // Карта значений
							if(boost::to_string(vals, map); map.empty()){ err("Конвертация в строку");
							}else if(auto npos = map.find_first_of("1"); npos == string::npos){ map ="0"; //mpre("Нулевое значение наличия" ,__LINE__);
							}else if(map = map.substr(npos ,-1); map.empty()){ err("Оптимизация строки карты");
							}else{ //mpre("Составление строки карты наличия str_exist=" +map ,__LINE__);
							}return map; }(); str_vals.empty()){ mpre("ОШИБКА установки карты", __LINE__);
						}else if(string ig1_id = (index_grp.end() == index_grp.find("ig1_id") ? "" : index_grp.at("ig1_id")); false){ //mpre(index_grp ,"Группа уже создана в базе" ,__LINE__);
						}else if(string index_id = (index_grp.end() == index_grp.find("index_id") ? "" : index_grp.at("index_id")); index_id.empty()){ mpre(index_grp ,"ОШИБКА Идентификатор группового морфа" ,__LINE__);
						}else if(string index_grp_id = (index_grp.end() == index_grp.find("index_grp_id") ? "" : index_grp.at("index_grp_id")); false){ mpre(index_grp ,"ОШИБКА Идентификатор родительского морфа" ,__LINE__);
						}else if(GRP[index_grp_id][index_id] = TMs({{"vals" ,str_vals} ,{"index_id" ,index_id} ,{"index_grp_id" ,index_grp_id} ,{"grow" ,grow}}); GRP.empty()){ err("Значение новой группы");
						}else if([&](){ // Сохранение значений групп
							if(SQL += ",\n(" +itog_id +" ," +bmf::dataset.at("id") +" ," +index_id +" ," +to_string(key) +" ," +grow +" ,'" +list +"' ," +(index_grp_id.empty() ?"NULL" :index_grp_id) +" ,0b" +str_exist +" ,0b" +str_dano +" ,0b" +str_vals +")"; SQL.empty()){ err("Формирование запроса на изменения");
							}else if(!INDEX_GRP.empty()){ //mpre("Только в конце цикла" ,__LINE__);
							}else if(string sql = "INSERT INTO index_grp (`itog_id` ,`dataset_id` ,`index_id` ,`key` ,`grow` ,`list` ,`index_grp_id` ,`exist` ,`dano` ,`vals`) VALUES " +SQL.substr(1 ,-1) +" ON DUPLICATE KEY UPDATE grow=VALUES(grow) ,list=VALUES(list) ,index_grp_id=VALUES(index_grp_id) ,exist=VALUES(exist) ,dano=VALUES(dano) ,vals=VALUES(vals);"; sql.empty()){ err("Проверка запроса");
							}else if(mysql_query(bmf::mysql ,sql.c_str())){ mpre("ОШИБКА запроса " +sql +"\n" +string(mysql_error(bmf::mysql)) ,__LINE__);
							}else if(SQL = "" ;false){ err("Обнуление списка изменений");
							}else{ //mpre("Сохранение значений групп " +sql ,__LINE__);
							}return false; }()){ err("Сохранение значений групп");
						}else{ //mpre("Сохранение значений группы " +sql ,__LINE__);
						}return false; }()){ err("Сохранение значения группы");
					}else{ //mpre("Расчет группы key=" +to_string(key) +" addr_grp=" +addr_grp ,__LINE__);
					}}while(INDEX_GRP.size() || PARENTS.size()); return SQL; }() ;false){ //mpre("Пропуск расчета key=" +to_string(key) +" itog_id=" +itog_id ,__LINE__); //mpre("Расчёт совпал с обучением itog_id=" +itog_id +" key=" +to_string(key) +" calc=" +string(calc ? "1" : "0") +" learn=" +string(learn ? "1" : "0") +" !!!!!!" ,__LINE__);
				}else if(string calc = [&](string calc = ""){ // Значение корневой группы
					if(string index_md5 = md5(itog_id +":1"); index_md5.empty()){ err("Идентификатор морфа");
					}else if(TMs index =(BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5)); index.empty()){ err("Корневой морф");
					}else if(string index_id =(index.end() ==index.find("id") ?"" :index.at("id")); index_id.empty()){ err("Идентификатор морфа");
					}else if(string sql = "SELECT id ,index_id ,CONV(hex(vals), 16, 2) AS vals FROM index_grp WHERE dataset_id=" +bmf::dataset.at("id") +" AND index_id=" +index_id +" AND itog_id=" +itog_id +" AND `key`=" +to_string(key) +";"; sql.empty()){ err("Запрос на выборку");
					}else if(TMMs _INDEX_GRP = bmf::List_mysql("index_grp", {}, sql, __LINE__); 1 != _INDEX_GRP.size()){ mpre("Не создана группа значений " +sql ,__LINE__);
					}else if(TMs index_grp = _INDEX_GRP.begin()->second; index_grp.empty()){ err("Значение исходного морфа");
					}else if(boost::dynamic_bitset<> vals = [&](boost::dynamic_bitset<> vals = {}){ //Карта значений
						if(string str = (index_grp.end() == index_grp.find("vals") ? "" : index_grp.at("vals")); false){ mpre(index_grp ,"ОШИБКА Значение общей карты наличия" ,__LINE__);
						}else if(string map = (string(64 -str.length() ,'0') +str); 64 != map.length()){ err("Полная строка карты");
						}else if(vals = boost::dynamic_bitset(map); 64 != vals.size()){ err("Битовая карта общего налия");
						}else{ //mpre("Значен vals vals=" +map ,__LINE__);
						}return vals; }(); vals.empty()){ err("Общая карта значений");
					}else if(calc = (vals.test(1) ? "1" : "0"); calc.empty()){ err("Значение расчетов");
					}else{ //mpre("Значение корневой группы key=" +to_string(key) +" itog_id=" +itog_id +" calc=" +calc ,__LINE__);
					}return calc; }(); 1 != calc.length()){ //mpre("Расчет calc=" +calc ,__LINE__);
				}else if([&](){ // Сохранение результата в индекс
					if(std::string itog_str = (bmf::ARGV.end() == bmf::ARGV.find("itog") ? "" : "." +bmf::ARGV.at("itog")); false){ mpre("ОШИБКА формирования итога в консоли", __LINE__);
					}else if(bmf::Progress("Набор:" +bmf::dataset.at("id") + itog_str+" Эпоха:" +epoch +" Примеров:" +to_string(key+1) +" Изменений:"+ to_string(err) /*+" (" +to_string(_microtime_) +" сек.)"*/ ,(float)(key+1)/dataset_count, __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
					}else if(std::string dataset_map_id = "index," +index.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					}else if(TMs index_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
					}else if(std::string map = (index_map.end() == index_map.find("map") ? std::string(dataset_count, '0') : index_map.at("map")); map.empty()){ mpre("ОШИБКА расчета карты", __LINE__);
					}else if(boost::dynamic_bitset<> imap(map); imap.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
					}else if(std::string val = (imap.test(key) ? "1" : "0"); val.empty()){ err("Результат в карте");
					}else if([&](){ // Сохранение изменений
						if(val == calc){ //mpre("Расчетный результат совпал с картой calc=" +calc +" learn=" +learn +" val=" +val +" ", __LINE__);
						}else if(imap[key] = atoi(calc.c_str()); imap.empty()){ err("Установка расчетного бита в карту");
						}else if(std::string map = [&](std::string map = ""){ boost::to_string(imap, map); return map; }(); (map.length() != dataset_count)){ mpre("ОШИБКА установки карты исходника", __LINE__);
						}else if(TMs index_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"map", map}}, {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"map", map}}, __LINE__); index_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
						}else{ //mpre("Сохранение результата в базу val=" +val ,__LINE__);
						}return false; }()){ err("Сохранение изменений");
					}else{ //mpre("Сохранение обновленной карты морфа itog_id=" +itog_id +" count=" +to_string(imap.count()), __LINE__);
					}return false; }()){ err("Сохранение результата в индекс")
				}else if([&](){ // Обучение
					if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Режим обучения отключен" ,__LINE__);
					}else if(calc == learn){ //mpre("Не обучаем значения совпали key=" +to_string(key) +" calc=" +string(calc ? "1" : "0") +" learn=" +string(learn ? "1" : "0") ,__LINE__);
					}else if(string grow = [&](string grow = ""){ // Значение роста
						//if(TMs index = bmf::Up_mysql("index" ,{{"md5" ,md5(itog_id +":1")}} ,{} ,{} ,__LINE__); index.empty()){ err("Корневой морф");
						if(string index_md5 = md5(itog_id +":1"); index_md5.empty()){ err("Составной идентификатор корневого морфа");
						}else if(TMs index = [&](TMs index = {}){ // Выборка морфа
							if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
							}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
							}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
							}return index; }(); index.empty()){ err("Выборка морфа");
						}else if(string index_id =(index.end() ==index.find("id") ?"" :index.at("id")); index_id.empty()){ err("Идентификатор морфа");
						}else if(TMs index_grp = bmf::Up_mysql("index_grp" ,{{"dataset_id" ,bmf::dataset.at("id")} ,{"itog_id", itog_id} ,{"index_id" ,index_id} ,{"key" ,to_string(key)}} ,{} ,{} ,__LINE__); index_grp.empty()){ err("Значение группы");
						}else if(string _itog_id = (index_grp.end() == index_grp.find("itog_id") ?"" :index_grp.at("itog_id")); _itog_id != itog_id){ err("Смешение итогов");
						}else if(grow = (index_grp.end() == index_grp.find("grow") ? "" : index_grp.at("grow")); grow.empty()){ err("Значение роста для расширения модели");
						}else{ //mpre("Значение роста grow=" +grow ,__LINE__);
						}return grow; }(); grow.empty()){ err("Значение роста");
					}else if(TMs index_grow = [&](TMs index_grow = {}){ // Морф роста
						if(string _grow = ("-" == grow.substr(0 ,1) ?grow.substr(1 ,-1) :grow); _grow.empty()){ err("Положительное значение роста");
						}else if(index_grow = bmf::Up("index" ,{{"id", _grow}} ,{} ,{} ,__LINE__); index_grow.empty()){ err("Морф роста");
						}else if(string _itog_id = (index_grow.end() == index_grow.find("itog_id") ?"" :index_grow.at("itog_id")); _itog_id != itog_id){ err("Смешение итогов");
						}else if(string addr_grow =(index_grow.end() ==index_grow.find("addr") ?"" :index_grow.at("addr")); addr_grow.empty()){ err("Адрес расширяемого морфа");
						}else{ //mpre("Расширение key=" +to_string(key) +" calc=" +calc +" learn=" +learn +" морф itog_id=" +itog_id +" addr_grow=" +addr_grow ,__LINE__); //mpre(index_grow ,"Расширяемый морф" ,__LINE__);
						}return index_grow; }(); index_grow.empty()){ err("Морф роста");
					}else if(string addr_grow = (index_grow.end() == index_grow.find("addr") ? "" : index_grow.at("addr")); addr_grow.empty()){ err("Адрес морфа расширения");
					}else if(string addr_grp = "1" +(addr_grow.length()%6 ?addr_grow.substr(addr_grow.length()%6 ,-1) :addr_grow.substr(6 ,-1)); addr_grp.empty()){ err("Адрес группы");
					}else if(TMs index_grp = [&](TMs index_grp = {}){ // Группа расширяемого морфа
						if(string index_md5 = md5(itog_id +":" +addr_grp); index_md5.empty()){ err("Хеш морфа");
						//}else if(TMs index =(BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5)); index.empty()){ mpre("ОШИБКА Адрес группы расширяемого морфа addr_grow=" +addr_grow +" addr_grp=" +addr_grp ,__LINE__);
						//}else if(TMs index = bmf::Up_mysql("index" ,{{"md5", index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф расширяемой группы");
						}else if(TMs index = [&](TMs index = {}){ // Выборка морфа
							if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
							}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
							}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
							}return index; }(); index.empty()){ err("Выборка морфа");
						}else if(string index_id =(index.end() ==index.find("id") ?"" :index.at("id")); index_id.empty()){ err("Идентификатор расширяемого морфа");
						}else if(string sql = "SELECT * ,CONV(hex(exist), 16, 2) AS exist ,CONV(hex(vals), 16, 2) AS vals ,CONV(hex(dano), 16, 2) AS dano FROM index_grp WHERE itog_id=" +itog_id +" AND dataset_id=" +bmf::dataset.at("id") +" AND index_id=" +index_id +" AND `key`=" +to_string(key) +";"; sql.empty()){ err("Запрос выборки обновлений");
						}else if(TMMs INDEX_GRP = bmf::List_mysql("index_grp", {}, sql, __LINE__); INDEX_GRP.empty()){ mpre(index ,"ОШИБКА Выборка значений морфа addr_grp=" +addr_grp +" addr_grow=" +addr_grow +" " +sql ,__LINE__);
						}else if(index_grp = INDEX_GRP.begin()->second; index_grp.empty()){ err("Значение исходного морфа");
						}else{ //mpre(index_grp ,"Группа расширяемого морфа" ,__LINE__);
						}return index_grp; }(); index_grp.empty()){ err("Получение группы расширяемого морфа");
					}else if(boost::dynamic_bitset<> exist = [&](boost::dynamic_bitset<> exist = {}){ //Карта наличия
						if(string str = (index_grp.end() == index_grp.find("exist") ? "" : index_grp.at("exist")); false){ mpre(index_grp ,"ОШИБКА Значение общей карты наличия" ,__LINE__);
						}else if(string map = (string(64 -str.length() ,'0') +str); 64 != map.length()){ err("Полная строка карты");
						}else if(exist = boost::dynamic_bitset(map); 64 != exist.size()){ err("Битовая карта общего наличия");
						}else{ //mpre("Карта ключ exist=" +map ,__LINE__);
						}return exist; }(); exist.empty()){ err("Общая карта наличия");
					}else if(int nn = [&](int nn = 0){ // Смещение адреса
						if(string adr = (addr_grow.length() %6 ?addr_grow.substr(0, addr_grow.length() %6) :addr_grow.substr(0 -addr_grow.length() %6, 6)); adr.empty()){ mpre("ОШИБКА Расчет локального адреса addr_grow=" +addr_grow ,__LINE__);
						}else if(nn = bmf::Bin2dec(adr); !nn){ mpre("Смещение локального адреса addr_grow=" +addr_grow ,__LINE__);
						}else{ //mpre("Локальный адрес addr_grow=" +addr_grow +" adr=" +adr ,__LINE__);
						}return nn; }(); !nn){ err("Значение смещения адреса");
					}else if(string link = ("-" == grow.substr(0 ,1) ?"0" :"1"); 1 != link.length()){ err("Значение ссылки");
					}else if(string adr = "1" +link +bmf::Dec2bin(nn).substr(1, -1); adr.empty()){ err("Адрес нового морфа")
					}else if(string list = [&](string list = ""){ // Список повторов
						if(list = (index_grp.end() == index_grp.find("list") ? "" : index_grp.at("list")); addr_grp.empty()){ //mpre("Группа первого уровня" ,__LINE__);
						}else if(TMs GRP = [&](TMs GRP = {}){ for(int i =1; i <=addr_grp.length(); i+=6){ // Список хешей групп
							if(string grp = "1" +addr_grp.substr(i, -1); grp.empty()){ mpre("Группа родителя grp=" +grp +" i=" +to_string(i) +" substr(" +to_string(grp.length()-i) +",-1)", __LINE__);
							}else if(string index_md5 = md5(itog_id +":" +grp); index_md5.empty()){ err("Хеш группы");
							}else if(TMs index = (BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5)); index.empty()){ //mpre("ОШИБКА Морф группы _grp=" +_grp +" index_md5" +index_md5 ,__LINE__);
							}else if(string index_id = (index.end() ==index.find("id") ?"" :index.at("id")); index_id.empty()){ err("Идентификатор морфа");
							}else if(GRP[grp] = index_id; GRP.empty()){ err("Список идентификаторов");
							}else{ //mpre("Разбивание на группы grp=" +grp +" index_md5=" +index_md5 ,__LINE__);
							}}return GRP; }(); GRP.empty()){ mpre("ОШИБКА Список md5 хешей групп addr_grp=" +addr_grp ,__LINE__);
						}else if(TMMi INDEX_GRP = [&](TMMi INDEX_GRP ={}){ // Расчетные значения
							if(string in = [&](string in = ""){ for(auto [grp ,index_id]:GRP){ in +="," +index_id; }return in.substr(1 ,-1); }(); in.empty()){ err("Список идентификаторов морфов групп");
							}else if(string sql = "SELECT id ,dataset_id ,`key` ,index_id ,list, grow FROM index_grp WHERE itog_id=" +itog_id +" AND dataset_id=" +bmf::dataset.at("id") +" AND `key`=" +to_string(key) +" AND index_id IN(" +in +");" ;sql.empty()){ err("Запрос на выборку списка значений");
							}else if(TMMs _INDEX_GRP = bmf::List_mysql("index_grp" ,{}, sql ,__LINE__); _INDEX_GRP.empty()){ mpre(GRP ,"ОШИБКА Список групп " +sql ,__LINE__);
							}else if([&](){ for(auto [index_grp_id ,index_grp]:_INDEX_GRP){ // По числовому ключу
								if(string id = (index_grp.end() == index_grp.find("id") ? "" : index_grp.at("id")); id.empty()){ err("Идентификатор группы");
								}else if(INDEX_GRP[atoi(id.c_str())] = index_grp; INDEX_GRP.empty()){ err("Добавление значения");
								}else{ //mpre("Сортировка по числовому значению" ,__LINE__);
								}}return false; }()){ err("С числовым ключем");
							}else{ //mpre("Запрос на выборку списка расчетов групп " +sql ,__LINE__);
							}return INDEX_GRP; }(); INDEX_GRP.empty()){ err("Список расчетных значений");
						}else if(list = [&](string list = ""){ for(auto [index_grp_id ,index_grp]:INDEX_GRP){ // Список повторов
							if(list += (index_grp.end() ==index_grp.find("list") ?"" :index_grp.at("list")); list.empty()){ err("Список повторов группы");
							}else{ //mpre("Добавление списка повторов группы в общий список _list=" +_list ,__LINE__);
							}}return list; }(); list.empty()){ err("Список повторов");
						}else{ //mpre(INDEX_GRP ,"Список групп" ,__LINE__); mpre(GRP ,"Cписок повторов addr_grp=" +addr_grp +" list=" +list ,__LINE__);
						}return list; }(); list.empty()){ err("Список повторов");
					}else if(TMs index_new = [&](TMs index_new ={}){ // Создание нового морфа
						if(string addr_new = adr +addr_grp.substr(1, -1); addr_new.empty()){ err("Расчет нового адреса");
						}else if(index_new = [&](TMs index_new = {}){ // Добавление нового морфа
							if(string index_md5 = md5(itog_id +":" +addr_new); index_md5.empty()){ err("Идентификатор морфа");
							}else if(index_new = (BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?index_new :BMF_INDEX.at(index_md5)); !index_new.empty()){ mpre("ОШИБКА Новый морф уже в базе key=" +to_string(key) +" grow=" +grow +" itog_id=" +itog_id +" addr_grow=" +addr_grow +" addr_grp=" +addr_grp +" link=" +link +" addr_new=" +addr_new ,__LINE__);
							}else if(string index_md5 = md5(itog_id +":" +addr_new); index_md5.empty()){ err("Хеш адреса");
							}else if(TMs dano = bmf::Choice(list, key ,itog_id, DANO, DANO_BITMAP ,__LINE__); dano.empty()){ err("Выборка первоначального исходника");
							}else if(string index_id = [&](string index_id = "0"){ // Морф группы
								if(1 == addr_new.length()%6){ //mpre("Собственно морф группы" ,__LINE__);
								}else if(index_id = (index_grp.end() == index_grp.find("index_id") ? "" : index_grp.at("index_id")); index_id.empty()){ err("Группа");
								}else{ //mpre("Идентификатор морфа группы index_id=" +index_id ,__LINE__);
								}return index_id; }(); index_id.empty()){ err("Морф группы");
							}else if(TMs _index_new = {{"grp", index_id}, {"addr", addr_new} ,{"md5" ,index_md5}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}}; _index_new.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
							}else if(string index_md5 = md5(itog_id +":" +addr_new); index_md5.empty()){ err("Хеш морфа");
							}else if(index_new = bmf::Up_mysql("index" ,{{"md5", index_md5}} ,_index_new ,{} ,__LINE__); index_new.empty()){ err("Добавление нового морфа");
							}else if(BMF_INDEX.insert(make_pair(index_md5 ,index_new)); BMF_INDEX.empty()){ err("Добавление морфа в справочник");
							}else if([&](){ // Уведомление
								if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ?"" :bmf::ARGV.at("verbose").c_str()); 1 != verbose){ //mpre("Не отображаем подробную информацию" ,__LINE__);
								//}else if(string vals = [&](string vals = ""){ for(auto [dano_id ,bitmap]:DANO_BITMAP){ vals += bitmap.test(key) ? "1" : "0"; }return vals; }(); vals.empty()){ err("Список значений исходника");
								}else if(string dano_id =(index_new.end() ==index_new.find("dano_id") ?"" :index_new.at("dano_id")); dano_id.empty()){ err("Идентификатор исхоного значения");
								}else{ mpre("Добавление нового морфа epoch=" +epoch +" key=" +to_string(key) +" addr_grow=" +addr_grow +" calc=" +calc +" learn=" +learn +" dano_id=" +dano_id +" link=" +link +" adr=" +adr +" addr_grp=" +addr_grp +" addr_new=" +addr_new +" " ,__LINE__);
								}return false; }()){ err("Уведомление");
							}else if(!++err){ err("Инкремент количества ошибок");
							}else{ //mpre("Добавление нового key=" +to_string(key) +" itog_id=" +itog_id +" морфа grow=" +grow +" addr_new=" +addr_new ,__LINE__);
						}return index_new; }(); index_new.empty()){ err("Добавление нового морфа");
						}else if(TMs _index_grp = [&](TMs index_grp = {}){ // Общая группа расширяемого морфа
							if(string index_md5 = md5(itog_id +":" +addr_grp); index_md5.empty()){ err("Хеш морфа");
							//}else if(TMs index =(BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5)); index.empty()){ mpre("ОШИБКА Адрес группы расширяемого морфа addr_grow=" +addr_grow +" addr_grp=" +addr_grp ,__LINE__);
							//}else if(TMs index = bmf::Up_mysql("index" ,{{"md5", index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Добавление нового морфа");
							}else if(TMs index = [&](TMs index = {}){ // Выборка морфа
								if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
								}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
								}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
								}return index; }(); index.empty()){ err("Выборка морфа");
							}else if(string index_id =(index.end() ==index.find("id") ?"" :index.at("id")); index_id.empty()){ err("Идентификатор расширяемого морфа");
							}else if(string sql = "SELECT * ,CONV(hex(exist), 16, 2) AS exist ,CONV(hex(vals), 16, 2) AS vals ,CONV(hex(dano), 16, 2) AS dano FROM index_grp WHERE itog_id=" +itog_id +" AND dataset_id=-1 AND index_id=" +index_id +" AND `key`=-1;"; sql.empty()){ err("Запрос выборки обновлений");
							}else if(TMMs INDEX_GRP = bmf::List_mysql("index_grp", {}, sql, __LINE__); INDEX_GRP.empty()){ mpre(index ,"ОШИБКА Выборка значений морфа addr_grp=" +addr_grp +" addr_grow=" +addr_grow +" " +sql ,__LINE__);
							}else if(index_grp = INDEX_GRP.begin()->second; index_grp.empty()){ err("Значение исходного морфа");
							}else{ //mpre(index_grp ,"Группа расширяемого морфа" ,__LINE__);
							}return index_grp; }(); index_grp.empty()){ err("Получение общей группы расширяемого морфа");
						}else if([&](){ // Дочерняя группа
							if(6 >= adr.length()){ //mpre("Не добавляем дочернюю группу adr=" +adr ,__LINE__);
							}else if(string addr_grp = addr_new.substr(addr_new.length() %6 ,-1); addr_grp.empty()){ err("Адрес гурппы");
							}else if(string index_md5 =md5(itog_id +":" +addr_new); index_md5.empty()){ err("Хеш морфа группы");
							//}else if(TMs _index = (BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?_index :BMF_INDEX.at(index_md5)); _index.empty()){ mpre("ОШИБКА Выборка морфа группы " +itog_id +":" +addr_grp ,__LINE__);
							//}else if(TMs _index = bmf::Up_mysql("index" ,{{"md5", index_md5}} ,{} ,{} ,__LINE__); _index.empty()){ err("Морф дочерней группы");
							}else if(TMs _index = [&](TMs index = {}){ // Выборка морфа
								if(index = BMF_INDEX.end() == BMF_INDEX.find(index_md5) ?index :BMF_INDEX.at(index_md5); !index.empty()){ //mpre("Морф в справочнике" ,__LINE__);
								}else if(index = bmf::Up_mysql("index" ,{{"md5" ,index_md5}} ,{} ,{} ,__LINE__); index.empty()){ err("Морф развития не найден");
								}else{ //mpre("Выборка морфа index_md5=" +index_md5 ,__LINE__);
								}return index; }(); _index.empty()){ err("Выборка морфа");
							}else if(string index_id = (_index.end() ==_index.find("id") ?"0" :_index.at("id")) ;!atoi(index_id.c_str())){ mpre(_index ,"Морф" ,__LINE__); err("Идентификатор корневого морфа");
							}else if(string index_grp_id =(_index_grp.end() ==_index_grp.find("id") ?"" :_index_grp.at("id")); index_grp_id.empty()){ err("Идентификатор основной группы");
							}else if(TMs _index_grp = bmf::Up_mysql("index_grp" ,{{"dataset_id" ,"-1"} ,{"itog_id" ,itog_id} ,{"key" ,"-1"} ,{"index_id" ,index_id}}, {}, {} ,__LINE__); !_index_grp.empty()){ //mpre("Дочерняя гурппа создана ранее" ,__LINE__);
							}else if(string sql = "INSERT INTO index_grp SET index_grp_id=" +index_grp_id +" ,dataset_id=-1 ,itog_id=" +itog_id +" ,index_id=" +index_id +" ,`key`=-1 ,count=1 ,exist =0b1" +link +" ON DUPLICATE KEY UPDATE itog_id=VALUES(itog_id);"; sql.empty()){ err("Запрос на создание группы");
							}else if(mysql_query(bmf::mysql ,sql.c_str())){ mpre("ОШИБКА запроса " +sql +"\n" +string(mysql_error(bmf::mysql)) ,__LINE__);
							}else{ //mpre("Группа новая addr_grow=" +addr_grow +" adr=" +adr +" link=" +link +" " +sql ,__LINE__);
							}return false; }()){ err("Добавление дочерней группы");
						}else if([&](){ // Обновление общей группы
							if(string str_exist = string("1") +string(bmf::Bin2dec(adr), '0'); str_exist.empty()){ err("Установка бита");
							}else if(string index_md5 =md5(itog_id +":" +addr_grp); index_md5.empty()){ err("Хер морфа");
							}else if(TMs _index =(BMF_INDEX.end() ==BMF_INDEX.find(index_md5) ?_index :BMF_INDEX.at(index_md5)); _index.empty()){ mpre("Морф группы не найден addr_grp=" +addr_grp ,__LINE__);
							}else if(string index_id =(_index.end() ==_index.find("id") ?"0" :_index.at("id")); index_id.empty()){ err("Идентификатор группового морфа");
							}else if(int count = exist.count() +(exist.test(0) ? 0 : 1); !count){ err("Количество морфов в группе");
							}else if(string sql = "UPDATE index_grp SET count=" +to_string(count) +" ,exist =exist |0b" +str_exist +" WHERE itog_id=" +itog_id +" AND dataset_id=-1 AND index_id=" +index_id +" AND `key`=-1;" ;sql.empty()){ err("Запрос на обновление");
							}else if(mysql_query(bmf::mysql ,sql.c_str())){ mpre("ОШИБКА запроса " +sql +"\n" +string(mysql_error(bmf::mysql)) ,__LINE__);
							}else{ //mpre("Запрос на обновление общей группы addr_grp=" +addr_grp +" nn=" +to_string(nn) +" " +sql ,__LINE__);
							}return false; }()){ err("Обновление общей группы");
						}else{ //mpre("Создание морфа addr_grow=" +addr_grow +" addr_grp=" +addr_grp +" link=" +link +" nn=" +to_string(nn) +" adr=" +adr ,__LINE__);
						}return index_new; }(); index_new.empty()){ err("Создание морфа");
					}else{ //mpre("Конец обучение key=" +to_string(key) +" calc=" +(calc ? "1" : "0") +" learn=" +(learn ? "1" : "0") ,__LINE__);
					}return false; }()){ err("Обучение");
				}else{ //mpre("Расчет итога key=" +to_string(key) +" " +itog_id ,__LINE__);
				}}return false; }()){ err("Расчет модели");
			}else if(bmf::dataset = [&](TMs dataset){ // Обновление ключа
				if(dataset = bmf::Up(bmf::DATASET, {{"id", dataset.at("id")}}, {}, {}, __LINE__); dataset.empty()){ err("Выборка набора данных");
				}else if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Не изменяем позицию без обучения", __LINE__);
				}else if(string _key = (bmf::ARGV.end() == bmf::ARGV.find("key") ? "" : bmf::ARGV.at("key")); !_key.empty()){ //mpre("Пропуск расчета" ,__LINE__);
				}else if(string::npos != _key.find_first_of("0123456789")){ //mpre("Не сохраняем значение ключа и ошибок" ,__LINE__);
				}else if(dataset["epoch"] != bmf::dataset["epoch"]){ err("Дублирование управляющего процесса");
				}else if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); !epoch){ //mpre("Не сохраняем свойства в эпоху" ,__LINE__);
				}else if(dataset["key"] = std::to_string(key); dataset.empty()){ err("Установка нового ключа");
				}else if(dataset["err"] = std::to_string(err); dataset.empty()){ err("Установка количества ошибок эпохи");
				}else if(dataset["microtime"] = std::to_string(microtime); dataset.empty()){ err("Установка времени расчета эпохи");
				}else if(dataset = bmf::Up(bmf::DATASET, {{"id", dataset.at("id")}}, {}, dataset, __LINE__); dataset.empty()){ err("Обновление ключа");
				}else{ //mpre(dataset, "Обновление ключа", __LINE__);
				}return dataset; }(bmf::dataset); bmf::dataset.empty()){ err("Обновление ключа");
			}else if(++key; false){ mpre("Автоинкремент ключа" ,__LINE__);
			}else if(rep = ((key >= dataset_count) || (0 > key) ? false : true); false){ mpre("ОШИБКА положительный повтор", __LINE__);
			}else{ //mpre("Итерации err=" +to_string(err) ,__LINE__);
			}}while(!(rep = !rep)); return err; }(); (0 > err)){ mpre("ОШИБКА проверки списка итогов", __LINE__);
		}else if([&](){ // Обнуление счетчика эпохи
			if(!err){ //mpre("Не увеличиваем эпохи без обучения", __LINE__);
			}else if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Не сохраняем результаты на расчёте" ,__LINE__);
			}else if(TMs dataset = bmf::dataset = bmf::Up(bmf::DATASET, {{"id", bmf::dataset.at("id")}}, {}, {}, __LINE__); bmf::dataset.empty()){ err("Выборка набора данных");
			}else if(bool base = (bmf::ARGV.end() == bmf::ARGV.find("key")); false){ err("Расчет основного процесса");
			}else if(dataset["epoch"] = (base ? to_string(atoi(dataset["epoch"].c_str())+1) : dataset["epoch"]); dataset.at("epoch").empty()){ err("Инкремента эпох");
			}else if(dataset["key"] = (base ? "0" : dataset["key"]); false){ err("Обнуление позиции");
			}else if(dataset["err"] = (base ? "0" : to_string(atoi(dataset["err"].c_str()) +err)); false){ err("Обнуление позиции");
			}else if(dataset["microtime"] = "0"; false){ err("Обнуление позиции");
			}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", dataset.at("id")}}, dataset, dataset, __LINE__); bmf::dataset.empty()){ err("Выборка набора данных");
			}else if(auto _microtime_ = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - microtime; false){ mpre("ОШИБКА расчета времени эпохи", __LINE__);
			}else if(!base){ //mpre("Не сохраняем значение ключа и ошибок" ,__LINE__);
			}else if(TMs epoch = bmf::Up_mysql("epoch", {}, {{"epoch", bmf::dataset.at("epoch")}, {"dataset_id", dataset.at("id")}, {"microtime", to_string(_microtime_)}, {"err", dataset.at("err")}}, {} ,__LINE__); epoch.empty()){ err("Сохранение результатов эпохи");
			}else{ //mpre("Инкремент эпох", __LINE__);
			}return bmf::dataset; }(); bmf::dataset.empty()){ err("Инкремент эпох");
		}else if([&](){ // Расчет процента бит
			if(1 == bmf::loop){ bmf::pips_first = (err ? (float)(dataset_count-err)/dataset_count : 1); //mpre("Расчет процента первой эпохи err=" +to_string(err) +" count=" +to_string(count), __LINE__);
			}else{ bmf::pips_last = (err ? (float)(dataset_count-err)/dataset_count : 1); //mpre("Расчет процента последней эпохи", __LINE__);
			}return false; }()){ mpre("ОШИБКА расчета процента бит", __LINE__);
		}else if(std::cerr << endl; false){ mpre("Пробел за прогрессом", __LINE__);
		}else if(errors += err; !err){ //mpre("Ошибок не обнаружено" ,__LINE__); //mpre("Ошибок не обнаружено количество Эпох обучения "+ to_string(epoch -loop) +" Размер модели " +to_string(BMF_INDEX_EX.at("").size()) +" морфов Время "+ to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 -bmf::microtime) +" сек", __LINE__);
		}else if(!(pass = !pass)){ mpre("ОШИБКА инвертирования признака повтора", __LINE__);
		}else{ //mpre("Результаты расчета Общее количество/Последней эпохи " +to_string(errors) +"/" +to_string(err) +"\n", __LINE__);
		}}while(!(pass = !pass)); return errors; }(); false){ mpre("ОШИБКА перебора эпох", __LINE__);
	}else if([&](){ // Вывод итоговых значений
		if(bmf::ARGV.end() == bmf::ARGV.find("dano")){ //mpre("Данные для расчета не указаны", __LINE__);
		}else if(TMMs DANO = bmf::List("dano", {}, __LINE__); DANO.empty()){ err("Список исходников");
		}else if(nlohmann::json _in = ("" == bmf::ARGV.at("dano") ? in : nlohmann::json::parse(bmf::ARGV.at("dano"))); _in.empty()){ mpre("ОШИБКА разбора строки атрибута -dano", __LINE__);
		}else if(nlohmann::json j = [&](nlohmann::json j = {}){ for(auto& js:_in.items()){ // Расчет списка теста
			if(nlohmann::json el = js.value(); el.empty()){ mpre("ОШИБКА элемент не найден", __LINE__);
			}else if(string key = js.key(); (0 >= key.length())){ mpre("ОШИБКА расчета ключа", __LINE__);
			}else if(el.end() == el.find("dano")){ mpre("ОШИБКА исходные данные не могут быть пустыми [dano]", __LINE__);
			}else if(TMs dano = [&](TMs dano = {}){ TMs _dano = el.at("dano"); dano = _dano; return dano; }(); dano.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
			}else if(TMs itog = [&](TMs itog = {}){ // Получение итога
				if(el.end() == el.find("itog")){ //mpre("Итог не задан", __LINE__);
				}else if([&](){ TMs _itog = el.at("itog"); itog = _itog; return itog.empty(); }()){ mpre("ОШИБКА получения итоговых знаков", __LINE__);
				}else{ //mpre(itog, __LINE__, "Итог");
				} return itog; }(); !itog.empty()){ mpre("ОШИБКА в расчете задан итог", __LINE__);
			}else if([&](){ for(auto dano_itr:dano){ j[key]["dano"][dano_itr.first] = dano_itr.second; }return false; }()){ err("Значения дано");
			}else if(mpre("РЕЗУЛЬТАТ: "+ el.dump(), __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(int cache = Crc32(el["dano"].dump().c_str()); false){ mpre("ОШИБКА расчета ключа кеша", __LINE__);
			}else if(TMMs DANO_VALUES = bmf::List("dano_values", {}, __LINE__); DANO_VALUES.empty()){ err("Список значений исходников");
			}else if(TMMs VALS = bmf::Vals(dano, "dano", cache, bmf::DANO_VALUES, bmf::DANO_TITLES); VALS.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
			}else if(TMMs BMF_INDEX = bmf::List_mysql("index", {}, "SELECT *, addr AS id FROM `index`", __LINE__); BMF_INDEX.empty()){ err("Список морфов");
			}else if(TMMs UPDATE = [&](TMMs UPDATE = {} ,TMMs _BMF_INDEX_VALS = {}){ for(auto index_itr:BMF_INDEX){ // Расчет значений модели
				if(string addr = (index_itr.second.end() == index_itr.second.find("addr") ? "" : index_itr.second.at("addr")); addr.empty()){ err("Выборка адреса");
				}else if(string index_id = (index_itr.second.end() == index_itr.second.find("id") ? "" : index_itr.second.at("id")); addr.empty()){ err("Выборка адреса");
				}else if(TMs index_vals = (_BMF_INDEX_VALS.end() == _BMF_INDEX_VALS.find(index_itr.second.at("id")) ? index_vals : _BMF_INDEX_VALS.at(index_itr.second.at("id"))); false){ err("Выборка значения морфа");
				}else if(string addr_1 = "11" +addr.substr(1, addr.length()); addr_1.empty()){ err("Пропуск расчета старшей связи");
				}else if(string addr_0 = "10" +addr.substr(1, addr.length()); addr_0.empty()){ err("Пропуск расчета младшей связи");
				}else if(string dano_val = [&](string dano_val = ""){ // Значение исходника
					if(string dano_id = (index_itr.second.end() == index_itr.second.find("dano_id") ? "" : index_itr.second.at("dano_id")); dano_id.empty()){ err("Идентификатор исходника");
					}else if(TMs dano = (DANO.end() == DANO.find(dano_id) ? dano : DANO.at(dano_id)); dano.empty()){ err("Исходник не найден");
					}else if(string dano_values_id = (dano.end() == dano.find("dano_values_id") ? "" : dano.at("dano_values_id")); dano_values_id.empty()){ err("Значение исходника");
					}else if(auto dano_values_itr = VALS.find(dano_values_id); VALS.end() == dano_values_itr){ mpre("Список значения не найден " +dano_values_id ,__LINE__);
					}else if(dano_val = (dano_values_itr->second.end() == dano_values_itr->second.find(dano_id) ? "0" : dano_values_itr->second.at(dano_id)); dano_val.empty()){ err("Значение исходника");
					}else{ //mpre("Расчет исходного значения dano_val=" +dano_val +" dano_id=" +dano_id, __LINE__);
					}return dano_val; }(); 1 != dano_val.length()){ err("Значение исходника");
				}else if(TMs index_vals_1 = [&](TMs index_vals_1 = {}){ // Значение старшего расчета
					if(BMF_INDEX.end() == BMF_INDEX.find(addr_1)){ //mpre("Морф старший не найден " +addr +" " +addr_1 ,__LINE__);
					}else if(string index_id = (BMF_INDEX.at(addr_1).end() == BMF_INDEX.at(addr_1).find("id") ? "" : BMF_INDEX.at(addr_1).at("id")); index_id.empty()){ err("Идентификатор морфа");
					}else if(_BMF_INDEX_VALS.end() == _BMF_INDEX_VALS.find(index_id)){ //mpre("Значение не найдено addr_1=" +addr_1 +" index_id=" +index_id  ,__LINE__);
					}else if(index_vals_1 = _BMF_INDEX_VALS.at(index_id); index_vals_1.empty()){ err("Выборка значения");
					}else{ //mpre("Значение старшего морфа " +addr ,__LINE__);
					}return index_vals_1; }(); false){ err("Значение старшего расчета");
				}else if(string v1 = [&](string v1){ // Значение старшей связи
					if(BMF_INDEX.end() == BMF_INDEX.find(addr_1)){ //mpre("Морф не создан v1=" +v1 +" " +addr_1 ,__LINE__);
					}else if(v1 = (index_vals.end() == index_vals.find("v1") ? dano_val : index_vals.at("v1")); v1.empty()){ err("Текущее значение поля");
					}else if(string _v0_ = (index_vals.end() == index_vals.find("v0") ? dano_val : index_vals.at("v0")); _v0_.empty()){ err("Текущее значение поля");
					}else if(index_vals_1.empty()){ //mpre("Берем значение расчитанное ранее" ,__LINE__);
					}else if(string _v1 = (index_vals_1.end() == index_vals_1.find("v1") ? v1 : index_vals_1.at("v1")); _v1.empty()){ err("Значение нижестоящего морфа");
					}else if(string _v0 = (index_vals_1.end() == index_vals_1.find("v0") ? _v0_ : index_vals_1.at("v0")); _v0.empty()){ err("Значение нижестоящего морфа");
					}else if(string grp = _v1 +_v0; grp.empty()){ err("Группа старшего морфа");
					}else if(v1 = ("01" == grp ? "1" : "0"); v1.empty()){ err("Значение расчитанное ранее");
					}else{ //mpre("Расчет значения старшей связи index_id=" +index_id, __LINE__);
					}return v1; }(dano_val); 1 != v1.length()){ err("Значение старшей связи");
				}else if(string g1 = [&](string g1){ // Значение роста старшего морфа
					if(BMF_INDEX.end() == BMF_INDEX.find(addr_1)){ //mpre("Морф не создан g1=" +g1 +" " +addr ,__LINE__);
					}else if(g1 = (index_vals.end() == index_vals.find("g1") ? g1 : index_vals.at("g1")); g1.empty()){ mpre("Значение не создано" ,__LINE__);
					}else if(g1 = (index_vals_1.end() == index_vals_1.find("grow_md5") ? g1 : index_vals_1.at("grow_md5")); g1.empty()){ err("Выборка значения из нижестоящего");
					}else{ //mpre("Загрузка значения от нижестоящего" ,__LINE__);
					}return g1; }(index_id); g1.empty()){ mpre("Морф " +index_id ,__LINE__); err("Значение роста старшего морфа");
				}else if(TMs index_vals_0 = [&](TMs index_vals_0 = {}){ // Значение старшего расчета
					if(BMF_INDEX.end() == BMF_INDEX.find(addr_0)){ //mpre("Морф младший не найден " +addr_0 ,__LINE__);
					}else if(string index_id = (BMF_INDEX.at(addr_0).end() == BMF_INDEX.at(addr_0).find("id") ? "" : BMF_INDEX.at(addr_0).at("id")); index_id.empty()){ err("Идентификатор морфа");
					}else if(_BMF_INDEX_VALS.end() == _BMF_INDEX_VALS.find(index_id)){ //mpre("Значение не найдено addr_0=" +addr_0 +" index_id=" +index_id +" " ,__LINE__);
					}else if(index_vals_0 = _BMF_INDEX_VALS.at(index_id); index_vals_0.empty()){ err("Выборка значения");
					}else{ //mpre("Значение младшего морфа " +addr ,__LINE__);
					}return index_vals_0; }(); false){ err("Значение старшего расчета");
				}else if(string v0 = [&](string v0){ // Значение старшей связи
					if(BMF_INDEX.end() == BMF_INDEX.find(addr_0)){ //mpre("Морф не создан g1=" +v0 +" " +addr ,__LINE__);
					}else if(string _v1_ = (index_vals.end() == index_vals.find("v1") ? dano_val : index_vals.at("v1")); _v1_.empty()){ mpre(index_vals ,"Значения dano_val=" +dano_val ,__LINE__); err("Текущее значение поля");
					}else if(v0 = (index_vals.end() == index_vals.find("v0") ? dano_val : index_vals.at("v0")); v0.empty()){ err("Текущее значение поля");
					}else if(index_vals_0.empty()){ //mpre("Использование значения расчитанного ранее" ,__LINE__);
					}else if(string _v1 = (index_vals_0.end() == index_vals_0.find("v1") ? _v1_ : index_vals_0.at("v1")); _v1.empty()){ err("Значение нижестоящего морфа");
					}else if(string _v0 = (index_vals_0.end() == index_vals_0.find("v0") ? v0 : index_vals_0.at("v0")); _v0.empty()){ err("Значение нижестоящего морфа");
					}else if(string grp = _v1 +_v0; grp.empty()){ err("Группа старшего морфа");
					}else if(v0 = ("01" == grp ? "0" : "1"); v0.empty()){ err("Значение расчитанное ранее");
					}else{ //mpre("Расчет значения младшей связи v0=" +v0 +" index_id=" +index_id, __LINE__);
					}return v0; }(dano_val); 1 != v0.length()){ err("Значение младшей связи");
				}else if(string g0 = [&](string g0){ // Значение роста старшего морфа
					if(BMF_INDEX.end() == BMF_INDEX.find(addr_0)){ //mpre("Морф не создан g0=" + g0 +" " +addr ,__LINE__);
					}else if(g0 = (index_vals.end() == index_vals.find("g0") ? g0 : index_vals.at("g0")); g0.empty()){ mpre("Значение не создано" ,__LINE__);
					}else if(g0 = (index_vals_0.end() == index_vals_0.find("grow_md5") ? g0 : index_vals_0.at("grow_md5")); g0.empty()){ err("Выборка значения из нижестоящего");
					}else{ //mpre("Загрузка расширения от нижестоящего" ,__LINE__);
					}return g0; }(index_id); g0.empty()){ mpre("Морф " +index_id ,__LINE__); err("Значение роста младшего морфа");
				}else if(string grow_md5 = [&](string grow_md5){ // Значение роста
					if(string grp = v1 +v0; 2 != grp.length()){ err("Значение группы");
					}else if(string link_grow = grp.substr(atoi(dano_val.c_str()), 1); 1 != link_grow.length()){ err("Ссылка расширяемого морфа");
					}else if(grow_md5 = ("0" == link_grow ? g0 : g1); grow_md5.empty()){ err("Значение роста");
					//}else if(string _addr = (index_vals.end() == index_vals.find("addr") ? "" : index_vals.at("addr")); "110-Два-1" != _addr){ //mpre("Прпоускаем все кроме указанного" ,__LINE__);
					}else{ //mpre("Расчет значения роста g1=" +g1 +" g0=" +g0 +" link_grow=" +link_grow +" dano_val=" +dano_val +" grp=" +grp +" grow_md5=" +grow_md5 +" " +addr +" " +index_itr.second.at("id") ,__LINE__);
					}return grow_md5; }(index_itr.second.at("id")); grow_md5.empty()){ err("Значение роста");
				}else if([&](){ // Добавление значения
					if(!index_vals.empty()){ //mpre("Расчет уже создан", __LINE__);
					}else if(string grp = v1 +v0; 2 != grp.length()){ err("Значение группы");
					}else if(TMs _index_vals = {{"index_md5", index_itr.second.at("id")} ,{"v1" ,v1} ,{"v0" ,v0} ,{"dataset_id", "0"}, {"key", to_string(-1)}, {"grow_md5", grow_md5} ,{"g1" ,g1} ,{"g0" ,g0}}; _index_vals.empty()){ err("Группа морфа");
					}else if(UPDATE.insert(make_pair(addr, _index_vals)); UPDATE.empty()){ err("Добавление нового значения");
					}else if(_BMF_INDEX_VALS.insert(make_pair(_index_vals.at("index_md5"), _index_vals)); _BMF_INDEX_VALS.empty()){ err("Обновление текущих значений");
					}else{ //mpre(_index_vals ,"Добавление нового значения key=-1 grow_md5=" +grow_md5 +" " +addr +" " ,__LINE__);
					}return false; }()){ err("Добавление значение расчета");
				}else{ //mpre("Окончение расчета модели key=" +to_string(key) ,__LINE__);
				}}return UPDATE; }(); false){ err("Расчет модели");
			}else if(TMMs ITOG = bmf::List("itog", {}, __LINE__); ITOG.empty()){ err("Список исходников");
			//}else if(mpre(ITOG ,"Итог" ,__LINE__); false){ err("Уведомление");
			//}else if(mpre(UPDATE ,"Обновление" ,__LINE__); false){ err("Уведомление");
			}else if(TMs IVAL = [&](TMs IVAL = {}){ for(auto itog_itr:ITOG){ // Результаты расчетов
				if(TMs itog = itog_itr.second; itog.empty()){ err("Выборка итога");
				}else if(string name = (itog.end() == itog.find("id") ? "" : itog.at("id")); name.empty()){ err("Имя итога");
				}else if(string addr = "1-" +name; addr.empty()){ err("Расчет адреса");
				}else if(TMs index_vals = (UPDATE.end() == UPDATE.find(addr) ? index_vals : UPDATE.at(addr)); index_vals.empty()){ mpre("Расчет итога не найден " +addr ,__LINE__);
				}else if(string v1 = (index_vals.end() == index_vals.find("v1") ? "" : index_vals.at("v1")); v1.empty()){ err("Старшее значение");
				}else if(string v0 = (index_vals.end() == index_vals.find("v0") ? "" : index_vals.at("v0")); v0.empty()){ err("Младшее значение");
				}else if(string grp = v1 +v0; 2 != grp.length()){ err("Значение группы");
				}else if(string val = ("01" == grp ? "0" : "1"); 1 != val.length()){ err("Расчет значения");
				}else if(IVAL[itog_itr.first] = val; IVAL.empty()){ err("Установка значения");
				}else{ //mpre(index_vals ,"Итог grp=" +grp +" val=" +val +" " +addr ,__LINE__);
				}}return IVAL; }(); IVAL.empty()){ err("Получение результатов итогов");
			//}else if(mpre(IVAL ,"Расчет итогов" ,__LINE__); false){ err("Уведомление");
			}else if(TMMs ITOG_VALUES = bmf::List("itog_values", {}, __LINE__); ITOG_VALUES.empty()){ err("Список исходников");

			}else if(TMs VAL = [&](TMs VAL = {}){ for(auto itog_values_itr:ITOG_VALUES){ // Значения исходников
				if(std::map<int,string> VL = [&](std::map<int,string> VL = {}){ for(auto itog_itr:ITOG){ // Список значений
					if(string name_itog = (itog_itr.second.end() == itog_itr.second.find("id") ? "" : itog_itr.second.at("id")); name_itog.empty()){ err("Уведомление");
					}else if(string val = (IVAL.end() == IVAL.find(name_itog) ? "0" : IVAL.at(name_itog)); val.empty()){ err("Знак исходника");
					}else if(string name = (itog_itr.second.end() == itog_itr.second.find("name") ? "" : itog_itr.second.at("name")); name.empty()){ err("Расчет позиции исходника");
					}else if(int nn = atoi(name.c_str()); false){ err("Числовое значение знака");
					}else if(VL[nn] = val; VL.empty()){ err("Сохранение значения");
					}else{ //mpre("Значение name_values=" +name_values +" name_itog=" +name_itog +" name=" +name +" nn=" +to_string(nn) +" val=" +val ,__LINE__);
					}}return VL; }(); VL.empty()){ err("Список значений");
				}else if(VL[0] = (VL.end() == VL.find(0) ? "0" : VL.at(0)); VL.empty()){ err("Добавление нулевого значения");
				}else if(string bin = [&](string val = ""){ for(auto vl_itr = VL.rbegin(); vl_itr != VL.rend(); vl_itr++){ // Двоичное число
					if(val += (vl_itr->first ? vl_itr->second : "."); val.empty()){ err("Пропуск");
					}else{ //mpre("Добавление разряда двоичного значения " +name_values +" vl_itr.first=" +to_string(vl_itr->first) +" vl_itr.second=" +vl_itr->second ,__LINE__);
					}}return val; }(); bin.empty()){ err("Расчет двоичного значения");
				}else if(bin = ("0" == VL[0] ? "" : "-") +bin; bin.empty()){ err("Добавление отрицания");
				}else if(double dec = bmf::Bin2dec(bin); false){ err("Приведение к десятичной форме");
				}else if(VAL[itog_values_itr.first] = to_string(dec); VAL.empty()){ err("Добавление в итогове значение");
				}else{ //mpre("Установка значений" ,__LINE__);
				}}return VAL; }(); VAL.empty()){ err("Перебор значений исходников");
			//}else if(mpre(VAL ,"Значения", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(string sql = "SELECT *, value AS id FROM itog_titles"; sql.empty()){ err("Составление запроса на выборку групп итогов");
			}else if(TMMs ITOG_TITLES = bmf::List_mysql("itog_titles", {} ,sql, __LINE__); false){ err("Список исходников");
			}else if([&](){ for(auto val_itr:VAL){ // Выводимые значения и группы
				if(string value = [&](string value = ""){ char dig[100]; sprintf(dig,"%g", stod(val_itr.second)); return value = string(dig); }(); value.empty()){ mpre("ОШИБКА присвоения значения массиву", __LINE__);
				}else if(ITOG_TITLES.empty()){ j[key]["itog"][val_itr.first] = value ;//mpre("Нет категориальных значений" ,__LINE__);
				}else if(TMs itog_values = (ITOG_VALUES.end() == ITOG_VALUES.find(val_itr.first) ? itog_values : ITOG_VALUES.at(val_itr.first)); itog_values.empty()){ mpre("ОШИБКА Выборки значения итога " +val_itr.first +" " +val_itr.second ,__LINE__);
				}else if(TMs itog_titles = (ITOG_TITLES.end() == ITOG_TITLES.find(value) ? itog_titles : ITOG_TITLES.at(value)); false){ err("Заголовок итога");
				}else if(j[key]["itog"][val_itr.first] = (itog_titles.end() == itog_titles.find("name") ? value : itog_titles.at("name")); j.empty()){ err("Формирование json");
				}else{ //mpre(itog_titles ,"Установка значения " +val_itr.first +" " +val_itr.second ,__LINE__);
				}}return false; }()){ err("Установка значений");
			}else{ //mpre("Итог "+ j.dump(), __LINE__);
			}} return j; }(); false){ mpre("ОШИБКА расчета списка теста", __LINE__);
		}else{ //mpre("j "+ j.dump(), __LINE__);
			std::cout << j.dump('\t');
		} return false; }()){ mpre("Ошибка отображение результата расчета", __LINE__);
	}else{ //std::cerr << endl; //mpre(_BMF_INDEX, __LINE__, "Сохранение");
	}
}

