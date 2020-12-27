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
#include <curses.h>
#include <ncurses.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <termios.h>
#include <memory>

#include <db_cxx.h>
#include <dbstl_map.h>
#include <pthread.h>
#include <unistd.h>

#include <mariadb/mysql.h>

//#define CL_HPP_MINIMUM_OPENCL_VERSION 110
//#define CL_HPP_TARGET_OPENCL_VERSION 210
//#define CL_HPP_CL_1_2_DEFAULT_BUILD
//	#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
//#include "CL/cl2.hpp"

#define pre(x) if(x){ printf("%d.%s\n", __LINE__, x); }
#define err(x) if(x){ printf("%d.\x1b[37;41m ОШИБКА \x1b[0m %s\n", __LINE__, x); exit(1); }

//using json = nlohmann::json; json in;

using namespace std; //setlocale(LC_ALL, "ru_RU");
namespace fs = std::experimental::filesystem;

typedef std::map<string, string> TMs;
typedef std::map<string, TMs> TMMs;
typedef std::map<int, TMs> TMMi;
typedef std::map<string, TMMi> TM3i;

typedef dbstl::db_map<string, TMs> TMMt;
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
	TMs COUNT; // Кеш результатов расчета
	TMs CALCULATE; // Список формул для быстрых расчетов для каждого из итогов
//	std::map<int, boost::dynamic_bitset<>> DANO; // Кеш результатов исходников
//	std::map<int, boost::dynamic_bitset<>> ITOG; // Кеш результатов расчета
//	std::map<int, boost::dynamic_bitset<>> MAPS; // Список битовых результатов расчета для каждого морфа


	MYSQL *mysql = NULL;
	TMs databases; // Список баз данных
	string dbname; //Имя БД
	TMs ARGV; // Массив параметров консоли
	int timestamp =  time(0); // Расчет выполнения участков кода
	auto microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9;

	DbEnv *env; //Окружение БД
	TMMt *TEST; // Справочнк морфов

	std::string INDEX = "index"; // Справочнк морфов
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
	std::function<TMs(TMs,TMs,TMs,int)> Choice; // Расчет истории
	std::function<double(string)> Bin2dec; // Перерасчет размерности из двоичной в десятеричную
	std::function<string(double)> Dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
	std::function<string(TMs,string,int)> Learn; // Рерасчет морфа

	std::function<void(string,float,int)> Progress; // Отрисовка полосы выполнения
	std::function<TMs(TMs&,std::string)> Maps; // Расчет карт результатов

	std::function<Db*(string)> Open; // Список баз данных
	std::function<bool(int)> Close; // Закрытие соединения с БД
	std::function<TMMs(TMs,string,int,string,string,string)> Vals; // Обучение
	std::function<TMs(string,TMs,TMs,TMs,int)> Up; // Обучение
	std::function<TMMs(string,TMs,int)> List; // Список выборки
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
	}else if([&](bool skip = true){ // Остановки из консоли
		if([&](bool skip = false){ // Версия программы
			if(bmf::ARGV.end() == bmf::ARGV.find("-v")){ //mpre("Пропускаем отображение версии", __LINE__);
			}else if(skip = !skip; false){ mpre("Условия выхода", __LINE__);
			}else{ std::cout << endl;
				std::cout << "bimorph v8.0" << endl;
				std::cout << "Copyright (C) 2021 биморф.рф" << endl;
				std::cout << "Нет НИКАКИХ ГАРАНТИЙ до степени, разрешённой законом." << endl << endl;
				std::cout << "Лицензия freemium https://ru.wikipedia.org/wiki/Freemium" << endl;
				std::cout << "Данная версия является условной бесплатной с граничением сети в "+ to_string(bmf::size_max)+ " морфов" << endl;
				std::cout << "Морф — наименьшая осмысленная единица языка (логики)" << endl << endl;

				std::cout << "Авторы программы -- Кривошлыков Евгений Павлович +79618063797" << endl << endl;
			}return skip; }()){ mpre("Информация о версии", __LINE__);
		}else if([&](){ // Отображение атрибутов командной строки
			if(bmf::ARGV.end() == bmf::ARGV.find("-a")){ return false; //mpre("Пропускаем отображение версии", __LINE__);
			}else{ mpre(bmf::ARGV, __LINE__, "Атрибуты командной строки");
			} return true; }()){ mpre("Атрибуты параметров командной строки", __LINE__);
		}else if([&](){ // Раздел помощи
			if(bmf::ARGV.end() == bmf::ARGV.find("-h")){ return false; //mpre("Пропускаем раздел помощи", __LINE__);
			}else if(string file = (1 <= argc ? argv[0] : "./bimorph"); (0 >= file.length())){ mpre("ОШИБКА получения имени файла", __LINE__);
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

				std::cout << "Оперативная справка: http://биморф.рф/" << endl;
				std::cout << "Об ошибках в переводе сообщений «биморф» сообщайте по адресу: <bimorph@mpak.su>" << endl;
				std::cout << "Полная документация: http://биморф.рф/" << endl << endl;
			} return true; return false; }()){ mpre("Раздел помощи", __LINE__);
		}else if(skip = !skip; false){ mpre("Условие выхода", __LINE__);
		}else{
		}return skip; }()){ exit(mpre("Остановка выполнения", __LINE__));
	}else if([&](){ // Подключение БД
		if([&](){ // Имя базы данных
			if(bmf::ARGV.end() == bmf::ARGV.find("db")){ mpre("ОШИБКА БД для сохранения не задана -db", __LINE__);
			}else if(0 >= bmf::ARGV.at("db").length()){ mpre("База данных для сохранения не указана", __LINE__);
			}else if(bmf::dbname = bmf::ARGV.at("db"); (0 >= bmf::dbname.length())){ mpre("ОШИБКА имя файла для БД не задано", __LINE__);
			}else{ //mpre("База данных "+ bmf::dbname, __LINE__);
			}return false; }()){ mpre("ОШИБКА получения имени базы данных", __LINE__);
		}else if([&](){ // Получение пути до файла БД
			if(int npos = bmf::dbname.rfind("/"); false){ //mpre("Слешей в пути до скопления не найдено", __LINE__);
			}else if(bmf::clump_id = (std::string::npos == npos ? bmf::dbname : bmf::dbname.substr(npos+1, bmf::dbname.length())); (0 >= bmf::clump_id.length())){ mpre("ОШИБКА сокращения пути до файла", __LINE__);
			}else{ //mpre("Путь до БД сокращен "+ bmf::clump_id, __LINE__);
			}return (0 >= bmf::clump_id.length()); }()){ mpre("ОШИБКА получения скопления", __LINE__);
		}else if([&](int result = 0){ // Подключение к БД
			if(bmf::mysql = mysql_init(NULL); (nullptr == bmf::mysql)){ err("ОШИБКА создания соединения");
			}else if(std::string host = (bmf::ARGV.end() == bmf::ARGV.find("host") ? "localhost" : bmf::ARGV.at("host")); host.empty()){ err("Выбор хоста");
			}else if(std::string user = (bmf::ARGV.end() == bmf::ARGV.find("user") ? "root" : bmf::ARGV.at("user")); user.empty()){ err("Выбор пользователя");
			}else if(std::string password = (bmf::ARGV.end() == bmf::ARGV.find("password") ? "" : bmf::ARGV.at("password")); false){ err("Выбор пароля");
			//}else if(std::string user = (bmf::ARGV.end() == bmf::ARGV.find("user") ? "root" : bmf::ARGV.at("user")); user.empty()){ err("Выбор пользователя");
			//}else if(std::string database = (bmf::ARGV.end() == bmf::ARGV.find("database") ? "bimorph" : bmf::ARGV.at("database")); database.empty()){ err("Выбор пользователя");
			}else if(bmf::mysql = mysql_real_connect(bmf::mysql, host.c_str(), user.c_str(), password.c_str(), bmf::dbname.c_str(), 0, NULL, 0); (nullptr == bmf::mysql)){ mpre("Параметры подключения host=" +host +" user=" +user +" password=" +password +" database=" +bmf::dbname, __LINE__); err("Подключение БД"); 
			}else if(mysql_query(bmf::mysql, std::string("SET NAMES utf8").c_str())){ err("Установка кодировки");
			}else{
			}return result; }()){ err("Подключение к БД");
		}else if([&](){ // Очистка БД
			if(bmf::ARGV.end() == bmf::ARGV.find("-c")){ //mpre("Не очищаем БД", __LINE__);
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `index`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dataset_map`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano_values`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `dano_titles`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog_values`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else if(mysql_query(bmf::mysql, std::string("DROP TABLE IF EXISTS `itog_titles`;").c_str())){ err("ОШИБКА удаления таблицы");
			}else{ mpre("Очистка базы данных", __LINE__);
			}return false; }()){ err("ОШИБКА очистки БД");
		}else if([&](){ // Создание таблиц 
			//}else if(mysql_query(bmf::mysql, std::string("DROP TABLE `index`; DROP TABLE dataset; DROP TABLE dataset_map; DROP TABLE dano; DROP TABLE dano_values; DROP TABLE dano_titles; DROP TABLE itog; DROP TABLE itog_values; DROP TABLES itog_titles").c_str())){ err("Удаление таблиц");
			if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `index` (`id` int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY, `itog_values_id` VARCHAR(256), `dano_id` VARCHAR(256), `itog_id` VARCHAR(256), `index_id` VARCHAR(256), `bmf-index` VARCHAR(256), KEY `itog_id` (`itog_id`))").c_str())){ err("Список морфов"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dataset` (`id` VARCHAR(256) NOT NULL UNIQUE, `name` VARCHAR(256), `count` int(11), `epoch` int(11))").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dataset_map` (`id` VARCHAR(256) NOT NULL UNIQUE, `dataset_id` VARCHAR(256), `alias` VARCHAR(256), `alias_id` VARCHAR(256), `itog_id` VARCHAR(256), `map` TEXT)").c_str())){ err("Вставка данных"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano` (`id` VARCHAR(256) NOT NULL UNIQUE, `dano_values_id` VARCHAR(256), `name` VARCHAR(256), `val` VARCHAR(256))").c_str())){ err("Дано"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano_values` (`id` VARCHAR(256) NOT NULL UNIQUE, `name` TEXT)").c_str())){ err("Дано значения"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `dano_titles` (`id` VARCHAR(256) NOT NULL UNIQUE, `dano_values_id` VARCHAR(256), `name` VARCHAR(256), `value` VARCHAR(256) UNIQUE)").c_str())){ err("Дано заголовки"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog` (`id` VARCHAR(256) NOT NULL UNIQUE, `itog_values_id` VARCHAR(256), `name` VARCHAR(256), `val` VARCHAR(256), `index_id` VARCHAR(256))").c_str())){ err("Итог"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog_values` (`id` VARCHAR(256) NOT NULL UNIQUE, `name` VARCHAR(256))").c_str())){ err("Итоги значения"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `itog_titles` (`id` VARCHAR(256) NOT NULL UNIQUE, `itog_values_id` VARCHAR(256), `name` VARCHAR(256), `value` VARCHAR(256))").c_str())){ err("Итоги заголовки"); 
			}else if(mysql_query(bmf::mysql, std::string("CREATE TABLE IF NOT EXISTS `test` (`id` VARCHAR(256) NOT NULL UNIQUE, `name` VARCHAR(256), `count` int(11) NOT NULL)").c_str())){ err("Вставка данных"); 
			}else{ //mpre("Создание таблиц", __LINE__); 
			}return false; }()){ err("Создание таблиц");		
		}else{ //mpre("Подключение БД " +bmf::dbname, __LINE__);
		}return false; }()){ mpre("ОШИБКА подключения БД", __LINE__);
	}else if([&](){ // Функции БД
		if(bmf::Up = ([&](std::string ROWS, TMs where, TMs insert, TMs update, int line){
			TMs _row; // Возвращаем обновленное значение
			if([&](){ // Проверка условия
				if(update.empty()){ //mpre("Не задано обнолвение", __LINE__);
				}else if(where.end() == where.find("id")){ mpre(where, "Условие", __LINE__); mpre("У поля не задан идентификатор", __LINE__);
				}else if(1 != where.size()){ mpre("ОШИБКА условий выборки больше одного", __LINE__);
				}else{ //mpre("Условие корректно", __LINE__);
				}return false; }()){ err("Проверка условия выборки");
			}else if([&](){ // Выборка
				if(where.empty()){ //mpre("Не заданы условия выборки", __LINE__);
				}else if(std::string _where = [&](std::string _where = ""){ // Условия выборки
					if([&](){ for(auto where_itr:where){ // Строка условий
						_where += string(_where.empty() ? "" : " AND ") +"`" +where_itr.first +"`='" +where_itr.second +"'";
						}return false; }()){ err("Строка условий");
					}else{ //mpre("Условия выборки "+ _where, __LINE__);
					}return _where; }(); false){ err("Составления условий выборки");
				}else if(std::string sql = "SELECT * FROM `" + ROWS+"` WHERE " +_where; sql.empty()){ err("Запрос к БД");
				}else if(_row = [&](TMs _row = {}){ // Получение значения
					if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Выбор данных из таблицы " +sql, __LINE__);
					}else if(MYSQL_RES *result = mysql_store_result(bmf::mysql); false){ err("ОШИБКА выполнения запроса");
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
			//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
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
					if(std::string VALUES = [&](std::string VALUES = "", std::string fields = "", std::string values = ""){ for(auto insert_itr:insert){
						if(fields += std::string(fields.empty() ? "" : " ,") +"`" +insert_itr.first +"`"; fields.empty()){ err("Поле значения");
						}else if(values += std::string(values.empty() ? "" : " ,") +"'" +insert_itr.second +"'"; values.empty()){ err("Значение");
						}else{ //mpre("Список полей значений " +insert_itr.first +" => " +insert_itr.second, __LINE__);
						}}return "(" +fields +") VALUES (" +values +")"; }(); VALUES.empty()){ err("Получение пар полей значений запроса");
					}else if(std::string sql = "INSERT INTO `" +ROWS +"` " +VALUES +";"; sql.empty()){ err("Запрос на добавление");
					}else if(mysql_query(bmf::mysql, sql.c_str())){ mpre("Запрос line=" +to_string(line) +" " +sql, __LINE__); mpre("Запрос на добавление " +string(mysql_error(bmf::mysql)), __LINE__); err("Запрос");
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
			}else if(mysql_query(bmf::mysql, string("SELECT * FROM `" +table +"`").c_str())){ err("Выбор данных из таблицы");
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
				}else{ //mpre(ROW, "Элемент списка", __LINE__);
				}}return false; }()){ err("ОШИБКА перебора значений");
			}else if(mysql_free_result(result); false){ err("Очистка ресурсов");
			}else{ //mpre("Запрос `" +table +"` списка:" +to_string(line) +" количество элементов:" +to_string(LIST.size()), __LINE__);
			}return LIST; }; false){ err("ОШИБКА выборки из базы");
		}else{
		}return false; }()){ mpre("ОШИБКА функции БД", __LINE__);
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
		if(bmf::Choice = ([&](TMs index, TMs stairs, TMs _stairs, int key, TMs _dano = {}){ //mpre("Выбор итога для расширения", __LINE__);
			if(TMMs _DANO = [&](TMMs _DANO = {}){ for(auto dano_itr:bmf::List(bmf::DANO, {}, __LINE__)){ // Получение списка исходников
				_DANO.insert(make_pair(dano_itr.first, dano_itr.second));
				}return _DANO; }(); _DANO.empty()){ mpre("ОШИБКА список исходников пуст", __LINE__);
			//if(TMMi _DANO = BMF_DANO_EX.at(""); _DANO.empty()){ mpre("ОШИБКА сохранения списка исходников", __LINE__);
			}else if(stairs.end() == stairs.find("list")){ mpre("ОШИБКА список в ступени не задан", __LINE__);
			}else if(string list = stairs.at("list"); false){ mpre("История не установлена", __LINE__);
			}else if([&](int pos = 0, int loop = 2e9){ while((string::npos != (pos = list.find_last_of(","))) && (loop-- > 0) && (1 < _DANO.size())){ // Исключение повторяющихся исходников
				if(false){ mpre("Пропуск исключений", __LINE__);
				}else if(std::string dano_id =list.substr(pos +1, list.length()); dano_id.empty()){ mpre("ОШИБКА получения идентификатор исходника " +list, __LINE__);
				}else if(int last = dano_id.find_last_of(";"); (string::npos == last)){ mpre("ОШИБКА символ завершения идентификатора не найден", __LINE__);
				}else if(dano_id = dano_id.substr(0, last); dano_id.empty()){ mpre("ОШИБКА удаления последнего символа", __LINE__);
				}else if(list =list.substr(0, pos); false){ mpre("ОШИБКА сокращение длинны списка", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_id)){ mpre(" `"+ stairs.at("list") +"` ", __LINE__); mpre("ОШИБКА Исходник не найден в списке list=`" +list +"` `"+ dano_id +"`", __LINE__);
				}else if(_DANO.erase(dano_id); _DANO.empty()){ mpre("ОШИБКА не удаляем последнее значение", __LINE__);
				}else{ //mpre("Пропуск выбора исходника list=" +list +" `" +dano_id +"`", __LINE__);
				}}return _DANO.empty(); }()){ mpre("ОШИБКА список не должен оставаться пустым", __LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("count")){ mpre("ОШИБКА поле количества элементов в наборе данных не установлено", __LINE__);
			}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества элементов в наборе данных", __LINE__);
			}else if(int prev = (!key ? dataset_count -1 : key -1); (0 > prev)){ mpre("ОШИБКА расчета предыдущей позиции", __LINE__);
			}else if([&](int pos = 0, bool loop = false){ for(auto dano_itr:bmf::List(bmf::DANO, {}, __LINE__)){ // Перебор исходных значений
				if(1 >= _DANO.size()){ //mpre("Не удаляем последнее значение", __LINE__);
				//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_itr.first)){ //mpre("Уже удален " +dano.at("id"), __LINE__);
				}else if(std::string dataset_map_id = "dano," +dano_itr.second.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
				//}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", dano_itr.second.at("id")}}, {}, {}); false){ mpre("ОШИБКА выборки карты исходника", __LINE__);
				}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("ОШИБКА выборки карты исходника", __LINE__);
				}else if(std::string _map = (dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); _map.empty()){ mpre("ОШИБКА получения карты исходника", __LINE__);
				}else if(boost::dynamic_bitset<> map = boost::dynamic_bitset<>(_map); (dataset_count != map.size())){ mpre("ОШИБКА расчета карты исходника", __LINE__);
				//}else if(_stairs.end() == _stairs.find("parent")){ mpre("Отсутсвует имя связи в ступени", __LINE__);
				//}else if(bool val = ("index_id" == _stairs.at("parent") ? false : true); false){ mpre("ОШИБКА получения предпочтительного значения исходника", __LINE__);
				//}else if(map.test(key) == val){ //mpre("Предпочтительное значение исходника", __LINE__);
				}else if(map.test(key) != map.test(prev)){ //mpre("Изменившийся исходник", __LINE__);
				}else if(_DANO.erase(dano_itr.first); false){ mpre("ОШИБКА неизменившегося исходника", __LINE__);
				}else{ //mpre("Удаление исходника " +std::to_string(dano_itr.first), __LINE__);
				}}return false; }()){ mpre("ОШИБКА перебора значений", __LINE__);
			}else if(_dano = [&](){ // Оставшееся значение
				if(auto dano_itr = _DANO.rbegin(); false){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
				}else if(_dano = dano_itr->second; _dano.empty()){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
				}else{ //mpre(_dano, "Оставшееся значение", __LINE__);
				}return _dano; }(); _dano.empty()){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
			}else{ //mpre(stairs, "Ступень", __LINE__); //mpre(_BMF_DANO_EX.at(""), "Список", __LINE__); mpre(_DANO, "Выбор", __LINE__); mpre(_dano, "Результат", __LINE__); //mpre("ОШИБКА функции выбора исходного значения", __LINE__);
			}return _dano; }); false){ mpre("ОШИБКА установки функции выбора Исходного значения", __LINE__);
		}else if(bmf::Vals = ([&](TMs VALUE, string alias, int key, std::string BMF_VALS, std::string BMF_VALUES, std::string BMF_TITLES){ // Установка входных значений
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
						//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
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
							}else if("." == val){ //mpre("ОШИБКА Символ отрицания "+ val+ " не должен попадаться", __LINE__);
							}else if([&](){ // Расчет значения отрицания
								if(0 < i){ //mpre("Вторая и последующие позиции", __LINE__);
								}else if(("-" != val) && ("+" != val)){ mpre("ОШИБКА Симовл не является знаком _value_="+ _value_+ " i="+ to_string(i)+ " val="+ val, __LINE__);
								}else if(val = ("-" == _value.substr(0, 1) ? "1" : "0"); (1 != val.length())){ mpre("ОШИБКА расчета знака числа", __LINE__);
								}else{ //mpre("Расчет знака "+ value, __LINE__);
								} return (1 != val.length()); }()){ mpre("ОШИБКА расчета значения знака", __LINE__);
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
								}else if(TMs _vals = {/*{"clump_id", bmf::clump_id},*/ {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}, {"val", ""}/*, {"values", ""}*/}; _vals.empty()){ mpre("ОШИБКА создания нового знака", __LINE__);
								}else if((vals = bmf::Up(BMF_VALS, {{"id", vals_id}}, _vals, {}, __LINE__)).empty()){ mpre(_vals, "Знак", __LINE__); mpre("ОШИБКА обновления знака", __LINE__);
								}else{ //mpre("Добавление нового знака в базу " +vals_id, __LINE__); //mpre("ОШИБКА добавление глобального знака", __LINE__); //mpre(*BMF, __LINE__, "Список знаков"); mpre("Добавление глобального знака `"+ values["name"]+ "` ["+ vals.at("id")+ "] "+ to_string(nn), __LINE__); mpre(vals, __LINE__, "Знак"); //mpre("ОШИБКА добавления знака "+ alias+ " "+ vals.at("id"), __LINE__);
								} return vals; }(); false){ mpre("ОШИБКА Добавления нового знака", __LINE__);
							/*}else if([&](){ // Установка значения
								if(vals.empty()){ //mpre("Отрицательный знак", __LINE__);
								}else if(vals.end() == vals.find("val")){ mpre("ОШИБКА поле значения val не найдено", __LINE__);
								}else if(vals.at("val") == val){ //mpre("Знак не изменился", __LINE__);
								}else if(vals.at("val") = val; (1 != val.length())){ mpre("ОШИБКА установки знака знака", __LINE__);
								}else if(vals.end() == vals.find("id")){ mpre("ОШИБКА идентификатора знака не задан", __LINE__);
								}else if(TMs _vals = bmf::Up(BMF_VALS, {{"id", vals.at("id")}}, vals, vals, __LINE__); _vals.empty()){ err("Сохранения значения знака");
								}else{ //mpre("Изменение знака", __LINE__);
								}return false; }()){ mpre("ОШИБКА установки знака", __LINE__);*/
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
		}else if(bmf::Learn = ([&](TMs bmf_index, string val = "", int key, TMs stair = {}){ //mpre("Обучение "+ bmf_index.at("id"), __LINE__);  //system("sleep 1"); // mpre(BMF_INDEX, __LINE__, "Список"); // if(remove){ mpre(row, __LINE__, "Ученик"); }
			if(int rep = 2e9; 0 >= rep){ mpre("ОШИБКА установки предельного размера обучения для сети", __LINE__);
			}else if(static TMMi STAIRS; false){ mpre("ОШИБКА лестница не пуста", __LINE__);
			}else if([&](){ // Требование к морфу
				if(bmf_index.end() == bmf_index.find("id")){ mpre("ОШИБКА идентификатор у морка не установлен", __LINE__);
				}else if(TMs stairs = {{"index_id", bmf_index.at("id")}, {"parent", "bmf-index"}}; stairs.empty()){ mpre("ОШИБКА создания первой ступени", __LINE__);
				}else if(STAIRS.insert(make_pair(STAIRS.size(), stairs)); STAIRS.empty()){ mpre("ОШИБКА устанвоки первоначального значения лестнице", __LINE__);
				}else if(bmf_index.empty()){ mpre("ОШИБКА входное значение лестницы пусто", __LINE__);
				}else{ //mpre("Установка требований к лестнице promise="+ val, __LINE__);
				}return false; }()){ mpre("ОШИБКА установки первоначального значения лестницы", __LINE__);
			}else if([&](bool pass = false, int loop = 2e9){ do{ //mpre("Повтор "+ to_string(rep), __LINE__); // Ступени расчета
				if(int size = STAIRS.size(); (0>=size)){ mpre("ОШИБКА Пустая лестница", __LINE__);
				}else if(auto stairs_itr = STAIRS.rbegin(); stairs_itr == STAIRS.rend()){ mpre("ОШИБКА получения итаратора следующей ступени", __LINE__);
				}else if(TMs stairs = stairs_itr->second; stairs.empty()){ mpre(STAIRS, __LINE__, "Лестница"); mpre("ОШИБКА получения ступени", __LINE__);
				//}else if(mpre(bmf_index, "Морф", __LINE__); false){ err("Уведомление");
				//}else if(TMs index = bmf::fk("index", {{"id", stairs.at("index_id")}}, {}, {}); index.empty()){ mpre("ОШИБКА обновления морфа", __LINE__);
				}else if(TMs index = bmf::Up(bmf::INDEX, {{"id", stairs.at("index_id")}}, {}, {}, __LINE__); index.empty()){ mpre("ОШИБКА выборки морфа", __LINE__);
				}else if(TMs dano = bmf::Up(bmf::DANO, {{"id", index.at("dano_id")}}, {}, {}, __LINE__); dano.empty()){ mpre("ОШИБКА выборки исходного значения", __LINE__);
				}else if(boost::dynamic_bitset<> map = [&](boost::dynamic_bitset<> map = {}){ // Карта морфа
					//if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", dano.at("id")}}, {}, {}); false){ mpre("ОШИБКА выборки карты исходника", __LINE__);
					if(std::string dataset_map_id = "dano," +dano.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("ОШИБКА выборки карты исходника", __LINE__);
					}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета размера обучающей выборки", __LINE__);
					}else if(map = boost::dynamic_bitset<>(dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); (dataset_count != map.size())){ mpre("ОШИБКА расчета карты исходника", __LINE__);
					//}else if(map = boost::dynamic_bitset<>(dataset_map.at("map")); map.empty()){ mpre("ОШИБКА загрузки данных карты", __LINE__);
					}else{ //mpre("Карта морфа " +dataset_map.at("map"), __LINE__);
					}return map; }(); map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				//}else if(TMs index_1 = bmf::fk("index", {{"id", index.at("index_id")}}, {}, {}); false){ mpre("ОШИБКА выборки старшего потомка", __LINE__);
				}else if(TMs index_1 = bmf::Up(bmf::INDEX, {{"id", index.at("index_id")}}, {}, {}, __LINE__); false){ err("Выборка старшего потомка");
				}else if(boost::dynamic_bitset<> maps1 = [&](boost::dynamic_bitset<> maps1){ // Карта морфа
					if(index_1.empty()){ //mpre("Старшего морфа нет берем карту исходника", __LINE__);
					}else if(std::string dataset_map_id = "index," +index_1.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					//}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_1.at("id")}}; where.empty()){ mpre("ОШИБКА составления условия выборки карты", __LINE__);
					//}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					/*}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Выборка карты исходника
						if(dataset_map = bmf::fk("main.dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта найдена в оперативной БД", __LINE__);
						}else if(dataset_map = bmf::fk("file.dataset_map", where, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты в осноной БД", __LINE__);
						}else if(dataset_map.erase("id"); false){ mpre("Удаление идентификатора карты", __LINE__);
						}else if(dataset_map = bmf::fk("main.dataset_map", where, dataset_map, {}); dataset_map.empty()){ mpre("ОШИБКА сохранения краты в оперативную БД", __LINE__);
						}else{ mpre("Восстановление карты из оперативной БД", __LINE__);
						}return dataset_map; }(); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);*/
					}else if(maps1 = boost::dynamic_bitset<>(dataset_map.at("map")); maps1.empty()){ mpre("ОШИБКА загрузки данных карты", __LINE__);
					}else{ //mpre("Карта морфа " +dataset_map.at("map"), __LINE__);
					}return maps1; }(map); maps1.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				//}else if(TMs index_0 = bmf::fk("index", {{"id", index.at("bmf-index")}}, {}, {}); false){ mpre("ОШИБКА выборки младшего потомка", __LINE__);
				}else if(TMs index_0 = bmf::Up(bmf::INDEX, {{"id", index.at("bmf-index")}}, {}, {}, __LINE__); false){ err("Выборка младшего потомка");
				}else if(boost::dynamic_bitset<> maps0 = [&](boost::dynamic_bitset<> maps0){ // Карта морфа
					if(index_0.empty()){ //mpre("Старшего морфа нет берем карту исходника", __LINE__);
					//}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_0.at("id")}}; where.empty()){ mpre("ОШИБКА формирования условий на выборку карты", __LINE__);
					}else if(std::string dataset_map_id = "index," +index_0.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					//}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					/*}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Выборка карты исходника
						if(dataset_map = bmf::fk("main.dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта найдена в оперативной БД", __LINE__);
						}else if(dataset_map = bmf::fk("file.dataset_map", where, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты в осноной БД", __LINE__);
						}else if(dataset_map.erase("id"); false){ mpre("Удаление идентификатора карты", __LINE__);
						}else if(dataset_map = bmf::fk("main.dataset_map", where, dataset_map, {}); dataset_map.empty()){ mpre("ОШИБКА сохранения краты в оперативную БД", __LINE__);
						}else{ mpre("Восстановление карты из оперативной БД", __LINE__);
						}return dataset_map; }(); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);*/
					}else if(maps0 = boost::dynamic_bitset<>(dataset_map.at("map")); maps0.empty()){ mpre("ОШИБКА загрузки данных карты", __LINE__);
					}else{ //mpre("Карта морфа " +dataset_map.at("map"), __LINE__);
					}return maps0; }(map); maps0.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				}else if(int count_1 = (bmf::COUNT.end() == bmf::COUNT.find(index.at("index_id")) ? 0 : atoi(bmf::COUNT.at(index.at("index_id")).c_str())); (0 > count_1)){ mpre("ОШИБКА расчета количества веса старшей ветви", __LINE__);
				}else if(int count_0 = (bmf::COUNT.end() == bmf::COUNT.find(index.at("bmf-index")) ? 0 : atoi(bmf::COUNT.at(index.at("bmf-index")).c_str())); (0 > count_0)){ mpre("ОШИБКА расчета количества веса младшей ветви", __LINE__);
				}else if(bmf::COUNT[index.at("id")] = to_string(count_1 +count_0 +1); (bmf::COUNT.end() == bmf::COUNT.find(index.at("id")))){ mpre("ОШИБКА обновления счетчиков морфа", __LINE__);
				}else if(TMs _stairs = [&](TMs _stairs = {}){ // Выбор направления развития
					if(stairs.end() != stairs.find("val")){ //mpre("Пропуск выбора направления расчета", __LINE__);
					}else if(std::string directions = "11123322"; directions.empty()){ mpre("ОШИБКА установки маски перехода", __LINE__);
					}else if(int vd = (map.test(key) ? 1 : 0); (0 > vd)){ mpre("ОШИБКА расчета собственного значения", __LINE__);
					}else if(int v1 = (maps1.test(key) ? 1 : 0); (0 > v1)){ mpre("ОШИБКА расчета значения старшей связи", __LINE__);
					}else if(int v0 = (maps0.test(key) ? 1 : 0); (0 > v0)){ mpre("ОШИБКА расчета значения младшей связи", __LINE__);
					}else if(int pos = (v1 << 2) +(v0 << 1) +vd; (0 > pos)){ mpre("ОШИБКА расчета позиции сигнала", __LINE__);
					}else if(std::string direction = directions.substr(pos, 1); (1 != direction.length())){ mpre("ОШИБКА выборки направления из маски", __LINE__);
					}else if(int direct = atoi(direction.c_str()); (1 >direct |3 <direct)){ mpre("ОШИБКА выборки направления развития " +to_string(direct), __LINE__);
					}else if(std::string list = [&](std::string list = ""){ // Список использованных исходников
						if(list = stairs["list"]; false){ mpre("ОШИБКА выборки списка исходников", __LINE__);
						}else if(string cur = ","+ dano.at("id")+ ";"; cur.empty()){ mpre("ОШИБКА дополнения история текущим исходником", __LINE__);
						}else if([&](){ // Удаление элемента внутри списка
							if(std::size_t pos = list.find(cur); false){ mpre("ОШИБКА определения текущей позиции исходника в списке", __LINE__);
							}else if(string::npos == pos){ //mpre("ОШИБКА Элемент не найден в списке list=" +list +" cur=" +cur, __LINE__);
							}else if(list.erase(pos, cur.length()); false){ mpre("ОШИБКА удаления символа из строки", __LINE__);
							}else{ //mpre("Удаления повторяющегося исходника list=" +list +" cur=" +cur, __LINE__);
							} return false; }()){ mpre("ОШИБКА удаления элемента внутри списка", __LINE__);
						}else if(list +=cur; list.empty()){ mpre("ОШИБКА сохранения истории родителю", __LINE__);
						}else{ //mpre("Список исходников итоговый "+ list, __LINE__);
						}return list; }(); false){ mpre("ОШИБКА получения списка использованных исходников", __LINE__);
					}else if(TMMi _STAIRS = [&](TMMi _STAIRS = {}){ // Создание массива направлений перехода
						if(_STAIRS[2] = {{"index_id", index.at("index_id")}, {"parent", "index_id"}, {"other_id", index.at("bmf-index")}, {"list", list}}; _STAIRS.empty()){ mpre("ОШИБКА установки старшего прогноза", __LINE__);
						}else if(_STAIRS[1] = {{"index_id", index.at("bmf-index")}, {"parent", "bmf-index"}, {"other_id", index.at("index_id")}, {"list", list}}; _STAIRS.empty()){ mpre("ОШИБКА установки младшего прогноза", __LINE__);
						}else if(_STAIRS[3] = (count_1 < count_0 ? _STAIRS[2] : _STAIRS[1]); _STAIRS.empty()){ mpre("ОШИБКА установки частичного перехода", __LINE__);
						}else{ //mpre("Расчет частичного перехода count_1=" +to_string(count_1) +" count_0=" +to_string(count_0), __LINE__);
						}return _STAIRS; }(); _STAIRS.empty()){ mpre("ОШИБКА получения списка переходов", __LINE__);
					}else if(_STAIRS.end() == _STAIRS.find(direct)){ mpre("ОШИБКА выбран несуществующий вариант перехода", __LINE__);
					}else if(_stairs = _STAIRS.at(direct); false){ mpre("ОШИБКА выбора направления перехода direct=" +to_string(direct), __LINE__);
					}else{ //mpre("Выбор направления " +index.at("id") +" count_1=" +to_string(count_1) +" count_0=" +to_string(count_0) +" vd=" +to_string(vd) +" v1=" +to_string(v1) +" v0=" +to_string(v0) +" " +directions +"[" +to_string(pos) +"]=" +direction, __LINE__);
					}return _stairs; }(); false){ mpre("ОШИБКА выбора направления развития", __LINE__);
				}else if([&](){ // Проверка согласованности данных
					if(_stairs.empty()){ //mpre("Только в прямом направлении", __LINE__);
					}else if(stairs.end() == stairs.find("parent")){ mpre("ОШИБКА поле родителя не задано", __LINE__);
					}else if(boost::dynamic_bitset<> _maps1 = maps1; _maps1.empty()){ mpre("Создание копии старшей карты", __LINE__);
					}else if(boost::dynamic_bitset<> _maps0 = maps0; _maps0.empty()){ mpre("Создание копии младшей карты", __LINE__);
					}else if("index_id" == stairs.at("parent") ? _maps1.flip() : _maps0.flip(); false){ mpre("ОШИБКА инвертирования сигнала", __LINE__);
					}else if(boost::dynamic_bitset<> maps = ("index_id" == stairs.at("parent") ? _maps1&_maps0 : _maps1|_maps0); maps.empty()){ mpre("ОШИБКА расчета значения", __LINE__);
					}else if(std::string map = [&](std::string map = ""){ boost::to_string(maps, map); return map; }(); map.empty()){ mpre("ОШИБКА установки значения морфа", __LINE__);
					}else if(std::string dataset_map_id = "index," +index.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					//}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}, {}, {}); false){ mpre("ОШИБКА установки карты расчетов нового морфа", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета размера выборки", __LINE__);
					}else if(std::string _map = (dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); map.empty()){ mpre("ОШИБКА получения карты", __LINE__);
					}else if(map == _map){ //mpre("Данные согласованы " +map " != " +_map, __LINE__);
					}else if(std::string map_ = (25 >= map.length() ? map : map.substr(0, 25) +"..."); map_.empty()){ mpre("ОШИБКА получения короткой строки карты", __LINE__);
					}else if(std::string _map_ = (25 >= _map.length() ? _map : _map.substr(0, 25) +"..."); _map_.empty()){ mpre("ОШИБКА получения короткой строки карты", __LINE__);
					}else{ mpre("Данные не согласованы index_id=" +index.at("id") +" " +(dataset_map.end() == dataset_map.find("id") ? "" : dataset_map.at("id")) +" map=" +map_ +" " +std::to_string(map.length()) +" _map=" +_map_ +" " +std::to_string(_map.length()), __LINE__);
					}return false; }()){ mpre("ОШИБКА проверка согласованности данных", __LINE__);
				}else if([&](){ // Добавление нового морфа
					if(_stairs.end() == _stairs.find("index_id")){ //mpre("Поле морфа в направлении не найдено", __LINE__);
					}else if(std::string index_id = _stairs.at("index_id"); !index_id.empty()){ //mpre("Не пусой идентификатор морфа перехода не создаем новый", __LINE__);
					//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else if(TMs _dano = [&](TMs _dano = {}){ // Исходник нового морфа
						if("" == _stairs.at("other_id")){ _dano = bmf::Choice(index, stairs, _stairs, key); //mpre("Выбор лучшего источника");
						//}else if(_dano = erb(BMF_DANO_EX, {{"id", index.at("dano_id")}}); _dano.empty()){ mpre("ОШИБКА выбоки исходника смежного морфа", __LINE__);
						}else if(_dano = bmf::Up(bmf::DANO, {{"id", index.at("dano_id")}}, {}, {}, __LINE__); _dano.empty()){ mpre("ОШИБКА выборки исходника смежного морфа", __LINE__);
						}else{ //mpre(_dano, "Выборка исходника смежного морфа", __LINE__);
						} return _dano; }(); _dano.empty()){ mpre("ОШИБКА выборки нового морфа", __LINE__);
					}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
					//}else if(mpre(bmf::databases, "clump_id=" +clump_id, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else if(TMs _index = {/*{"clump_id", clump_id},*/ {"itog_values_id", index.at("itog_values_id")}, {"dano_id", _dano.at("id")}, {"itog_id", index.at("itog_id")}, {"index_id", ""}, {"bmf-index", ""}}; _index.empty()){ mpre("ОШИБКА создания нового морфа "+ index.at("id"), __LINE__);
					}else if(index.end() == index.find(_stairs.at("parent"))){ mpre("ОШИБКА у морфа поле родителя не найдено", __LINE__);
					}else if(_stairs.end() == _stairs.find("parent")){ mpre("ОШИБКА поле родителя не указано", __LINE__);
					//}else if(bmf::exec("BEGIN TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
					//}else if(_index = bmf::fk("index", {}, _index, {}); _index.empty()){ mpre("ОШИБКА добавления нового морфа в БД", __LINE__);
					}else if(_index = bmf::Up(bmf::INDEX, {}, _index, {}, __LINE__); _index.empty()){ mpre("ОШИБКА добавления нового морфа", __LINE__);
					}else if(index_1 = ("index_id" == _stairs.at("parent") ? _index : index_1); false){ mpre("ОШИБКА обновления старшего морфа", __LINE__);
					}else if(index_0 = ("bmf-index" == _stairs.at("parent") ? _index : index_0); false){ mpre("ОШИБКА обновления младшего морфа", __LINE__);
					}else if(TMs dataset_map = bmf::Maps(_index, _stairs.at("parent")); dataset_map.empty()){ mpre("ОШИБКА установки карт нового морфа", __LINE__);
					}else if(index.at(_stairs.at("parent")) = _index.at("id"); index.empty()){ mpre("ОШИБКА установки поля родителя нового морфа", __LINE__);
					//}else if(index = bmf::fk("index", {{"id", index.at("id")}}, {}, index); index.empty()){ mpre("ОШИБКА добавления морфа в БД", __LINE__);
					}else if(index = bmf::Up(bmf::INDEX, {{"id", index.at("id")}}, {}, index, __LINE__); index.empty()){ mpre("ОШИБКА обновления связей морфа родителя", __LINE__);
					//}else if(bmf::exec("COMMIT TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
					}else if([&](){ // Обновление счетчиков нового морфа и родителя
						if(bmf::COUNT[_index.at("id")] = "1"; (bmf::COUNT.end() == bmf::COUNT.find(_index.at("id")))){ mpre("ОШИБКА установки четчика нового морфа", __LINE__);
						}else if(bmf::COUNT.end() == bmf::COUNT.find(index.at("id"))){ mpre("ОШИБКА счетчик морфа не задан", __LINE__);
						}else if(int count = atoi(bmf::COUNT.at(index.at("id")).c_str()); !count){ mpre("ОШИБКА выборки счетчика морфа", __LINE__);
						}else if(bmf::COUNT[index.at("id")] = to_string(count +1); (bmf::COUNT.end() == bmf::COUNT.find(index.at("id")))){ mpre("ОШИБКА обновления счетчика родителя", __LINE__);
						}else{ //mpre("Обновления счетчика нового морфа и родителя", __LINE__);
						} return false; }()){ mpre("ОШИБКА установки счетчиков", __LINE__);
					}else{ //mpre(_index, "Добавление нового морфа " +index.at("id") +" > " +_index.at("id"), __LINE__);
					} return false; }()){ mpre("ОШИБКА Добавление морфа", __LINE__);
				}else if([&](){ // Расчет значения
					if(TMs dataset_map = bmf::Maps(index, stairs.at("parent")); dataset_map.empty()){ mpre("ОШИБКА расчета значений наборов данных", __LINE__);
					}else if(boost::dynamic_bitset<> maps(dataset_map.at("map")); maps.empty()){ mpre("ОШИБКА получения карты текущего морфа", __LINE__);
					}else if(stairs_itr->second["val"] = stairs["val"] = (maps.test(key) ? "1" : "0"); (1 != stairs.at("val").length())){ mpre("ОШИБКА установки значения текущей ступени", __LINE__);
					}else{ //mpre("Значение текущей ступени "+ stairs.at("val"), __LINE__);
					}return (stairs.end() == stairs.find("val")); }()){ mpre("ОШИБКА расчета значения текущей ступени", __LINE__);
				}else if([&](){ // Изменение размера лестницы
					if([&](){ // Добавление новой ступени
						if(_stairs.end() == _stairs.find("index_id")){ //mpre("Направление не задано", __LINE__);
						}else if(_stairs.at("index_id").empty()){ //mpre("Ступень для пустого морфа не добавляем", __LINE__);
						}else if(STAIRS.insert(make_pair(STAIRS.size(), _stairs)); STAIRS.empty()){ mpre("ОШИБКА добавления ступени в лестницу", __LINE__);
						}else{ //mpre("Добавление новой ступени " +_stairs.at("index_id"), __LINE__);
						}return (size < STAIRS.size()); }()){ //mpre("Увеличиваем лестницу", __LINE__);
					}else if([&](){ // Сохранение результата расчета лестницы
						if(1 < STAIRS.size()){ //mpre("Не последняя ступень", __LINE__);
						}else if(stair = STAIRS.rbegin()->second; stair.empty()){ mpre("ОШИБКА сохранения результата расчета лестницы", __LINE__);
						}else{ //mpre(stair, "Результат лестницы", __LINE__);
						}return false; }()){ mpre("ОШИБКА Сохранение результата расчеты лестницы", __LINE__);
					}else if(STAIRS.erase(stairs_itr->first); false){ mpre("ОШИБКА удаления ступени из лестницы", __LINE__);
					}else{ //mpre("Уменьшение размера лестницы", __LINE__);
					}return (size == STAIRS.size()); }()){ mpre("ОШИБКА изменения размеров очереди", __LINE__);
				}else if(0 > --loop){ mpre("ОШИБКА максимальное количество циклов", __LINE__);
				}else if(0 >= STAIRS.size()){ //mpre("Конец лестницы", __LINE__);
				}else if(pass = true; !pass){ mpre("Продолжаем выполнять цикл", __LINE__);
				}else{ //mpre("Итерация "+ to_string(loop), __LINE__);
				}}while(!(pass = !pass)); }(); STAIRS.size()){ mpre("ОШИБКА получения результатов расчета", __LINE__);
			}else if(string revision = [&](string revision = ""){ // Получение строки изменений
				if(stair.end() == stair.find("revision")){ //mpre("Изменения результата не указаны", __LINE__);
				}else if(revision = stair.at("revision"); false){ mpre("ОШИБКА получения строки изменений", __LINE__);
				}else{ //mpre("Строка изменений revision=" +revision, __LINE__);
				}return revision; }(); false){ mpre("ОШИБКА получения изменений", __LINE__);
			}else if([&](){ // Расчет результата
				if(stair.end() == stair.find("val")){ mpre("ОШИБКА получения результата лестницы val", __LINE__);
				}else if(stair["result"] = revision+ stair.at("val"); (0 >= stair.at("result").length())){ mpre("ОШИБКА формирования результата с изменениями", __LINE__);
				}else{ //mpre(stair, __LINE__, "Результат с изменениями _val="+ stair.at("result"));
				}return stair.end() == stair.find("result"); }()){ mpre("ОШИБКА расчета значения морфа "+ to_string(rep)+ " "+ bmf_index["id"], __LINE__);
			}else if(stair.end() == stair.find("result")){ mpre("ОШИБКА результат расчетов не найден", __LINE__);
			}else{ //mpre(stair, __LINE__, "Результаты вычислений"); //mpre("Количество итераций "+ to_string(rep), __LINE__);
			}return stair["result"]; }); false){ mpre("ОШИБКА создания функции обучения", __LINE__);
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
				}else if(index["val"] = [&](std::string calc = ""){ // Быстрая проверка
					if(std::string calculate = (bmf::CALCULATE.end() == bmf::CALCULATE.find(itog.at("id")) ? "" : bmf::CALCULATE.at(itog.at("id"))); false){ //pre("Удаленная ранее формула");
					}else if(calculate = ("" == calculate ? bmf::Calculate(index) : calculate); calculate.empty()){ err("Расчет формулы калькуляции");
					}else if(bmf::CALCULATE[itog.at("id")] = calculate; bmf::CALCULATE.empty()){ err("Сохранение значений калькуляции");
					}else if(calc = bmf::Calc(calculate, BMF_DANO_EX); (1 != calc.length())){ mpre("ОШИБКА Расчета Калькуляции " +calc +" " +calculate, __LINE__);
					}else{ //mpre("Быстрая проверка calculate="+ calculate, __LINE__);
					} return calc; }(); (1 != index.at("val").length())){ err("Расчет формулы"); //mpre("Пропуск быстрой проверки calc=" +calc +" itog.at(val)=" +itog.at("val"), __LINE__);
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
		}else if( bmf::Progress = [&](string mess, float val, int line, int pos = 0){
				fprintf(stderr, "\r%d.[", line);
				for(pos=0; pos<val*20; pos++) fprintf(stderr, "%s", string("#").c_str());
				for(;pos<20; pos++) fprintf(stderr, "%s", string(" ").c_str());
				fprintf(stderr, "] (%.1f %%) %s\r", val*100, mess.c_str());
				std::cerr.flush();
			}; false){ mpre("ОШИБКА создания функции отображения прогресса", __LINE__);
		}else if(bmf::Maps = ([&](TMs index ,std::string parent, TMs _dataset_map_ = {}){ // Расчет карт наборов данных
			//if(BMF_DATASET_EX.end() == BMF_DATASET_EX.find("")){ mpre("ОШИБКА список обучающих выборок не установлен", __LINE__);
			if([&](){ for(auto dataset_itr:bmf::List(bmf::DATASET, {}, __LINE__)){ // Расчет списка набора данных
				if(TMs dataset = dataset_itr.second; dataset.empty()){ mpre("ОШИБКА выборки набора данных", __LINE__);
				/*}else if([&](bool skip = false){ // Пропуск расчета итога
					if(bmf::ARGV.end() == bmf::ARGV.find("itog")){ //mpre("Итог не указан", __LINE__);
					}else if(std::string itog_id = bmf::ARGV.at("itog"); itog_id.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
					}else if(bmf::ARGV.end() == bmf::ARGV.find("ds")){ mpre("ОШИБКА не указан набор данных", __LINE__);
					}else if(bmf::ARGV.at("ds") == dataset.at("id")){ //mpre("Указан итог и совпадение", __LINE__);
					}else if(skip = !skip; false){ mpre("ОШИБКА Инкремент пропуска", __LINE__);
					}else{ //mpre("Условие пропуска сработало", __LINE__);
					}return skip; }()){ //mpre("Пропускаем расчет итога " +dataset.at("id"), __LINE__);*/
				}else if(int dataset_count = atoi(dataset.end() == dataset.find("count") ? "0" : dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в выборке", __LINE__);
				//}else if(int dataset_count = atoi(dataset.end() == dataset.find("count") ? "0" : dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в выборке", __LINE__);
				}else if(boost::dynamic_bitset<> _mp = [&](boost::dynamic_bitset<> _mp = {}){ // Карта исходного значения
					if(false){ mpre("Пропуск", __LINE__);
					//}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "dano"}, {"alias_id", index.at("dano_id")}}, {}, {}); false){ mpre("ОШИБКА выборки карты расчетов исходников", __LINE__);
					}else if(std::string dataset_map_id = "dano," +index.at("dano_id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ err("Выборка карты исходника");
					}else if(_mp = boost::dynamic_bitset<>(dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); (dataset_count != _mp.size())){ mpre("ОШИБКА расчета карты исходника", __LINE__);
					}else{ //mpre("Расчет карты исодника", __LINE__);
					}return _mp; }(); (dataset_count != _mp.size())){ mpre("ОШИБКА выборки исходного значения морфа", __LINE__);
				}else if(boost::dynamic_bitset<> mp1 = [&](boost::dynamic_bitset<> mp1){ // Выборка карты основного морфа
					if(index.end() == index.find("index_id")){ mpre("ОШИБКА Поле старшего морфа не задано", __LINE__);
					}else if(std::string index_id = index.at("index_id"); index_id.empty()){ //mpre("Идентификатор старшего морфа не установлен", __LINE__);
					//}else if(TMs where = {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", index_id}}; where.empty()){ mpre("ОШИБКА составления условий выборки", __LINE__);
					}else if(std::string dataset_map_id = "index," +index_id +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					//}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки карты", __LINE__); mpre("ОШИБКА выборки карты расчетов", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ err("Выборка карты старшего потомка");
					/*}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Выборка карты исходника
						if(dataset_map = bmf::fk("main.dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта найдена в оперативной БД", __LINE__);
						}else if(dataset_map = bmf::fk("file.dataset_map", where, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты в осноной БД", __LINE__);
						}else if(dataset_map.erase("id"); false){ mpre("Удаление идентификатора карты", __LINE__);
						}else if(dataset_map = bmf::fk("main.dataset_map", where, dataset_map, {}); dataset_map.empty()){ mpre("ОШИБКА сохранения краты в оперативную БД", __LINE__);
						}else{ mpre("Восстановление карты из оперативной БД", __LINE__);
						}return dataset_map; }(); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);*/
					}else if(dataset_map.empty()){ mpre("Берем значение равное исходнику ds=" +dataset.at("id") +" index_id=" +index.at("id"), __LINE__);
					}else if(mp1 = boost::dynamic_bitset<>(dataset_map.at("map")); mp1.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
					}else{ //mpre("Выборка карты морфа", __LINE__);
					}return mp1; }(_mp); (dataset_count != mp1.size())){ mpre("ОШИБКА выборки карты основного морфа", __LINE__);
				}else if(boost::dynamic_bitset<> mp0 = [&](boost::dynamic_bitset<> mp0){ // Выборка карты основного морфа
					if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА Поле младшего морфа не задано", __LINE__);
					}else if(std::string bmf_index = index.at("bmf-index"); bmf_index.empty()){ //mpre("Идентификатор младшего морфа не установлен", __LINE__);
					//}else if(TMs where = {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", bmf_index}}; where.empty()){ mpre("ОШИБКА составления условий выборки", __LINE__);
					}else if(std::string dataset_map_id = "index," +bmf_index +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
					//}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки карты", __LINE__); mpre("ОШИБКА выборки карты расчетов", __LINE__);
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ err("Выборка карты младшего потомка");
					/*}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Выборка карты исходника
						if(dataset_map = bmf::fk("main.dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта найдена в оперативной БД", __LINE__);
						}else if(dataset_map = bmf::fk("file.dataset_map", where, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты в осноной БД", __LINE__);
						}else if(dataset_map.erase("id"); false){ mpre("Удаление идентификатора карты", __LINE__);
						}else if(dataset_map = bmf::fk("main.dataset_map", where, dataset_map, {}); dataset_map.empty()){ mpre("ОШИБКА сохранения краты в оперативную БД", __LINE__);
						}else{ mpre("Восстановление карты из оперативной БД", __LINE__);
						}return dataset_map; }(); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);*/
					}else if(dataset_map.empty()){ //mpre("Берем значение равное исходнику ds=" +dataset.at("id") +" bmf_index=" +index.at("id"), __LINE__);
					}else if(mp0 = boost::dynamic_bitset<>(dataset_map.at("map")); mp0.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
					}else{ //mpre("Выборка карты морфа", __LINE__);
					}return mp0; }(_mp); (dataset_count != mp0.size())){ mpre("ОШИБКА выборки карты основного морфа", __LINE__);
				}else if("index_id" == parent ? mp1.flip() : mp0.flip(); false){ mpre("ОШИБКА инвертирования сигнала", __LINE__);
				}else if(boost::dynamic_bitset<> mps = ("index_id" == parent ? mp1&mp0 : mp1|mp0); mps.empty()){ mpre("ОШИБКА расчета значения", __LINE__);
				}else if(std::string _map = [&](std::string map = ""){ boost::to_string(mps, map); return map; }(); _map.empty()){ mpre("ОШИБКА установки значения морфа", __LINE__);
				}else if(index.end() == index.find("itog_id")){ mpre("ОШИБКА поле итога не найдено у морфа", __LINE__);
				}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
				}else if(TMs _dataset_map = {/*{"clump_id", clump_id},*/ {"itog_id", index.at("itog_id")}, {"map", _map}}; _dataset_map.empty()){ mpre("ОШИБКА устанвоки карты", __LINE__);
				}else if(std::string dataset_map_id = "index," +index.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
				}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map, __LINE__); dataset_map.empty()){ err("Выборка карты младшего потомка");
				//}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}, _dataset_map, _dataset_map); dataset_map.empty()){ mpre("ОШИБКА установки карты расчетов", __LINE__);
				}else if(bmf::dataset.at("id") != dataset.at("id")){ //mpre("Не возвращаем результатом " +bmf::dataset.at("id") +" " +dataset.at("id"), __LINE__);
				}else if(_dataset_map_ = dataset_map; _dataset_map_.empty()){ mpre("ОШИБКА установки значения возврата", __LINE__);
				}else{ //mpre("Расчет значения морфа ds=" +dataset.at("id") +" index_id=" +index.at("id"), __LINE__);
				}}return false; }()){ mpre("Расчет значения морфа", __LINE__);
			}else if(_dataset_map_.empty()){ mpre("ОШИБКА карта текущего набора данных не задана", __LINE__);
			}return _dataset_map_; }); false){ mpre("ОШИБКА расчета наборов данных", __LINE__);
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
		//}else if(bmf::Open(1)){ mpre("ОШИБКА подключения к БД", __LINE__);
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
		}else if([&](){ // Установка набора данных
			if(std::string _md5 = md5(bmf::ARGV.at("-")); _md5.empty()){ mpre("ОШИБКА расчета хеша набора обучающих данных", __LINE__);
			}else if(std::string dataset_id = _md5; dataset_id.empty()){ mpre("ОШИБКА установки идентификатора набора данных", __LINE__);
			}else if(TMs _dataset = {{"count", to_string(in.size())}}; _dataset.empty()){ mpre("ОШИБКА создания записи нового набора данных", __LINE__);
			}else if((bmf::dataset = bmf::Up(bmf::DATASET, {{"id", dataset_id}}, _dataset, _dataset, __LINE__)).empty()){ mpre("ОШИБКА добавления набора данных", __LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("id")){ mpre("ОШИБКА набор данных не содержит поля идентификатора", __LINE__);
			}else{ mpre("Набор данных " +bmf::dataset.at("id"), __LINE__);
			}return bmf::dataset.empty(); }()){ mpre("ОШИБКА устанвоки набора данных", __LINE__);
		}else if(TMMs _DANO = bmf::List(bmf::DANO, {}, __LINE__); _DANO.empty()){ err("Список исходников");
		}else if([&](int progress = 0){ for(auto dano_itr:_DANO){ // Двоичная карта исходников
			if(bmf::Progress("Индекс исходных сигналов " +to_string(progress), (float)++progress/_DANO.size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(TMs dano = dano_itr.second; dano.empty()){ mpre("ОШИБКА получения исходника", __LINE__);
			}else if(std::string dano_id = dano_itr.second.at("id"); dano_id.empty()){ mpre("ОШИБКА получения идентификатора дано", __LINE__);
			}else if(boost::dynamic_bitset<> bit(in.size()); false){ mpre("ОШИБКА создания бита исходника", __LINE__);
			}else if([&](){ for(auto &js:in.items()){ // Количество обучающих примеров для исходника
				if(int key = atoi(js.key().c_str()); (0 > key)){ mpre("ОШИБКА расчета номера обучающего примера", __LINE__);
				}else if(DANO.end() == DANO.find(key)){ mpre("ОШИБКА расчет примера не найден", __LINE__);
				}else if(std::string val = (DANO.at(key).end() == DANO.at(key).find(dano_id) ? "0" : DANO.at(key).at(dano_id)); (1 != val.length())){ mpre("ОШИБКА выборки значения дано", __LINE__);
				}else if(bit[key] = atoi(val.c_str()); false){ mpre("ОШИБКА установки бита", __LINE__);
				}else{ //mpre(_dano, "Дано " +val, __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения обучающих примеров", __LINE__);
			}else if(std::string map = [&](std::string map = ""){ boost::to_string(bit, map); return map; }(); map.empty()){ mpre("ОШИБКА установки карты исходника", __LINE__);
			}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
			}else if(std::string dataset_map_id = "dano," +dano.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
			}else if(TMs _dataset_map = {{"id", dataset_map_id}, /*{"clump_id", clump_id},*/ {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА создания карты набора данных", __LINE__);
			}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map, __LINE__); dataset_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
			}else{ //mpre("Установка карты морфа "+ dataset_map_id, __LINE__);//mpre("Исходник " +dano_itr.second.at("id") +" " +dano_itr.second.at("map"), __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА составления двоичной карты", __LINE__);
		}else if(TMMs _ITOG = bmf::List(bmf::ITOG, {}, __LINE__); _ITOG.empty()){ err("Список итогов");
		}else if([&](int progress = 0){ for(auto& itog_itr:_ITOG){ // Двоичная карта итогов
			if(bmf::Progress("Индекс итоговых сигналов " +to_string(progress), (float)++progress/_ITOG.size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
			/*}else if([&](bool pass = false){ // Пропуск расчета по условию -itog
				if(itog_itr.second.end() == itog_itr.second.find("id")){ mpre("ОШИБКА поле идентификатора у итога не найдено", __LINE__);
				}else if(bmf::ARGV.end() == bmf::ARGV.find("itog")){ //mpre("Условие не установлено", __LINE__);
				}else if(std::string itog = bmf::ARGV.at("itog"); itog.empty()){ mpre("ОШИБКА итог не указан", __LINE__);
				}else if(pass = (itog_itr.second.at("id") == itog); false){ mpre("ОШИБКА расчета пропуска по номеру итога", __LINE__);
				}else if(int npos = itog.find("."); (std::string::npos == npos)){ mpre("Делитель не найден", __LINE__);
				}else if(int divider = atoi((std::string::npos == npos) ? itog.c_str() : itog.substr(0, npos).c_str()); !divider){ mpre("ОШИБКА получения делителя " +itog, __LINE__);
				}else if(int remains = atoi((std::string::npos == npos) ? string("0").c_str() : itog.substr(npos+1, itog.length()).c_str()); (0 > remains)){ mpre("ОШИБКА получения остатка", __LINE__);
				}else if(int id = atoi(itog_itr.second.at("id").c_str()); (0 == id)){ mpre(itog_itr.second, "Итог", __LINE__); mpre("ОШИБКА расчета идентификатора итога", __LINE__);
				}else if(pass = (id%divider != remains); false){ mpre("ОШИБКА расчета условия выхода по делителю с остатком", __LINE__);
				}else{ //mpre("Условие выхода по делителю с остатком id=" +to_string(id) +" divider=" +to_string(divider) +" remains=" +to_string(remains) +" " +(pass ? "Пропуск" : "Расчет"), __LINE__);
				}return pass; }()){ //mpre("Пропуск расчета itog_id=`" +itog_itr.second.at("id") +"`", __LINE__);*/
			}else if(std::string itog_id = itog_itr.second.at("id"); itog_id.empty()){ mpre("ОШИБКА получения идентификатора итога", __LINE__);
			}else if(boost::dynamic_bitset<> bit(in.size()); false){ mpre("ОШИБКА создания бита исходника", __LINE__);
			}else if([&](){ for(auto& js:in.items()){ // Количество обучающих примеров для исходника
				if(int key = atoi(js.key().c_str()); (0 > key)){ mpre("ОШИБКА расчета номера обучающего примера", __LINE__);
				}else if(ITOG.end() == ITOG.find(key)){ mpre("ОШИБКА расчет примера не найден", __LINE__);
				}else if(std::string val = (ITOG.at(key).end() == ITOG.at(key).find(itog_id) ? "0" : ITOG.at(key).at(itog_id)); (1 != val.length())){ mpre("ОШИБКА выборки значения дано", __LINE__);
				}else if(bit[key] = atoi(val.c_str()); false){ mpre("ОШИБКА установки бита", __LINE__);
				}else{ //mpre(_dano, "Дано " +val, __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения обучающих примеров", __LINE__);
			}else if(std::string map = [&](std::string map = ""){ boost::to_string(bit, map); return map; }(); map.empty()){ mpre("ОШИБКА установки карты итога", __LINE__);
			}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
			}else if(std::string dataset_map_id = "itog," +itog.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
			}else if(TMs _dataset_map = {/*{"clump_id", clump_id},*/ {"itog_id", itog.at("id")}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА создания карты набора данных", __LINE__);
			}else if(bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map, __LINE__).empty()){ mpre("ОШИБКА обновления набора данных", __LINE__);
			}else{ //mpre("Итог " +itog_itr.second.at("id") +" " +itog_itr.second.at("map"), __LINE__); //mpre(dano_itr.second, "Исходник", __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА составления двоичной карты", __LINE__);
		}else if(TMMs _ITOG = bmf::List(bmf::ITOG, {}, __LINE__); _ITOG.empty()){ err("Список итогов");
		}else if([&](int progress = 0){ for(auto itog_itr:_ITOG){ // Расчет списка карт
			if(bmf::Progress("Расчет значений " +to_string(progress), (float)++progress/_ITOG.size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(itog_itr.second.end() == itog_itr.second.find("index_id")){ //mpre("Не расчитываем вновь сохданные значения " +itog.at("id"), __LINE__);
			}else if(boost::dynamic_bitset<> maps = ([&](boost::dynamic_bitset<> maps = {}){ // Функция строки расчета карты морфов
				if(false){ mpre("Пропуск расчетов", __LINE__);
				//}else if(TMs bmf_index = bmf::fk("index", {{"id", itog_itr.second["index_id"]}}, {}, {}); bmf_index.empty()){ //mpre("Пустой морф итога", __LINE__);
				}else if(itog_itr.second.end() == itog_itr.second.find("index_id")){ mpre("ОШИБКА поле корневого морфа у итога не найдено", __LINE__);
				//}else if(TMs bmf_index = (*bmf::INDEX)[itog_itr.second.at("index_id")]; bmf_index.empty()){ mpre("ОШИБКА выборки корневого морфа", __LINE__);
				}else if(TMs bmf_index = bmf::Up(bmf::INDEX, {{"id", itog_itr.second.at("index_id")}}, {}, {}, __LINE__); bmf_index.empty()){ //mpre("Корневой морф не установлен", __LINE__);
				}else if(static TMMi STAIRS; !STAIRS.empty()){ mpre("Создание статической лестницы", __LINE__);
				}else if([&](){ // Заполнение лестницы первоначальным значением
					if(false){ pre("Пропуск первоначального заполнения лестницы");
					}else if(bmf_index.end() == bmf_index.find("id")){ err("Идентификатор морфа не указан");
					}else if(TMs stairs = {{"index_id", bmf_index.at("id")}, {"parent", "bmf-index"}}; stairs.empty()){ err("Получение первоначальной ступени");
					}else if(STAIRS.insert(make_pair(0, stairs)); STAIRS.empty()){ err("Добавление ступени в лестницу");
					}else{ //pre("Первоначальное заполнение лестницы");
					}return false; }()){ err("Создание первоначальной ступени");
				}else if([&](){ do{ // Перебор дерева
					if(false){ pre("Пропуск расчета лестницы");
					}else if(1 > STAIRS.size()){ err("Пустая лестница");
					}else if(auto stairs_itr = STAIRS.rbegin(); (STAIRS.rend() == stairs_itr)){ err("Выборка итаратора ступени");
					}else if(TMs stairs = stairs_itr->second; stairs.empty()){ err("Выборка ступени");
					}else if(stairs.end() == stairs.find("index_id")){ err("ОШИБКА У ступени не найдено поле идентификатора морфа");
					}else if(TMs index = bmf::Up(bmf::INDEX, {{"id", stairs.at("index_id")}}, {}, {}, __LINE__); index.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(std::string map = [&](std::string map = {}){ // Выборка карты морфа
						if(index.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
						}else if(index.end() == index.find("dano_id")){ mpre("ОШИБКА не установлен идентификатор исходника", __LINE__);
						}else if(std::string dataset_map_id = "dano," +index.at("dano_id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
						}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
						}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА не найдено поле карты", __LINE__);
						}else if(map = dataset_map.at("map"); map.empty()){ mpre("ОШИБКА пустая карта исходника", __LINE__);
						}else{ //mpre("Карта исходника " +index.at("dano_id") +" " +map, __LINE__);
						}return map; }(); map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					}else if(TMs index_1 = bmf::Up(bmf::INDEX, {{"id", index.at("index_id")}}, {}, {}, __LINE__); false){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs index_0 = bmf::Up(bmf::INDEX, {{"id", index.at("bmf-index")}}, {}, {}, __LINE__); false){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(std::string field = [&](std::string field = ""){ // Направление расчета
						if(std::string _field = (stairs.end() == stairs.find("field") ? "" : stairs.at("field")); false){ pre("Определение старого направления");
						}else if(TMs fields = {{"", "index_id"}, {"index_id", "bmf-index"}, {"bmf-index", ""}}; fields.empty()){ err("Установка списка переходов");
						}else if(fields.end() == fields.find(_field)){ err("Значение перехода не задано");
						}else if(field = fields.at(_field); false){ err("Определение нового направления");
						}else if(stairs_itr->second["field"] = stairs["field"] = field; false){ err("Установка направления в ступень");
						}else{ //mpre("Установка направления " +field, __LINE__);
						} return field; }(); false){ err("Установка направления расчета");
					}else if(std::string index_id = [&](std::string bmf_index_id = ""){ // Расчет значения
						if(bmf_index_id = (index.end() == index.find(field) ? "" : index.at(field)); false){ err("Выборка поля");
						}else if(!field.empty()){ //mpre("Не расчитываем ступень "+ index.at("id") +" " +field, __LINE__);
						}else if(index.end() == index.find("bmf-index")){ err("Поле младшей связи у морфа не найдено");
						}else if(std::string map_1 = [&](std::string map_1){ // Выборка карты морфа
							if(index_1.empty()){ //mpre("ОШИБКА морф не найден", __LINE__);
							}else if(index_1.end() == index_1.find("id")){ mpre("ОШИБКА не установлен идентификатор исходника", __LINE__);
							}else if(std::string dataset_map_id = "index," +index_1.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
							}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
							}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА не найдено поле карты", __LINE__);
							}else if(map_1 = dataset_map.at("map"); map_1.empty()){ mpre("ОШИБКА пустая карта исходника", __LINE__);
							}else{ //mpre("Карта исходника " +index.at("dano_id") +" " +map, __LINE__);
							}return map_1; }(map); map_1.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
						}else if(std::string map_0 = [&](std::string map_0){ // Выборка карты морфа
							if(index_0.empty()){ //mpre("ОШИБКА морф не найден", __LINE__);
							}else if(index_0.end() == index_0.find("id")){ mpre("ОШИБКА не установлен идентификатор исходника", __LINE__);
							}else if(std::string dataset_map_id = "index," +index_0.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
							}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
							}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА не найдено поле карты", __LINE__);
							}else if(map_0 = dataset_map.at("map"); map_0.empty()){ mpre("ОШИБКА пустая карта исходника", __LINE__);
							}else{ //mpre("Карта исходника " +index.at("dano_id") +" " +map, __LINE__);
							}return map_0; }(map); map_0.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
						}else if(boost::dynamic_bitset<> maps0(index_0.empty() ? map : map_0); maps0.empty()){ err("Получение первого значения");
						}else if(boost::dynamic_bitset<> maps1(index_1.empty() ? map : map_1); maps1.empty()){ err("Получение первого значения");
						}else if(stairs.end() == stairs.find("parent")){ err("Поле родителя не найдено в ступени");
						}else if(std::string parent = stairs.at("parent"); parent.empty()){ err("Выборка родительского поля");
						}else if(maps1 = ("index_id" == parent ? maps1.flip() : maps1); maps1.empty()){ err("Инвертирование старшей связи");
						}else if(maps0 = ("index_id" == parent ? maps0 : maps0.flip()); maps0.empty()){ err("Инвертирование младшей связи");
						}else if(boost::dynamic_bitset<> maps = ("index_id" == parent ? maps1&maps0 : maps1|maps0); maps.empty()){ err("Расчет результата");
						}else if(TMs _index = index; _index.empty()){ mpre("ОШИБКА клонирования морфа", __LINE__);
						}else if(std::string map = [&](std::string map = ""){ boost::to_string(maps, map); return map; }(); map.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
						}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
						}else if(TMs _dataset_map = {/*{"clump_id", clump_id},*/ {"itog_id", itog_itr.second.at("id")}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА устанвоки карты", __LINE__);
						}else if(std::string dataset_map_id = "index," +index.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
						}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map, __LINE__); dataset_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
						}else if([&](){ // Расчет размера веток
							if(int count_1 = (bmf::COUNT.end() == bmf::COUNT.find(index.at("index_id")) ? 0 : atoi(bmf::COUNT.at(index.at("index_id")).c_str())); 0 > count_1){ mpre("ОШИБКА расчета рамера старшей ветки", __LINE__);
							}else if(int count_0 = (bmf::COUNT.end() == bmf::COUNT.find(index.at("bmf-index")) ? 0 : atoi(bmf::COUNT.at(index.at("bmf-index")).c_str())); 0 > count_1){ mpre("ОШИБКА расчета рамера старшей ветки", __LINE__);
							}else if(std::string count = bmf::COUNT[index.at("id")] = to_string(count_1 +count_0 +1); (bmf::COUNT.end() == bmf::COUNT.find(index.at("id")))){ mpre("ОШИБКА расчета общего веса морфа вместе с ветками", __LINE__);
							}else{ //mpre("Размер морфа с ветками " +count, __LINE__);
							}return false; }()){ mpre("ОШИБКА расчета размера веток", __LINE__);
						}else{ //mpre("Расчет значения ветвей parent=" +parent +" " +index.at("id") +" map=" +index["map"], __LINE__); //mpre("ОШИБКА Расчет ступени " +index.at("id"), __LINE__);
						}return bmf_index_id; }(); false){ err("Расчет имени поля");
					}else if(TMs _stairs = [&](TMs _stairs = {}){ // Добавление ступени
						if(index_id.empty()){ //pre("Нет морфа для добавления");
						}else if(_stairs = {{"index_id", index_id}, {"parent", field}}; _stairs.empty()){ err("Создание следующей ступени");
						}else if(STAIRS.insert(make_pair(STAIRS.size(), _stairs)); STAIRS.empty()){ err("Добавление новой ступени");
						}else{ //mpre("Добавление новой ступени " +index.at("id") +" > " +index_id, __LINE__);
						} return _stairs; }(); false){ err("Добавление ступени");
					}else if([&](){ // Удаление ступени
						if(!_stairs.empty()){ //pre("Уже добавлена нижестоящая");
						}else if(!field.empty()){ //pre("Не удаляем старшее направление");
						}else if(STAIRS.erase(STAIRS.size()-1); STAIRS.empty()){ //err("Удаление последней ступени");
						}else{ //mpre("Удаление ступени " +index.at("id"), __LINE__);
						} return false; }()){ err("Удаление ступени");
					}else{ //mpre("Расчет лестницы "+ std::to_string(loop), __LINE__);
					}}while(0 < STAIRS.size()); return false; }()){ err("Перебор всех элементов дерева для составления строки расчетов");
				}else{ //Tree(bmf_index, BMF_INDEX_EX); mpre("Расчет строки польской нотации " +calculate, __LINE__);
				}return maps; }()); false){ err("Создание функции расчета польской нотации");
			}else{ //mpre("Расчет значений итога "+ itog_itr.second.at("id"), __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА расчета карты модели", __LINE__);
		//}else if(bmf::Close(0)){ mpre("ОШИБКА закрытия БД", __LINE__);
		}else{ //mpre(DANO, "Расчет итогов", __LINE__);
		}return false; }()){ mpre("ОШИБКА расчета исходников", __LINE__);
	}else if(bmf::dataset = [&](){ // Выборка набора данных для расчета
		if(false){ mpre("Пропуск определения набора данных", __LINE__);
		}else if(bmf::ARGV.end() != bmf::ARGV.find("dano")){ //mpre("Расчетный пример", __LINE__);
		}else if(std::string ds = (bmf::ARGV.end() == bmf::ARGV.find("ds") ? "" : bmf::ARGV.at("ds")); ds.empty()){ //mpre("ОШИБКА выборки идентификатора набора данных", __LINE__);
		}else if(!(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", ds}}, {}, {}, __LINE__)).empty()){ //mpre({{"id", ds}}, "Условия выборки набора данных " +to_string((*bmf::DATASET).size()), __LINE__); mpre("ОШИБКА выборки набора данных", __LINE__);
		}else if(bmf::ARGV.end() != bmf::ARGV.find("-")){ //mpre("Не выводим статистику на загузке набора данных", __LINE__);
		}else if(!ds.empty()){ mpre("Набора данных не найден в базе "+ ds, __LINE__);
		}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> ITOG; false){ mpre("Справочник карт", __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> INDEX; false){ mpre("Справочник карт", __LINE__);
		}else if([&](){ for(auto& dataset_itr:bmf::List(bmf::DATASET, {}, __LINE__)){ // Вывод списка набора данных
			if(TMs dataset = dataset_itr.second; dataset.empty()){ mpre("ОШИБКА выборки набора данных", __LINE__);
			}else if(dataset.end() == dataset.find("count")){ mpre("ОШИБКА поле количества примеров не установлено", __LINE__);
			}else if(int dataset_count = atoi(dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в наборе данных", __LINE__);
			}else if(float diff = [&](int ers = 0, int count = 0){ for(auto itog_itr:BMF_ITOG_EX.at("")){ // Расчет результата поитогово
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
				//}else if(TMs itog_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "itog"}, {"alias_id", itog.at("id")}}, {}, {}); itog_map.empty()){ //mpre(TMs({{"dataset_id", dataset.at("id")}, {"alias", "itog"}, {"alias_id", itog.at("id")}}), "Условия", __LINE__); //mpre("ОШИБКА выборки расчета итога", __LINE__);
				}else if(std::string dataset_map_id = "itog," +itog.at("id") +"," +dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
				}else if(TMs itog_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); itog_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
				}else if(boost::dynamic_bitset<> gmap(itog_map.at("map")); gmap.empty()){ mpre("ОШИБКА установки карты итога", __LINE__);
				}else if(ITOG[itog.at("id")] = gmap; ITOG.empty()){ mpre("ОШИБКА сохраенения карты итога", __LINE__);
				//}else if(TMs index_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", itog.at("index_id")}}, {}, {}); false){ mpre("ОШИБКА выборки расчета итога", __LINE__);
				}else if(std::string dataset_map_id = "index," +itog.at("index_id") +"," +dataset.at("id"); dataset_map_id.empty()){ mpre("ОШИБКА составления идентификатора карты", __LINE__);
				}else if(TMs index_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); index_map.empty()){ mpre("ОШИБКА обнолвения карты исходника", __LINE__);
				}else if(std::string map = (index_map.end() == index_map.find("map") ? std::string(dataset_count, '0') : index_map.at("map")); map.empty()){ mpre("ОШИБКА расчета карты", __LINE__);
				}else if(boost::dynamic_bitset<> imap(map); imap.empty()){ mpre("ОШИБКА установки карты морфа", __LINE__);
				}else if(INDEX[itog.at("id")] = imap; INDEX.empty()){ mpre("ОШИБКА сохраенения карты морфа", __LINE__);
				}else if(auto temp = gmap ^ imap; temp.empty()){ mpre("Получение разнциы результатов", __LINE__);
				}else if(ers += temp.count(); (0 > ers)){ mpre("ОШИБКА инкремента разницы", __LINE__);
				}else if(count += dataset_count; (0 > count)){ mpre("ОШИБКА расчета количества сигналов", __LINE__);
				}else{ //mpre("Карта itog " +itog_map.at("map"), __LINE__); mpre("Карта index " +index_map.at("map"), __LINE__); mpre("Разница count=" +to_string(count) +" ers=" +to_string(ers), __LINE__);
				}}return (!ers ? 1 : (float)(count-ers)/count); }(); (0 > diff)){ mpre("ОШИБКА расчета результата набора данных", __LINE__);
			}else if(float perc = [&](int count = 0){ for(int i = 0; i < dataset_count; i++){ // Совпадение по значениям
				if(int ers = [&](int ers = 0){ for( auto itog_itr:BMF_ITOG_EX.at("")){ // Проверка всех итогов
					if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
					}else if(itog.end() == itog.find("id")){ mpre("ОШИБКА у итога не найдено поле идентификатора", __LINE__);
					}else if(ITOG.end() == ITOG.find(itog.at("id"))){ mpre("ОШИБКА в справочнике карт итог не найден " +itog.at("id"), __LINE__);
					}else if(boost::dynamic_bitset<> gmap = ITOG.at(itog.at("id")); gmap.empty()){ mpre("ОШИБКА восстановления карты итога", __LINE__);
					}else if(INDEX.end() == INDEX.find(itog.at("id"))){ mpre("ОШИБКА в справочнике карт морфа итог не найден", __LINE__);
					}else if(boost::dynamic_bitset<> imap = INDEX.at(itog.at("id")); imap.empty()){ mpre("ОШИБКА восстановления карты морфа итога", __LINE__);
					}else if(gmap.test(i) == imap.test(i)){ //mpre("Значения равны", __LINE__);
					}else if(ers++; (0 >= ers)){ mpre("Инкремент ошибки", __LINE__);
					}else{ //mpre("Не совпадение itog_id=" +itog.at("id") +" i=" +to_string(i), __LINE__);
					}}return ers; }(); (0 > ers)){ mpre("ОШИБКА расчета значения", __LINE__);
				}else if(count += (ers ? 0 : 1); (0 > count)){ mpre("ОШИБКА количество правильных ответов", __LINE__);
				}else{ //mpre("Количество правильных ответов i=" +to_string(i) +" dataset_count=" +to_string(dataset_count) +" count=" +to_string(count) +" ers=" +to_string(ers), __LINE__);
				}}return (float)(count*100)/dataset_count; }(); (0 > perc)){ mpre("ОШИБКА расчета процента полных совпадений по значениям", __LINE__);
			}else if(string _perc = [&](string _perc = ""){ char chr[10]; sprintf(chr ,"%.2f" ,perc); return string(chr); }(); _perc.empty()){ mpre("ОШИБКА расчета строки процента ошибки", __LINE__);
			}else{ mpre("Набор #" +dataset["id"] +" количество:"+ dataset["count"] +" точность:"+ to_string(diff) +" (" +_perc +"%)" +("0" == dataset["epoch"] ? "" : " эпох:" +dataset["epoch"]), __LINE__);
			}}return false; }()){ mpre("ОШИБКА отображения списка набора данных", __LINE__);
		//}else if(bmf::Close(0)){ mpre("ОШИБКА закрытия БД", __LINE__);
		}else{ //mpre("Выберете набор данных для расчета -ds и количество -epoch", __LINE__);
		}return bmf::dataset; }(); bmf::dataset.empty()){ //mpre("ОШИБКА набор данных не установлен", __LINE__);
	}else if(int loop = [&](int loop = 0){ // Количетсво повторений
		if(int epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? 0 : atoi(bmf::ARGV.at("epoch").c_str())); (0 > epoch)){ mpre("ОШИБКА расчета количества эпох", __LINE__);
		}else if(bmf::ARGV.end() == bmf::ARGV.find("epoch")){ //mpre("Для обучения укажите количество эпох epoch", __LINE__);
		}else if(!epoch){ loop = -1; mpre("Указано нулевое количество эпох расчитываем до полного совпадения", __LINE__);
		}else if(loop = epoch){ //mpre("Набор данных -ds " +bmf::dataset.at("id")+ " количество эпох -epoch " +to_string(loop), __LINE__);
		}else{ mpre("ОШИБКА расчтеа количества повторений", __LINE__);
		}return loop; }(); false){ mpre("ОШИБКА расчета колчиества повторений", __LINE__);
	//}else if(mpre("Обучение -ds " +bmf::dataset.at("id") +" -epoch " +to_string(loop), __LINE__); false){ mpre("ОШИБКА уведомления о свойствах расчетов", __LINE__);
	}else if(int errors = [&](int errors = 0, bool pass = false){ do{ // Итерации обучения
		if(bmf::loop++ >= loop){ //mpre("Остановка по максимальному количеству эпох", __LINE__);
		}else if(bmf::dataset.empty()){ mpre("ОШИБКА набор данных не установлен", __LINE__);
		}else if(int dataset_count = atoi(bmf::dataset["count"].c_str()); (0 >= dataset_count)){ mpre("ОШИБКА нулевое количество в наборе данных", __LINE__);
		//}else if(bmf::Open(1)){ mpre("ОШИБКА подключения к БД", __LINE__);
		}else if(auto microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9; false){ mpre("ОШИБКА расчета времени", __LINE__);
		}else if(int err = [&](int err = 0, int progress = 0, bool rep = false, int key = 0){ do{ // Сравнение результата расчета
			if(auto _microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 - microtime; false){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(std::string epoch = [&](std::string epoch = "1"){ // Расчет эпохи
				if(bmf::dataset.end() == bmf::dataset.find("epoch")){ // mpre("Поле эпохи не задано", __LINE__);
				}else if(epoch = bmf::dataset.at("epoch"); false){ err("Выборка значения эпохи");
				}else if(epoch = to_string(atoi(epoch.c_str()) + 1); epoch.empty()){ err("Инкремент эпохи");
				}else{ //mpre("Увеличение значения эпохи " +epoch, __LINE__);
				}return epoch; }(); epoch.empty()){ mpre("ОШИБКА расчета эпохи", __LINE__);
			//}else if(std::string epoch = to_string(atoi(( ? "0" : bmf::dataset.at("epoch")).c_str()))+1); epoch.empty()){ mpre("ОШИБКА расчета текущей эпохи", __LINE__);
			}else if(std::string itog_str = (bmf::ARGV.end() == bmf::ARGV.find("itog") ? "" : "." +bmf::ARGV.at("itog")); false){ mpre("ОШИБКА формирования итога в консоли", __LINE__);
			}else if(bmf::Progress("Набор:" +bmf::dataset.at("id") + itog_str+" Эпоха:" +epoch +" Примеров:" +to_string(progress) +" Изменений:"+ to_string(err)+ " (" +to_string(_microtime) +" сек.)", (float)++progress/dataset_count, __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			/*}else if(std::string itog_id = [&](){ // Получение итога консоли
				if(itog_id = (bmf::ARGV.end() == bmf::ARGV.find("itog") ? "" : bmf::ARGV.at("itog")); false){ mpre("ОШИБКА выборки номера итога", __LINE__);
				}else if(std::string sql = "SELECT * FROM index WHERE " +(itog_id.empty() ? "1" : "itog_id=" +itog_id); sql.empty()){ mpre("ОШИБКА формирования запроса выбора", __LINE__);
				}else if(TMMi INDEX = bmf::Tab(sql); INDEX.empty()){ //mpre("Пустой список на реиндексацию", __LINE__);
				}else if([&](){ for(auto index_itr:INDEX){ // Реиндексация морфов
					if(TMs index = index_itr.second; index.empty()){ mpre("ОШИБКА выборки морфа", __LINE__);
					}else if(index.end() == index.find("id")){ mpre("ОШИБКА у морфа не найдено поле идентификатора", __LINE__);
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}; where.empty()){ mpre("ОШИБКА получения условия выборки карты", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта не пуста", __LINE__);
					}else if(TMs bmf_index = bmf::Reindex(index); bmf_index.empty()){ mpre("ОШИБКА реиндексации морфа", __LINE__);
					}else{ //mpre("Реиндексация", __LINE__);
					}}return false; }()){ mpre("ОШИБКА реиндексации", __LINE__);
				}else{ //mpre("ОШИБКА Реиндексации морфов", __LINE__);
				}return itog_id; }(); false){ mpre("ОШИБКА реиндексации морфов", __LINE__);*/
			}else if([&](){ for(auto &itog_itr:bmf::List(bmf::ITOG, {}, __LINE__)){ // Проверка списка итогов
				if(false){ mpre("Пропуск расчета", __LINE__);
				}else if(TMs itog = itog_itr.second; itog.empty()){ //mpre("ОШИБКА выборки итога", __LINE__);
				}else if(itog.end() == itog.find("id")){ mpre("ОШИБКА не найден идентификатор итога", __LINE__);
				}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Карта итога
					if(std::string dataset_map_id = "itog," +itog.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ err("Составления идентификатора карты");
					}else if(dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); dataset_map.empty()){ err("Выборка карты итога");
					}else{ //mpre("Выборка карты итога " +dataset_map_id, __LINE__);
					}return dataset_map; }(); dataset_map.empty()){ mpre("ОШИБКА получения карты итога", __LINE__);
				}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА поле карты и неайдено у итога", __LINE__);
				}else if(boost::dynamic_bitset<> ITOG(dataset_map.at("map")); ITOG.empty()){ mpre("ОШИБКА получения результата расчета", __LINE__);
				}else if(key >= ITOG.size()){ mpre("ОШИБКА карта итога меньше количество примеров в выборке", __LINE__);
				}else if(TMs index = [&](TMs index = {}){ // Добавление первоначального морфа
					if(TMMs _DANO = bmf::List(bmf::DANO, {}, __LINE__); _DANO.empty()){ err("Список итогов");
					}else if(TMs dano = _DANO.begin()->second; dano.empty()){ mpre("ОШИБКА выборки первого исходника", __LINE__);
					}else if([&](){ // Выборка морфа из базы
						if(itog.end() == itog.find("index_id")){ //mpre("У итога не найдено поле морфа", __LINE__);
						}else if(std::string index_id = itog.at("index_id"); index_id.empty()){ //mpre("Пустое поле корневого морфа", __LINE__);
						}else if((index = bmf::Up(bmf::INDEX, {{"id", itog.at("index_id")}}, {}, {}, __LINE__)).empty()){ mpre("ОШИБКА добавления корневого морфа в базу", __LINE__);
						}else{ //mpre(index, "Морф найден в базе", __LINE__);
						}return !index.empty(); }()){ //mpre("Морф уже в базе", __LINE__);
					}else if(TMs _index = {/*{"clump_id", bmf::clump_id},*/ {"itog_values_id", itog.at("itog_values_id")}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}, {"index_id", ""}, {"bmf-index", ""}}; _index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
					}else if((index = bmf::Up(bmf::INDEX, {}, _index, {}, __LINE__)).empty()){ mpre("ОШИБКА добавления корневого морфа в базу", __LINE__);
					}else if(TMs dataset_map = bmf::Maps(index, "bmf-index"); dataset_map.empty()){ mpre("Установка карт", __LINE__);
					}else if(itog["index_id"] = index.at("id"); itog.empty()){ mpre("ОШИБКА установки свойства связи итога с морфом", __LINE__);
					}else if(TMs _itog = bmf::Up(bmf::ITOG, {{"id", itog.at("id")}}, {}, itog, __LINE__); _itog.empty()){ err("Обновление поля корневого морфа у итога");
					}else if(bmf::COUNT[index.at("id")] = "0"; (bmf::COUNT.end() == bmf::COUNT.find(index.at("id")))){ mpre("ОШИБКА установки счетчика корневого морфа", __LINE__);
					}else{ //mpre("Добавление первоначального морфа " +index.at("id"), __LINE__);
					}return index; }(); index.empty()){ mpre("ОШИБКА морф связи не найден", __LINE__);
				}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета размера карты", __LINE__);
				}else if(std::string map = [&](std::string map = ""){ // Карта корневого морфа
					if(false){ err("Пропуск");
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}; where.empty()){ //mpre("ОШИБКА задания условий на выборку карты", __LINE__);
					}else if(std::string dataset_map_id = "index," +index.at("id") +"," +bmf::dataset.at("id"); dataset_map_id.empty()){ err("Составления идентификатора карты");
					}else if(TMs dataset_map = bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, {}, {}, __LINE__); false){ mpre("Выборка карты корневого морфа", __LINE__);
					}else if(map = (dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); map.empty()){ mpre("ОШИБКА установки карты основного морфа", __LINE__);
					}else if(!dataset_map.empty()){ //mpre("Уже найден в базе", __LINE__);
					}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА в карте нет поля битовой карты", __LINE__);
					}else if(map = dataset_map.at("map"); map.empty()){ mpre("ОШИБКА выборки карты корневого морфа", __LINE__);
					}else{ mpre("Добавление карты корневого морфа index_id=" +index.at("id") +" map=" +map, __LINE__);
					}return map; }(); false){ mpre("ОШИБКА расчета карты корневого морфа", __LINE__);
				}else if(boost::dynamic_bitset<> INDEX(map); (dataset_count != map.length())){ mpre("ОШИБКА не верный размер карты", __LINE__);
				}else if(std::string learn = (ITOG.test(key) ? "1" : "0"); (1 != learn.length())){ mpre("ОШИБКА получения бит", __LINE__);
				}else if(std::string calc = (INDEX.test(key) ? "1" : "0"); (1 != calc.length())){ mpre("ОШИБКА получения расчета", __LINE__);
				}else if([&](){ // Обучение
					if(learn == calc){ //mpre("Сравнение itog[]=" +calc, __LINE__);
					}else if(string _calc = bmf::Learn(index, learn, key); (1 <= _calc.length() >= 2)){ mpre("ОШИБКА обучения морфа значение не верная длинна результата", __LINE__);
					}else if(err+=1; !err){ mpre("Инкремент ошибок", __LINE__);
					}else{ //mpre("Обучение " +index["map"], __LINE__);
					}return false; }()){ mpre("ОШИБКА обучения", __LINE__);
				}else{ //mpre("Обучение сигнала " +to_string(key) +" itog[" +itog.at("id") +"]", __LINE__);
				}} return false; }()){ mpre("ОШИБКА сравнения результата расчета", __LINE__);
			}else if(++key >= dataset_count){ //mpre("Инкремент номера примера", __LINE__);
			}else if(!(rep = !rep)){ mpre("ОШИБКА положительный повтор", __LINE__);
			}else{
			}}while(!(rep = !rep)); return err; }(); (0 > err)){ mpre("ОШИБКА проверки списка итогов", __LINE__);
		//}else if(bmf::Close(err)){ mpre("ОШИБКА закрытия БД", __LINE__);
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

