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
#include <sqlite3.h>
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
#include <ncurses.h>
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

#define pre(x) if(x){ printf("%d.%s\n", __LINE__, x); }
#define err(x) if(x){ printf("%d.\x1b[37;41m ОШИБКА \x1b[0m %s\n", __LINE__, x); exit(1); }

//using json = nlohmann::json; json in;

using namespace std; //setlocale(LC_ALL, "ru_RU");
namespace fs = std::experimental::filesystem;


struct comp{ bool operator()(const std::string& lhs, const std::string& rhs) const{ size_t ls = lhs.size(); size_t rs = rhs.size(); return std::tie(ls, lhs) > std::tie(rs, rhs); }};

typedef std::map<string, string, comp> TMs;
typedef std::map<string, TMs, comp> TMMs;
typedef std::map<int, TMs> TMMi;
typedef std::map<string, TMMi> TM3i;
typedef std::map<string,boost::dynamic_bitset<>> TMMb;

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
	std::function<TMs(string,int,TMMs&,TMMb&)> Choice; // Расчет истории
	std::function<double(string)> Bin2dec; // Перерасчет размерности из двоичной в десятеричную
	std::function<string(double)> Dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
	std::function<string(TMs,string,int)> Learn; // Рерасчет морфа

	std::function<void(string,float,int)> Progress; // Отрисовка полосы выполнения
	std::function<TMs(TMs&,std::string,int)> Maps; // Расчет карт результатов

//	std::function<Db*(string)> Open; // Список баз данных
	std::function<bool(int)> Close; // Закрытие соединения с БД
	std::function<TMMs(TMs,string,int,string,string,string)> Vals; // Обучение
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
	}else if(nlohmann::json in = [&](nlohmann::json in = {}){ // Параметры консоли
		if([&](){ for(int i = 0; i < argc; i++){ // Перебор всех параметров
			if(0 >= i){ //mpre("Имя файла", __LINE__);
			}else if(string str = argv[i]; (0 >= str.length())){ mpre("ОШИБКА получения аргумента консоли", __LINE__);
			}else if(int npos = str.find("="); false){ mpre("Расчет позиции равно в строке аргументов", __LINE__);
			}else if(std::string arg = (std::string::npos == npos ? str : str.substr(0, npos)); false){ mpre("ОШИБКА получения имени аргумента", __LINE__);
			}else if(std::string val = (std::string::npos == npos ? "" : str.substr(npos +1, str.length())); false){ mpre("ОШИБКА получения значения аргумента", __LINE__);
			}else if([&](bool skip = false){
				if(std::string::npos != npos){ //mpre("Задан аргумент", __LINE__);
				}else if("-" != arg){ //mpre("Не признак загрузки", __LINE__);
				}else if(bmf::ARGV.insert(make_pair("-", "")); bmf::ARGV.empty()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
				}else if(!(skip = true)){ mpre("Установка значения пропуска", __LINE__);
				}return skip; }()){ // Признак загрузки
			}else if([&](bool skip = false){ // Установка БД
				if(std::string::npos != npos){ //mpre("Задан аргумент", __LINE__);
				}else if(bmf::ARGV.end() != bmf::ARGV.find("db")){ //mpre("ОШИБКА дублирование аргументов командной строки `" +bmf::ARGV.at("db") + "` " +arg, __LINE__);
				}else if(bmf::ARGV.insert(make_pair("db", arg)); bmf::ARGV.empty()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
				}else if(!(skip = true)){ mpre("Установка значения пропуска", __LINE__);
				}else{ //mpre("Установка БД " +arg, __LINE__);
				}return skip; }()){ //mpre("ОШИБКА установки БД", __LINE__);
			}else if(bmf::ARGV.insert(make_pair(arg, val)); bmf::ARGV.empty()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
			}else{ //mpre("Аргумент консоли "+ arg + "=" +val, __LINE__);
			}}return false; }()){ mpre("ОШИБКА разбора параметров консоли", __LINE__);
		}else if(in = [&](nlohmann::json in = {}){ // Входной поток
			if(bmf::ARGV.end() == bmf::ARGV.find("-")){ //mpre("Данные для обучения не установлены", __LINE__);
			}else if([&](string str = ""){ while(getline(std::cin, str)){ bmf::ARGV.at("-") += ("\n"+ str); }; return (0 >= bmf::ARGV.at("-").length()); }()){ mpre("ОШИБКА входящий параметр не задан", __LINE__);
			}else if(in = nlohmann::json::parse(bmf::ARGV.at("-")); false){ mpre("ОШИБКА Входящий параметр с обучающими данными пуст", __LINE__);
			}else{ //mpre("Данные из потока std::cin "+ in.dump(), __LINE__);
			}return in; }(); false){ mpre("ОШИБКА получения входного потока", __LINE__);
		}else{ //mpre(bmf::ARGV, "Список параметров", __LINE__);
		}return in; }(); false){ mpre("ОШИБКА параметры консоли не заданы", __LINE__);
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
	}else if([&](){ // Подключение БД
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
			}else if(bmf::mysql = mysql_init(NULL); (nullptr == bmf::mysql)){ err("ОШИБКА создания соединения");
			}else if(bmf::mysql = mysql_real_connect(bmf::mysql, bmf::dbhost.c_str(), bmf::dbuser.c_str(), bmf::dbpassword.c_str(), bmf::dbname.c_str(), bmf::dbport, NULL, 0); !bmf::mysql){ mpre("Параметры подключения dbhost=" +bmf::dbhost +" dbport=" +to_string(bmf::dbport) +" dbuser=" +bmf::dbuser +" dbpassword=" +bmf::dbpassword +" database=" +bmf::dbname, __LINE__); err("Подключение БД"); 
			}else if(mysql_query(bmf::mysql, std::string("SET NAMES utf8").c_str()); !bmf::mysql){ err("Установка кодировки");
			}else{ //mpre("ОШИБКА: " +string(mysql_error(bmf::mysql)), __LINE__);
			}return false; }()){ err("Подключение к БД mysql");
		}else if([&](){ // Подключение к БД redis
			if("redis" != bmf::dbtype){ //mpre("Используем не mysql", __LINE__);
			}else if(bmf::redis = redisConnect(bmf::dbhost.c_str() ,bmf::dbport); (bmf::redis == NULL)){ err("Подключение к redis");
			}else if(redisReply *reply = (redisReply *)redisCommand(bmf::redis ,string("SELECT " +bmf::dbname).c_str()); ("OK" != string(reply->str))){ mpre("ОШИБКА Выбор БД " +bmf::dbname +" (В redis доступны базы от 0 до 15)", __LINE__);
			}else{ //pre("Подключение к redis");
			}return false; }()){ err("Подключение к БД redis");
		}else{ //mpre("Подключение БД " +bmf::dbname, __LINE__);
		}return false; }()){ mpre("ОШИБКА подключения БД", __LINE__);
	}else if([&](){ // Функции БД
		if(bmf::Up = ([&](std::string ROWS, TMs where, TMs insert, TMs update, int line){
			TMs _row; // Возвращаем обновленное значение
			if([&](){ // Проверка условия
				if(where.end() == where.find("id")){ mpre(where, "Условие", __LINE__); mpre("ОШИБКА У поля не задан идентификатор", __LINE__);
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
		}else if(bmf::Up_mysql = ([&](std::string ROWS, TMs where, TMs insert, TMs update, int line){
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
				}else if(std::string sql = "SELECT * FROM `" + ROWS+"` WHERE " +_where; sql.empty()){ err("Запрос к БД");
				}else if(_row = [&](TMs _row = {}){ // Получение значения
					if(mysql_query(bmf::mysql, sql.c_str()); !bmf::mysql){ mpre("Выбор данных из таблицы " +sql, __LINE__);
					}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); !result){ err("ОШИБКА выполнения запроса");
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
				//}else if((*ROWS)[update.at("id")] = update; false){ mpre("ОШИБКА выборки значения из базы", __LINE__);
				}else if(std::string values = [&](std::string values = ""){ for(auto update_itr:update){ // Составление запроса
					if("id" == update_itr.first){ //mpre("Не редактируем идентификатор", __LINE__);
					}else if(values += string(values.empty() ? "" : ", ") +"`" +update_itr.first + "`='" +update_itr.second +"'"; values.empty()){ err("Подстановка значения");
					}else{ //mpre("Список значений " +values, __LINE__);
					}}return values; }(); values.empty()){ err("Установка значений");
				}else if(std::string sql = "UPDATE `" +ROWS + "` SET " +values +" WHERE `id`='" +where["id"] +"'"; sql.empty()){ err("Составление запроса");
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
				//}else if(auto [itr, ret] = ROWS->insert(make_pair(insert["id"], insert)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);a
				}else if(_row = [&](){ // Добавление значения
					if(std::string VALUES = [&](string VALUES = "", string fields = "", string values = "", string duplicate = ""){ for(auto insert_itr:insert){
						if(fields += string(fields.empty() ? "" : " ,") +"`" +insert_itr.first +"`"; fields.empty()){ err("Поле значения");
						}else if(values += string(values.empty() ? "" : " ,") +"'" +insert_itr.second +"'"; values.empty()){ err("Значение");
						//}else if("id" == insert_itr.first){ //mpre("Не дублируем идентификатор" ,__LINE__);
						//}else if(duplicate += string(duplicate.empty() ? "" : " ,") + "`" +insert_itr.first +"` = VALUES(`" +insert_itr.first +"`)"; duplicate.empty()){ err("Установка дубликата");
						}else{ //mpre("Список полей значений " +insert_itr.first +" => " +insert_itr.second, __LINE__);
						}}return "(" +fields +") VALUES (" +values +")" /*+" ON DUPLICATE KEY UPDATE " +duplicate*/; }(); VALUES.empty()){ err("Получение пар полей значений запроса");
					}else if(std::string sql = "INSERT INTO `" +ROWS +"` " +VALUES +";"; sql.empty()){ err("Запрос на добавление");
					}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Запрос таблица " +ROWS +" line=" +to_string(line) +" " +sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__); //err("Результат выполнения запроса");
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
			}else if(mysql_query(bmf::mysql, _sql.c_str())){ mpre("Выбор данных из таблицы " +_sql +"\n" +string(mysql_error(bmf::mysql)), __LINE__);
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
				}else if(ROW.end() == ROW.find("id")){ err("Поле идентификатора");
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
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `index_vals`;").c_str())){ err("ОШИБКА удаления таблицы");
			//}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `index_shift`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset_map`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset_vals`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano_values`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano_titles`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog_values`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog_titles`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if([&](){ // Шард таблицы
				if(string sql = "SHOW TABLES FROM iris;"; sql.empty()){ err("Запрос списка всех таблиц базы данных");
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
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `index` (`id` VARCHAR(32) NOT NULL UNIQUE, parent VARCHAR(32), addr TEXT, `timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, `itog_values_id` TEXT, `dano_id` TEXT, `itog_id` TEXT)").c_str())){ err("Список морфов"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `index_vals` (`id` VARCHAR(32) NOT NULL UNIQUE ,dataset_id VARCHAR(32) NOT NULL ,`index_md5` VARCHAR(32) NOT NULL ,`parent` VARCHAR(32) NOT NULL ,`key` int(11) DEFAULT 0, `grp` enum ('00', '01', '10', '11'), `grow_md5` VARCHAR(32) NOT NULL, UNIQUE KEY `index_vals_unique` (`dataset_id` ,`index_md5` ,`key`))").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dataset` (`id` VARCHAR(32) NOT NULL UNIQUE, `name` TEXT, `count` int(11), `key` int(11) DEFAULT 0, `epoch` int(11) DEFAULT 0, `err` int(11) DEFAULT 0, `microtime` BIGINT(20) DEFAULT 0)").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dataset_map` (`id` VARCHAR(750) NOT NULL UNIQUE, `dataset_id` VARCHAR(32), `alias` VARCHAR(750), `alias_id` VARCHAR(750), `itog_id` VARCHAR(750), `map` TEXT, KEY `dataset_id`(`dataset_id`), KEY `alias`(`alias`))").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano` (`id` VARCHAR(750) NOT NULL UNIQUE, `dano_values_id` VARCHAR(750), `name` VARCHAR(750), `val` VARCHAR(750))").c_str())){ err("Дано"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano_values` (`id` VARCHAR(750) NOT NULL UNIQUE, `name` TEXT)").c_str())){ err("Дано значения"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano_titles` (`id` VARCHAR(750) NOT NULL UNIQUE, `dano_values_id` VARCHAR(750), `name` VARCHAR(750), `value` VARCHAR(750) UNIQUE)").c_str())){ err("Дано заголовки"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog` (`id` VARCHAR(750) NOT NULL UNIQUE, `itog_values_id` VARCHAR(750), `name` VARCHAR(750), `val` VARCHAR(750), `index_id` VARCHAR(32))").c_str())){ err("Итог"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog_values` (`id` VARCHAR(750) NOT NULL UNIQUE, `name` VARCHAR(750))").c_str())){ err("Итоги значения"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog_titles` (`id` VARCHAR(750) NOT NULL UNIQUE, `itog_values_id` VARCHAR(750), `name` VARCHAR(750), `value` VARCHAR(750))").c_str())){ err("Итоги заголовки"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `test` (`id` VARCHAR(750) NOT NULL UNIQUE, `name` VARCHAR(750), `count` int(11) NOT NULL)").c_str())){ err("Вставка данных"); 
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
			} return binary; }); false){ mpre("ОШИБКА формирования функции пересчета разрядности чисел", __LINE__);
		}else if(bmf::Bin2dec = ([](string binary, double decimal = 0){ // Получаем число возвращаем двойное число
			string _bin, _dec, _binary = binary;
			if(std::function<long long(string)> bin2dec = ([&](string binary, unsigned long long decimal = 0){ // Получаем число возвращаем двойное число
				for(int i = 0; i < binary.length(); i++){
					if(int pos = binary.length()-i-1; (0 > pos)){ mpre("ОШИБКА получения позиции символа", __LINE__);
					}else if(int _pow = (unsigned long long)pow(2, i); (0 > _pow)){ mpre("ОШИБКА получения степени числа", __LINE__);
					}else if(string val = binary.substr(pos, 1); (1 != val.length())){ mpre("ОШИБКА получения символа", __LINE__);
					}else if(decimal += ("1" == val ? _pow : 0); (0 > decimal)){ mpre("ОШИБКА получения результата", __LINE__);
					}else{ //mpre("Смещение/степень i="+ to_string(i)+ " pos="+ to_string(pos)+ " val="+ val+ " _pow="+ to_string(_pow)+ " >> "+ to_string(decimal), __LINE__);
					}
				} return decimal; }); false){ mpre("ОШИБКА создания функции перевода из двоичного числа в десятичное", __LINE__);
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
		if(bmf::Choice = ([&](std::string list, int key, TMMs &DANO, TMMb &DANO_BITMAP){ //mpre("Выбор итога для расширения", __LINE__);
			TMs _dano;
			if(DANO_BITMAP.empty()){ err("Карта бит пуста");
			}else if(TMMs _DANO = DANO; _DANO.empty()){ err("Копия исходников");
			}else if([&](int pos = 0, int loop = 2e9){ while((string::npos != (pos = list.find_last_of(";"))) && (loop-- > 0) && (1 < _DANO.size())){ // Исключение повторяющихся исходников
				if(false){ mpre("Пропуск исключений", __LINE__);
				}else if(std::string dano_id =list.substr(pos +1, list.length()); dano_id.empty()){ mpre("ОШИБКА получения идентификатор исходника " +list, __LINE__);
				}else if(list =list.substr(0, pos); false){ mpre("ОШИБКА сокращение длинны списка", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_id)){ mpre(_DANO, " `"+ list +"` ", __LINE__); mpre("ОШИБКА Исходник не найден в списке list=`" +list +"` `"+ dano_id +"`", __LINE__);
				}else if(_DANO.erase(dano_id); _DANO.empty()){ mpre("ОШИБКА не удаляем последнее значение", __LINE__);
				}else{ //mpre("Пропуск выбора исходника list=" +list +" `" +dano_id +"`", __LINE__);
				}}return _DANO.empty(); }()){ mpre("ОШИБКА список не должен оставаться пустым", __LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("count")){ mpre("ОШИБКА поле количества элементов в наборе данных не установлено", __LINE__);
			}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества элементов в наборе данных", __LINE__);
			}else if(int prev = (!key ? dataset_count -1 : key -1); (0 > prev)){ mpre("ОШИБКА расчета предыдущей позиции", __LINE__);
			}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени эпохи", __LINE__);
			}else if([&](int pos = 0, bool loop = false){ for(auto dano_itr:bmf::List(bmf::DANO, {}, __LINE__)){ // Перебор исходных значений
				if(1 >= _DANO.size()){ //mpre("Не удаляем последнее значение", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_itr.first)){ //mpre("Уже удален " +dano.at("id"), __LINE__);
				}else if(DANO_BITMAP.end() == DANO_BITMAP.find(dano_itr.second.at("id"))){ err("Карта исходника не найдена");
				}else if(DANO_BITMAP[dano_itr.second.at("id")].test(key) != DANO_BITMAP[dano_itr.second.at("id")].test(prev)){ //mpre("Положительный изменившийся исходник", __LINE__);
				}else if(_DANO.erase(dano_itr.first); false){ mpre("ОШИБКА неизменившегося исходника", __LINE__);
				}else{ //mpre("Удаление исходника " +std::to_string(dano_itr.first), __LINE__);
				}}return false; }()){ mpre("ОШИБКА перебора значений", __LINE__);
			}else if([&](int pos = 0, bool loop = false){ for(auto dano_itr:bmf::List(bmf::DANO, {}, __LINE__)){ // Перебор исходных значений
				if(1 >= _DANO.size()){ //mpre("Не удаляем последнее значение", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_itr.first)){ //mpre("Уже удален " +dano.at("id"), __LINE__);
				}else if(DANO_BITMAP.end() == DANO_BITMAP.find(dano_itr.second.at("id"))){ err("Карта исходника не найдена");
				}else if(DANO_BITMAP[dano_itr.second.at("id")].test(key)){ //mpre("Положительный изменившийся исходник", __LINE__);
				}else if(_DANO.erase(dano_itr.first); false){ mpre("ОШИБКА неизменившегося исходника", __LINE__);
				}else{ //mpre("Удаление исходника " +std::to_string(dano_itr.first), __LINE__);
				}}return false; }()){ mpre("ОШИБКА перебора значений", __LINE__);
			}else if(_dano = [&](){ // Оставшееся значение
				if(auto dano_itr = _DANO.rbegin(); false){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
				}else if(_dano = dano_itr->second; _dano.empty()){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
				}else{ //mpre(_dano, "Оставшееся значение", __LINE__);
				}return _dano; }(); _dano.empty()){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
			}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обновления группы", __LINE__);
			}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Выбор подходящей связи size=" +to_string(_DANO.size()) +" list.length()=" +to_string(list.length()) +" ", __LINE__); //mpre("Сохрание значения INDEX_GROUP[" +group +"][" +to_string(key) +"]=" +GROUP.at(group).at(group), __LINE__); //mpre("Обновление " +sql, __LINE__);
			}return _dano; }); false){ mpre("ОШИБКА установки функции выбора Исходного значения", __LINE__);
		}else if(bmf::Vals = ([&](TMs VALUE, string alias, int key, std::string table, std::string BMF_VALUES, std::string BMF_TITLES){ // Установка входных значений
			TMMs VALS = {};
			if([&](){ // Расчет знаков
				for(auto &val_itr:VALUE){ //mpre("VALUE ", __LINE__);
					if(string value = val_itr.second; false){ mpre("ОШИБКА получения значения итератора", __LINE__);
					}else if(TMs values = [&](TMs values = {}){ // Добавление значения
						if(string name = val_itr.first; (0 >= name.length())){ mpre("ОШИБКА получения имени значения", __LINE__);
						}else if(string _val = val_itr.second; false){ mpre("ОШИБКА получения значения из данных", __LINE__);
						}else if(TMs _values = {{"name", name}, /*{"clump_id", bmf::clump_id}, {"value", _val}*/}; _values.empty()){ mpre("ОШИБКА фонмирования нового значения", __LINE__);
						}else if((values = bmf::Up(BMF_VALUES, {{"id", name}}, _values, {}, __LINE__)).empty()){ mpre("ОШИБКА обновления значения", __LINE__);
						}else{ //mpre("Добавляем новое значение `"+ name+ "` ("+ alias+ ")", __LINE__); //mpre("Добавление значения", __LINE__);
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
								}else if((vals = bmf::Up(table, {{"id", vals_id}}, _vals, {}, __LINE__)).empty()){ mpre(_vals, "Знак", __LINE__); mpre("ОШИБКА обновления знака", __LINE__);
								}else{ //mpre("Добавление нового знака в базу " +vals_id, __LINE__); //mpre("ОШИБКА добавление глобального знака", __LINE__); //mpre(*BMF, __LINE__, "Список знаков"); mpre("Добавление глобального знака `"+ values["name"]+ "` ["+ vals.at("id")+ "] "+ to_string(nn), __LINE__); mpre(vals, __LINE__, "Знак"); //mpre("ОШИБКА добавления знака "+ alias+ " "+ vals.at("id"), __LINE__);
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
				}return false; }()){ mpre("ОШИБКА расчета знаков", __LINE__);
			}else{ //mpre("Vals окончен", __LINE__);
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
			}else if(TMMs _DANO_ = bmf::Vals(dano, "dano", key, bmf::DANO, bmf::DANO_VALUES, bmf::DANO_TITLES); false){ mpre("ОШИБКА установки входящих значений", __LINE__);
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
			}else if(TMMs _ITOG_ = bmf::Vals(itog, "itog", key, bmf::ITOG, bmf::ITOG_VALUES, bmf::ITOG_TITLES); false){ mpre("ОШИБКА установки входящих значений", __LINE__);
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
			if(std::string _md5 = md5(bmf::ARGV.at("-")); _md5.empty()){ mpre("ОШИБКА расчета хеша набора обучающих данных", __LINE__);
			}else if(std::string dataset_id = _md5; dataset_id.empty()){ mpre("ОШИБКА установки идентификатора набора данных", __LINE__);
			}else if(TMs _dataset = {{"count", to_string(dataset_count)}, {"key", "0"}}; _dataset.empty()){ mpre("ОШИБКА создания записи нового набора данных", __LINE__);
			}else if((bmf::dataset = bmf::Up(bmf::DATASET, {{"id", dataset_id}}, _dataset, _dataset, __LINE__)).empty()){ mpre("ОШИБКА добавления набора данных", __LINE__);
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
			if(bmf::ARGV.end() == bmf::ARGV.find("key")){ //mpre("Ключ в консоли не указан", __LINE__);
			}else if(bmf::dataset.empty()){ //mpre("Набор данных не пуст", __LINE__);
			}else if(std::string key = bmf::ARGV.at("key"); key.empty()){ err("Значение ключа в консоли");
			}else if(int _key = atoi(key.c_str()); 0 > _key){ err("Числовое значение ключа");
			}else if(bmf::dataset["key"] = to_string(_key); bmf::dataset.empty()){ err("Установка значения ключа в обучающий набор");
			}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", bmf::dataset.at("id")}}, {}, bmf::dataset, __LINE__); bmf::dataset.empty()){ err("Обновление позиции расчетов");
			}else{ mpre("Обновление позиции расчетов ds=" +bmf::dataset.at("id") +" key=" +bmf::dataset.at("key"), __LINE__);
			}return false; }()){ err("Установка ключа");
		}else if(!bmf::dataset.empty()){ //mpre("Набор данных не пуст", __LINE__);
		}else if(bmf::ARGV.end() != bmf::ARGV.find("-")){ //mpre("Не выводим статистику на загузке набора данных", __LINE__);
		}else if(!ds.empty()){ mpre("Набора данных не найден в базе "+ ds, __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> ITOG; false){ mpre("Справочник карт", __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> INDEX; false){ mpre("Справочник карт", __LINE__);
		}else if([&](){ for(auto& dataset_itr:bmf::List(bmf::DATASET, {}, __LINE__)){ // Вывод списка набора данных
			if(TMs dataset = dataset_itr.second; dataset.empty()){ mpre("ОШИБКА выборки набора данных", __LINE__);
			}else if(dataset.end() == dataset.find("count")){ mpre("ОШИБКА поле количества примеров не установлено", __LINE__);
			}else if(int dataset_count = atoi(dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в наборе данных", __LINE__);
			}else if(TMMs _ITOG_ = bmf::List(bmf::ITOG, {}, __LINE__); _ITOG_.empty()){ err("Выборка списка итогов");
			}else if(float diff = [&](int ers = 0, int count = 0){ for(auto itog_itr:_ITOG_){ // Расчет результата поитогово
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
				}}return (!ers ? 1 : (float)(count-ers)/count); }(); (0 > diff)){ mpre("ОШИБКА расчета результата набора данных", __LINE__);
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
			}else if(string _perc = [&](string _perc = ""){ char chr[10]; sprintf(chr ,"%.2f" ,perc); return string(chr); }(); _perc.empty()){ mpre("ОШИБКА расчета строки процента ошибки", __LINE__);
			}else if(float epoch = [&](float epoch = 0){ // Расчет эпох
				if(dataset.end() == dataset.find("epoch")){ err("Поле эпох не задано");
				}else if(epoch = atoi(dataset.at("epoch").c_str()); 0 > epoch){ err("Целое количество эпох");
				}else if(dataset.end() == dataset.find("key")){ err("Поле позиции не задано");
				}else if(epoch += ((float)atoi(dataset.at("key").c_str())/ dataset_count); 0 > epoch){ err("Дробное количество эпох");
				}else{ //mpre("Эпох dataset.at(epoch)=" +dataset.at("epoch") +" dataset_count=" +to_string(dataset_count) +" key=" +dataset.at("key") +" epoch=" +to_string(epoch), __LINE__);
				}return epoch; }(); false){ err("Расчет эпох");
			}else{ mpre("Набор " +dataset["id"] +" количество:"+ dataset["count"] +" точность:"+ to_string(diff) +" (" +_perc +"%)" +(0 == epoch ? "" : " эпох:" +to_string(epoch)), __LINE__);
			}}return false; }()){ mpre("ОШИБКА отображения списка набора данных", __LINE__);
		}else{ //mpre("Размер модели ", __LINE__);
		}return bmf::dataset; }(); bmf::dataset.empty()){ //mpre("ОШИБКА набор данных не установлен", __LINE__);
	}else if(int loop = [&](int loop = 0){ // Количетсво повторов
		if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); (0 > epoch)){ mpre("ОШИБКА расчета количества эпох", __LINE__);
		}else if(bmf::ARGV.end() == bmf::ARGV.find("epoch")){ //mpre("Для обучения укажите количество эпох epoch", __LINE__);
		}else if(!epoch){ loop = 1; //mpre("Указано нулевое количество эпох расчитываем до полного совпадения", __LINE__);
		}else if(loop = epoch){ //mpre("Набор данных -ds " +bmf::dataset.at("id")+ " количество эпох -epoch " +to_string(loop), __LINE__);
		}else{ mpre("ОШИБКА расчтеа количества повторений", __LINE__);
		}return loop; }(); false){ mpre("ОШИБКА расчета колчиества повторений", __LINE__);
	}else if(TMMs BMF_INDEX /*= [&](TMMs BMF_INDEX = {}){ // Справочник морфов
		if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if(BMF_INDEX = bmf::List(bmf::INDEX, {}, __LINE__); false){ err("ОШИБКА выборки списка морфов");
		}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
		}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Выборка модели размер " +to_string(BMF_INDEX.size()), __LINE__);
		}return BMF_INDEX; }()*/; false){ err("Выборка данных");
	}else if(int errors = [&](int errors = 0, bool pass = false){ do{ // Итерации обучения
		if(bmf::loop++ >= loop){ //mpre("Остановка по максимальному количеству эпох", __LINE__);
		//}else if(mpre("Количетсво эпох bmf::loop=" +to_string(bmf::loop) +" loop=" +to_string(loop), __LINE__); false){ err("Уведомление");
		}else if(bmf::dataset.empty()){ mpre("ОШИБКА набор данных не установлен", __LINE__);
		}else if(int dataset_count = atoi(bmf::dataset["count"].c_str()); (0 >= dataset_count)){ mpre("ОШИБКА нулевое количество в наборе данных", __LINE__);
		}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if(TMMs DANO = bmf::List(bmf::DANO, {}, __LINE__); DANO.empty()){ err("Список итогов");
		}else if(TMMb DANO_BITMAP = [&](TMMs DATASET_MAP, std::map<string,boost::dynamic_bitset<>> DANO_BITMAP = {}){ for(auto dano_itr:DANO){ // Список карт исходников
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
			if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
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
			if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
			}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Загрузка итоговых сигналов size=" +to_string(ITOG_BITMAP.size()), __LINE__);
			}return false; }()){ err("Время расчета");
		}else if(int key = [&](int key = 0){ // Позиция обучения
			if(bmf::dataset.end() == bmf::dataset.find("key")){ err("Поле ключа не найдено в базе");
			}else if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); !epoch){ //mpre("Локальный ключ" ,__LINE__);
			}else if(key = atoi(bmf::dataset.at("key").c_str()); 0 > key){ err("Расчет значения последнего ключа");
			}else{ //mpre("Текущее состояние ключа key=" +to_string(key), __LINE__);
			}return key; }(); false){ err("Расчет позиции обучения");
		}else if(int err = [&](int err = 0){ // Позиция обучения
			if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); !epoch){ //mpre("Локальный ключ" ,__LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("err")){ err("Поле ключа не найдено в базе");
			}else if(err = atoi(bmf::dataset.at("err").c_str()); 0 > err){ err("Расчет значения последнего ключа");
			}else{ //mpre("Текущее состояние ключа err=" +to_string(err), __LINE__);
			}return err; }(); false){ err("Расчет позиции обучения");
		}else if(long microtime = (!key ? (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 : atol(bmf::dataset.at("microtime").c_str())); !microtime){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if([&](bool rep = false, TMMs VALUES = {}){ do{ // Повторение расчетов эпохи
			if(std::string epoch = [&](std::string epoch = "1"){ // Расчет эпохи
				if(bmf::dataset.end() == bmf::dataset.find("epoch")){ // mpre("Поле эпохи не задано", __LINE__);
				}else if(epoch = bmf::dataset.at("epoch"); false){ err("Выборка значения эпохи");
				}else if(epoch = to_string(atoi(epoch.c_str()) + 1); epoch.empty()){ err("Инкремент эпохи");
				}else{ //mpre("Увеличение значения эпохи " +epoch, __LINE__);
				}return epoch; }(); epoch.empty()){ mpre("ОШИБКА расчета эпохи", __LINE__);
			}else if([&](){ // Обнуление позиции
				if(string epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? "" : bmf::ARGV.at("epoch")); "0" != epoch){ //mpre("Только для нулевой эпохи");
				}else if(static int err_current = 0; false){ err("Текущее количество ошибок");
				}else if(err == err_current){ //mpre("Количество ошибок не изменилось", __LINE__);
				}else if(err_current = err; false){ err("Сохранение текущего количества ошибок");
				}else if(key = round(key/2); false){ err("Начинаем с середины от пройденного");
				}else{ //mpre("Обнуление позиции при нулевой эпохе" ,__LINE__);
				}return false; }()){ err("Обнуление позиции расчета при нулевом количестве эпох");
			}else if(std::string itog_str = (bmf::ARGV.end() == bmf::ARGV.find("itog") ? "" : "." +bmf::ARGV.at("itog")); false){ mpre("ОШИБКА формирования итога в консоли", __LINE__);
			}else if([&](){ // Выборка обновлений модели
				if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
				}else if(TMMs _INDEX = bmf::Scan(bmf::INDEX, BMF_INDEX, __LINE__); _INDEX.empty()){ //mpre("Нет изменений в структуре", __LINE__);
				}else if([&](){ for(auto index_itr:_INDEX){ // Обнолвение структуры расчетов групп
					if(TMs index = index_itr.second; index.empty()){ err("Выборка обновленного морфа");
					}else if(index.end() == index.find("addr")){ err("Идентификатор не указан");
					}else if(BMF_INDEX.insert(make_pair(index.at("addr"), index)); BMF_INDEX.empty()){ err("Обновление справочника");
					}else{ //mpre(_INDEX ,"Обновление струкруты групп", __LINE__);
					}}return false; }()){ err("Обновление структуры групп и списка морфов");
				}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
				}else{ mpre("", __LINE__); mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Обновление модели " +to_string(_INDEX.size()) +" size=" +to_string(BMF_INDEX.size()) +" ", __LINE__);
				}return false; }()){ err("Обновление модели");
			//}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(string table_vals = "index_vals_" +bmf::dataset.at("id") + "_"+to_string(key); table_vals.empty()){ err("ОШИБКА расчета имени таблицы");
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `" +table_vals +"` LIKE `index_vals`;").c_str())){ mpre("Вставка данных " +string(mysql_error(bmf::mysql)) ,__LINE__); 
			}else if([&](){ //Список изменений
				if(TMMs INDEX = [&](TMMs INDEX = {}){ // Все прародители
					if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
					}else if(string sql = "SELECT i.* FROM `index` AS i LEFT JOIN `" +table_vals +"` AS iv ON i.id=iv.index_md5 AND iv.`dataset_id`='" +bmf::dataset.at("id") +"' AND iv.key=" +to_string(key) +" WHERE iv.id IS NULL;"; sql.empty()){ err("Запрос на выборку изменений");
					}else if(TMMs _INDEX = bmf::List_mysql("index" ,{} ,sql ,__LINE__); _INDEX.empty()){ //mpre("Список изменений пуст" ,__LINE__);
					}else if([&](){ for(auto index_itr:_INDEX){ // Полный список изменений
						if(TMs index = index_itr.second; index.empty()){ err("Выбор морфа");
						}else if(string addr = (index.end() == index.find("addr") ? "" : index.at("addr")); addr.empty()){ err("Адрес морфа");
						}else if(auto npos = addr.find_first_of("-"); string::npos == npos){ err("Не найден разделитель в адресе");
						}else if([&](){ for(int i=0; i<npos; i++){ // Родители морфа
							if(false){ mpre("Пропуск формирования родителя", __LINE__);
							}else if(string str = addr.substr(i, npos-i); str.empty()){ err("Формирование части адреса");
							}else if(string adr = "1" + str.substr(1, str.length()); adr.empty()){ err("Установка формата адреса");
							}else if(string _addr = adr +addr.substr(npos, addr.length()); _addr.empty()){ err("Формирование адреса");
							}else if(TMs _index = (BMF_INDEX.end() == BMF_INDEX.find(_addr) ? _index : BMF_INDEX.at(_addr)); _index.empty()){ //mpre("Родительский морф " +addr +" > " +_addr ,__LINE__);
							}else if(INDEX.insert(make_pair(_addr, _index)); INDEX.empty()){ err("Список родителей");
							}else{ //mpre("Формирование родителя i=" +to_string(i) +" adr=" +adr +" _addr=" +_addr +" " +addr ,__LINE__);
							}}return false; }()){ err("Список родителей морфа");
						}else{ //mpre(index ,"Изменившийся морф" ,__LINE__);
						}}return false; }()){ err("Составление списк изменений");
					}else if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ? "" : bmf::ARGV.at("verbose").c_str()); bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
					}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Выборка обновлений морфов" +(verbose ? " " +sql : "") +" INDEX.size()=" +to_string(INDEX.size()), __LINE__);
					}return INDEX; }(); INDEX.empty()){ //mpre("Список родственной структуры пуст" ,__LINE__);
				}else if([&](){ // Выборка списка расчитанных ранее групп
					if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
					}else if(TMMs _BMF_INDEX_VALS = [&]( TMMs _BMF_INDEX_VALS = {}){ // Расчетные значения
						if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
						}else if(string in = [&](string in = "''"){ for(auto index_itr:INDEX){ // Список id через запятую
							if(TMs index = index_itr.second; index.empty()){ err("Список всех родных");
							}else if(string id = (index.end() == index.find("id") ? "" : index.at("id")); id.empty()){ err("Выборка идентификатора родных");
							}else if(in += ",'" +id +"'"; in.empty()){ err("Список");
							}else{ //mpre("Новый идентификатор в список id=" +id, __LINE__);
							}}return in; }(); in.empty()){ err("Уведомление");
						}else if(string sql = "SELECT * ,index_md5 AS id ,id as uid FROM `" +table_vals +"` WHERE `dataset_id`='" +bmf::dataset.at("id") +"' AND `key`=" +to_string(key) +" AND parent IN(" +in +");"; sql.empty()){ err("Запрос на выборку значений");
						//}else if(mpre("Выборка значений " +sql, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);	
						}else if(_BMF_INDEX_VALS = bmf::List_mysql(table_vals ,{} ,sql ,__LINE__); false){ mpre("Список значений пуст sql=" +sql ,__LINE__);
						}else if(int verbose = atoi(bmf::ARGV.end() == bmf::ARGV.find("verbose") ? "" : bmf::ARGV.at("verbose").c_str()); bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
						}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Выборка значений" +(verbose ? " " +sql +" " : " ") +to_string(_BMF_INDEX_VALS.size()) +" INDEX.size()=" +to_string(INDEX.size()), __LINE__);
						}return _BMF_INDEX_VALS; }(); false){ err("Расчетные значения");
					}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
					}else if(TMMs UPDATE = [&](TMMs UPDATE = {}){ for(auto index_itr:INDEX){ // Расчет значений модели
						if(TMs index = index_itr.second; index.empty()){ mpre("Выборка морфа", __LINE__);
						}else if(string addr = (index.end() == index.find("addr") ? "" : index.at("addr")); addr.empty()){ err("Выборка адреса");
						}else if(string dano_val = [&DANO_BITMAP, &index, &key](string dano_val = ""){ // Значение исходника
							if(string dano_id = (index.end() == index.find("dano_id") ? "" : index.at("dano_id")); dano_id.empty()){ err("Идентификатор исходника");
							}else if(DANO_BITMAP.end() == DANO_BITMAP.find(dano_id)){ mpre("Карта значений для исходника не задана dano_id="+ dano_id ,__LINE__);
							}else if(dano_val = (DANO_BITMAP.at(dano_id).test(key) ? "1" : "0"); dano_val.empty()){ err("Значение исходника");
							}else{ //mpre("Расчет исходного значения dano_val=" +dano_val +" dano_id=" +dano_id, __LINE__);
							}return dano_val; }(); 1 != dano_val.length()){ err("Значение исходника");
						}else if(TMs index_vals_1 = [&](TMs index_vals_1 = {}){ // Значение старшего расчета
							if(string index_addr = "11" +addr.substr(1, addr.length()); index_addr.empty()){ err("Пропуск расчета старшей связи");
							}else if(BMF_INDEX.end() == BMF_INDEX.find(index_addr)){ //mpre("Морф старший не найден " +addr +" " +index_addr ,__LINE__);
							}else if(string index_id = (BMF_INDEX.at(index_addr).end() == BMF_INDEX.at(index_addr).find("id") ? "" : BMF_INDEX.at(index_addr).at("id")); index_id.empty()){ err("Идентификатор морфа");
							}else if(_BMF_INDEX_VALS.end() == _BMF_INDEX_VALS.find(index_id)){ mpre("Значение не найдено index_addr=" +index_addr +" index_id=" +index_id  ,__LINE__);
							}else if(index_vals_1 = _BMF_INDEX_VALS.at(index_id); index_vals_1.empty()){ err("Выборка значения");
							}else{ //mpre("Значение старшего морфа " +addr ,__LINE__);
							}return index_vals_1; }(); false){ err("Значение старшего расчета");
						}else if(string v1 = [&](string v1){ // Значение старшей связи
							if(index_vals_1.end() == index_vals_1.find("grp")){ //mpre("Значение группы" ,__LINE__);
							}else if(v1 = ("01" == index_vals_1.at("grp") ? "1" : "0"); v1.empty()){ err("Значение расчитанное ранее");
							}else{ //mpre("Расчет значения старшей связи index_id=" +index_id, __LINE__);
							}return v1; }(dano_val); 1 != v1.length()){ err("Значение старшей связи");
						}else if(TMs index_vals_0 = [&](TMs index_vals_0 = {}){ // Значение старшего расчета
							if(string index_addr = "10" +addr.substr(1, addr.length()); index_addr.empty()){ err("Пропуск расчета младшей связи");
							}else if(BMF_INDEX.end() == BMF_INDEX.find(index_addr)){ //mpre("Морф младший не найден " +index_addr ,__LINE__);
							}else if(string index_id = (BMF_INDEX.at(index_addr).end() == BMF_INDEX.at(index_addr).find("id") ? "" : BMF_INDEX.at(index_addr).at("id")); index_id.empty()){ err("Идентификатор морфа");
							}else if(_BMF_INDEX_VALS.end() == _BMF_INDEX_VALS.find(index_id)){ mpre("Значение не найдено index_addr=" +index_addr +" index_id=" +index_id +" " ,__LINE__);
							}else if(index_vals_0 = _BMF_INDEX_VALS.at(index_id); index_vals_0.empty()){ err("Выборка значения");
							}else{ //mpre("Значение младшего морфа " +addr ,__LINE__);
							}return index_vals_0; }(); false){ err("Значение старшего расчета");
						}else if(string v0 = [&](string v0){ // Значение старшей связи
							if(index_vals_0.end() == index_vals_0.find("grp")){ //mpre("Значение группы", __LINE__);
							}else if(v0 = ("01" == index_vals_0.at("grp") ? "0" : "1"); v0.empty()){ err("Значение расчитанное ранее");
							}else{ //mpre("Расчет значения младшей связи v0=" +v0 +" index_id=" +index_id, __LINE__);
							}return v0; }(dano_val); 1 != v0.length()){ err("Значение младшей связи");
						}else if(TMs index_vals = [&](TMs index_vals = {}){ // Сохранение значения
							if(string index_id = (index.end() == index.find("id") ? "" : index.at("id")); index_id.empty()){ err("Хеш морфа не установлен");
							}else if(index_vals = (_BMF_INDEX_VALS.end() == _BMF_INDEX_VALS.find(index_id) ? index_vals : _BMF_INDEX_VALS.at(index_id)); false){ err("Выборка значения морфа");
							}else{ //mpre("Выборка расчета морфа " +addr ,__LINE__);
							}return index_vals; }(); false){ err("Сохренние значения");
						}else if(string grow_md5 = [&](string grow_md5){ // Значение роста
							if(string grp = v1 +v0; 2 != grp.length()){ err("Значение группы");
							}else if(string grow_1_md5 = (index_vals_1.end() == index_vals_1.find("grow_md5") ? grow_md5 : index_vals_1.at("grow_md5")); grow_1_md5.empty()){ err("Рост старшей связи");
							}else if(string grow_0_md5 = (index_vals_0.end() == index_vals_0.find("grow_md5") ? grow_md5 : index_vals_0.at("grow_md5")); grow_0_md5.empty()){ err("Рост младшей связи");
							}else if(string link_grow = grp.substr(atoi(dano_val.c_str()), 1); 1 != link_grow.length()){ err("Ссылка расширяемого морфа");
							}else if(grow_md5 = ("0" == link_grow ? grow_0_md5 : grow_1_md5); grow_md5.empty()){ err("Значение роста");
							}else if(string _addr = (index_vals.end() == index_vals.find("addr") ? "" : index_vals.at("addr")); "110-Два-1" != _addr){ //mpre("Прпоускаем все кроме указанного" ,__LINE__);
							}else{ //mpre(INDEX ,"Справочник морфов" ,__LINE__); mpre(_BMF_INDEX_VALS ,"Список значений" ,__LINE__); mpre(index_vals_1 ,"Старшее значение" ,__LINE__); mpre(index_vals_0 ,"Младшее значение" ,__LINE__); mpre("Расчет значения роста grow_1_md5=" +grow_1_md5 +" grow_0_md5=" +grow_0_md5 +" link_grow=" +link_grow +" dano_val=" +dano_val +" grp=" +grp +" grow_md5=" +grow_md5 +" " +addr ,__LINE__);
							}return grow_md5; }(index.at("id")); grow_md5.empty()){ err("Значение роста");
						}else if([&](){ // Сохранение значения
							if(index_vals.empty()){ //mpre("Расчет еще не создан", __LINE__);
							}else if(string grp = v1 +v0; 2 != grp.length()){ err("Сохранение значения морфа");
							}else if([&](){ // Изменение значения
								if(string _grp = (index_vals.end() == index_vals.find("grp") ? _grp : index_vals.at("grp")); _grp.empty()){ err("Старой значение направления");
								}else if(_grp == grp){ //mpre("Направление не изменилось" ,__LINE__);
								}else if(index_vals["grp"] = grp; index_vals.empty()){ err("Установка значения роста");
								}else{ //mpre("Изменение значения морфа " +addr ,__LINE__);
								}return false; }()){ err("Изменение значения");
							}else if([&](){ // Направление роста
								if(string _grow_md5 = (index_vals.end() == index_vals.find("grow_md5") ? _grow_md5 : index_vals.at("grow_md5")); _grow_md5.empty()){ err("Старой значение направления");
								}else if(_grow_md5 == grow_md5){ //mpre("Направление не изменилось" ,__LINE__);
								}else if(index_vals["grow_md5"] = grow_md5; index_vals.empty()){ err("Установка значения роста");
								}else{ //mpre("Изменение направления развития key=" +to_string(key) +" _grow_md5=" +_grow_md5 +" grow_md5=" +grow_md5 +" " +addr ,__LINE__);
								}return false; }()){ err("Изменение направления роста");
							}else if(string uid = (index_vals.end() == index_vals.find("uid") ? "" : index_vals.at("uid")); uid.empty()){ err("Значение идентификатора записи");
							}else if(index_vals.erase("uid"); false){ err("Удаление верменного поля идентификатора");
							}else if(index_vals.erase("id"); false){ err("Удаление верменного поля идентификатора");
							}else if(UPDATE.insert(make_pair(uid, index_vals)); UPDATE.empty()){ err("Изменяемое значение");
							}else if(_BMF_INDEX_VALS[index_vals.at("index_md5")] = index_vals; _BMF_INDEX_VALS.empty()){ err("Обновление значения в списке значений");
							}else{ //mpre(index_vals ,"Сохранение значения в расчет " +addr ,__LINE__);
							}return false; }()){ err("Сохранение значения в уже установленный расчет");
						}else if([&](){ // Добавление значения
							if(!index_vals.empty()){ //mpre("Расчет уже создан", __LINE__);
							}else if(string grp = v1 +v0; 2 != grp.length()){ err("Значение группы");
							}else if(TMs _index_vals = {{"index_md5", index.at("id")}, {"parent", index.at("parent")}, {"grp", grp}, {"dataset_id", bmf::dataset.at("id")}, {"key", to_string(key)}, {"grow_md5", grow_md5}}; _index_vals.empty()){ err("Группа морфа");
							}else if(TMs update = {{"index_md5", index.at("id")}, {"addr", addr}, {"grp", grp}, {"dataset_id", bmf::dataset.at("id")}, {"key", to_string(key)}, {"grow_md5", grow_md5}}; update.empty()){ err("Значение обновления");
							}else if(string uid = md5(addr +"-" +bmf::dataset.at("id") +"-" +to_string(key)); uid.empty()){ err("Расчет идентификатора");
							}else if(UPDATE.insert(make_pair(uid, _index_vals)); UPDATE.empty()){ err("Добавление нового значения");
							}else if(_BMF_INDEX_VALS.insert(make_pair(_index_vals.at("index_md5"), _index_vals)); _BMF_INDEX_VALS.empty()){ err("Обновление текущих значений");
							}else{ //mpre(index_vals ,"Добавление нового значения key=" +index_vals.at("key") +" index_vals_id=" +index_vals.at("id") +" grow_md5=" +grow_md5 +" " +addr +" " ,__LINE__);
							}return false; }()){ err("Добавление значение расчета");
						}else{ //mpre("Окончение расчета модели key=" +to_string(key) ,__LINE__);
						}}return UPDATE; }(); false){ err("Расчет модели");
					}else if([&](){ // Уведомление о времени расчета модели
						if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
						}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Расчет модели " +to_string(INDEX.size()) +" ", __LINE__);
						}return false; }()){ err("Уведомление");
					}else if([&](){ // Сохранение значений
						if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
						}else if(TMs titles = UPDATE.begin()->second; titles.empty()){ err("Заголовки не найдены");
						}else if(string names = [&](string names = "`id`"){ for(auto titles_itr:titles){ names += ", `" +titles_itr.first +"`"; }return names; }(); names.empty()){ err("Имена полей");
						}else if(string VALUES = [&](string VALUES = ""){ for(auto update_itr:UPDATE){
							if(string uid = "'" +update_itr.first +"'"; uid.empty()){ err("Идентификатор записи");
							}else if(string values = [&](string values){ for(auto values_itr:update_itr.second){ values += ", '" +values_itr.second +"'"; }return values; }(uid); values.empty()){ err("Значения полей");
							}else{ VALUES += "\n(" +values +"),";
							}}return VALUES.substr(0, VALUES.length() -1); }(); VALUES.empty()){ err("Добавление значений");
						}else if(string sql = "INSERT INTO `" +table_vals +"` (" +names +") VALUES " +VALUES +" ON DUPLICATE KEY UPDATE `grow_md5`=VALUES(grow_md5), `grp`=VALUES(grp);"; sql.empty()){ err("Начало запроса");
						//}else if(mpre("Запрос\n" +sql ,__LINE__); false){ err("Уведомление");
						}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("ОШИБКА " +string(mysql_error(bmf::mysql)), __LINE__);
						}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
						}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Сохранение модели UPDATE.size()=" +to_string(UPDATE.size()) +" ", __LINE__);
						}return false; }()){ err("Сохранение значений");
					}return false; }()){ err("Выборка всех нужных групп морфов");
				//}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
				//}else{ //mpre("==========", __LINE__); mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Расчет модели " +to_string(_BMF_INDEX.size()) +" size=" +to_string(BMF_INDEX.size()) +" ", __LINE__);
				}else{ //mpre(INDEX ,"Изменившиеся морфы" ,__LINE__); mpre(_BMF_INDEX_VALS ,"Список значений изменений " ,__LINE__);
				}return false; }()){ err("Список изменений");
			}else if([&](){ for(auto &itog_itr:bmf::List(bmf::ITOG, {}, __LINE__)){ // Проверка списка итогов
				if(auto _microtime_ = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - microtime; false){ mpre("ОШИБКА расчета времени эпохи", __LINE__);
				}else if(bmf::Progress("Набор:" +bmf::dataset.at("id") + itog_str+" Эпоха:" +epoch +" Примеров:" +to_string(key+1) +" Изменений:"+ to_string(err)+ " (" +to_string(_microtime_) +" сек.)", (float)(key+1)/dataset_count, __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
				}else if(TMs itog = itog_itr.second; itog.empty()){ //mpre("ОШИБКА выборки итога", __LINE__);
				}else if(itog.end() == itog.find("id")){ mpre("ОШИБКА не найден идентификатор итога", __LINE__);
				}else if(TMs index = [&](TMs index = {}){ // Добавление первоначального морфа
					if(TMs dano = DANO.begin()->second; dano.empty()){ mpre("ОШИБКА выборки первого исходника", __LINE__);
					}else if([&](){ // Выборка морфа из базы
						if(itog.end() == itog.find("index_id")){ //mpre("У итога не найдено поле морфа", __LINE__);
						}else if(std::string index_id = itog.at("index_id"); index_id.empty()){ //mpre("Пустое поле корневого морфа", __LINE__);
						}else if(index = bmf::Up(bmf::INDEX, {{"id", itog.at("index_id")}}, {}, {}, __LINE__); false){ mpre("ОШИБКА выборки корневого морфа", __LINE__);
						}else{ //mpre(index, "Морф найден в базе", __LINE__);
						}return !index.empty(); }()){ //mpre("Морф уже в базе", __LINE__);
					}else if(std::string addr = "1-" +itog.at("id"); addr.empty()){ err("Идентификатор первоначального морфа");
					}else if(TMs _index = {{"itog_values_id", itog.at("itog_values_id")}, {"parent", ""}, {"addr", addr}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}}; _index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
					}else if((index = bmf::Up(bmf::INDEX, {{"id", md5(addr)}}, _index, {}, __LINE__)).empty()){ mpre("ОШИБКА добавления корневого морфа в базу", __LINE__);
					}else if(itog["index_id"] = index.at("id"); itog.empty()){ mpre("ОШИБКА установки свойства связи итога с морфом", __LINE__);
					}else if(TMs _itog = bmf::Up(bmf::ITOG, {{"id", itog.at("id")}}, {}, itog, __LINE__); _itog.empty()){ err("Обновление поля корневого морфа у итога");
					}else if(BMF_INDEX[addr] = index; BMF_INDEX.empty()){ err("Добавление нового морфа в список");
					}else if(string dano_val = (DANO_BITMAP.at(dano.at("id")).test(key) ? "1" : "0"); dano_val.empty()){ err("Значение исходника");
					//}else if(TMs _index_vals = {{"index_md5", index.at("id")}, {"addr", addr}, {"grp", string(dano_val +dano_val)}, {"dataset_id", bmf::dataset.at("id")} ,{"key", to_string(key)} ,{"grow_md5", index.at("id")}}; _index_vals.empty()){ err("Группа морфа");
					//}else if(TMs index_vals = bmf::Up("index_vals", {{"id", md5(addr +"-" +to_string(key))}}, _index_vals, {}, __LINE__); index_vals.empty()){ err("Сохранение группы морфа");
					}else{ //mpre(index, "Добавление первоначального морфа " +index.at("id"), __LINE__);
					}return index; }(); index.empty()){ mpre("ОШИБКА морф связи не найден", __LINE__);
				}else if(TMs index_vals = [&](TMs index_vals = {}){ // Выборка значения из базы
					if(string index_id = "1-" +itog.at("id"); index_id.empty()){ err("Корневой идентификатор");
					}else if(index_vals = bmf::Up_mysql(table_vals, {{"dataset_id", bmf::dataset.at("id")} ,{"index_md5", index.at("id")}, {"key", to_string(key)}} ,{} ,{} ,__LINE__); !index_vals.empty()){ //mpre("Значение корневого морфа найдена" ,__LINE__);
					}else if(string addr = (index.end() == index.find("addr") ? "" : index.at("addr")); addr.empty()){ err("Расчет адреса");
					}else if(string dano_id = (index.end() == index.find("dano_id") ? "" : index.at("dano_id")); dano_id.empty()){ err("Идентификатор исходника");
					}else if(TMs dano = (DANO.end() == DANO.find(dano_id) ? dano : DANO.at(dano_id)); dano.empty()){ err("Выборка исходника");
					}else if(string dano_val = (DANO_BITMAP.at(dano.at("id")).test(key) ? "1" : "0"); dano_val.empty()){ err("Значение исходника");
					}else if(TMs _index_vals = {{"index_md5", index.at("id")}, {"parent", ""}, {"grp", string(dano_val +dano_val)}, {"dataset_id", bmf::dataset.at("id")} ,{"key", to_string(key)} ,{"grow_md5", index.at("id")}}; _index_vals.empty()){ err("Группа морфа");
					}else if(index_vals = bmf::Up(table_vals, {{"id", md5(addr +"-" +bmf::dataset.at("id") +"-" +to_string(key))}}, _index_vals, {}, __LINE__); index_vals.empty()){ err("Сохранение группы морфа");
					}else{ //mpre(bmf::dataset ,"Набор данных" ,__LINE__); mpre(index_vals, "Добавление расчета корневого морфа", __LINE__);
					}return index_vals; }(); index_vals.empty()){ err("Значение корневого морфа");
				}else if(std::string learn = [&](std::string learn = ""){ // Обучающее значение
					if(ITOG_BITMAP.end() == ITOG_BITMAP.find(itog.at("id"))){ err("Битовая карта итога");
					}else if(learn = (ITOG_BITMAP.at(itog.at("id")).test(key) ? "1" : "0"); (1 != learn.length())){ err("Получения бит");
					}else{ //mpre("Выборка обучающего значения learn=" +learn, __LINE__);
					}return learn; }(); (1 != learn.length())){ err("Выборка обучающего значения");
				}else if(string calc = [&](string calc = ""){ // Расчет значения корневого морфа
					if(string grp = (index_vals.end() == index_vals.find("grp") ? "" : index_vals.at("grp")); 2 != grp.length()){ err("Группа морфа");
					}else if(string addr = (index.end() == index.find("addr") ? "" : index.at("addr")); addr.empty()){ err("Расчет адреса");
					}else if(auto npos = addr.find_first_of("-"); string::npos == npos){ err("Позиция разделителя");
					}else if(string link = (1 >= npos ? "0" : addr.substr(npos, 1)); 1 != link.length()){ err("Ссылка");
					}else if(string _val = ("0" == link ? "1" : "0"); 1 != _val.length()){ err("Основное значение связи");
					}else if(string _val_ = ("0" == _val ? "1" : "0"); 1 != _val_.length()){ err("Исключительное значение связи");
					}else if(calc = ("01" == grp ? _val_ : _val); calc.empty()){ err("Расчет значения");
					}else{ //mpre("Расчет значения корневого морфа calc=" +calc +" " +addr ,__LINE__);
					}return calc; }(); 1 != calc.length()){ err("Расширение модели");
				}else if([&](){ // Расширение модели
					if(calc == learn){ //mpre("Расчетные и обучающие значения совпадают", __LINE__);
					}else if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
					}else if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Пропускаем обучение" ,__LINE__);
					}else if(string grow_md5 = (index_vals.end() == index_vals.find("grow_md5") ? "" : index_vals.at("grow_md5")); grow_md5.empty()){ err("Идентификатор расширения");
					}else if(TMs index_grow = bmf::Up("index" ,{{"id", grow_md5}} ,{} ,{} ,__LINE__); index_grow.empty()){ err("Расширяемый морф");
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")} ,{"index_md5", grow_md5}, {"key", to_string(key)}}; where.empty()){ err("Условие выборки");
					}else if(TMs index_vals_grow = bmf::Up_mysql(table_vals ,where ,{} ,{} ,__LINE__); index_vals_grow.empty()){ mpre(where ,"Условие" ,__LINE__); err("Выборка значения расширяемого морфа");
					}else if(string grp_grow = (index_vals_grow.end() == index_vals_grow.find("grp") ? "" : index_vals_grow.at("grp")); grp_grow.empty()){ err("Группа расширяемого морфа");
					}else if(string dano_id_grow = (index_grow.end() == index_grow.find("dano_id") ? "" : index_grow.at("dano_id")); dano_id_grow.empty()){ err("Идентификатор исходника расширяемого морфа");
					}else if(DANO_BITMAP.end() == DANO_BITMAP.find(dano_id_grow)){ err("Исходные значения не найдены");
					}else if(string dano_val_grow = (DANO_BITMAP.at(dano_id_grow).test(key) ? "1" : "0"); dano_val_grow.empty()){ err("Группа расширяемого морфа");
					}else if(string link_grow = grp_grow.substr(atoi(dano_val_grow.c_str()), 1); 1 != link_grow.length()){ err("Ссылка расширяемого морфа");
					}else if(string addr_grow = (index_grow.end() == index_grow.find("addr") ? "" : index_grow.at("addr")); addr_grow.empty()){ err("Расчет адреса расширяющего морфа");
					}else if(string index_id_grow = "1" +link_grow +addr_grow.substr(1, addr_grow.length()); index_id_grow.empty()){ err("Новый адрес");
					}else if(BMF_INDEX.end() != BMF_INDEX.find(index_id_grow)){ mpre(index_vals_grow ,"ОШИБКА Морф уже в базе key=" +to_string(key) +" grow_md5=" +grow_md5 +" addr_grow=" +addr_grow +" > " +index_id_grow ,__LINE__);
					}else if(string list = [&](string list = ""){ // Список родителей
						if(auto npos = index_id_grow.find_first_of("-"); string::npos == npos){ err("Разделитель в новом идентификаторе не найден");
						}else if([&](){ for(int i = 2; i <= npos; i++){ // Список исходников
							if(string adr = "1" +index_id_grow.substr(i, npos-i); adr.empty()){ err("Расчетный адрес");
							}else if(string _addr = adr +index_id_grow.substr(npos, -1); _addr.empty()){ err("Полный адрес");
							}else if(BMF_INDEX.end() == BMF_INDEX.find(_addr)){ mpre("ОШИБКА Родитель в справочнике не найден " +adr +" ", __LINE__);
							}else if(string dano_id = (BMF_INDEX.at(_addr).end() == BMF_INDEX.at(_addr).find("dano_id") ? "" : BMF_INDEX.at(_addr).at("dano_id")); dano_id.empty()){ err("Выборка исходного идентификатора");
							}else if(list = ";" +dano_id +list; list.empty()){ err("Инкремент списка");
							}else{ //mpre("Значение i=" +to_string(i) +" adr=" +adr +" index_id_grow=" +index_id_grow ,__LINE__);
							}}return false; }()){ err("Составление списка исходников");
						}else{ //mpre("Строка исключений list=" +list, __LINE__);
						}return list; }(); false){ err("Список родителей");
					}else if(TMs dano_grow = bmf::Choice(list, key, DANO, DANO_BITMAP); dano_grow.empty()){ err("Выбор исходного значения");
					}else if(TMs _index = {{"itog_values_id", index_grow.at("itog_values_id")}, {"parent", index.at("id")}, {"addr", index_id_grow}, {"dano_id", dano_grow.at("id")}, {"itog_id", index_grow.at("itog_id")}}; _index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
					}else if(_index = bmf::Up("index", {{"id", md5(index_id_grow)}}, _index, {}, __LINE__); _index.empty()){ mpre("ОШИБКА добавления корневого морфа в базу", __LINE__);
					}else if(!(err += 1)){ err("Инкримент изменений");
					//}else{ //mpre(_index ,"Расширение index_id_grow=" +index_id_grow ,__LINE__); //mpre(index_vals_grow, "Значение рашисряемого морфа" ,__LINE__);
					}else if(bmf::ARGV.end() == bmf::ARGV.find("verbose")){ //mpre("Не отображаем время обучения", __LINE__);
					}else{ mpre(" " +to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - _microtime) +" Расширение модели " +index_id_grow +" ", __LINE__);
					}return false; }()){ err("Расширение");
				}else if([&](){ // Сохранение результата в индекс
					if(std::string dataset_map_id = "index," +index.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					}else if(TMs index_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
					}else if(std::string map = (index_map.end() == index_map.find("map") ? std::string(dataset_count, '0') : index_map.at("map")); map.empty()){ mpre("ОШИБКА расчета карты", __LINE__);
					}else if(boost::dynamic_bitset<> imap(map); imap.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
					}else if(std::string val = (imap.test(key) ? "1" : "0"); val.empty()){ err("Результат в карте");
					}else if(val == calc){ //mpre("Расчетный результат совпал с картой", __LINE__);
					}else if(imap[key] = atoi(calc.c_str()); imap.empty()){ err("Установка расчетного бита в карту");
					}else if(std::string map = [&](std::string map = ""){ boost::to_string(imap, map); return map; }(); (map.length() != dataset_count)){ mpre("ОШИБКА установки карты исходника", __LINE__);
					}else if(TMs index_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"map", map}}, {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"map", map}}, __LINE__); index_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
					}else{ //mpre("Сохранение обновленной карты морфа map=" +map, __LINE__);
					}return false; }()){ err("Сохранение результата в индекс")
				}else{ //mpre("Обучение сигнала " +to_string(key) +" itog[" +itog.at("id") +"]", __LINE__);
				}}return false; }()){ mpre("ОШИБКА сравнения результата расчета", __LINE__);
			}else if(bmf::dataset = [&](){ // Обновление ключа
				if(bmf::ARGV.end() == bmf::ARGV.find("learn")){ //mpre("Не изменяем позицию без обучения", __LINE__);
				}else if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); !epoch){ //mpre("Не сохраняем свойства в эпоху" ,__LINE__);
				}else if(bmf::dataset["key"] = std::to_string(key); bmf::dataset.empty()){ err("Установка нового ключа");
				}else if(bmf::dataset["err"] = std::to_string(err); bmf::dataset.empty()){ err("Установка количества ошибок эпохи");
				}else if(bmf::dataset["microtime"] = std::to_string(microtime); bmf::dataset.empty()){ err("Установка времени расчета эпохи");
				}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", bmf::dataset.at("id")}}, {}, bmf::dataset, __LINE__); bmf::dataset.empty()){ err("Обновление ключа");
				}else{ //mpre(bmf::dataset, "Обновление ключа", __LINE__);
				}return bmf::dataset; }(); bmf::dataset.empty()){ err("Обновление ключа");
			}else if(rep = (++key >= dataset_count ? false : true); false){ mpre("ОШИБКА положительный повтор", __LINE__);
			//}else if(true){ mpre("ОШИБКА Остановка", __LINE__);
			}else{ //mpre("Итерации err=" +to_string(err) ,__LINE__);
			}}while(!(rep = !rep)); return err; }(); (0 > err)){ mpre("ОШИБКА проверки списка итогов", __LINE__);
		}else if([&](){ // Прибавление эпохи
			if(!err){ //mpre("Не увеличиваем эпохи без обучения", __LINE__);
			}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", bmf::dataset.at("id")}}, {}, {}, __LINE__); bmf::dataset.empty()){ err("Выборка набора данных");
			}else if(bmf::dataset["epoch"] = to_string(atoi(bmf::dataset["epoch"].c_str())+1); bmf::dataset.at("epoch").empty()){ err("Инкремента эпох");
			}else if(bmf::dataset["key"] = "0"; false){ err("Обнуление позиции");
			}else if(bmf::dataset["err"] = "0"; false){ err("Обнуление позиции");
			}else if(bmf::dataset["microtime"] = "0"; false){ err("Обнуление позиции");
			}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", bmf::dataset.at("id")}}, bmf::dataset, bmf::dataset, __LINE__); bmf::dataset.empty()){ err("Выборка набора данных");
			}else{ //mpre("Инкремент эпох", __LINE__);
			}return bmf::dataset; }(); bmf::dataset.empty()){ err("Инкремент эпох");
		}else if([&](){ // Расчет процента бит
			if(1 == bmf::loop){ bmf::pips_first = (err ? (float)(dataset_count-err)/dataset_count : 1); //mpre("Расчет процента первой эпохи err=" +to_string(err) +" count=" +to_string(count), __LINE__);
			}else{ bmf::pips_last = (err ? (float)(dataset_count-err)/dataset_count : 1); //mpre("Расчет процента последней эпохи", __LINE__);
			}return false; }()){ mpre("ОШИБКА расчета процента бит", __LINE__);
		}else if(std::cerr << endl; false){ mpre("Пробел за прогрессом", __LINE__);
		}else if(errors += err; !err){ //mpre("Ошибок не обнаружено количество Эпох обучения "+ to_string(epoch -loop) +" Размер модели " +to_string(BMF_INDEX_EX.at("").size()) +" морфов Время "+ to_string((std::chrono::system_clock::now().time_since_epoch()).count()/1e9 -bmf::microtime) +" сек", __LINE__);
		}else if(!(pass = !pass)){ mpre("ОШИБКА инвертирования признака повтора", __LINE__);
		}else{ //mpre("Результаты расчета Общее количество/Последней эпохи " +to_string(errors) +"/" +to_string(err) +"\n", __LINE__);
		}}while(!(pass = !pass)); return errors; }(); false){ mpre("ОШИБКА перебора эпох", __LINE__);
	}else if([&](){ // Вывод итоговых значений
		if(false){ mpre("Пропуск расчетов", __LINE__);
		}else if(bmf::ARGV.end() == bmf::ARGV.find("dano")){ //mpre("Данные для расчета не указаны", __LINE__);
		}else if(nlohmann::json _in = ("" == bmf::ARGV.at("dano") ? in : nlohmann::json::parse(bmf::ARGV.at("dano"))); _in.empty()){ mpre("ОШИБКА разбора строки атрибута -dano", __LINE__);
		//}else if(true){ mpre("Проверка", __LINE__);
		}else if(nlohmann::json j = [&](nlohmann::json j = {}){ for(auto& js:_in.items()){ // Расчет списка теста
			if(nlohmann::json el = js.value(); el.empty()){ mpre("ОШИБКА элемент не найден", __LINE__);
			}else if(string key = js.key(); (0 >= key.length())){ mpre("ОШИБКА расчета ключа", __LINE__);
			}else if(el.end() == el.find("dano")){ mpre("ОШИБКА исходные данные не могут быть пустыми", __LINE__);
			}else if(TMs dano = [&](TMs dano = {}){ TMs _dano = el.at("dano"); dano = _dano; return dano; }(); dano.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
			}else if(TMs itog = [&](TMs itog = {}){ // Получение итога
				if(el.end() == el.find("itog")){ //mpre("Итог не задан", __LINE__);
				}else if([&](){ TMs _itog = el.at("itog"); itog = _itog; return itog.empty(); }()){ mpre("ОШИБКА получения итоговых знаков", __LINE__);
				}else{ //mpre(itog, __LINE__, "Итог");
				} return itog; }(); !itog.empty()){ mpre("ОШИБКА в расчете задан итог", __LINE__);
			}else if(mpre("РЕЗУЛЬТАТ: "+ el.dump(), __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(int cache = Crc32(el["dano"].dump().c_str()); false){ mpre("ОШИБКА расчета ключа кеша", __LINE__);
			}else if(bmf::Vals(dano, "dano", cache, bmf::DANO, bmf::DANO_VALUES, bmf::DANO_TITLES); BMF_DANO_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
			}else if(BMF_ITOG_VALUES_EX.end() == BMF_ITOG_VALUES_EX.find("")){ mpre("ОШИБКА список значений не задан", __LINE__);
			}else if([&](){ for(auto &itog_values_itr:BMF_ITOG_VALUES_EX.at("")){ //for_each(BMF_ITOG_VALUES_EX.at("").begin(), BMF_ITOG_VALUES_EX.at("").end(), [&](auto &itog_values_itr){ // Расчет значений
				if(TMs itog_values = itog_values_itr.second; itog_values.empty()){ mpre("ОШИБКА получения значения", __LINE__);
				}else if(itog_values_itr.second = bmf::Value(itog_values); itog_values_itr.second.empty()){ mpre("ОШИБКА расчета значения", __LINE__);
				}else{ //mpre(itog_values, __LINE__, "Расчетное значение"); //mpre("Расчетное значение `"+ itog_values["name"]+ "` "+ to_string(dec)+ " ("+ bin+ ")", __LINE__);
				}}; return false; }()){ mpre("Ошибка отображение результата расчета", __LINE__);
			}else if([&](){ for(auto &itog_values_itr:BMF_ITOG_VALUES_EX.at("")){
				if(TMs itog_values = itog_values_itr.second; itog_values.empty()){ mpre("ОШИБКА получения значения итога", __LINE__);
				}else if(string name = itog_values.at("name"); (0 >= name.length())){ mpre("ОШИБКА слишком короткое имя параметра", __LINE__);
				}else if(string value = itog_values.at("value"); (0 > value.length())){ mpre("ОШИБКА слишком короткое значение параметра", __LINE__);
				}else if([&](){ char dig[100]; sprintf(dig,"%g", stod(value)); j[key][name] = value = dig; return false; }()){ mpre("ОШИБКА присвоения значения массиву", __LINE__);
				}else if([&](){ // Проверка совпадения расчета с итогом
					if(itog.empty()){ //mpre("Итоги не заданы", __LINE__);
					}else if(itog.end() == itog.find(name)){ mpre("Имя в итогах не найдено "+ name, __LINE__);
					}else if(value != itog.at(name)){ mpre(itog, __LINE__, "Итог"); mpre("ОШИБКА значение итога не совпало "+ name+ "="+ value, __LINE__);
					}else{ //mpre("Итоги совпадают", __LINE__);
					} return false; }()){ mpre("ОШИБКА проверки итога", __LINE__);
				}else if([&](){
					if(TMs itog_titles = erb(BMF_ITOG_TITLES_EX, {{"itog_values_id", itog_values["id"]}, {"value", value}}); itog_titles.empty()){ //mpre("Заголовок значения `"+ itog_values["name"]+ "` не найден", __LINE__);
					}else if(itog_titles.end() == itog_titles.find("name")){ mpre("ОШИБКА поле `name` не найдено у заголовка", __LINE__);
					}else if(j[key][name] = itog_titles.at("name"); false){ mpre("ОШИБКА установки заголовка значению", __LINE__);
					}else{ //mpre(itog_titles, __LINE__, "Замения значения заголовком");
					} return false; }()){ mpre("ОШИБКА замены значения заголовком", __LINE__);
				}else{ //mpre(itog_titles, __LINE__, "Заголовок значения "+ value);
				}} return false; }()){ mpre("ОШИБКА расчета результата", __LINE__);
			}else{ //mpre("Итог "+ j.dump(), __LINE__);
			}} return j; }(); false){ mpre("ОШИБКА расчета списка теста", __LINE__);
		}else{ //mpre("j "+ j.dump(), __LINE__);
			std::cout << j.dump('\t');
		} return false; }()){ mpre("Ошибка отображение результата расчета", __LINE__);
	}else{ //std::cerr << endl; //mpre(_BMF_INDEX, __LINE__, "Сохранение");
	}
}

