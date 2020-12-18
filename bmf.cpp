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

	TMs databases; // Список баз данных
	string dbname; //Имя БД
	sqlite3_stmt* stmt; // Соединение
	sqlite3* db = 0; //Идентификатор БД
	dbstl::db_map<std::string, std::string> maps; // Карты БД
	TMs ARGV; // Массив параметров консоли
	int timestamp =  time(0); // Расчет выполнения участков кода
	auto microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9;

	DbEnv *env; //Окружение БД
	TMMt *TEST; // Справочнк морфов

	TMMt *INDEX; // Справочнк морфов
	TMMt *DATASET; // Наборы данных
	TMMt *DATASET_MAP; // Карты
	TMMt *DANO; // Исходники
	TMMt *DANO_VALUES; // Значения исходников
	TMMt *DANO_TITLES; // Заголовки исходников
	TMMt *ITOG; // Итоги
	TMMt *ITOG_VALUES; // Значения итогов
	TMMt *ITOG_TITLES; // Заголовки итогов

	std::function<TMMi(string,TM3i&)> Save; // Сохранение информации в базу
	std::function<std::string(TMs)> Calculate; // Расчет строки для расчета польской нотации
	std::function<std::string(std::string,TM3i&)> Calc; // Расчет строки польской нотации
	std::function<TMs(TMs,int)> Tree; // История морфа
	std::function<TMs(TMs)> Value; // История морфа
	std::function<bool(int,int)> Do; // Непосредственно расчет
	std::function<TMs(TMs,TMs,TMs,int)> Choice; // Расчет истории
	std::function<double(string)> Bin2dec; // Перерасчет размерности из двоичной в десятеричную
	std::function<string(double)> Dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
	std::function<int(TMs,int,TM3i&,TM3i&)> Learning; // Обучение
	std::function<int(nlohmann::json,int,int,int,TM3i&,TM3i&)> LearningAll; // Обучение
	std::function<string(TMs,string,int)> Learn; // Рерасчет морфа

	std::function<int(string)> exec; // Выполнение запроса к БД
	std::function<int(string)> prepare; // Таблица с интервалами времени
	std::function<TMs(string,TMs,TMs,TMs)> fk; // Сохранение информации в базу
	std::function<TMMi(string)> Tab; // Выборка таблицы из БД
	std::function<TMMi(TMMi&,string,string,string)> Dataset; // Выгрузка данных из базы с проверкой равенства
	std::function<void(string,float,int)> Progress; // Отрисовка полосы выполнения
	std::function<int(void)> Getch; // Получение нажатий клавиш в консоли
	//std::function<boost::dynamic_bitset<>(TMs&)> Mapping; // Расчет карт результатов
	std::function<TMs(TMs&)> Reindex; // Реиндексация
	std::function<TMs(TMs&,std::string)> Maps; // Расчет карт результатов
	//std::function<bool(bool)> Open; // Открытие соединения с БД
	std::function<bool(int)> Close; // Закрытие соединения с БД
	std::function<bool(string,bool)> Mem; // Расчет карт результатов
	std::function<bool()> Databases; // Список баз данных

	std::function<Db*(string)> Open; // Список баз данных
	std::function<string(TMMt*)> Id; // Расчет локального идентификатора
	std::function<TMMi(std::string,TMs)> Rb; // Список баз данных
	std::function<TMs(std::string,TMs)> Erb; // Список баз данных
	std::function<TMs(std::string,std::string,TMs)> Erb_insert; // Список баз данных
	std::function<TMMt*(std::string)> Guide; // Поиск справочника
	std::function<int(TMs,string,int,TMMt*,TMMt*,TMMt*)> Vals; // Обучение
	std::function<TMs(TMMt*,TMs,TMs,TMs)> Up; // Обучение
}

  

int main(int argc, char **argv){
	if(setlocale(LC_ALL, "LC_ALL=C.UTF-8"); false){ mpre("ОШИБКА установки кирилицы", __LINE__);
	}else if([&](){ // Параметры консоли
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
		}else{ //mpre(bmf::ARGV, "Список параметров", __LINE__);
		}return bmf::ARGV.empty(); }()){ mpre("ОШИБКА параметры консоли не заданы", __LINE__);
	}else if(nlohmann::json in = [&](nlohmann::json in = {}){ // Входной поток
		if(bmf::ARGV.end() == bmf::ARGV.find("-")){ //mpre("Данные для обучения не установлены", __LINE__);
		}else if([&](string str = ""){ while(getline(std::cin, str)){ bmf::ARGV.at("-") += ("\n"+ str); }; return (0 >= bmf::ARGV.at("-").length()); }()){ mpre("ОШИБКА входящий параметр не задан", __LINE__);
		}else if(in = nlohmann::json::parse(bmf::ARGV.at("-")); false){ mpre("ОШИБКА Входящий параметр с обучающими данными пуст", __LINE__);
		}else{ //mpre("Данные из потока std::cin "+ in.dump(), __LINE__);
		} return in; }(); false){ mpre("ОШИБКА получения входного потока", __LINE__);
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
		}else if(bmf::env = [&](DbEnv *envp = NULL){ // Окружение БД
			if(envp = new DbEnv(DB_CXX_NO_EXCEPTIONS); (nullptr == envp)){ mpre("ОШИБКА создания окружения", __LINE__);
			}else if(envp->set_lk_detect(DB_LOCK_MINWRITE); (nullptr == envp)){ mpre("ОШИБКА установки записи ошибок", __LINE__);
			}else if(u_int32_t envFlags = DB_CREATE | DB_RECOVER | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_TXN | DB_INIT_MPOOL | DB_THREAD; !envFlags){ mpre("ОШИБКА созадания флагов окружения", __LINE__);
			}else if(!fs::exists(bmf::dbname) && !fs::create_directory(bmf::dbname)){ mpre("ОШИБКА директория БД не задана", __LINE__);
			}else if(envp->open(bmf::dbname.c_str(), envFlags, 0); (nullptr == envp)){ mpre("ОШИБКА открытия окружения", __LINE__);
			}else{ //mpre("Создание окружения", __LINE__);
			}return envp; }(); (nullptr == bmf::env)){ mpre("ОШИБКА подключения окружения БД", __LINE__);
		}else if(dbstl::DbstlElemTraits<TMs>::instance()->set_size_function([](const TMs& elem){ // size
			u_int32_t size = (sizeof(std::string::size_type) + std::to_string(elem.size()).length());
			for(auto elem_itr:elem){
				size += sizeof(std::string::size_type) + elem_itr.first.length();
				size += sizeof(std::string::size_type) + elem_itr.second.length();
			}return size; }); false){ mpre("ОШИБКА установки функции размера", __LINE__);
		}else if(dbstl::DbstlElemTraits<TMs>::instance()->set_copy_function([](void* dest, const TMs& elem){
			std::function save_str = [&](const std::string& str, void* dest){
				auto size = str.length();
				memcpy(dest, &size, sizeof(size));
				char* charDest = static_cast<char*>(dest) + sizeof(size);
				memcpy(charDest, str.data(), str.length());
				return charDest + str.length();
			}; //dest = save_str(elem.id, dest); dest = save_str(elem.name, dest);
			std::string size = std::to_string(elem.size()); dest = save_str(size, dest);
			for(auto elem_itr:elem){
				dest = save_str(elem_itr.first, dest);
				dest = save_str(elem_itr.second, dest);
			}}); false){ mpre("ОШИБКА установки функции сохранения", __LINE__);
		}else if(dbstl::DbstlElemTraits<TMs>::instance()->set_restore_function([](TMs& elem, const void* src){ // restore
			std::function restore_str = [&](std::string& str, const void* src){
				std::string::size_type size;
				memcpy(&size, src, sizeof(size));
				str.reserve(size);
				const char* strSrc = static_cast<const char*>(src) + sizeof(size);
				str.insert(0, strSrc, size);
				return strSrc + size;
			}; //src = restore_str(elem.id, src); src = restore_str(elem.name, src);
			std::string size; src = restore_str(size, src);
			for(int i = atoi(size.c_str()); i >0; i--){
				std::string key; src = restore_str(key, src);
				std::string val; src = restore_str(val, src);
				elem.insert(make_pair(key, val));
			}}); false){ mpre("ОШИБКА установки функции перезаписи", __LINE__);
		}else if(dbstl::dbstl_startup(); false){ mpre("ОШИБКА запуска", __LINE__);
		}else if(dbstl::register_db_env(bmf::env); false){ mpre("ОШИБКА регистрации окружения", __LINE__);
		}else if(bmf::Open = ([&](std::string table, Db *db = NULL){ // Открытие БД
			if(db = new Db(bmf::env, DB_CXX_NO_EXCEPTIONS); false){ mpre("ОШИБКА создания соединения", __LINE__);
			//}else if(db->set_flags(DB_DUP); false){ mpre("ОШИБКА установки флага", __LINE__);
			}else if(u_int32_t openFlags = DB_CREATE | DB_READ_UNCOMMITTED | DB_AUTO_COMMIT | DB_THREAD; !openFlags){ mpre("ОШИБКА устанвоки флагов", __LINE__);
			}else if(db->open(NULL, table.c_str(), NULL, DB_BTREE, openFlags, 0); false){ mpre("ОШИБКА открытия БД", __LINE__);
			}else if(dbstl::register_db(db); false){ mpre("ОШИБКА регистрации БД", __LINE__);
			}else{ //mpre("Прдключение таблицы " +table, __LINE__);
			}return db; }); false){ mpre("ОШИБКА открытия БД", __LINE__);
		/*}else if([&](){ // Запись в БД
			if(false){ //mpre("Пропуск теста", __LINE__);
			//}else if(Db *dbp = ; (nullptr == dbp)){ mpre("ОШИБКА подключения к таблице", __LINE__);
			}else if(TMMt test = TMMt(bmf::Open("test.db"), bmf::env); false){ mpre("ОШИБКА регистрации хранилища", __LINE__);
			}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
			}else if([&](){ for (int j = 0; j < 10; j++){// Добавление значений в базу
				if(TMs row = {{"key1", "val1"}, {"key2", "val2"}}; row.empty()){ mpre("ОШИБКА создания нового значения", __LINE__);
				}else if(auto [itr, ret] = test.insert(make_pair(to_string(rand()), row)); !ret){ mpre("ОШИБКА сохранения значения в БД", __LINE__);
				}else{ //mpre(row, "Сохранение занчения в базу", __LINE__);
				}}return false; }()){ mpre("ОШИБКА добавления значений в базу", __LINE__);
			}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
			//}else if(int count = [&](int cnt = 0){ for(auto itr = test.begin(); itr != test.end(); itr++){ cnt += 1; }; return cnt; }(); !count){ mpre("ОШИБКА расчета количества записей в базе", __LINE__);
			}else if(int count = test.size(); !count){ mpre("ОШИБКА расчета количества записей в базе", __LINE__);
			}else{ mpre("Количество записей в базе COUNT(test)=" + std::to_string(count), __LINE__);
			}return false; }()){ mpre("ОШИБКА проверки записи в БД", __LINE__);*/
		}else if([&](){ // Подключение таблиц	
			if(false){ mpre("Пропуск подключения таблиц", __LINE__);
			}else if(bmf::INDEX = new TMMt(bmf::Open("index.db"), bmf::env); (nullptr == bmf::INDEX)){ mpre("ОШИБКА создания таблицы", __LINE__);
			}else if(bmf::DATASET = new TMMt(bmf::Open("dataset.db"), bmf::env); (nullptr == bmf::DATASET)){ mpre("ОШИБКА открытия соединения с базой наборов данных", __LINE__);
			}else if(bmf::DATASET_MAP = new TMMt(bmf::Open("dataset_map.db"), bmf::env); (nullptr == bmf::DATASET_MAP)){ mpre("ОШИБКА открытия соединения с базой карт", __LINE__);
			}else if(bmf::DANO = new TMMt(bmf::Open("dano.db"), bmf::env); (nullptr == bmf::DANO)){ mpre("ОШИБКА открытия соединения с базой исходных значений", __LINE__);
			}else if(bmf::DANO_VALUES = new TMMt(bmf::Open("dano_values.db"), bmf::env); (nullptr == bmf::DANO_VALUES)){ mpre("ОШИБКА открытия соединения с базой значений исходников", __LINE__);
			}else if(bmf::DANO_TITLES = new TMMt(bmf::Open("dano_titles.db"), bmf::env); (nullptr == bmf::DANO_TITLES)){ mpre("ОШИБКА открытия соединения с базой заголовков исходников", __LINE__);
			}else if(bmf::ITOG = new TMMt(bmf::Open("itog.db"), bmf::env); (nullptr == bmf::ITOG)){ mpre("ОШИБКА открытия соединения с базой итоговых значений", __LINE__);
			}else if(bmf::ITOG_VALUES = new TMMt(bmf::Open("itog_values.db"), bmf::env); (nullptr == bmf::ITOG_VALUES)){ mpre("ОШИБКА открытия соединения с базой значений итогов", __LINE__);
			}else if(bmf::ITOG_TITLES = new TMMt(bmf::Open("itog_titles.db"), bmf::env); (nullptr == bmf::ITOG_TITLES)){ mpre("ОШИБКА открытия соединения с базой заголовков итогов", __LINE__);
			}else{ //mpre("Подключение списка таблиц", __LINE__);
			}return false; }()){ mpre("ОШИБКА подключения таблиц", __LINE__);
		/*}else if((test = [&](TMMt *ROWS, TMs row, bool up = 0, TMs _row = {}){ // Добавление заголовока в базу
			if(row.end() == row.find("id")){ mpre("У поля не задан идентификатор", __LINE__);
			}else if([&](){ // Редактирование значения в БД
				if(ROWS->end() == ROWS->find(row.at("id"))){ //mpre("Значение не найдено в базе", __LINE__);
				}else if(_row = (*ROWS)[row.at("id")]; _row.empty()){ mpre("ОШИБКА выборки значения из базы", __LINE__);
				}else if(!up){ //mpre("ОШИБКА обновления значения", __LINE__);
				}else if((*ROWS)[row.at("id")] = row; false){ mpre("ОШИБКА выборки значения из базы", __LINE__);
				}else if((_row = row).empty()){ mpre("ОШИБКА Обновленное значение", __LINE__);
				}else{ //mpre("Обновление значения", __LINE__);
				}return false; }(); !_row.empty()){ //mpre("Запись в базе", __LINE__);
			}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
			}else if(auto [_row, ret] = ROWS->insert(make_pair(row["id"], row)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
			}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
			}else{ mpre("Добавление заголовока в базу " +row.at("id"), __LINE__);
			}return _row; }(bmf::TEST, _test)).empty()){ mpre("ОШИБКА добавления значения в БД", __LINE__);*/
		/*}else if([&](){ // Тест на поиск и запись
			if(trie){ mpre("Пропуск теста", __LINE__);
			}else if(bmf::TEST = new TMMt(bmf::Open("test.db"), bmf::env); (nullptr == bmf::ITOG_TITLES)){ mpre("ОШИБКА открытия соединения с базой заголовков итогов", __LINE__);
			}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
			}else if(auto [itr, ret] = bmf::TEST->insert(make_pair("test1", TMs({{"Ключ1", "Значение1"}, {"Ключ2", "Значение2"}}))); !ret){ mpre("ОШИБКА добавления значения в тест", __LINE__);
			}else if(auto [itr, ret] = bmf::TEST->insert(make_pair("test2", TMs({{"Ключ1", "Значение1"}, {"Ключ2", "Значение2"}}))); !ret){ mpre("ОШИБКА добавления значения в тест", __LINE__);
			}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
			}else if(bmf::TEST->end() == bmf::TEST->find("test1")){ mpre("ОШИБКА первое значение в БД не найдено", __LINE__);
			}else{
			}return false; }()){ mpre("ОШИБКА теста на запись", __LINE__);*/
		}else{ //mpre("Подключение БД " +bmf::dbname, __LINE__);
		}return false; }()){ mpre("ОШИБКА подключения БД", __LINE__);
	}else if([&](){ // Функции БД
		if(bmf::Up = ([&](TMMt *ROWS, TMs where, TMs insert, TMs update, TMs _row = {}){
			if(where.end() == where.find("id")){ mpre("У поля не задан идентификатор", __LINE__);
			}else if(1 != where.size()){ mpre("ОШИБКА условий выборки больше одного", __LINE__);
			}else if([&](){ // Выборка
				if(ROWS->end() == ROWS->find(where.at("id"))){ //mpre("Значение не найдено в базе", __LINE__);
				}else if(_row = (*ROWS)[where.at("id")]; _row.empty()){ mpre("ОШИБКА выборки значения из базы", __LINE__);
				}else{ //mpre("Выборка по условию", __LINE__);
				}return false; }()){ err("Выборка");
			}else if([&](){ // Редактирование
				if(update.empty()){ //mpre("Не обновляем значение", __LINE__);
				}else if(_row.empty()){ //mpre("Элемент по условию не найден", __LINE__);
				}else if(update["id"] = where.at("id"); false){ err("Установка идентификатора");
				}else if(update == _row){ //mpre("При редактировании значения совпали", __LINE__);
				}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
				}else if((*ROWS)[update.at("id")] = update; false){ mpre("ОШИБКА выборки значения из базы", __LINE__);
				}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
				}else if(_row = update; false){ mpre("ОШИБКА установки результата", __LINE__);
				}else{ //mpre("Обновление значения", __LINE__);
				}return false; }()){ err("Редактирование");
			}else if([&](){ // Добавление
				if(insert.empty()){ //mpre("Не добавляем в базу", __LINE__);
				}else if(!_row.empty()){ //mpre("Элемент по условию найден", __LINE__);
				}else if(insert["id"] = where.at("id"); false){ err("Установка идентификатора");
				}else if(insert == _row){ mpre("ОШИБКА значения при обновлении совпали", __LINE__);
				}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
				}else if(auto [itr, ret] = ROWS->insert(make_pair(insert["id"], insert)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
				}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
				}else if((_row = insert).empty()){ mpre("ОШИБКА Обновленное значение", __LINE__);
				}else{ //mpre("Добавление значения в базу", __LINE__);
				}return false; }()){ err("Обновление");
			}else{ //mpre("Добавление заголовока в базу `" +row.at("id") +"` ", __LINE__);
			}return _row; }); false){ err("Функция обновления");
		}else if(bmf::Id = ([&](TMMt *ROWS, int id = -1){ // Генерация локального идентификатора
			if(false){ mpre("ОШИБКА пропуска", __LINE__);
			}else{// mpre("Максимальный id_min: "+ to_string(id_min)+ " id_max: " + to_string(id_max) + " id_next: "+ to_string(id_next)+ " id:"+ id, __LINE__);
			} return to_string(id); }); false){ mpre("ОШИБКА создания функции локального идентификатора", __LINE__);
		}else if(bmf::Rb = ([&](std::string table, TMs values, TMMi LIST = {}){ // Поиск списков из базы
			if(mpre(values, "Поиск списка по базе", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(values.empty()){ mpre("ОШИБКА запрос с пустыми результатами", __LINE__);
			//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
			}else if(string fields = [&](string fields = ""){ for(auto& values_itr:values){ // Поле индекса
					if(string field = values_itr.first; (0 >= field.length())){ mpre("ОШИБКА получения имени поля", __LINE__);
					}else if(fields += (0 >= fields.length() ? "" : ",")+ field; (0 >= fields.length())){ mpre("ОШИБКА инкремента имени поля к строке полей", __LINE__);
					}else{ //mpre("Формирование списка полей "+ fields, __LINE__);
					}
				} return fields; }(); (0 >= fields.length())){ mpre("ОШИБКА получения поля индекса", __LINE__);
			}else if([&](){ // Выборка по идентификатору
				if(1 < values.size()){ //mpre("Индексов меньше одного", __LINE__);
				}else if("id" != values.begin()->first){ //mpre("Не идентификатор", __LINE__);
				}else{ mpre("ОШИБКА выборки по идентификатору", __LINE__);
				} return false; }()){ mpre("ОШИБКА выборки по идентификатору", __LINE__);
			}else if([&](){ // Создание индекса
				if(true){ mpre("Создание индекса "+ fields, __LINE__);
				/*}else if([&](){ // Пропуск проверки индекса по индектификатору
					if(1 > values.size()){ mpre("Индексов меньше одного", __LINE__);
					}else if("id" != values.begin()->first){ //mpre("Не идентификатор", __LINE__);
					}else if(!LIST.empty()){ mpre("ОШИБКА список уже не пуст", __LINE__);
					}else{ //mpre(values, __LINE__, "Запрос"); mpre("Пропускаем создание индексных полей", __LINE__);
						return true;
					} return false; }()){ //mpre("Запрос по индентификатору", __LINE__);
				}else if([&](){ TABS->insert(make_pair(fields, TMMi({}))); return (TABS->end() == TABS->find(fields)); }()){ mpre("ОШИБКА добавления нового ключа", __LINE__);
				}else if(TABS[field] = new TMMt(bmf::Open(field+ ".db"), bmf::env); bmf::INDEX.empty()){ mpre("ОШИБКА создания таблицы", __LINE__);
				}else if([&](){ // Индексация нового ключа
					if(TABS->end() == TABS->find(fields)){ //mpre("Ключ поиска не создан", __LINE__);
					}else if(TABS->at(fields)->empty()){ //mpre("Ключ поиска пуст", __LINE__);
					}else{ return true;
					} return false; }()){ //mpre("Ключ уже проиндексирован", __LINE__);
				}else if([&](){ for(auto& tabs_itr:TABS.at("")){ // Создание содержимого индекса
						if(TMs tabs = tabs_itr.second; tabs.empty()){ mpre("ОШИБКА получения элемента", __LINE__);
						}else if(string value = ""; 0 < value.length()){ mpre("ОШИБКА первоначальное значения не пусто", __LINE__);
						}else if([&](){ for(auto& values_itr:values){ value += (tabs.end() == tabs.find(values_itr.first) ? "" : tabs.at(values_itr.first))+ "\n"; }; return false; }()){ mpre("ОШИБКА получения строки значений поля", __LINE__);
						}else if(int value_crc32 = Crc32(value.c_str()); (0 == value_crc32)){ mpre("ОШИБКА получения crc значения поля", __LINE__);
						}else if(string value_md5 = md5(value); (0 == value_md5.length())){ mpre("ОШИБКА получения crc значения поля", __LINE__);
						}else if([&](){ // Создание списка crc32
							if(TABS.at(fields).end() != TABS.at(fields).find(value_crc32)){ //mpre("Список value_crc32 уже присутствует в индекса", __LINE__);
							}else if(TABS.at(fields).insert(make_pair(value_crc32, TMs({}))); (TABS.at(fields).end() == TABS.at(fields).find(value_crc32))){ mpre("ОШИБКА создания списка value_crc32", __LINE__);
							}else{ //mpre("Создание списка value_crc32="+ to_string(value_crc32)+ " в индексe", __LINE__);
							} return false; }()){ mpre("ОШИБКА создания нового списка crc32", __LINE__);
						}else if(TABS.at(fields).at(value_crc32).insert(make_pair(tabs.at("id"), value_md5)); TABS.at(fields).empty()){ mpre("ОШИБКА добавления нового индекса", __LINE__);
						}else{ //mpre("Добавление нового индекса "+ fields+ " value="+ value+ " value_crc32="+ to_string(value_crc32)+ " id="+ tabs.at("id")+ " value_md5="+ value_md5, __LINE__);
						}
					} return false; }()){ mpre("ОШИБКА создание содержимого индекса", __LINE__);*/
				}else{ //mpre("Список полей `"+ fields, __LINE__); mpre(values, __LINE__, "Список полей"); mpre(TABS, __LINE__, "Массив"); mpre("ОШИБКА добавления списка полей", __LINE__);
				} return false; }()){ mpre("ОШИБКА проверки наличия индексов", __LINE__);
			/*}else if([&](){ // Список результатов
				if(TABS->end() == TABS->find(fields)){ mpre("ОШИБКА индексный ключ поля не найден "+ fields, __LINE__);
				}else if(string value = ""; 0 < value.length()){ mpre("ОШИБКА первоначальное значение не пусто", __LINE__);
				}else if([&](){ for(auto& values_itr:values){ value += values_itr.second+ "\n"; }; return false; }()){ mpre("ОШИБКА получения строки значений", __LINE__);
				}else if(int value_crc32 = Crc32(value.c_str()); (0 == value_crc32)){ mpre("ОШИБКА получения crc значения поля", __LINE__);
				}else if(string value_md5 = md5(value); (0 == value_md5.length())){ mpre("ОШИБКА получения crc значения поля", __LINE__);
				//}else if([&](){ mpre(values, __LINE__, "Условия"); mpre("value_crc32="+ to_string(value_crc32), __LINE__); mpre("value_md5="+ value_md5, __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(TABS->at(fields)->end() == TABS->at(fields)->find(value_crc32)){ //mpre("Списка ключей по запросу не найден field="+ fields+ " crc32="+ to_string(value_crc32) , __LINE__);
				}else if([&](){ for(auto& index_itr:TABS[fields).at(value_crc32)){ // Формируем список по ключам
						if(index_itr.second != value_md5){ mpre("ОШИБКА md5 не совпал для индекса (Коллизия crc32)", __LINE__);
						}else if(string id = index_itr.first; (0 >= id.length())){ mpre("ОШИБКА нулевая длинна ключа индекса "+ id, __LINE__);
						}else if(TABS.at("").end() == TABS.at("").find(atoi(id.c_str()))){ mpre("ОШИБКА элемент по индексному ключю не найден в списке", __LINE__);
						}else if(LIST.insert(make_pair(atoi(id.c_str()), TABS.at("").at(atoi(id.c_str())))); LIST.empty()){ mpre("ОШИБКА добавления нового элемента в список результата", __LINE__);
						}else{ //mpre(TABS.at(fields).at(value_crc32), __LINE__, "Список индексных полей");
						}
					} return false; }()){ mpre("ОШИБКА формирования совпадений по crc32/md5", __LINE__);
				}else{ //mpre(LIST, __LINE__, "Результат совпадения с индексами");
				} return false; }()){ mpre("ОШИБКА формирования списка результатов", __LINE__);*/
			}else{ //mpre(LIST, __LINE__, "Результат выборки"); //mpre("ОШИБКА формирования списка индексов", __LINE__);
			}return LIST; }); false){ mpre("ОШИБКА установки функции поиска списков", __LINE__);
		}else if(bmf::Erb = ([&](std::string table, TMs values, TMs line = {}){ // Поиск списков из базы
			if(false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(mpre(values, "Поиск элемента по базе " +table, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			/*}else if([&](){ // Получение значения выборки по идентификатору
				if(1 != values.size()){ //mpre("ОШИБКА полей для поиска больше одного", __LINE__);
				}else if(values.end() == values.find("id")){ //mpre("Выборка только по id", __LINE__);
				}else if(TABS->end() == TABS->find("")){ mpre("ОШИБКА поле таблицы в справочнике не найдено", __LINE__);
				}else if(TMMt *TAB = TABS->at(""); (nullptr == TAB)){ mpre("ОШИБКА выборки ссылки на таблицу", __LINE__);
				}else if(string id = values.at("id"); (0 >= id.length())){ //mpre("Значение id в выборке не установлено", __LINE__);
				}else if(TAB->end() == TAB->find(atoi(id.c_str()))){ mpre(values, "условия", __LINE__); mpre("ОШИБКА Запись по id не найдена "+ id, __LINE__);
				}else if(line = (*TAB)[atoi(id.c_str())]; line.empty()){ mpre("ОШИБКА выборка по id пуста", __LINE__);
				}else{ //mpre("ОШИБКА Выборка по идентификатору "+ id, __LINE__);
				} return false; }()){ mpre("ОШИБКА получения значения выборки по id", __LINE__);
			}else if([&](){// mpre(values, __LINE__, "Запрос");
				if([&](){ // Проверка на запрос по id
					if(1 < values.size()){ //mpre("Больше одного значения", __LINE__);
					}else if(values.end() == values.find("id")){ //mpre("Нет значения id", __LINE__);
					}else{ return true;
					} return false; }()){ //mpre(values, __LINE__, "Значения"); mpre("Не выбираем по нескольким полям", __LINE__);
				}else if(TMMi LINE = bmf::Rb(table, values); false){ mpre("ОШИБКА выборки списка полей", __LINE__);
				}else if(1 < LINE.size()){ mpre(values, __LINE__, "Условия выборки"); mpre(LINE, __LINE__, "Результат"); mpre("ОШИБКА список выбранных полей больше одного", __LINE__);
				}else if(line = (1 > LINE.size() ? line : LINE.begin()->second); false){ mpre("ОШИБКА выборки первого результата", __LINE__);
				}else{ //mpre(values, __LINE__, "Список требований"); mpre("Выборка erb", __LINE__);
				} return false; }()){ mpre("ОШИБКА выборки не по идентификатору", __LINE__);*/
			}else{ //mpre(values, __LINE__, "Значения"); mpre(line, __LINE__, "Результат"); mpre(TABS, __LINE__, "Список"); //mpre("ОШИБКА выборка из индексированной таблицы", __LINE__);
			}return line; }); false){ mpre("ОШИБКА установки функции поиска списков", __LINE__);
		}else if(bmf::Erb_insert = ([&](std::string table, std::string id, TMs row, TMs _row = {}){ // Поиск списков из базы
			mpre(row, "Элемент", __LINE__);
			mpre("ОШИБКА Вставка элемента", __LINE__);
			return _row; }); false){ mpre("ОШИБКА установки функции поиска списков", __LINE__);
		}else if(bmf::Guide = ([&](std::string guide, TMMt *GUIDE = NULL){ // Поиск справочника
			if("index" == guide){ GUIDE = bmf::INDEX;
			}else if("dataset" == guide){ GUIDE = bmf::DATASET;
			}else if("dataset_map" == guide){ GUIDE = bmf::DATASET_MAP;
			}else if("dano" == guide){ GUIDE = bmf::DANO;
			}else if("dano_values" == guide){ GUIDE = bmf::DANO_VALUES;
			}else if("dano_titles" == guide){ GUIDE = bmf::DANO_TITLES;
			}else if("itog" == guide){ GUIDE = bmf::ITOG;
			}else if("itog_values" == guide){ GUIDE = bmf::ITOG_VALUES;
			}else if("itog_titles" == guide){ GUIDE = bmf::ITOG_TITLES;
			}else{ mpre("ОШИБКА справочник не найден `" +guide +"`", __LINE__);
			} return GUIDE; }); false){ mpre("ОШИБКА функции поиска справочника", __LINE__);
		}else if(bmf::exec = ([&](string sql, bool pass = false, int sleep = 0, int result = 1){ do{ // Запрос к БД
			if(!(result = 0)){ //mpre("Пропускаем запрос", __LINE__);
			}else if(char *error_report = NULL; false){ mpre("ОШИБКА Установки строки ошибки", __LINE__);
			}else if(result = sqlite3_exec(bmf::db, sql.c_str(), 0, 0, &error_report); (SQLITE_OK == result)){ //mpre("ОШИБКА выполенения запроса", __LINE__);
			}else if("COMMIT TRANSACTION" == sql){ mpre("COMMIT TRANSACTION Не повторяем", __LINE__);
			}else if([&](){ mpre("Запрос: "+ sql, __LINE__); mpre("Ошибка: " + std::string(error_report), __LINE__); mpre("Повторный запрос к БД через "+ to_string(sleep += 10) +" сек.", __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(0 != system(("sleep "+ to_string(sleep)).c_str())){ mpre("Отмена запроса", __LINE__); exit(1);
			}else{ pass = true; //mpre("Повторный запрос к БД", __LINE__);
			} }while(!(pass = !pass)); return result; }); false){ mpre("ОШИБКА установки функции запроса к БД", __LINE__);
		}else if(bmf::prepare = ([&](string sql, bool repeat = false, int result = 1, int sleep =30){ do{ // Запрос к БД
			if(!(result = 0)){ //mpre("Пропускаем запрос", __LINE__);
			}else if(result = sqlite3_prepare_v2(bmf::db, sql.c_str(), -1, &bmf::stmt, 0); (SQLITE_OK == result)){ //mpre("Запрос выполнен без ошибок");
			}else if([&](){ mpre("Запрос "+ sql, __LINE__); mpre("БД вернула ошибку `" +string(sqlite3_errmsg(bmf::db)) +"`", __LINE__); mpre("Повторная запрос через "+ to_string(sleep *= 2), __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(int request = system(("sleep "+ to_string(sleep)).c_str()); (0 != request)){ mpre("Выход из запроса", __LINE__); exit(1);
			}else if(repeat = true; false){ mpre("Установка повтора", __LINE__);
			}else{ //mpre("Повторный запрос к БД", __LINE__);
			} }while(repeat); return result; }); false){ mpre("ОШИБКА установки функции запроса к БД", __LINE__);
		}else if(bmf::fk = ([&](string table, TMs where, TMs insert, TMs update, TMs row = {}){ // Обновление базы данных
			if([&](){ // Запрос к dbstl
				if(false){ mpre("Пропуск запроса к dbstl", __LINE__);
				}else if([&](){ // Вставка
					if(insert.empty()){ mpre("Не добавляем элемент", __LINE__);
					}else{ mpre("Добавляем элемент в БД", __LINE__);
					}return false; }()){ mpre("ОШИБКА вставки", __LINE__);
				}else{ mpre("Запрос table=" +table, __LINE__); mpre(where, "Условие", __LINE__); mpre(insert, "Вставка", __LINE__); mpre(update, "Обновление", __LINE__);
				}return true; }()){ mpre("Запрос к dbstl", __LINE__);
			}else if([&](){ // Выборка по условию
				if(where.empty()){// std::cerr << __LINE__ << " Условие на выборку записей не указано" << endl;
				}else if(string sql = [&](string sql, string values = ""){
					for(auto itr = where.begin(); itr != where.end(); itr++){
						string key = itr->first;
						string val = itr->second;
						string separ = (values.size() ? " AND " : "");
						values += separ+ "`"+ key+ "`='"+ val+ "'";
					}return sql += " WHERE "+ values; }("SELECT * FROM "+ table); (0 >= sql.length())){ std::cerr << __LINE__ << " Запрос добавления выборку записи из БД" << endl;
				}else if([&](){ // Выборка массива
					if(TMMi TAB = bmf::Tab(sql); TAB.empty()){ // Выполнение запроса к базе дыннх
					}else if(TAB.size() < 1){// std::cerr << __LINE__ << " Размер выборки меньше нуля << " << sql << endl;
					}else if([&](){ row = TAB.begin()->second; return false; }()){ std::cerr << __LINE__ << "Список измененных данных" << endl;
					}else{ //mpre(row, __LINE__); // std::cerr << __LINE__ << " Запрос к БД:" << sql << endl; // mpre(row);
					} return false; }()){ std::cerr << " Запрос на получения вновь установленной записи" << endl;
				}else{// std::cerr << __LINE__ << " Запрос на выборку значений: " << sql << endl; // mpre(where, __LINE__);
				}return false; }()){ std::cerr << __LINE__ << " Выборка записи по условию" << endl;
			}else if([&](){ int rowid; // Добавление
				if(insert.empty()){// std::cerr << __LINE__ << " Условия добавления новой записи не указаны" << endl;
				}else if(!row.empty()){// std::cerr << __LINE__ << " Запись уже найдена по условиям << " << row["id"] << endl; mpre(where, __LINE__); mpre(insert, __LINE__); mpre(update, __LINE__);
				}else if(insert.erase("id"); (insert.end() != insert.find("id"))){ mpre("ОШИБКА Устанавливаем нулевой идентификатор", __LINE__);
				}else if(string sql = [&](string sql, string fields = "", string values = ""){
					for(auto itr = where.begin(); itr != where.end(); itr++){
						string key = itr->first;
						string val = itr->second;
						string separ = (fields.size() ? ", " : "");
						fields += separ+ "`"+ key+ "`";
						values += separ+ "'"+ val+ "'";
					} for(auto itr = insert.begin(); itr != insert.end(); itr++){
						string key = itr->first;
						string val = itr->second;
						string separ = (fields.size() ? ", " : "");
						fields += separ+ "`"+ key+ "`";
						values += separ+ "'"+ val+ "'";
					}return sql += "("+ fields+ ") VALUES ("+ values+ ")"; }("INSERT INTO "+ table); (0 >= sql.length())){ std::cerr << __LINE__ << " Запрос добавления новой записи в БД" << endl;
				}else if(bmf::exec(sql)){ mpre("ОШИБКА выполнения запроса к БД", __LINE__); //std::cerr << __LINE__ << " ОШИБКА выполнения запроса к базе данных: " << sql << " >> " << sqlite3_errmsg(db) << endl;// return false;
				}else if(rowid = sqlite3_last_insert_rowid(bmf::db); false){ mpre("ОШИБКА получения идентификатора последней установленной записи", __LINE__); //std::cerr << __LINE__ << " Идентификатор последней установленной записи << " << rowid << endl;
				}else if(sql = "SELECT * FROM "+ table+ " WHERE id="+ to_string(rowid); sql.empty()){ mpre("ОШИБКА установки запроса на выборку", __LINE__); //std::cerr << __LINE__ << " Запрос на выборку внось установленной записи" << endl;
				}else if(TMMi TAB = bmf::Tab(sql); TAB.empty()){ mpre("ОШИБКА выборки списка " +sql, __LINE__);
				}else if(row = erb(TAB, {{"id", to_string(rowid)}}); false){ mpre("ОШИБКА получения элемента", __LINE__);
				}else{// std::cerr << __LINE__ << " Добавление новой записи к базе `"+ table+ "` << " << rowid << " " << endl;
				}return false; }()){ std::cerr << __LINE__ << "ОШИБКА добавление запроса если не найден по условиям" << endl;
			}else if([&](){ // Обновление
				if(update.empty()){ //std::cerr << __LINE__ << " Условия обновления не указаны" << endl;
				}else if(row.empty()){ std::cerr << __LINE__ << " Запись для обновления не найдена " << endl;
				}else if(string sql = [&](string sql, string fields = "", string values = ""){
					for(auto itr = update.begin(); itr != update.end(); itr++){
						if(string key = itr->first; (key == "id")){ // Сохраняем идентификатора записи
						}else if(key.substr(0, 1) == "_"){ // Технические поля
						}else{
							string val = itr->second;
							string separ = (values.size() ? ", " : "");
							values += separ+ "`"+ key+ "`='"+ val+ "'";
						}
					}return sql += " SET "+ values+ " WHERE id="+ row["id"]; }("UPDATE "+ table); (0 >= sql.length())){ std::cerr << __LINE__ << " Запрос добавления новой записи в БД" << endl;
				}else if(bmf::exec(sql); false){ mpre("ОШИБКА обновления записи", __LINE__); //std::cerr << __LINE__ << " ОШИБКА обновления записи" << endl;
				}else if(sql = "SELECT * FROM "+ table+ " WHERE id="+ row["id"]; false){ mpre("ОШИБКА запроса на выборку вновь установленных данных", __LINE__); //std::cerr << __LINE__ << " Запрос на выборку внось установленной записи" << endl;
				}else if(TMMi TAB = bmf::Tab(sql); TAB.empty()){ mpre("ОШИБКА выборки списка", __LINE__);
				}else if(row = erb(TAB, {{"id", row["id"]}}); row.empty()){ mpre("ОШИБКА полуения списка измененных данных", __LINE__); //std::cerr << __LINE__ << "Список измененных данных" << endl;
				}else{ //std::cerr << __LINE__ << " Обновляем запись: " << sql << endl;// mpre(update, __LINE__);
				}return false; }()){ std::cerr << __LINE__ << " Обновение найденной по условию записи" << endl;
			}else{// mpre(where, __LINE__); mpre(insert, __LINE__); mpre(update, __LINE__);
			}return row; }); false){ mpre("ОШИБКА обьявления функции Crc32", __LINE__);
		}else if(bmf::Tab = ([&](string sql, TMMi TAB = {}){ // Выборка таблицы
			if([&](){ // Запрос к dbstl
				if(false){ mpre("Пропуск запроса к dbstl", __LINE__);
				}else{ mpre("Запрос " +sql, __LINE__);
				}return true; }()){ mpre("Запрос а dbstl", __LINE__);
			}else if(bmf::prepare(sql)){ mpre("ОШИБКА запроса к БД", __LINE__);
			}else if(int count = sqlite3_column_count(bmf::stmt); false){ std::cerr << " ОШИБКА расчета количества записей в таблице";
			}else if([&](int id = 0){ while(SQLITE_ROW == sqlite3_step(bmf::stmt)){
					TMs row; string row_id; char *val;
					for(int num = 0; num < count; num++){
						string field = (string) sqlite3_column_name(bmf::stmt, num);
						val = (char*) sqlite3_column_text(bmf::stmt, num);
						string value = (val == NULL ? "" : val);
						row.insert(std::make_pair(field, value));
					} //exit(mpre(mask, __LINE__, "Маска"));
					int next_id = (row.end() == row.find("id") ? ++id : atoi(row["id"].c_str()));
					//
					TAB.insert(std::make_pair(next_id, row));
				}return false; }()){ mpre("ОШИБКА выгрузки данных", __LINE__);
			}else if(sqlite3_reset(bmf::stmt); false){ mpre("ОШИБКА скидывания шага", __LINE__);
			}else if(sqlite3_finalize(bmf::stmt); false){ mpre("ОШИБКА сериализации", __LINE__);
			}else{
			}return TAB; }); false){ mpre("ОШИБКА установки функции выборки списка таблциы", __LINE__);
		}else{
		}return false; }()){ mpre("ОШИБКА функции БД", __LINE__);
	}else if([&](bool skip = true){ // Подключение базы
		if(bmf::Tree = ([&](TMs bmf_index, int key){ // Отображение дерева
			if(string after_char = "  "; (0 >= after_char.length())){ mpre("ОШИБКА установки префикса отображения", __LINE__);
			}else if(TMMi STAIRS = [&](TMMi STAIRS = {}){ // Заполнение буфера
				if(bmf_index.end() == bmf_index.find("id")){ mpre("ОШИБКА у морфа не указан идентификатор", __LINE__);
				//}else if(stairs["index_id"] = bmf_index.at("id"); stairs.empty()){ mpre("ОШИБКА Установка идентификатора морфа", __LINE__);
				//}else if(stairs["after"] = after_char; stairs.empty()){ mpre("ОШИБКА установки первоначального отступа", __LINE__);
				}else if(TMs stairs = {{"index_id", bmf_index.at("id")}, {"after", after_char}}; stairs.empty()){ mpre("ОШИБКА создания следующей ступени", __LINE__);
				}else if(STAIRS.insert(make_pair(STAIRS.size(), stairs)); STAIRS.empty()){ mpre("ОШИБКА установки первой ступени", __LINE__);
				}else if(TMs dano = erb(BMF_DANO_EX, {{"id", bmf_index.at("dano_id")}}); dano.empty()){ mpre("ОШИБКА получения исходного значения", __LINE__);
				//}else if(bmf::MAPS.end() == bmf::MAPS.find(atoi(bmf_index.at("id").c_str()))){ mpre("ОШИБКА карта корневого морфа не найдена", __LINE__);
				}else if(boost::dynamic_bitset<> maps(bmf_index.at("map")); maps.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				}else if(std::string vals = [&](std::string vals = ""){ boost::to_string(maps, vals); return vals; }(); vals.empty()){ err("Строка старшей связи");
				}else if(std::string val = maps.test(key) ? "1" : "0"; (1 != val.length())){ mpre("ОШИБКА расчета значения морфа", __LINE__);
				//}else if(bmf::DANO.end() == bmf::DANO.find(atoi(bmf_index.at("dano_id").c_str()))){ mpre("ОШИБКА карта корневого морфа не найдена", __LINE__);
				}else if(boost::dynamic_bitset<> mps(dano.at("map")); mps.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				}else if(std::string vls = [&](std::string vls = ""){ boost::to_string(mps, vls); return vls; }(); vls.empty()){ err("Строка старшей связи");
				}else if(std::string vl = mps.test(key) ? "1" : "0"; (1 != vl.length())){ mpre("ОШИБКА расчета исходника морфа", __LINE__);
				}else if(bmf::COUNT.end() == bmf::COUNT.find(bmf_index.at("id"))){ mpre("ОШИБКА карта счетчика морфа не найдена", __LINE__);
				}else if(std::string count = bmf::COUNT.at(bmf_index.at("id")); count.empty()){ mpre("ОШИБКА выборки значение счетчика морфа", __LINE__);
				}else if(mpre("bmf_index["+ bmf_index["id"]+ "]=" +val +" dano["+ dano.at("id")+ "]=" +vl +" count=" +count +" maps=" +vals +" map=" +vls, __LINE__); false){ mpre("ОШИБКА отображения родителя", __LINE__);
				}else{ //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); //mpre(stairs, __LINE__, "Устанавливаем начало лестницы", __LINE__);
				} return STAIRS; }(); false){ mpre("ОШИБКА добавления элемента в буфер", __LINE__);
			}else if([&](int rep = 0){ do{ // Перебор ступеней лестницы
					if(STAIRS.empty()){ mpre("ОШИБКА лестница пуста", __LINE__);
					}else if(TMMi::reverse_iterator stairs_itr = STAIRS.rbegin(); (STAIRS.rend() == stairs_itr)){ mpre("ОШИБКА выборки итератора", __LINE__);
					}else if(TMs stairs = stairs_itr->second; stairs.empty()){ mpre("ОШИБКА выборки первой ступени", __LINE__);
					}else if(stairs.end() == stairs.find("index_id")){ mpre("ОШИБКА поле со ссылкой на морф у ступени не найдено", __LINE__);
					}else if(TMs index = erb(BMF_INDEX_EX, {{"id", stairs.at("index_id")}}); index.empty()){ mpre("ОШИБКА выборки морфа", __LINE__);
					}else if([&](){ // Статус
						if(int status = 1; (0 >= status)){ mpre("ОШИБКА статус должен быть положительным", __LINE__);
						}else if([&](){ // Инкремент статуса
							if(stairs.end() == stairs.find("status")){ //mpre("Статус не установлен", __LINE__);
							}else if(string::npos != stairs.at("status").find_last_not_of("0123456789")){ mpre("ОШИБКА формат status="+ stairs["status"], __LINE__);
							}else if([&](){ status = atoi(stairs.at("status").c_str())+1; return (0 >= status); }()){ mpre("ОШИБКА не допустимый статус", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Статус установка STAIRS["+ to_string(size-1)+ "] calc_pos="+ calc_pos.at("name")+ " status="+ to_string(status), __LINE__); //mpre(index["id"]+ " Обновленный статус "+ to_string(status), __LINE__);
							} return (0 >= status); }()){ mpre("ОШИБКА установки статуса", __LINE__);
						}else if([&](){ stairs_itr->second["status"] = stairs["status"] = to_string(status); return (0 >= stairs.at("status").length()); }()){ mpre("ОШИБКА установки статуса", __LINE__);
						}else{ //mpre(stairs["index_id"]+ " Установка STAIRS["+ to_string(STAIRS.size()-1)+ "] status="+ stairs["status"], __LINE__);
						} return /*(string::npos != stairs.at("status").find_last_not_of("123"));*/ false; }()){ mpre("ОШИБКА установки состояния", __LINE__);
					}else if(string index_field = [&](string index_field = ""){ // Направление дерева
						if(stairs.end() == stairs.find("status")){ mpre("ОШИБКА статус в ступени не установлен", __LINE__);
						}else if("1" == stairs.at("status")){ index_field = "index_id"; //mpre("Выбран старший переход", __LINE__);
						}else if("2" == stairs.at("status")){ index_field = "bmf-index"; //mpre("Выбрано младший переход", __LINE__);
						}else{ //mpre("", __LINE__);
						} return index_field; }(); (0 > index_field.length())){ mpre("Определение направления", __LINE__);
					}else if(TMs index_next = [&](TMs index_next = {}){ // Дочерний морф направления
						if("" == index_field){ //mpre("Направление не устанволено", __LINE__);
						}else if(index.end() == index.find(index_field)){ mpre("ОШИБКА поле с направления у морфа не найдены", __LINE__);
						}else if([&](){ index_next = erb(BMF_INDEX_EX, {{"id", index.at(index_field)}}); return false; }()){ mpre("ОШИБКА выборки морфа направления", __LINE__);
						}else{ //mpre(index_next, __LINE__, "Морф направления "+ index["id"]+ " "+ index_field);
						}; return index_next; }(); false){ mpre("ОШИБКА морф выборки дочернего морфа направления", __LINE__);
					}else if([&](){ // Отображение
						if(index_next.empty()){ //mpre("Пустой морф", __LINE__);
						}else if(TMs dano_next = erb(BMF_DANO_EX, {{"id", index_next.at("dano_id")}}); dano_next.empty()){ mpre("ОШИБКА получения исходника нижестоящего морфа", __LINE__);
						//}else if(bmf::MAPS.end() == bmf::MAPS.find(atoi(index_next.at("id").c_str()))){ mpre("ОШИБКА карта корневого морфа не найдена", __LINE__);
						}else if(boost::dynamic_bitset<> maps(index_next["map"]); maps.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
						}else if(std::string vals = [&](std::string vals = ""){ boost::to_string(maps, vals); return vals; }(); vals.empty()){ err("Строка старшей связи");
						}else if(std::string val = maps.test(key) ? "1" : "0"; (1 != val.length())){ mpre("ОШИБКА расчета значения морфа", __LINE__);
						//}else if(bmf::DANO.end() == bmf::DANO.find(atoi(index.at("dano_id").c_str()))){ mpre("ОШИБКА карта корневого морфа не найдена", __LINE__);
						}else if(boost::dynamic_bitset<> mps(dano_next["map"]); mps.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
						}else if(std::string vls = [&](std::string vls = ""){ boost::to_string(mps, vls); return vls; }(); vls.empty()){ err("Строка старшей связи");
						}else if(std::string vl = mps.test(key) ? "1" : "0"; (1 != vl.length())){ mpre("ОШИБКА расчета исходника морфа", __LINE__);
						}else if(mpre(stairs["after"]+ index_field+ " => index["+ index_next.at("id")+ "]="+val + " dano["+ dano_next.at("id")+ "]=" +vl +" count=" +" maps=" +vals + " map=" +vls, __LINE__); false){ mpre("ОШИБКА отображения", __LINE__);
						}else{
						}return false; }()){ mpre("Отображение морфа", __LINE__);
					}else if([&](){ // Добавление супени
						TMs stairs_next;
						if(index_next.empty()){ //mpre("Не добавляем пустую", __LINE__);
						}else if(index_next.end() == index_next.find("id")){ mpre("ОШИБКА идентификатор у морфа не задан", __LINE__);
						}else if(stairs_next["index_id"] = index_next.at("id"); stairs_next.empty()){ mpre("ОШИБКА создания ступени", __LINE__);
						}else if(stairs_next["after"] = stairs.at("after") +after_char; stairs_next.empty()){ mpre("ОШИБКА добавления отступа", __LINE__);
						}else if(STAIRS.insert(make_pair(STAIRS.size(), stairs_next)); STAIRS.empty()){ mpre("ОШИБКА добавления новой ступени", __LINE__);
						}else{ //mpre("Добавление новой ступени "+ index_next["id"], __LINE__);
						} return false; }()){ mpre("Добавление новой ступени", __LINE__);
					}else if([&](){ // Удаление
						if(!index_next.empty()){ //mpre("Не удаляем с нижней ступенью", __LINE__);
						}else if(stairs.end() == stairs.find("status")){ mpre(stairs["index_id"]+ "ОШИБКА Статус последней ступени не установлен", __LINE__);
						}else if(string::npos == stairs.at("status").find_last_not_of("1")){ //mpre("Не удаляем статус "+ stairs.at("status"), __LINE__);
						}else if(STAIRS.erase(STAIRS.size()-1); false){ mpre("Окончание пустая лестница", __LINE__);
						}else{ //mpre("Удаление ступени "+ stairs.at("status"), __LINE__);
						} return false; }()){ mpre("ОШИБКА удаления ступени", __LINE__);
					}else{ //mpre(stairs, __LINE__, "Ступень"); mpre(index, __LINE__, "Морф");
					}
				}while((1e3 > ++rep) && (0 < STAIRS.size())); return false; }()){ mpre("ОШИБКА цикла", __LINE__);
			}else{
			} return bmf_index; }); false){ mpre("ОШИБКА функции отображения дерева", __LINE__);
		}else if(bmf::Dec2bin = ([&](double decimal, string binary = ""){ // Получаем число возвращаем двойное число
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
		/*}else if(bmf::History = ([&](TMs index, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX, string values = ""){ //mpre("Получение истории морфа", __LINE__);
			if(false){ mpre("ОШИБКА", __LINE__);
			}else if(TMs index_1 = [&](TMs index_1 = {}){ // Старший потомок
				if("" == index.at("index_id")){ //mpre("Связь со старшим потомком не указана", __LINE__);
				}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
				}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(atoi(index.at("index_id").c_str()))){ mpre("ОШИБКА морф не найден в справочнике", __LINE__);
				}else if(index_1 = BMF_INDEX_EX.at("").at(atoi(index.at("index_id").c_str())); index_1.empty()){ mpre("ОШИБКА выборки старшего морфа index_id", __LINE__);
				}else{ //mpre(index_1, __LINE__, "Старший морф");
				} return index_1; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
			}else if(TMs index_0 = [&](TMs index_0 = {}){ // Младший потомок
				if("" == index.at("bmf-index")){ //mpre("Связь с младшим потомком не указана", __LINE__);
				}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
				}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(atoi(index.at("bmf-index").c_str()))){ mpre("ОШИБКА морф не найден в справочнике", __LINE__);
				}else if(index_0 = BMF_INDEX_EX.at("").at(atoi(index.at("bmf-index").c_str())); index_0.empty()){ mpre("ОШИБКА выборки младшего морфа bmf-index", __LINE__);
				}else{ //mpre(index_0, __LINE__, "Младший морф");
				} return index_0; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
			}else if(TMs dano = [&](TMs dano = {}){ // Исходник локального морфа
				if(_BMF_DANO_EX.at("").end() == _BMF_DANO_EX.at("").find(atoi(index.at("dano_id").c_str()))){ mpre("ОШИБКА связь морфа с дано не указана", __LINE__);
				}else if( dano = _BMF_DANO_EX.at("").at(atoi(index.at("dano_id").c_str())); dano.empty()){ mpre("Морф не найден в локальных исходниках", __LINE__);
				}else{ //mpre("Исходник морфа", __LINE__);
				} return dano; }(); dano.empty()){ mpre("ОШИБКА получения исходника морфа", __LINE__);
			}else if(string values_1 = (index_1.empty() ? dano.at("values") : bmf::History(index_1, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_1.length())){ mpre("ОШИБКА получения истории старшего морфа", __LINE__);
			}else if(string values_0 = (index_0.empty() ? dano.at("values") : bmf::History(index_0, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_0.length())){ mpre("ОШИБКА получения истории младшего морфа", __LINE__);
			}else if(TMs calc_pos = [&](TMs calc_pos = {}){ // Позиция морфа
				if(BMF_CALC_POS.end() == BMF_CALC_POS.find(atoi(index.at("calc_pos_id").c_str()))){ mpre("ОШИБКА позиция морфа не найдена", __LINE__);
				}else if(calc_pos = BMF_CALC_POS.at(atoi(index.at("calc_pos_id").c_str())); calc_pos.empty()){ mpre("ОШИБКА выборки позици морфа", __LINE__);
				}else{ //mpre(calc_pos, __LINE__, "Позиция морфа");
				} return calc_pos; }(); calc_pos.empty()){ mpre("ОШИБКА выборки позиции морфа", __LINE__);
			}else if(values = [&](string values = ""){ // Расчет истории морфа
				int pos = 0, size = min(values_1.length(), values_0.length());
				while(++pos <= size){
					if(string val_1 = values_1.substr(values_1.length()-size+pos-1, 1); (1 != val_1.length())){ mpre("ОШИБКА получения значения старшей истории", __LINE__);
					}else if(string val_0 = values_0.substr(values_0.length()-size+pos-1, 1); (1 != val_0.length())){ mpre("ОШИБКА получения значения младшей истории", __LINE__);
					}else if(string val = (((calc_pos.at("v1") == val_1) && (calc_pos.at("v1") == val_0)) ? calc_pos.at("calc-1") : calc_pos.at("calc-0")); (1 != val.length())){ mpre("ОШИБКА расчета значения", __LINE__);
					}else if(values += val; (0 >= values.length())){ mpre("ОШИБКА формирования истории", __LINE__);
					}else{ //mpre("Цикл "+ to_string(size)+ " "+ to_string(pos)+ " "+ values+ " ("+ values_1+ ","+ values_0+ ")", __LINE__);
						continue;
					} break;
				}; return values; }(); false){ mpre("ОШИБКА расчета истории морфа", __LINE__);
			}else{ //mpre(index, __LINE__, "Морф"); mpre("ОШИБКА нахождения истории "+ values, __LINE__);
			} return values; }); false){ mpre("ОШИБКА создания функции расчета истории", __LINE__);*/
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
			if(TMMi _DANO = BMF_DANO_EX.at(""); _DANO.empty()){ mpre("ОШИБКА сохранения списка исходников", __LINE__);
			}else if(stairs.end() == stairs.find("list")){ mpre("ОШИБКА список в ступени не задан", __LINE__);
			}else if(string list = stairs.at("list"); false){ mpre("История не установлена", __LINE__);
			}else if([&](int pos = 0, int loop = 2e9){ while((string::npos != (pos = list.find_last_of(","))) && (loop-- > 0) && (1 < _DANO.size())){ // Исключение повторяющихся исходников
				if(false){ mpre("Пропуск исключений", __LINE__);
				}else if(int dano_id =atoi(list.substr(pos +1, list.length()-1).c_str()); (0 == dano_id)){ mpre("ОШИБКА получения идентификатор исходника " +list, __LINE__);
				}else if(list =list.substr(0, pos); false){ mpre("ОШИБКА сокращение длинны списка", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_id)){ mpre(_DANO, "Список", __LINE__); mpre("ОШИБКА Исходник не найден в списке " +list +" "+ to_string(dano_id), __LINE__);
				}else if(_DANO.erase(dano_id); _DANO.empty()){ mpre("ОШИБКА не удаляем последнее значение", __LINE__);
				}else{ //mpre("Пропуск выбора исходника "+ to_string(dano_id) +" list=" +list, __LINE__);
				}}return _DANO.empty(); }()){ mpre("ОШИБКА список не должен оставаться пустым", __LINE__);
			}else if(bmf::dataset.end() == bmf::dataset.find("count")){ mpre("ОШИБКА поле количества элементов в наборе данных не установлено", __LINE__);
			}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества элементов в наборе данных", __LINE__);
			}else if(int prev = (!key ? dataset_count -1 : key -1); (0 > prev)){ mpre("ОШИБКА расчета предыдущей позиции", __LINE__);
			}else if([&](int pos = 0, bool loop = false){ for(auto dano_itr:BMF_DANO_EX.at("")){ // Перебор исходных значений
				if(1 >= _DANO.size()){ //mpre("Не удаляем последнее значение", __LINE__);
				}else if(_DANO.end() == _DANO.find(dano_itr.first)){ //mpre("Уже удален " +dano.at("id"), __LINE__);
				}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", dano_itr.second.at("id")}}, {}, {}); false){ mpre("ОШИБКА выборки карты исходника", __LINE__);
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
		}else if(bmf::Vals = ([&](TMs VALUE, string alias, int key, TMMt *BMF_VALS, TMMt *BMF_VALUES, TMMt *BMF_TITLES){ // Установка входных значений
			if([&](TMMi VALS = {}){ // Расчет знаков
				for(auto &val_itr:VALUE){ //mpre("VALUE ", __LINE__);
					if(string value = val_itr.second; false){ mpre("ОШИБКА получения значения итератора", __LINE__);
					}else if(TMs values = [&](TMs values = {}){ // Добавление значения
						if(string name = val_itr.first; (0 >= name.length())){ mpre("ОШИБКА получения имени значения", __LINE__);
						}else if(string _val = val_itr.second; false){ mpre("ОШИБКА получения значения из данных", __LINE__);
						//}else if(values = bmf::Erb(alias+ "_values", {{"name", _values}}); !values.empty()){ //mpre("Значение уже создано `"+ _values+ "`", __LINE__);
						//}else if(mpre(BMF_VALUES, "Проверка `" +_values +"`", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
						/*}else if([&](){ // Выборка значения из базы
							if(BMF_VALUES->end() == BMF_VALUES->find(_values)){ //mpre("Значения нет в базе", __LINE__);
							}else if(values = (*BMF_VALUES)[_values]; values.empty()){ err("Выборка значения из базы");
							}else{ //mpre("Выборка ранее созданного значения " + _values, __LINE__);
							}return !values.empty(); }()){ //mpre("Значение уже в базе ", _values, __LINE__);
						}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
						}else if(auto [itr, ret] = BMF_VALUES->insert(make_pair(_values, values)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
						}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);*/
						//}else if(std::string table = alias +"_values"; table.empty()){ mpre("ОШИБКА установки таблицы записи", __LINE__);
						//}else if(values = bmf::fk(table, {{"name", _values}}, values, {}); values.empty()){ mpre("ОШИБКА динамического обнолвения знака", __LINE__);
						//}else if(BMF_VALUES[values["name"]] = values; values.empty()){ mpre("ОШИБКА добавления значения в базу", __LINE__);
						//}else if(bmf::Erb_insert(alias +"_values", values["id"], values); false){ mpre("ОШИБКА добавления нового значения в справочник", __LINE__);
						/*}else if((values = [&](TMMt *ROWS, TMs row, bool up = 0, TMs _row = {}){ // Добавление заголовока в базу
							if(row.end() == row.find("id")){ mpre("У поля не задан идентификатор", __LINE__);
							}else if([&](){ // Редактирование значения в БД
								if(ROWS->end() == ROWS->find(row.at("id"))){ //mpre("Значение не найдено в базе", __LINE__);
								}else if(_row = (*ROWS)[row.at("id")]; _row.empty()){ mpre("ОШИБКА выборки значения из базы", __LINE__);
								}else if(!up){ //mpre("ОШИБКА обновления значения", __LINE__);
								}else if((*ROWS)[row.at("id")] = row; false){ mpre("ОШИБКА выборки значения из базы", __LINE__);
								}else if((_row = row).empty()){ mpre("ОШИБКА Обновленное значение", __LINE__);
								}else{ //mpre("Обновление значения", __LINE__);
								}return false; }(); !_row.empty()){ //mpre("Запись в базе", __LINE__);
							}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
							}else if(auto [_row, ret] = ROWS->insert(make_pair(row["id"], row)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
							}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
							}else{ mpre("Добавление заголовока в базу " +row.at("id"), __LINE__);
							}return _row; }(BMF_VALUES, _values)).empty()){ mpre("ОШИБКА добавления значения в БД", __LINE__);*/
						}else if(TMs _values = {{"name", name}, {"clump_id", bmf::clump_id}, {"value", _val}}; _values.empty()){ mpre("ОШИБКА фонмирования нового значения", __LINE__);
						}else if((values = bmf::Up(BMF_VALUES, {{"id", name}}, _values, {})).empty()){ mpre("ОШИБКА обновления значения", __LINE__);
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
						}else if([&](){ // Выборка ранее созданного значения
							if(BMF_TITLES->end() == BMF_TITLES->find(titles_id)){ //mpre("Заголовок не найден в базе", __LINE__);
							}else if(bmf_titles = (*BMF_TITLES)[titles_id]; bmf_titles.empty()){ mpre("ОШИБКА выборки значения заголовока", __LINE__);
							}else{ //mpre("Выборка ранее созданного заголовка " +value, __LINE__);
							}return !bmf_titles.empty(); }()){ //mpre("Значение найдено из ранее созданных", __LINE__);
						}else if([&](){ // Проверка на символ возврата коретки \r
							if(int npos = value.find("\r"); (std::string::npos != npos)){ mpre("ОШИБКА заголовок содержит символ возврат коретки " +val_itr.first +":" + value+"", __LINE__);
							}else if(int npos = value.find("\n"); (std::string::npos != npos)){ mpre("ОШИБКА заголовок содержит символ переноса строки " +val_itr.first +":" + value+"", __LINE__);
							}else{ //mpre("Данные не содержат недопустимых символов", __LINE__);
							} return false; }()){ mpre("ОШИБКА замены переноса строки", __LINE__);							
						}else if(int nn = [&](int nn = 0){ // Установка номера заголовка
							if(TMMi NN; false){ mpre("ОШИБКА создания первоначальной структуры", __LINE__);
							}else if([&](){ for(auto titles_itr:(*BMF_TITLES)){ //for_each(TITLES.begin(), TITLES.end(), [&](auto &titles_itr){
								if(TMs titles = titles_itr.second; titles.empty()){ mpre("ОШИБКА получения заголовка из списка", __LINE__);
								}else if(titles.end() == titles.find("value")){ mpre(titles, "Заголовок", __LINE__); err("Значение заголовока не установлено");
								}else if(int _nn = atoi(titles.at("value").c_str()); !_nn){ mpre("ОШИБКА расчета значения", __LINE__);
								}else if(NN.insert(make_pair(_nn, titles)); NN.empty()){ mpre("ОШИБКА добавления значения в список", __LINE__);
								}else{ mpre("Создаем список категорий", __LINE__);
								}}; return false; }()){ mpre("ОШИБКА получени списка по номерам", __LINE__);
							}else if(nn = (NN.empty() ? -1 : NN.begin()->first-1); (0 <= nn)){ mpre("ОШИБКА нахождения следующего номера "+ to_string(nn), __LINE__);
							}else{ mpre("Определение следующего значения заголовока " +to_string(nn), __LINE__);
							}return nn; }(); !nn){ mpre("ОШИБКА определения номера заголовка", __LINE__);
						}else if(TMs _bmf_titles = {{"clump_id", bmf::clump_id}, {alias+ "_values_id", values["id"]}, {"value", to_string(nn)}, {"name", value}}; _bmf_titles.empty()){ mpre("ОШИБКА получения нового заголовка", __LINE__);
						/*}else if([&](TMMt *ROWS, TMs row, bool up = 1, TMs _row = {}){ // Добавление заголовока в базу
							if(row.end() == row.find("id")){ mpre("У поля не задан идентификатор", __LINE__);
							}else if([&](){ // Редактирование значения в БД
								if(ROWS->end() == ROWS->find(row.at("id"))){ //mpre("Значение не найдено в базе", __LINE__);
								}else if(!up){ mpre("ОШИБКА значение найдено в базе", __LINE__);
								}else if((*ROWS)[row.at("id")] = row; false){ mpre("ОШИБКА выборки значения из базы", __LINE__);
								}else if((_row = row).empty()){ mpre("ОШИБКА Обновленное значение", __LINE__);
								}else{ //mpre("Обновление значения", __LINE__);
								}return false; }(); !_row.empty()){ //mpre("Обновление значения", __LINE__);
							}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
							}else if(auto [_row, ret] = ROWS->insert(make_pair(row["id"], row)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
							}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
							}else{ mpre("Добавление заголовока в базу " +titles_id, __LINE__);
							}return _row; }(BMF_TITLES, _bmf_titles).empty()){ mpre("ОШИБКА добавления значения в БД", __LINE__);*/
						}else if((bmf_titles = bmf::Up(BMF_TITLES, {{"id", titles_id}}, _bmf_titles, _bmf_titles)).empty()){ mpre("ОШИБКА обновления значения заголовка", __LINE__);
						/*}else if(true){ mpre("ОШИБКА установки заголовоков " +value, __LINE__);
						}else if(bmf_titles = bmf::Erb(alias +"_titles", {{alias+ "_values_id", values["id"]}, {"name", value}}); !bmf_titles.empty()){ //mpre("Заголовок найден в справочнике "+ value, __LINE__);
						}else if(TMMi TITLES = bmf::Rb(alias +"_titles", {{alias+ "_values_id", values["id"]}}); false){ mpre("ОШИБКА получения полного списка", __LINE__);
						}else if(int nn = [&](int nn = 0){ //mpre(bmf_titles, __LINE__, "Заголовок"); // Получение номера следующего заголовка
							if(false){ mpre("Пропуск", __LINE__);
							//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установка блокировки", __LINE__);
							}else if(TMMi NN; false){ mpre("ОШИБКА создания первоначальной структуры", __LINE__);
							}else if([&](){ for(auto &titles_itr:TITLES){ //for_each(TITLES.begin(), TITLES.end(), [&](auto &titles_itr){
								if(TMs titles = titles_itr.second; titles.empty()){ mpre("ОШИБКА получения заголовка из списка", __LINE__);
								}else if(titles.end() == titles.find("value")){ mpre("ОШИБКА поле value не установлено в заголовоке", __LINE__);
								}else if(string _nn = titles.at("value"); (0 >= _nn.length())){ mpre("ОШИБКА значение номера заголовока не задано", __LINE__);
								}else if(NN.insert(make_pair(atoi(_nn.c_str()), titles)); NN.empty()){ mpre("ОШИБКА добавления значения в список", __LINE__);
								}else{ //mpre(NN, __LINE__, "NN");
								} }; return false; }()){ mpre("ОШИБКА получени списка по номерам", __LINE__);
							}else if(nn = (NN.empty() ? -1 : NN.begin()->first-1); (0 <= nn)){ mpre("ОШИБКА нахождения следующего номера "+ to_string(nn), __LINE__);
							}else if([&](){ // Проверка на символ возврата коретки \r
								if(int npos = value.find("\r"); (std::string::npos != npos)){ mpre("ОШИБКА заголовок содержит символ возврат коретки " +val_itr.first +":" + value+"", __LINE__);
								}else if(int npos = value.find("\n"); (std::string::npos != npos)){ mpre("ОШИБКА заголовок содержит символ переноса строки " +val_itr.first +":" + value+"", __LINE__);
								//}else if(val.erase(npos, 1); false){ mpre("ОШИБКА замены переноса в значении", __LINE__);
								}else{ //mpre("Данные не содержат недопустимых символов", __LINE__);
								} return false; }()){ mpre("ОШИБКА замены переноса строки", __LINE__);							
							}else if(bmf_titles = {{"id", bmf::Id(BMF_TITLES)}, {"clump_id", bmf::clump_id}, {alias+ "_values_id", values["id"]}, {"value", to_string(nn)}, {"name", value}}; bmf_titles.empty()){ mpre("ОШИБКА получения нового заголовка", __LINE__);
							}else if(std::string table = alias +"_titles"; table.empty()){ mpre("ОШИБКА установки таблицы записи", __LINE__);
							}else if(bmf_titles = bmf::fk(table, {{"name", value}}, bmf_titles, {}); bmf_titles.empty()){ mpre("ОШИБКА сохранения заголовка", __LINE__);
							//}else if(bmf::Erb_insert(alias +"_titles", bmf_titles["id"], bmf_titles); false){ mpre("ОШИБКА добавления заголовка в справочник", __LINE__);
							}else{ //mpre(bmf_titles, __LINE__, "Значение следующего заголовка `"+ value+ "` "+ to_string(nn)); mpre("Поле "+ val_itr.first, __LINE__); mpre("ОШИБКА добавление заголовока value ===>" +value +"<=== key=" +to_string(key), __LINE__);
							} return nn; }(); (0 < nn)){ mpre("ОШИБКА определения номера заголовока `"+ value+ "`", __LINE__);*/
						}else{ //mpre(bmf_titles, __LINE__, "Заголовок"); mpre("ОШИБКА Установлено наличие заголовка", __LINE__);
						} return bmf_titles; }(); false){ mpre("ОШИБКА получения заголовка", __LINE__);
					}else if([&](){ // Расчитываем и устанавливаем значение
						if(value = (bmf_titles.empty() ? value : bmf_titles.at("value")); (0 >= value.length())){ mpre("ОШИБКА Значение не определено", __LINE__);
						}else if(values["value"] = value; values.empty()){ mpre("ОШИБКА установки результата значению", __LINE__);
						//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
						//}else if(BMF_VALUES.at("").at(atoi(values["id"].c_str())) = values; (BMF_VALUES.find("values") != BMF_VALUES.end())){ mpre(BMF_VALUES, __LINE__, "Справочник"); mpre("ОШИБКА сохранения результата в справочнике", __LINE__);
						//}else if(bmf::Erb_insert(alias +"_values", values["id"], values); false){ mpre("ОШИБКА добавления нового значения в справочник", __LINE__);
						}else{ //mpre(values, __LINE__, "Устанавливаем значние"); //mpre("Установка результата значению `"+ values["name"]+ "` "+ value, __LINE__);
						} return values.empty(); }()){ mpre("ОШИБКА установки результата значению", __LINE__);
					}else if(string _value = bmf::Dec2bin(stod(value)); (0 >= _value.length())){ mpre("ОШИБКА получения строки сиволов двоичного розультата "+ value, __LINE__);
					}else if(string _value_ = ("-" == _value.substr(0, 1) ? _value : "+"+ _value); (0 >= _value.length())){ mpre("ОШИБКА получения значения без знака", __LINE__);
					}else if(int pos = _value_.rfind("."); false){ mpre("ОШИБКА получения позиции точки", __LINE__);
					/*}else if(TMMg* BMF = [&](TMMg* BMF = NULL){ // Глобавльный справочник
						if(false){ mpre("Пропуск", __LINE__);
						//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
						}else if("dano" == alias){ BMF = &bmf::DANO;
						}else if("itog" == alias){ BMF = &bmf::ITOG;
						}else{ mpre("ОШИБКА выборки глобального справочника", __LINE__);
						} return BMF; }(); (nullptr == BMF)){ mpre("ОШИБКА выборки глобального справочника", __LINE__);*/
					//}else if(TMs VALS; false){ mpre("Создание промеждуточных данных для истории", __LINE__);
					}else if([&](){ // //mpre("Добавляем новый знак "+ values["name"]+ " "+ to_string(VALS.size())+ "=>"+ to_string(_value.length())+ " "+ _value+ " ("+ to_string(value)+ ")" , __LINE__); //system("sleep 1");
							for(int i = 0; i < _value_.length(); i++){ // Добавление знаков значению
							//for(int i = _value_.length()-1; i >=0 ; i--){ // Добавление знаков значению
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
									}else if(std::string vals_id = values.at("id") +to_string(nn); vals_id.empty()){ mpre("ОШИБКА составления идентификатора значения", __LINE__);
									//}else if(vals = bmf::Erb(alias, {{"clump_id", bmf::clump_id}, {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}}); !vals.empty()){ //mpre(vals, __LINE__, "Знак `"+ values["name"]+ "` уже добавлен "+ to_string(nn));
									/*}else if([&](){ // Выборка знака
										if(BMF_VALS->end() == BMF_VALS->find(vals_id)){ //mpre("Знак не найден в базе", __LINE__);
										}else if(vals = (*BMF_VALS)[vals_id]; vals.empty()){ mpre("ОШИБКА выборки значения из базы", __LINE__);
										}else{ //mpre("Выборка знака из базы", __LINE__);
										}return !vals.empty(); }()){ //mpre("Знак создан ранее", __LINE__);
									}else if(("-" != _value.substr(0, 1)) && (0 == nn)){ //mpre("Не создаем положительный знак отрицания", __LINE__);
									}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
									}else if(auto [itr, ret] = BMF_VALS->insert(make_pair(vals_id, vals)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
									}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);*/
									}else if(TMs _vals = {{"clump_id", bmf::clump_id}, {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}, {"val", ""}, {"values", ""}}; _vals.empty()){ mpre("ОШИБКА создания нового знака", __LINE__);
									}else if((vals = bmf::Up(BMF_VALS, {{"id", vals_id}}, _vals, {})).empty()){ mpre(_vals, "Знак", __LINE__); mpre("ОШИБКА обновления знака", __LINE__);
									}else{ //mpre("Добавление нового знака в базу " +vals_id, __LINE__); //mpre("ОШИБКА добавление глобального знака", __LINE__); //mpre(*BMF, __LINE__, "Список знаков"); mpre("Добавление глобального знака `"+ values["name"]+ "` ["+ vals.at("id")+ "] "+ to_string(nn), __LINE__); mpre(vals, __LINE__, "Знак"); //mpre("ОШИБКА добавления знака "+ alias+ " "+ vals.at("id"), __LINE__);
									} return vals; }(); false){ mpre("ОШИБКА Добавления нового знака", __LINE__);
								/*}else if(TMs _vals = [&](TMs _vals = {}){ // Синхронизация глобального значения с локальным
									if(vals.empty()){ //mpre("Не найден знак", __LINE__);
									//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА устанвоки блокировки", __LINE__);
									//}else if(_vals = (BMF_VALS.at("").end() == BMF_VALS.at("").find(atoi(vals.at("id").c_str())) ? TMs({}) : BMF_VALS.at("").at(atoi(vals.at("id").c_str()))); !_vals.empty()){ //mpre("Знак есть в локальной базе", __LINE__);
									}else if(_vals = bmf::Erb(alias +"_values", {{"id", vals.at("id")}}); !_vals.empty()){ //mpre("Знак есть в локальной базе", __LINE__);
									}else if(_vals = vals; _vals.empty()){ mpre("ОШИБКА копирования глобального значения", __LINE__); //mpre(_vals, __LINE__, "Знак `"+ values["name"]+ "` уже добавлен "+ to_string(nn));
									}else if(bmf::Erb_insert(alias +"_values", _vals.at("id"), _vals); _vals.empty()){ mpre("ОШИБКА обновления справочника знаков", __LINE__);
									}else{ //mpre("Добавление глобального знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); mpre(_vals, __LINE__, "Знак"); mpre("ОШИБКА добавления знака "+ alias+ " "+ _vals.at("id"), __LINE__);
									} return _vals; }(); false){ mpre("ОШИБКА Добавления локального знака", __LINE__);*/
								//}else if(mpre("Проверка 1.6", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
								/*}else if([&](){ // Установка свойств знака
									if(_vals.empty()){ //mpre("ОШИБКА знак не найден", __LINE__);
									}else if(_vals.at("val") = val; (1 != val.length())){ mpre("ОШИБКА установки знака знака", __LINE__);
									}else if(string _values = _vals.at("values")+ val; (0 >= _values.length())){ mpre("ОШИБКА инкремента знака к истории", __LINE__);
									}else if(_vals.at("values") = (bmf::values_length < _values.length() ? _values.substr(_values.length()-bmf::values_length, _values.length()) : _values); (0 >= _values.length())){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
									}else{ //mpre("Установка свойст знака", __LINE__);
									} return false; }()){ mpre("ОШИБКА установки свойств нового знака", __LINE__);*/
								}else if([&](){ // Установка значения
									if(vals.empty()){ //mpre("Отрицательный знак", __LINE__);
									}else if(vals.at("val") == val){ //mpre("Знак не изменился", __LINE__);
									}else if(vals.at("val") = val; (1 != val.length())){ mpre("ОШИБКА установки знака знака", __LINE__);
									}else if(vals.end() == vals.find("id")){ mpre("ОШИБКА идентификатора знака не задан", __LINE__);
									}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
									}else if((*BMF_VALS)[vals.at("id")] = vals; false){ mpre("ОШИБКА добавления значения в базу", __LINE__);
									}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
									}else{ //mpre("Изменение знака", __LINE__);
									}return false; }()){ mpre("ОШИБКА установки знака", __LINE__);
								/*}else if([&](){ // Сохранение
									if(_vals.empty()){ //mpre("ОШИБКА пустое значение знака", __LINE__);
									//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
									//}else if(BMF_VALS.at("").end() == BMF_VALS.at("").find(atoi(_vals["id"].c_str()))){ mpre("ОШИБКА знак не найден в списке", __LINE__);
									//}else if(BMF_VALS.at("").at(atoi(_vals["id"].c_str())).at("val") = _vals.at("val"); false){ mpre("ОШИБКА установки значения создан индекс `vals`", __LINE__);
									//}else if(BMF_VALS.at("").at(atoi(_vals["id"].c_str())).at("values") = _vals["values"]; false){ mpre("ОШИБКА установки значения создан индекс `values`", __LINE__);
									}else if(bmf::Erb_insert(alias +"_values", _vals.at("id"), _vals); false){ mpre("ОШИБКА сохранения локального знака", __LINE__);
									}else if(VALS.insert(make_pair(atoi(_vals.at("id").c_str()), _vals)); VALS.empty()){ mpre("ОШИБКА формирования списка измененных знаков", __LINE__);
									}else{ //mpre("Сохранение знака", __LINE__);
									} return false; }()){ mpre("ОШИБКА сохраенния знака", __LINE__);*/
								}else{ //mpre("Установка знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); //mpre(_vals, __LINE__, "Добавление нового знака "+ to_string(i));
								}
							}  return false;
						}()){ mpre("ОШИБКА добавления знаков значению", __LINE__);
					/*}else if([&](){ // Установка истории
						for(auto& vals_itr:bmf::Rb(alias +"_values", {{alias+ "_values_id", values["id"]}})){ // Установка истории
							if(TMs vals = vals_itr.second; vals.empty()){ mpre("ОШИБКА выборки знака", __LINE__);
							}else if([&](){ // Пропускаем проверяем на заполненность
								if(VALS.end() == VALS.find(atoi(vals.at("id").c_str()))){ //mpre("Знак не изменен", __LINE__);
								}else if(TMs _vals = VALS.at(atoi(vals.at("id").c_str())); _vals.empty()){ mpre("ОШИБКА выборки измененного знака", __LINE__);
								}else if(_vals.end() == _vals.find("val")){ mpre("ОШИБКА у измененного знака не установлено поле val", __LINE__);
								}else if(1 != _vals.at("val").length()){ mpre("ОШИБКА формате значения измененного знака", __LINE__);
								}else{ return true;
								} return false; }()){ //mpre("Уже измененный морф", __LINE__);
							}else if(vals.at("val") = "0"; (1 != vals.at("val").length())){ mpre("ОШИБКА установки знака знака", __LINE__);
							}else if(string _values = vals.at("values")+ vals.at("val"); (0 >= _values.length())){ mpre("ОШИБКА инкремента знака к истории", __LINE__);
							}else if(vals.at("values") = (bmf::values_length < _values.length() ? _values.substr(_values.length()-bmf::values_length, _values.length()) : _values); (0 >= _values.length())){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
							//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
							//}else if(BMF_VALS.at("").at(atoi(vals.at("id").c_str())).at("val") = vals.at("val"); vals.empty()){ mpre("ОШИБКА сохраенния расчетов", __LINE__);
							//}else if(BMF_VALS.at("").at(atoi(vals.at("id").c_str())).at("values") = vals.at("values"); vals.empty()){ mpre("ОШИБКА сохраенния расчетов", __LINE__);
							//}else if(bmf::Erb_insert(alias +"_values", vals.at("id"), vals); false){ mpre("ОШИБКА сохранения истории", __LINE__);
							}else{ //mpre("Скидывание значения "+ alias+ " _value_="+ _value_+ " "+ vals.at("id"), __LINE__);
							}
						} return false; }()){ mpre("ОШИБКА скидывания значения", __LINE__);*/
					}else{ //mpre(BMF_VALS.at(""), __LINE__, "Список знаков"); //mpre(BMF_VALS, __LINE__, "Знаки"); mpre("ОШИБКА Знака "+ alias, __LINE__); //mpre(values, __LINE__, "Значение"); mpre(VALS, __LINE__, "Значения");
					}
				}; return false; }()){ mpre("ОШИБКА расчета знаков", __LINE__);
			}else{ //mpre("Vals окончен", __LINE__);
			}return false; }); false){ mpre("ОШИБКА установки значений", __LINE__);
		}else if(bmf::Learn = ([&](TMs bmf_index, string val = "", int key, TMs stair = {}){ //mpre("Обучение "+ bmf_index.at("id"), __LINE__);  //system("sleep 1"); // mpre(BMF_INDEX, __LINE__, "Список"); // if(remove){ mpre(row, __LINE__, "Ученик"); }
			if(int rep = 2e9; 0 >= rep){ mpre("ОШИБКА установки предельного размера обучения для сети", __LINE__);
			//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
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
				}else if(TMs index = bmf::fk("index", {{"id", stairs.at("index_id")}}, {}, {}); index.empty()){ mpre("ОШИБКА обновления морфа", __LINE__);
				}else if(TMs dano = [&](TMs dano = {}){ // Получение исходника
					if(index.end() == index.find("dano_id")){ mpre("ОШИБКА у морфа ненайдено поле идентификатора источника", __LINE__);
					}else if(int dano_id = atoi(index.at("dano_id").c_str()); !dano_id){ mpre("ОШИБКА выборки идентификатора источника", __LINE__);
					}else if(BMF_DANO_EX.at("").end() == BMF_DANO_EX.at("").find(dano_id)){ mpre("ОШИБКА в справочнике источник не найден", __LINE__);
					}else if(dano = BMF_DANO_EX.at("").at(dano_id); dano.empty()){ mpre("ОШИБКА выборки исходника морфа", __LINE__);
					}else{ //mpre(dano, "Исходник морфа", __LINE__);
					}return dano; }(); dano.empty()){ mpre("ОШИБКА получения исходника", __LINE__);
				}else if(boost::dynamic_bitset<> map = [&](boost::dynamic_bitset<> map = {}){ // Карта морфа
					if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", dano.at("id")}}, {}, {}); false){ mpre("ОШИБКА выборки карты исходника", __LINE__);
					}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета размера обучающей выборки", __LINE__);
					}else if(map = boost::dynamic_bitset<>(dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); (dataset_count != map.size())){ mpre("ОШИБКА расчета карты исходника", __LINE__);
					//}else if(map = boost::dynamic_bitset<>(dataset_map.at("map")); map.empty()){ mpre("ОШИБКА загрузки данных карты", __LINE__);
					}else{ //mpre("Карта морфа " +dataset_map.at("map"), __LINE__);
					}return map; }(); map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				}else if(TMs index_1 = bmf::fk("index", {{"id", index.at("index_id")}}, {}, {}); false){ mpre("ОШИБКА выборки старшего потомка", __LINE__);
				}else if(boost::dynamic_bitset<> maps1 = [&](boost::dynamic_bitset<> maps1){ // Карта морфа
					if(index_1.empty()){ //mpre("Старшего морфа нет берем карту исходника", __LINE__);
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_1.at("id")}}; where.empty()){ mpre("ОШИБКА составления условия выборки карты", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);
					/*}else if(TMs dataset_map = [&](TMs dataset_map = {}){ // Выборка карты исходника
						if(dataset_map = bmf::fk("main.dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта найдена в оперативной БД", __LINE__);
						}else if(dataset_map = bmf::fk("file.dataset_map", where, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты в осноной БД", __LINE__);
						}else if(dataset_map.erase("id"); false){ mpre("Удаление идентификатора карты", __LINE__);
						}else if(dataset_map = bmf::fk("main.dataset_map", where, dataset_map, {}); dataset_map.empty()){ mpre("ОШИБКА сохранения краты в оперативную БД", __LINE__);
						}else{ mpre("Восстановление карты из оперативной БД", __LINE__);
						}return dataset_map; }(); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);*/
					}else if(maps1 = boost::dynamic_bitset<>(dataset_map.at("map")); maps1.empty()){ mpre(where, "Условия выборки", __LINE__); mpre("ОШИБКА загрузки данных карты", __LINE__);
					}else{ //mpre("Карта морфа " +dataset_map.at("map"), __LINE__);
					}return maps1; }(map); maps1.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
				}else if(TMs index_0 = bmf::fk("index", {{"id", index.at("bmf-index")}}, {}, {}); false){ mpre("ОШИБКА выборки младшего потомка", __LINE__);
				}else if(boost::dynamic_bitset<> maps0 = [&](boost::dynamic_bitset<> maps0){ // Карта морфа
					if(index_0.empty()){ //mpre("Старшего морфа нет берем карту исходника", __LINE__);
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_0.at("id")}}; where.empty()){ mpre("ОШИБКА формирования условий на выборку карты", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки itog_id=" +index.at("itog_id") +" ", __LINE__); mpre("ОШИБКА выборки карты исходника", __LINE__);
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
					}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}, {}, {}); false){ mpre("ОШИБКА установки карты расчетов нового морфа", __LINE__);
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
					}else if(TMs _dano = [&](TMs _dano = {}){ // Исходник нового морфа
						if("" == _stairs.at("other_id")){ _dano = bmf::Choice(index, stairs, _stairs, key); //mpre("Выбор лучшего источника");
						}else if(_dano = erb(BMF_DANO_EX, {{"id", index.at("dano_id")}}); _dano.empty()){ mpre("ОШИБКА выбоки исходника смежного морфа", __LINE__);
						}else{ //mpre(_dano, "Выборка исходника смежного морфа", __LINE__);
						} return _dano; }(); _dano.empty()){ mpre("ОШИБКА выборки нового морфа", __LINE__);
					}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
					//}else if(mpre(bmf::databases, "clump_id=" +clump_id, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else if(TMs _index = {{"clump_id", clump_id}, {"itog_values_id", index.at("itog_values_id")}, {"dano_id", _dano.at("id")}, {"itog_id", index.at("itog_id")}, {"index_id", ""}, {"bmf-index", ""}}; _index.empty()){ mpre("ОШИБКА создания нового морфа "+ index.at("id"), __LINE__);
					}else if(index.end() == index.find(_stairs.at("parent"))){ mpre("ОШИБКА у морфа поле родителя не найдено", __LINE__);
					}else if(_stairs.end() == _stairs.find("parent")){ mpre("ОШИБКА поле родителя не указано", __LINE__);
					//}else if(bmf::exec("BEGIN TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
					}else if(_index = bmf::fk("index", {}, _index, {}); _index.empty()){ mpre("ОШИБКА добавления нового морфа в БД", __LINE__);
					}else if(index_1 = ("index_id" == _stairs.at("parent") ? _index : index_1); false){ mpre("ОШИБКА обновления старшего морфа", __LINE__);
					}else if(index_0 = ("bmf-index" == _stairs.at("parent") ? _index : index_0); false){ mpre("ОШИБКА обновления младшего морфа", __LINE__);
					}else if(TMs dataset_map = bmf::Maps(_index, _stairs.at("parent")); dataset_map.empty()){ mpre("ОШИБКА установки карт нового морфа", __LINE__);
					}else if(index.at(_stairs.at("parent")) = _index.at("id"); index.empty()){ mpre("ОШИБКА установки поля родителя нового морфа", __LINE__);
					}else if(index = bmf::fk("index", {{"id", index.at("id")}}, {}, index); index.empty()){ mpre("ОШИБКА добавления морфа в БД", __LINE__);
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
			}else{ //Tree(bmf_index, _BMF_DANO_EX); //mpre(stair, __LINE__, "Результаты вычислений"); //mpre("Количество итераций "+ to_string(rep), __LINE__);
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
		}else if(bmf::Save = ([&](string table, TM3i& TABS, int count_insert = 0, int count_update = 0, int count_delete = 0, TMMi ROW = {}, int progress = 0){ // Расчет значения формулы
			if(TABS.end() == TABS.find("")){ //mpre("Список сохранения не задан", __LINE__);
			}else if(TMMi& INDEX = TABS.at(""); INDEX.empty()){ //mpre("Список пуст не сохраняем table=" +table, __LINE__);
			}else if(int remains = INDEX.size()%1000; false){ mpre("ОШИБКА получение делителя", __LINE__);
			}else if([&](){ for(auto index_itr = INDEX.rbegin(); index_itr != INDEX.rend(); index_itr++){ // Сохранение данных справочника
				if(TMs index = index_itr->second; index.empty()){ mpre("ОШИБКА выборки значения итератора", __LINE__);
				}else if([&](){ // Прогресс
					if(progress++ && progress%10000 && (progress != INDEX.size())){ //mpre("Отображаем прогресс первый, последний и кратный тысячи разы", __LINE__);
					}else if(bmf::Progress("Анализ изменений " +table +" " +to_string(progress), (float)progress/INDEX.size(), __LINE__); false){ mpre("ОШИБКА Индикатор прогресса", __LINE__);
					}else{ //mpre("Изменение прогресса не каждый раз а только через 1000 итераций", __LINE__);
					}return false; }()){ mpre("ОШИБКА отображения информации", __LINE__);
				}else if(int index_id = index_itr->first; (0 == index_id)){ mpre("ОШИБКА получения ключа строки", __LINE__);
				}else if([&](){ // Обновление
					if("0" != index["id"]){ //mpre("Не удаляем идентификатор не пуст", __LINE__);
					}else if(0 > index_id){ //mpre("Морф еще не сохранен в таблицу", __LINE__);
					}else if([&](){ bmf::exec("DELETE FROM `"+ table+ "` WHERE id="+ to_string(index_id)); return false; }()){ mpre("ОШИБКА удаления морфа", __LINE__);
					}else{ //mpre(index, __LINE__, "Удаление морфа");
						count_delete++;
					} return false; }()){ mpre("ОШИБКА удаления морфа", __LINE__);
				}else if([&](){ // Обновление
					if(0 > index_id){ //mpre("Не обновляем вновь добавленный "+ to_string(id), __LINE__);
					}else if("0" == index["id"]){ mpre("ОШИБКА Не обновляем удаленный морф", __LINE__);
					}else if([&](int pass = false){ // Ограничение редактирование не связанного с итогом морфа
						if("itog" != table){ //mpre("Ограничение только для итогов", __LINE__);
						}else if(index.end() == index.find("index_id")){ mpre("ОШИБКА у итога не найдено поле морфа", __LINE__);
						}else if(!index.at("index_id").empty()){ //mpre("Не пустое поле связи с морфом", __LINE__);
						}else{ pass = true; //mpre("Пропуск обновления итога " +index.at("id"), __LINE__);
						}return pass; }()){ //mpre("Пропуск редактирования итога не связанного с морфом " +index.at("id"), __LINE__);
					/*}else if([&](bool pass = false){ // Сохраняем только изменившиеся морфы
						if("index" != table){ //mpre("Только морфы", __LINE__);
						}else if(index.end() == index.find("index_id")){ mpre("ОШИБКА поле старшей связи не найдено", __LINE__);
						}else if(int index_id = atoi(index.at("index_id").c_str()); (0 > index_id)){ //mpre("Отрицательная старшая связь", __LINE__);
						}else if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА поле старшей связи не найдено", __LINE__);
						}else if(int bmf_index = atoi(index.at("bmf-index").c_str()); (0 > bmf_index)){ //mpre("Отрицательная младшая связь", __LINE__);
						}else if(pass = true; false){ mpre("Обновление морфа", __LINE__);
						}else{ //mpre("ОШИБКА расчета изменений ранее добавленных морфов", __LINE__);
						}return pass; }()){ //mpre(index, "Морф", __LINE__); mpre("Не сохраняем морф", __LINE__);*/
					}else if(index = bmf::fk(table, {{"id", index["id"]}}, {}, index); index.empty()){ mpre(index, __LINE__, "ОШИБКА обновления морфа");
					}else if(ROW[index_id] = index; index.empty()){ mpre(index, __LINE__, "ОШИБКА обновления справочника");
					}else{ //mpre(index, __LINE__, "Обновление морфа");
						count_update++;
					}return false; }()){ mpre("ОШИБКА обновления значения морфа", __LINE__);
				}else if([&](){ // Добавление
					if(0 < index_id){ //mpre("Не добавляем морф (создан ранее)", __LINE__);
					}else if("0" == index["id"]){ //mpre("Не обновляем удаленный морф", __LINE__);
					//}else if(TMs where = ("dano" == table ? TMs({{"name", index.at("name")}}) : TMs({})); false){ mpre("ОШИБКА расчетов условий на вставку", __LINE__);
					}else if(TMs where = [&](TMs where = {}){ // Условия обновления
						if("index" == table){ //mpre("Список морфов", __LINE__);
						}else if(index.end() == index.find("name")){ mpre("ОШИБКА поле имени не задано у записи", __LINE__);
						}else if(std::string name = index.at("name"); name.empty()){ mpre("ОШИБКА имя не указано table=" +table, __LINE__);
						}else if(where = {{"name", name}}; where.empty()){ mpre("ОШИБКА установки условия обновления", __LINE__);
						}else if(where = ("dano" == table ? TMs({{"name", name}, {"dano_values_id", index.at("dano_values_id")}}) : where); where.empty()){ mpre("ОШИБКА условия дано", __LINE__);
						}else if(where = ("itog" == table ? TMs({{"name", name}, {"itog_values_id", index.at("itog_values_id")}}) : where); where.empty()){ mpre("ОШИБКА условия итога", __LINE__);
						}else{ //mpre(where, "Условие обновления table=" +table, __LINE__);
						} return where; }(); false){ mpre("ОШИБКА установки условий обновления", __LINE__);
					}else if(index = bmf::fk(table, where, index, {}); index.empty()){ mpre("ОШИБКА сохранения нового значения в базу", __LINE__);
					}else if(ROW[index_id] = index; index.empty()){ mpre(index, __LINE__, "ОШИБКА Сохраняем в справочник");
					}else{ //mpre(where, "Условие добавления table=" +table, __LINE__); //mpre(index, __LINE__, "Добавление нового морфа "+ to_string(index_id));
						count_insert++;
					} return false; }()){ mpre("ОШИБКА добавления нового морфа", __LINE__);
				}else{ //exit(mpre(ROW, __LINE__, "Обновление "+ to_string(index_id)));
				} }return false; }()){ mpre("ОШИБКА перебора", __LINE__);
			}else if([&](){ if(INDEX.size()){ std::cerr << endl; }return false; }()){ mpre("ОШИБКА завершающего переноса", __LINE__);
			}else{
			}return ROW; }); false){ mpre("ОШИБКС создания функции сохранения в базу данных", __LINE__);
		}else if(bmf::Maps = ([&](TMs index ,std::string parent, TMs _dataset_map_ = {}){ // Расчет карт наборов данных
			if(BMF_DATASET_EX.end() == BMF_DATASET_EX.find("")){ mpre("ОШИБКА список обучающих выборок не установлен", __LINE__);
			}else if([&](){ for(auto dataset_itr:BMF_DATASET_EX.at("")){ // Расчет списка набора данных
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
				}else if(int dataset_count = atoi(dataset.end() == dataset.find("count") ? "0" : dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в выборке", __LINE__);
				}else if(boost::dynamic_bitset<> _mp = [&](boost::dynamic_bitset<> _mp = {}){ // Карта исходного значения
					if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "dano"}, {"alias_id", index.at("dano_id")}}, {}, {}); false){ mpre("ОШИБКА выборки карты расчетов исходников", __LINE__);
					}else if(_mp = boost::dynamic_bitset<>(dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); (dataset_count != _mp.size())){ mpre("ОШИБКА расчета карты исходника", __LINE__);
					}else{ //mpre("Расчет карты исодника", __LINE__);
					}return _mp; }(); (dataset_count != _mp.size())){ mpre("ОШИБКА выборки исходного значения морфа", __LINE__);
				}else if(boost::dynamic_bitset<> mp1 = [&](boost::dynamic_bitset<> mp1){ // Выборка карты основного морфа
					if(index.end() == index.find("index_id")){ mpre("ОШИБКА Поле старшего морфа не задано", __LINE__);
					}else if(std::string index_id = index.at("index_id"); index_id.empty()){ //mpre("Идентификатор старшего морфа не установлен", __LINE__);
					}else if(TMs where = {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", index_id}}; where.empty()){ mpre("ОШИБКА составления условий выборки", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки карты", __LINE__); mpre("ОШИБКА выборки карты расчетов", __LINE__);
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
					}else if(TMs where = {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", bmf_index}}; where.empty()){ mpre("ОШИБКА составления условий выборки", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); dataset_map.empty()){ mpre(where, "Условия выборки карты", __LINE__); mpre("ОШИБКА выборки карты расчетов", __LINE__);
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
				}else if(TMs _dataset_map = {{"clump_id", clump_id}, {"itog_id", index.at("itog_id")}, {"map", _map}}; _dataset_map.empty()){ mpre("ОШИБКА устанвоки карты", __LINE__);
				}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}, _dataset_map, _dataset_map); dataset_map.empty()){ mpre("ОШИБКА установки карты расчетов", __LINE__);
				}else if(bmf::dataset.at("id") != dataset.at("id")){ //mpre("Не возвращаем результатом " +bmf::dataset.at("id") +" " +dataset.at("id"), __LINE__);
				}else if(_dataset_map_ = dataset_map; _dataset_map_.empty()){ mpre("ОШИБКА установки значения возврата", __LINE__);
				}else{ //mpre("Расчет значения морфа ds=" +dataset.at("id") +" index_id=" +index.at("id"), __LINE__);
				}}return false; }()){ mpre("Расчет значения морфа", __LINE__);
			}else if(_dataset_map_.empty()){ mpre("ОШИБКА карта текущего набора данных не задана", __LINE__);
			}return _dataset_map_; }); false){ mpre("ОШИБКА расчета наборов данных", __LINE__);
		}else if(bmf::Reindex = ([&](TMs index, TMs bmf_index = {}){ // Реиндексация
			/*if(index.end() == index.find("id")){ mpre("ОШИБКА морф не содержит поля автоинкремента", __LINE__);
			}else if(TMs parent = bmf_index = index; bmf_index.empty()){ mpre("ОШИБКА сохранения иденкса для выхода", __LINE__);
			}else if([&](TMs _index = {}){ do{ // Реиндексация
				if(bmf_index = _index = parent; _index.empty()){ mpre("ОШИБКА установки родительского морфа", __LINE__);
				}else if(parent.clear(); false){ mpre("ОШИБКА обнуления родителя", __LINE__);
				}else if(_index.end() == _index.find("index_id")){ mpre("ОШИБКА у морфа не найдено поле старшей связи", __LINE__);
				}else if(int index_id = atoi(_index.at("index_id").c_str()); false){ mpre("ОШИБКА расчета идентификатора старшего потомка", __LINE__);
				}else if(_index.end() == _index.find("bmf-index")){ mpre("ОШИБКА у морфа не найдено поле младшей связи", __LINE__);
				}else if(int bmf_index = atoi(_index.at("bmf-index").c_str()); false){ mpre("ОШИБКА расчета идентификатора младшего потомка", __LINE__);
				}else if(std::string sql = "SELECT * FROM index WHERE index_id=" +_index.at("id") +" OR `bmf-index`=" + _index.at("id")+ " OR id IN(" +std::to_string(index_id) +"," +std::to_string(bmf_index) +")"; sql.empty()){ mpre("ОШИБКА составления запрос на выборку родителя", __LINE__);
				}else if(TM3i INDEX_EX = TM3i({{"" ,bmf::Tab(sql)}}); INDEX_EX.empty()){ mpre("Родитель не найден", __LINE__);
				}else if(_index.end() == _index.find("index_id")){ mpre("ОШИБКА не найдено поле связи старшего потомка", __LINE__);
				}else if(_index.end() == _index.find("bmf-index")){ mpre("ОШИБКА не найдено поле связи младшего потомка", __LINE__);
				}else if(TMs dano = [&](TMs dano = {}){ // Исходник старшей связи
					if(_index.end() == _index.find("dano_id")){ mpre("ОШИБКА морф не содержит поля исходной связи", __LINE__);
					}else if(dano = erb(BMF_DANO_EX, {{"id", _index.at("dano_id")}}); dano.empty()){ mpre("ОШИБКА получения исходника морфа", __LINE__);
					}else{ //mpre("Выборка исходника морфа", __LINE__);
					}return dano; }(); false){ mpre("ОШИБКА получения исходника старшей связи", __LINE__);
				}else if(TMs dano_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", dano.at("id")}}, {}, {}); dano_map.empty()){ mpre("ОШИБКА выборки карты старшего исходника", __LINE__);
				}else if(TMs index_1 = (_index.end() == _index.find("index_id") ? index_1 : erb(INDEX_EX, {{"index_id", _index.at("id")}})); false){ mpre("ОШИБКА выборки старшего потомка", __LINE__);
				}else if(TMs index_0 = (_index.end() == _index.find("bmf-index") ? index_0 : erb(INDEX_EX, {{"bmf-index", _index.at("id")}})); false){ mpre("ОШИБКА выборки младшего потомка", __LINE__);
				}else if(TMs dataset_map_1 = [&](TMs dataset_map_1 = {}){ // Карта старшего потомка
					if(index_1.empty()){ //mpre("Не расчитываем для пустого морфа", __LINE__);
					}else if(index_1.end() == index_1.find("id")){ mpre("ОШИБКА поле идентификатора морфа не найдено", __LINE__);
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_1.at("id")}}; where.empty()){ mpre("ОШИБКА создания условий на выборку карты морфа", __LINE__);
					}else if(dataset_map_1 = bmf::fk("dataset_map", where, {}, {}); dataset_map_1.empty()){ mpre("ОШИБКА выборки карты старшего морфа", __LINE__);
					}else{ //mpre("Выборка карты старшего морфа", __LINE__);
					}return dataset_map_1; }(); (!index_1.empty() && dataset_map_1.empty())){ mpre("ОШИБКА получения карты старшего потомка", __LINE__);
				}else if(TMs dataset_map_0 = [&](TMs dataset_map_0 = {}){ // Карта старшего потомка
					if(index_0.empty()){ //mpre("Не расчитываем для пустого морфа", __LINE__);
					}else if(index_0.end() == index_0.find("id")){ mpre("ОШИБКА поле идентификатора морфа не найдено", __LINE__);
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_0.at("id")}}; where.empty()){ mpre("ОШИБКА создания условий на выборку карты морфа", __LINE__);
					}else if(dataset_map_0 = bmf::fk("dataset_map", where, {}, {}); dataset_map_0.empty()){ mpre("ОШИБКА выборки краты младшего морфа", __LINE__);
					}else{ //mpre("Выборка карты старшего морфа", __LINE__);
					}return dataset_map_0; }(); (!index_0.empty() && dataset_map_0.empty())){ mpre("ОШИБКА получения карты младшего потомка", __LINE__);
				}else if(std::string field = [&](std::string field = ""){ // Выборка родителя
					if(parent = erb(INDEX_EX, {{(field = "index_id"), _index.at("id")}}); parent.empty()){ //mpre("Родитель найден по старшей связи", __LINE__);
					}else if(parent = erb(INDEX_EX, {{(field = "bmf-index"), _index.at("id")}}); parent.empty()){ //mpre("Родитель найден по младшей связи", __LINE__);
					}else{ mpre("Родитель не найден", __LINE__);
					}return field; }(); false){ mpre("ОШИБКА выборки родительского морфа", __LINE__);
				}else if(std::string map_1 = [&](std::string map_1 = ""){ // Карта старшего потомка
					if(dano_map.empty()){ mpre("ОШИБКА карта исходника пуста", __LINE__);
					}else if(dano_map.end() == dano_map.find("map")){ mpre("ОШИБКА в кате исходника отсутствует поле карты", __LINE__);
					}else if(map_1 = dano_map.at("map"); dataset_map_1.empty()){ //mpre("Карта их исходника", __LINE__);
					}else if(dataset_map_1.end() == dataset_map_1.find("map")){ mpre("ОШИБКА в карте морфа не найдено поле", __LINE__);
					}else if(map_1 = dataset_map_1.at("map"); map_1.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					}else{ //mpre("Строка из карты морфа " +map_1, __LINE__);
					}return map_1; }(); map_1.empty()){ mpre("ОШИБКА получения карты старшего потомка", __LINE__);
				}else if(std::string map_0 = [&](std::string map_0 = ""){ // Карта старшего потомка
					if(dano_map.empty()){ mpre("ОШИБКА карта исходника пуста", __LINE__);
					}else if(dano_map.end() == dano_map.find("map")){ mpre("ОШИБКА в кате исходника отсутствует поле карты", __LINE__);
					}else if(map_0 = dano_map.at("map"); dataset_map_0.empty()){ //mpre("Карта их исходника", __LINE__);
					}else if(dataset_map_0.end() == dataset_map_0.find("map")){ mpre("ОШИБКА в карте морфа не найдено поле", __LINE__);
					}else if(map_0 = dataset_map_0.at("map"); map_0.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					}else{ //mpre("Строка из карты морфа " +map_1, __LINE__);
					}return map_0; }(); map_0.empty()){ mpre("ОШИБКА получения карты старшего потомка", __LINE__);
				}else{ mpre("Карта field=" +field +" " +_index.at("id"), __LINE__); //mpre(INDEX, "Морф", __LINE__); //mpre("Ни одна связь не совпала", __LINE__);
				}}while(!(_index = parent).empty()); return false; }()){ mpre("ОШИБКА реиндексации морфа", __LINE__);
			}else{ //mpre("Реиндексация index_id=" +index.at("id"), __LINE__);
			}*/return bmf_index; }); false){ mpre("ОШИБКА функции реиндексации", __LINE__);
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
			}else if(bmf::Vals(dano, "dano", key, bmf::DANO, bmf::DANO_VALUES, bmf::DANO_TITLES); false){ mpre("ОШИБКА установки входящих значений", __LINE__);
			}else if([&](){ for(auto dano_itr:(*bmf::DANO)){ // Сохранение результатов
				if(dano_itr.second.end() == dano_itr.second.find("id")){ mpre("ОШИБКА поле идентификатора не найдено", __LINE__);
				}else if(std::string id = dano_itr.second.at("id"); id.empty()){ mpre("ОШИБКА получения идентификатора итога", __LINE__);
				}else if(dano_itr.second.end() == dano_itr.second.find("val")){ mpre("ОШИБКА поле значения не найдено", __LINE__);
				}else if(std::string val = dano_itr.second.at("val"); val.empty()){ mpre("ОШИБКА выборки знака", __LINE__);
				}else if(DANO[key][id] = val; false){ mpre("ОШИБКА сохарнения значения расчета", __LINE__);
				}else{ //mpre("Сохранения сигнала расчета", __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения результатов", __LINE__);
			}else if(TMs itog = TMs(el["itog"]); itog.empty()){ mpre("ОШИБКА получения итоговых знаков", __LINE__);
			}else if(bmf::Vals(itog, "itog", key, bmf::ITOG, bmf::ITOG_VALUES, bmf::ITOG_TITLES); false){ mpre("ОШИБКА установки входящих значений", __LINE__);
			//}else if(mpre("Расчет карты", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if([&](){ for(auto& itog_itr:(*bmf::ITOG)){ // Сохранение результатов
				if(itog_itr.second.end() == itog_itr.second.find("id")){ mpre("ОШИБКА поле идентификатора не найдено", __LINE__);
				}else if(std::string id = itog_itr.second.at("id"); id.empty()){ mpre("ОШИБКА получения идентификатора итога", __LINE__);
				}else if(itog_itr.second.end() == itog_itr.second.find("val")){ mpre("ОШИБКА поле значения не найдено", __LINE__);
				}else if(std::string val = itog_itr.second.at("val"); val.empty()){ mpre("ОШИБКА выборки значения расчета", __LINE__);
				}else if(ITOG[key][id] = val; false){ mpre("ОШИБКА сохарнения значения расчета", __LINE__);
				}else{ //mpre("Сохранения сигнала расчета", __LINE__);
				}}return false; }()){ mpre("ОШИБКА сохранения результатов", __LINE__);
			}else{ //mpre("Расчет " +to_string(key), __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА расчета карты", __LINE__);
		}else if([&](){ // Установка набора данных
			if(std::string _md5 = md5(bmf::ARGV.at("-")); _md5.empty()){ mpre("ОШИБКА расчета хеша набора обучающих данных", __LINE__);
			}else if(std::string dataset_id = _md5; dataset_id.empty()){ mpre("ОШИБКА установки идентификатора набора данных", __LINE__);
			}else if(TMs _dataset = {{"time", to_string(time(0))}, {"clump_id", bmf::clump_id}, {"count", to_string(in.size())}}; _dataset.empty()){ mpre("ОШИБКА создания записи нового набора данных", __LINE__);
			}else if((bmf::dataset = bmf::Up(bmf::DATASET, {{"id", dataset_id}}, _dataset, _dataset)).empty()){ mpre("ОШИБКА добавления набора данных", __LINE__);
			}else{ mpre("Набор данных " +bmf::dataset["id"] +" " +bmf::dataset["md5"], __LINE__);
			}return bmf::dataset.empty(); }()){ mpre("ОШИБКА устанвоки набора данных", __LINE__);
		//}else if(mpre("Проверка", __LINE__); true){ mpre("ОШИБКА уведомления", __LINE__);
		}else if([&](int progress = 0){ for(auto dano_itr:(*bmf::DANO)){ // Двоичная карта исходников
			if(bmf::Progress("Индекс исходных сигналов " +to_string(progress), (float)++progress/bmf::DANO->size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
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
			}else if(TMs _dataset_map = {{"id", dataset_map_id}, {"clump_id", clump_id}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА создания карты набора данных", __LINE__);
			}else if([&](TMs _row = {}){ // Проверка уже установленной карты
				if(_dataset_map.end() == _dataset_map.find("id")){ mpre("ОШИБКА не установлен идентификатор карты", __LINE__);
				}else if(bmf::DATASET_MAP->end() == bmf::DATASET_MAP->find(_dataset_map.at("id"))){ //mpre("Набор данных не найден в базе", __LINE__);
				}else if((*bmf::DATASET_MAP)[_dataset_map.at("id")] = _dataset_map; _dataset_map.empty()){ mpre("ОШИБКА выборки карты из базы", __LINE__);
				}else{ mpre("Выборка ранее загруженной карты в базу", __LINE__);
				}return _row; }().empty()){ //mpre("Карта установлена ранее", __LINE__);
			}else if(TMs _row = [&](TMMt *ROWS, TMs row, TMs _row = {}){ // Добавление заголовока в базу
				if(row.end() == row.find("id")){ mpre("У поля не задан идентификатор", __LINE__);
				}else if(DbTxn *txn = dbstl::begin_txn(0, bmf::env); (nullptr == txn)){ mpre("ОШИБКА создания транзакции", __LINE__);
				}else if(auto [_row, ret] = ROWS->insert(make_pair(row["id"], row)); !ret){ mpre("ОШИБКА добавления значения в базу", __LINE__);
				}else if(dbstl::commit_txn(bmf::env); false){ mpre("ОШИБКА закрытия транзакции", __LINE__);
				}else{ //mpre("Добавление заголовока в базу " +titles_id, __LINE__);
				}return _row; }(bmf::DATASET_MAP, _dataset_map); _row.empty()){ mpre("ОШИБКА добавления значения в БД", __LINE__);
			//}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", dano.at("id")}}, _dataset_map, _dataset_map); dataset_map.empty()){ mpre("ОШИБКА установки карты исходника набора данных", __LINE__);
			}else{ mpre("Установка карты морфа "+ dataset_map_id, __LINE__);//mpre("Исходник " +dano_itr.second.at("id") +" " +dano_itr.second.at("map"), __LINE__);
			} } std::cerr << endl; return false; }()){ mpre("ОШИБКА составления двоичной карты", __LINE__);
		}else if([&](int progress = 0){ for(auto& itog_itr:(*bmf::ITOG)){ // Двоичная карта итогов
			if(bmf::Progress("Индекс итоговых сигналов " +to_string(progress), (float)++progress/(*bmf::ITOG).size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
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
			}else if(TMs _dataset_map = {{"clump_id", clump_id}, {"itog_id", itog.at("id")}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА создания карты набора данных", __LINE__);
			}else if(bmf::Up(bmf::DATASET_MAP, {{"id", dataset_map_id}}, _dataset_map, _dataset_map).empty()){ mpre("ОШИБКА обновления набора данных", __LINE__);
			}else{ //mpre("Итог " +itog_itr.second.at("id") +" " +itog_itr.second.at("map"), __LINE__); //mpre(dano_itr.second, "Исходник", __LINE__);
			}} std::cerr << endl; return false; }()){ mpre("ОШИБКА составления двоичной карты", __LINE__);
		}else if([&](int progress = 0){ for(auto itog_itr:(*bmf::ITOG)){ // Расчет списка карт
			if(bmf::Progress("Расчет значений " +to_string(progress), (float)++progress/(*bmf::ITOG).size(), __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(itog_itr.second.end() == itog_itr.second.find("index_id")){ //mpre("ОШИБКА Поле идентификатора морфа не найдено у итога " +itog.at("id"), __LINE__);
			//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(boost::dynamic_bitset<> maps = ([&](boost::dynamic_bitset<> maps = {}){ // Функция строки расчета карты морфов
				if(false){ mpre("Пропуск расчетов", __LINE__);
				//}else if(TMs bmf_index = bmf::fk("index", {{"id", itog_itr.second["index_id"]}}, {}, {}); bmf_index.empty()){ //mpre("Пустой морф итога", __LINE__);
				}else if(itog_itr.second.end() == itog_itr.second.find("index_id")){ mpre("ОШИБКА поле корневого морфа у итога не найдено", __LINE__);
				}else if(TMs bmf_index = (*bmf::INDEX)[itog_itr.second.at("index_id")]; bmf_index.empty()){ mpre("ОШИБКА выборки корневого морфа", __LINE__);
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
					}else if(TMs index = bmf::fk("index", {{"id", stairs.at("index_id")}}, {}, {}); index.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(std::string map = [&](std::string map = {}){ // Выборка карты морфа
						if(index.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
						}else if(index.end() == index.find("dano_id")){ mpre("ОШИБКА не установлен идентификатор исходника", __LINE__);
						}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "dano"}, {"alias_id", index.at("dano_id")}}, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты исходника", __LINE__);
						}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА не найдено поле карты", __LINE__);
						}else if(map = dataset_map.at("map"); map.empty()){ mpre("ОШИБКА пустая карта исходника", __LINE__);
						}else{ //mpre("Карта исходника " +index.at("dano_id") +" " +map, __LINE__);
						}return map; }(); map.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
					}else if(TMs index_1 = bmf::fk("index", {{"id", index.at("index_id")}}, {}, {}); false){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs index_0 = bmf::fk("index", {{"id", index.at("bmf-index")}}, {}, {}); false){ mpre("ОШИБКА морф не найден", __LINE__);
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
							}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_1.at("id")}}, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты старшего морофа", __LINE__);
							}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА не найдено поле карты", __LINE__);
							}else if(map_1 = dataset_map.at("map"); map_1.empty()){ mpre("ОШИБКА пустая карта исходника", __LINE__);
							}else{ //mpre("Карта исходника " +index.at("dano_id") +" " +map, __LINE__);
							}return map_1; }(map); map_1.empty()){ mpre("ОШИБКА выборки карты морфа", __LINE__);
						}else if(std::string map_0 = [&](std::string map_0){ // Выборка карты морфа
							if(index_0.empty()){ //mpre("ОШИБКА морф не найден", __LINE__);
							}else if(index_0.end() == index_0.find("id")){ mpre("ОШИБКА не установлен идентификатор исходника", __LINE__);
							}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index_0.at("id")}}, {}, {}); dataset_map.empty()){ mpre("ОШИБКА выборки карты старшего морофа", __LINE__);
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
						//}else if(index = bmf::fk("index", {{"id", index.at("id")}, {"map", index.at("map")}}, {}, _index); index.empty()){ mpre("ОШИБКА сохранения карты", __LINE__);
						}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета базы", __LINE__);
						}else if(TMs _dataset_map = {{"clump_id", clump_id}, {"itog_id", itog_itr.second.at("id")}, {"map", map}}; _dataset_map.empty()){ mpre("ОШИБКА устанвоки карты", __LINE__);
						}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}, _dataset_map, _dataset_map); dataset_map.empty()){ mpre("ОШИБКА установки карты расчетов", __LINE__);
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
		//}else if(mpre("Остановка", __LINE__); true){ mpre("Уведомление", __LINE__);
		//}else if(bmf::exec("COMMIT TRANSACTION")){ mpre("Сбой закрытия транзакции сохранения набора данных", __LINE__);
		//}else if(bmf::Close(0)){ mpre("ОШИБКА закрытия БД", __LINE__);
		}else{ //mpre(DANO, "Расчет итогов", __LINE__);
		}return false; }()){ mpre("ОШИБКА расчета исходников", __LINE__);
	}else if(bmf::dataset = [&](){ // Выборка набора данных для расчета
		if(false){ mpre("Пропуск определения набора данных", __LINE__);
		}else if(bmf::ARGV.end() != bmf::ARGV.find("dano")){ //mpre("Расчетный пример", __LINE__);
		//}else if(BMF_DATASET_EX.end() == BMF_DATASET_EX.find("")){ mpre("ОШИБКА не найден список набора данных", __LINE__);
		}else if(std::string ds = (bmf::ARGV.end() == bmf::ARGV.find("ds") ? "" : bmf::ARGV.at("ds")); false){ mpre("ОШИБКА выборки идентификатора набора данных", __LINE__);
		//}else if(bmf::dataset = erb(BMF_DATASET_EX, {{"id", ds}}); !bmf::dataset.empty()){ //mpre("Набор данных указанный в командной строке", __LINE__);
		}else if(bmf::dataset = bmf::Up(bmf::DATASET, {{"id", ds}}, {}, {}); !bmf::dataset.empty()){ //mpre({{"id", ds}}, "Условия выборки набора данных " +to_string((*bmf::DATASET).size()), __LINE__); mpre("ОШИБКА выборки набора данных", __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> ITOG; false){ mpre("Справочник карт", __LINE__);
		}else if(std::map<string, boost::dynamic_bitset<>> INDEX; false){ mpre("Справочник карт", __LINE__);
		//}else if(bmf::Open(0)){ mpre("ОШИБКА подключения к БД", __LINE__);
		}else if([&](){ for(auto& dataset_itr:(*bmf::DATASET)){ // Вывод списка набора данных
			if(TMs dataset = dataset_itr.second; dataset.empty()){ mpre("ОШИБКА выборки набора данных", __LINE__);
			}else if(dataset.end() == dataset.find("count")){ mpre("ОШИБКА поле количества примеров не установлено", __LINE__);
			}else if(int dataset_count = atoi(dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета количества примеров в наборе данных", __LINE__);
			}else if(BMF_ITOG_EX.end() == BMF_ITOG_EX.find("")){ mpre("ОШИБКА список итогов не установлен", __LINE__);
			}else if(float diff = [&](int ers = 0, int count = 0){ for(auto itog_itr:BMF_ITOG_EX.at("")){ // Расчет результата поитогово
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА выборки итога", __LINE__);
				}else if(TMs itog_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "itog"}, {"alias_id", itog.at("id")}}, {}, {}); itog_map.empty()){ //mpre(TMs({{"dataset_id", dataset.at("id")}, {"alias", "itog"}, {"alias_id", itog.at("id")}}), "Условия", __LINE__); //mpre("ОШИБКА выборки расчета итога", __LINE__);
				}else if(boost::dynamic_bitset<> gmap(itog_map.at("map")); gmap.empty()){ mpre("ОШИБКА установки карты итога", __LINE__);
				}else if(ITOG[itog.at("id")] = gmap; ITOG.empty()){ mpre("ОШИБКА сохраенения карты итога", __LINE__);
				}else if(TMs index_map = bmf::fk("dataset_map", {{"dataset_id", dataset.at("id")}, {"alias", "index"}, {"alias_id", itog.at("index_id")}}, {}, {}); false){ mpre("ОШИБКА выборки расчета итога", __LINE__);
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
		//}else if(BMF_DANO_EX.end() == BMF_DANO_EX.find("")){ mpre("Поле списка исходников не найдено", __LINE__);
		//}else if(0 >= BMF_DANO_EX.at("").size()){ mpre("ОШИБКА Не найдены исходники", __LINE__);
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
			//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(std::string itog_str = (bmf::ARGV.end() == bmf::ARGV.find("itog") ? "" : "." +bmf::ARGV.at("itog")); false){ mpre("ОШИБКА формирования итога в консоли", __LINE__);
			}else if(bmf::Progress("Набор:" +bmf::dataset.at("id") + itog_str+" Эпоха:" +epoch +" Примеров:" +to_string(progress) +" Изменений:"+ to_string(err)+ " (" +to_string(_microtime) +" сек.)", (float)++progress/dataset_count, __LINE__); false){ mpre("Индикатор прогресса", __LINE__);
			}else if(std::string itog_id = [&](){ // Получение итога консоли
				if(itog_id = (bmf::ARGV.end() == bmf::ARGV.find("itog") ? "" : bmf::ARGV.at("itog")); false){ mpre("ОШИБКА выборки номера итога", __LINE__);
				}else if(std::string sql = "SELECT * FROM index WHERE " +(itog_id.empty() ? "1" : "itog_id=" +itog_id); sql.empty()){ mpre("ОШИБКА формирования запроса выбора", __LINE__);
				}else if(TMMi INDEX = bmf::Tab(sql); INDEX.empty()){ //mpre("Пустой список на реиндексацию", __LINE__);
				/*}else if([&](){ for(auto index_itr:INDEX){ // Реиндексация морфов
					if(TMs index = index_itr.second; index.empty()){ mpre("ОШИБКА выборки морфа", __LINE__);
					}else if(index.end() == index.find("id")){ mpre("ОШИБКА у морфа не найдено поле идентификатора", __LINE__);
					}else if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}; where.empty()){ mpre("ОШИБКА получения условия выборки карты", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); !dataset_map.empty()){ //mpre("Карта не пуста", __LINE__);
					}else if(TMs bmf_index = bmf::Reindex(index); bmf_index.empty()){ mpre("ОШИБКА реиндексации морфа", __LINE__);
					}else{ //mpre("Реиндексация", __LINE__);
					}}return false; }()){ mpre("ОШИБКА реиндексации", __LINE__);*/
				}else{ //mpre("ОШИБКА Реиндексации морфов", __LINE__);
				}return itog_id; }(); false){ mpre("ОШИБКА реиндексации морфов", __LINE__);
			}else if([&](){ for(auto itog_itr:(*bmf::ITOG)){ // Проверка списка итогов
				if(false){ mpre("Пропуск расчета", __LINE__);
				}else if(TMs itog = itog_itr.second; itog.empty()){ //mpre("ОШИБКА выборки итога", __LINE__);
				}else if(!itog_id.empty() && (itog.at("id") != itog_id)){ //mpre("Пропуск расчетов", __LINE__);
				}else if(itog.end() == itog.find("id")){ mpre("ОШИБКА не найден идентификатор итога", __LINE__);
				}else if(TMs dataset_map = bmf::fk("dataset_map", {{"dataset_id", bmf::dataset.at("id")}, {"alias", "itog"}, {"alias_id", itog.at("id")}}, {}, {}); dataset_map.empty()){ //mpre("Неизвестное итоговое значение", __LINE__); //mpre("ОШИБКА установки карты исходника набора данных", __LINE__);
				}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА поле карты и неайдено у итога", __LINE__);
				}else if(boost::dynamic_bitset<> ITOG(dataset_map.at("map")); ITOG.empty()){ mpre("ОШИБКА получения результата расчета", __LINE__);
				}else if(key >= ITOG.size()){ mpre("ОШИБКА карта итога меньше количество примеров в выборке", __LINE__);
				//}else if(std::string prefix = (bmf::dataset.end() == bmf::dataset.find("file") ? "file" : "main"); prefix.empty()){ mpre("ОШИБКА составления префикса базы данных", __LINE__);
				}else if(bmf::exec("BEGIN TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
				}else if(TMs index = [&](TMs index = {}){ // Добавление первоначального морфа
					if(index = bmf::fk("index", {{"id", itog.at("index_id")}}, {}, {}); !index.empty()){ //mpre("Морф уже создан", __LINE__);
					}else if(TMs dano = BMF_DANO_EX.at("").begin()->second; dano.empty()){ mpre("ОШИБКА выборки первого дано", __LINE__);
					}else if(std::string clump_id = (bmf::databases.end() == bmf::databases.find("file") ? bmf::clump_id : ":memory:"); clump_id.empty()){ mpre("ОШИБКА расчета признака базы", __LINE__);
					}else if(TMs _index = {{"id", bmf::Id(bmf::INDEX)}, {"clump_id", clump_id}, {"itog_values_id", itog.at("itog_values_id")}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}, {"index_id", ""}, {"bmf-index", ""}}; _index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
					}else if(index = bmf::fk("index", {}, _index, {}); index.empty()){ mpre("ОШИБКА обновления морфа", __LINE__);
					}else if(TMs dataset_map = bmf::Maps(index, "bmf-index"); dataset_map.empty()){ mpre("Установка карт", __LINE__);
					}else if(itog["index_id"] = index.at("id"); itog.empty()){ mpre("ОШИБКА установки свойства связи итога с морфом", __LINE__);
					}else if(TMs _itog = bmf::fk("itog", {{"id", itog.at("id")}}, {}, itog); _itog.empty()){ mpre("ОШИБКА обновления итога " +itog.at("id"), __LINE__);
					}else if(erb_insert(BMF_ITOG_EX, itog.at("id"), _itog); _itog.empty()){ mpre("ОШИБКА индексирования итога", __LINE__);
					}else if(bmf::COUNT[index.at("id")] = "0"; (bmf::COUNT.end() == bmf::COUNT.find(index.at("id")))){ mpre("ОШИБКА установки счетчика корневого морфа", __LINE__);
					}else{ //mpre("Добавление первоначального морфа " +index.at("id"), __LINE__);
					}return index; }(); index.empty()){ mpre("ОШИБКА морф связи не найден", __LINE__);
				}else if(int dataset_count = atoi(bmf::dataset.at("count").c_str()); !dataset_count){ mpre("ОШИБКА расчета размера карты", __LINE__);
				}else if(std::string map = [&](std::string map = ""){ // Карта корневого морфа
					if(TMs where = {{"dataset_id", bmf::dataset.at("id")}, {"alias", "index"}, {"alias_id", index.at("id")}}; where.empty()){ //mpre("ОШИБКА задания условий на выборку карты", __LINE__);
					}else if(TMs dataset_map = bmf::fk("dataset_map", where, {}, {}); false){ mpre("ОШИБКА выборки карты корневого морфа", __LINE__);
					}else if(map = (dataset_map.end() == dataset_map.find("map") ? std::string(dataset_count, '0') : dataset_map.at("map")); map.empty()){ mpre("ОШИБКА установки карты основного морфа", __LINE__);
					}else if(!dataset_map.empty()){ //mpre("Уже найден в базе", __LINE__);
					}else if(dataset_map = bmf::fk("dataset_map", where, {{"map", map}, {"itog_id", index.at("itog_id")}}, {}); dataset_map.empty()){ mpre("ОШИБКА добавления карты корневого морфа", __LINE__);
					}else if(dataset_map.end() == dataset_map.find("map")){ mpre("ОШИБКА в карте нет поля битовой карты", __LINE__);
					}else if(map = dataset_map.at("map"); map.empty()){ mpre("ОШИБКА выборки карты корневого морфа", __LINE__);
					}else{ mpre("Добавление карты корневого морфа index_id=" +index.at("id") +" map=" +map, __LINE__);
					}return map; }(); false){ mpre("ОШИБКА расчета карты корневого морфа", __LINE__);
				//}else if(bmf::exec("COMMIT TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
				}else if(boost::dynamic_bitset<> INDEX(map); (dataset_count != map.length())){ mpre("ОШИБКА не верный размер карты", __LINE__);
				}else if(std::string learn = (ITOG.test(key) ? "1" : "0"); (1 != learn.length())){ mpre("ОШИБКА получения бит", __LINE__);
				}else if(std::string calc = (INDEX.test(key) ? "1" : "0"); (1 != calc.length())){ mpre("ОШИБКА получения расчета", __LINE__);
				}else if([&](){ // Обучение
					if(learn == calc){ //mpre("Сравнение itog[" +itog.at("id") +"]=" +calc, __LINE__);
					//}else if(mpre("Начало транзакции", __LINE__); false){ mpre("ОШИБКА транзакции", __LINE__);
					//}else if(bmf::exec("BEGIN TRANSACTION")){ mpre("Сбой запуска начала транзакции", __LINE__);
					}else if(string _calc = bmf::Learn(index, learn, key); (1 <= _calc.length() >= 2)){ mpre("ОШИБКА обучения морфа значение не верная длинна результата", __LINE__);
					//}else if(mpre("Конец транзакции", __LINE__); false){ mpre("ОШИБКА транзакции", __LINE__);
					}else if(err+=1; !err){ mpre("Инкремент ошибок", __LINE__);
					}else{ //mpre("Обучение " +index["map"], __LINE__);
					}return false; }()){ mpre("ОШИБКА обучения", __LINE__);
				}else if(bmf::exec("COMMIT TRANSACTION")){ mpre("Сбой закрытия транзакции", __LINE__);
				}else{ //mpre("Обучение сигнала " +to_string(key) +" itog[" +itog.at("id") +"]", __LINE__);
				} } return false; }(); false){ mpre("ОШИБКА сравнения результата расчета", __LINE__);
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
		if(true){ mpre("Пропуск расчетов", __LINE__);
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
	}else if([&](){ // Сохранение	
		if(true){ //mpre("Пропуск сохранения", __LINE__);
		//if(bmf::Open(0)){ mpre("ОШИБКА подключения к БД", __LINE__);
		}else if([&](){ //mpre("Сохранение результатов в тест", __LINE__);
			if(bmf::exec("CREATE TABLE IF NOT EXISTS test (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `date` TEXT, `size` INTEGER, `args` TEXT, `change` INTEGER, `duration` INTEGER, `index` INTEGER, `epoch` INTEGER, `loop` INTEGER, `first` REAL, `last` REAL, `clump` TEXT)"); false){ mpre("ОШИБКА создания значения итога", __LINE__);
			}else if(string date = [&](string date = ""){ char mbstr[100]; time_t t = time(nullptr); std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d_%X", std::localtime(&t)); date = string(mbstr); return date; }(); (0 >= date.length())){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(nlohmann::json arg = [&](nlohmann::json arg = {}){ arg = bmf::ARGV; arg.erase("-"); arg.erase("dano"); return arg; }(); arg.empty()){ mpre("ОШИБКА получения строки аргументов", __LINE__);
			}else if(auto microtime = (std::chrono::system_clock::now().time_since_epoch()).count()/1e9 -bmf::microtime; false){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(std::string first = [&](std::string _first = ""){ char first[10]; sprintf(first, "%0.4f", bmf::pips_first); _first = first; return _first; }(); false){ mpre("ОШИБКА получения первого процента", __LINE__);
			}else if(std::string last = [&](std::string _last = ""){ char last[10]; sprintf(last, "%0.4f", bmf::pips_last); _last = last; return _last; }(); false){ mpre("ОШИБКА получения первого процента", __LINE__);
			}else if(std::string epoch = (bmf::ARGV.end() == bmf::ARGV.find("epoch") ? "" : bmf::ARGV.at("epoch")); false){ mpre("ОШИБКА расчета количества эпох указанного в консоли", __LINE__);
			}else if(std::string size = (BMF_INDEX_EX.end() == BMF_INDEX_EX.find("") ? "" : to_string(BMF_INDEX_EX.at("").size())); false){ mpre("ОШИБКА расчета размера модели", __LINE__);
			}else if(TMs test = bmf::fk("test", {}, {{"time", to_string(bmf::timestamp)}, {"date", date}, {"size", to_string(in.size())}, {"change", to_string(errors)}, {"duration", to_string(microtime)}, {"clump", bmf::clump_id}, {"epoch", epoch}, {"loop", to_string(bmf::loop)}, {"first", first}, {"last", (0 <= bmf::pips_last ? last : "")}, {"index", size}, {"args", arg.dump()}}, {}); test.empty()){ mpre("ОШИБКА сохранения результатов тестов", __LINE__);
			}else{ //mpre("Сохранение статистики id "+ test["id"], __LINE__);
			} return false; }()){ mpre("ОШИБКА сохранения теста", __LINE__);
		}else if(true){ //mpre("Пустой массив обучающей выборки. Не сохраняем данные в БД", __LINE__);
		}else if(bmf::exec("BEGIN TRANSACTION"); false){ mpre("ОШИБКА запуска начала транзакции", __LINE__);
		}else if(TMMi _BMF_DANO_VALUES = bmf::Save("dano_values", BMF_DANO_VALUES_EX); _BMF_DANO_VALUES.empty()){ mpre("ОШИБКА сохранения изначальных значений", __LINE__);
		}else if(TMMi _BMF_DANO = bmf::Save("dano", BMF_DANO_EX); _BMF_DANO.empty()){ mpre("ОШИБКА сохранения исходных данных", __LINE__);
		}else if(TMMi _BMF_DANO_TITLES = bmf::Save("dano_titles", BMF_DANO_TITLES_EX); false){ mpre("ОШИБКА сохранения изначальных заголовков", __LINE__);
		}else if(TMMi _BMF_ITOG_VALUES = bmf::Save("itog_values", BMF_ITOG_VALUES_EX); _BMF_ITOG_VALUES.empty()){ mpre("ОШИБКА сохранения итоговых значений", __LINE__);
		}else if(TMMi _BMF_ITOG = bmf::Save("itog", BMF_ITOG_EX); false){ mpre("ОШИБКА сохранения итоговых данных", __LINE__);
		}else if(TMMi _BMF_ITOG_TITLES = bmf::Save("itog_titles", BMF_ITOG_TITLES_EX); false){ mpre("ОШИБКА сохранения итоговых заголовков", __LINE__);
		}else if(TMMi _BMF_INDEX = bmf::Save("index", BMF_INDEX_EX); false){ mpre("ОШИБКА сохранения справочника морфа", __LINE__);
		}else if([&](){ for(auto &itog_itr:_BMF_ITOG){ //for_each(_BMF_ITOG.begin(), _BMF_ITOG.end(), [&](auto itog_itr){ // Корректировка связи с итогом
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if(TMs _itog = itog_itr.second; _itog.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if("" == itog["index_id"]){ //mpre(BMF_ITOG_EX.at(""), __LINE__, "Итог"); //mpre("ОШИБКА Идентификатор морфа не указан", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(atoi(itog["index_id"].c_str()))){ //mpre("Морф не найден", __LINE__);
					}else if(TMs _index = _BMF_INDEX.at(atoi(itog["index_id"].c_str())); _index.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_index.end() == _index.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if([&](){ itog["index_id"] = _index.at("id"); return false; }()){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(itog, "Итог", __LINE__); mpre(_index, "Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_ITOG_VALUES.end() == _BMF_ITOG_VALUES.find(atoi(itog.at("itog_values_id").c_str()))){ mpre("ОШИБКА морф не значение", __LINE__);
					}else if(TMs _itog_values = _BMF_ITOG_VALUES.at(atoi(itog.at("itog_values_id").c_str())); _itog_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_itog_values.end() == _itog_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if([&](){ itog["itog_values_id"] = _itog_values.at("id"); return false; }()){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(itog == _itog){ //mpre("Изменений в итоге не найдено", __LINE__);
				}else if([&](){ _itog = bmf::fk("itog", {{"id", itog["id"]}}, {}, itog); return _itog.empty(); }()){ mpre(itog, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(itog, "Сохранение итога ", __LINE__);
				}
			}; return false; }()){ mpre("ОШИБКА корректировки свзи с итогом", __LINE__);
		}else if([&](){ for(auto &dano_itr:_BMF_DANO){ //for_each(_BMF_DANO.begin(), _BMF_DANO.end(), [&](auto dano_itr){ // Корректировка связи с итогом
				if(TMs dano = dano_itr.second; dano.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if(TMs _dano = dano_itr.second; _dano.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_DANO_VALUES.end() == _BMF_DANO_VALUES.find(atoi(dano.at("dano_values_id").c_str()))){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs _dano_values = _BMF_DANO_VALUES.at(atoi(dano.at("dano_values_id").c_str())); _dano_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_dano_values.end() == _dano_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if([&](){ dano["dano_values_id"] = _dano_values.at("id"); return false; }()){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(dano == _dano){ //mpre("Изменений в итоге не найдено", __LINE__);
				}else if([&](){ _dano = bmf::fk("dano", {{"id", dano["id"]}}, {}, dano); return _dano.empty(); }()){ mpre(dano, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(_index, __LINE__, "Связь 1"); mpre(index, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			}; return false; }()){ mpre("ОШИБКА корректировки свзи с итогом", __LINE__);
		}else if(int remains = _BMF_INDEX.size()%1000; false){ mpre("ОШИБКА расчета остатка списка", __LINE__);
		}else if([&](int progress = 0){ for(auto &index_itr:_BMF_INDEX){ //for_each(_BMF_INDEX.begin(), _BMF_INDEX.end(), [&](auto &index_itr){ // Правка связей
				if(TMs index = index_itr.second; index.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if([&](){ // Прогресс
					if(progress++ && progress%1000 && (progress != _BMF_INDEX.size())){ //mpre("Отображаем прогресс первый, последний и кратный тысячи разы", __LINE__);
					}else if(bmf::Progress("Сохранение изменений модели " +to_string(progress), (float)progress/_BMF_INDEX.size(), __LINE__); false){ mpre("ОШИБКА Индикатор прогресса", __LINE__);
					}else{ //mpre("Изменение прогресса не каждый раз а только через 1000 итераций", __LINE__);
					}return false; }()){ mpre("ОШИБКА отображения информации", __LINE__);
				}else if(TMs _index = index_itr.second; _index.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if("0" == index["id"]){ //mpre("Не правим удаленные морфы", __LINE__);
				}else if([&](){ // Установка младшего морфа
					if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string index_id = index.at("bmf-index"); (0 >= index_id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(atoi(index_id.c_str()))){ //mpre("Морф в списке измененных не найден "+ index_id, __LINE__);
					}else if(TMs _index_ = _BMF_INDEX.at(atoi(index_id.c_str())); _index_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["bmf-index"] = _index_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка старшего морфа
					if(index.end() == index.find("index_id")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string id = index.at("index_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(atoi(id.c_str()))){ //mpre("Морф в списке не найден "+ id, __LINE__);
					}else if(TMs _index_ = _BMF_INDEX.at(atoi(id.c_str())); _index_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["index_id"] = _index_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка связи со значением
					if(index.end() == index.find("itog_values_id")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string id = index.at("itog_values_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_ITOG_VALUES.end() == _BMF_ITOG_VALUES.find(atoi(id.c_str()))){ mpre("ОШИБКА морф в списке не найден "+ id, __LINE__);
					}else if(TMs _itog_values_ = _BMF_ITOG_VALUES.at(atoi(id.c_str())); _itog_values_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["itog_values_id"] = _itog_values_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка связи со значением
					if(index.end() == index.find("dano_id")){ mpre("ОШИБКА поле с исходным значением", __LINE__);
					}else if(string id = index.at("dano_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_DANO.end() == _BMF_DANO.find(atoi(id.c_str()))){ //mpre("ОШИБКА исходник в списке не найден "+ id, __LINE__);
					}else if(TMs dano = _BMF_DANO.at(atoi(id.c_str())); dano.empty()){ mpre("ОШИБКА дано не найден", __LINE__);
					}else if(index["dano_id"] = dano.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка связи со значением
					if(index.end() == index.find("itog_id")){ mpre("ОШИБКА поле с итоговым значением", __LINE__);
					}else if(string id = index.at("itog_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_ITOG.end() == _BMF_ITOG.find(atoi(id.c_str()))){ mpre("ОШИБКА итог в списке не найден "+ id, __LINE__);
					}else if(TMs itog = _BMF_ITOG.at(atoi(id.c_str())); itog.empty()){ mpre("ОШИБКА итог не найден", __LINE__);
					}else if(index.at("itog_id") = itog.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if(index == _index){ //mpre("Изменений не установлено", __LINE__);
				}else if(_index = bmf::fk("index", {{"id", index["id"]}}, {}, index); _index.empty()){ mpre(index, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(_index, __LINE__, "Связь 1"); mpre(index, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} std::cerr << endl; return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if([&](){ for(auto &dano_titles_itr:_BMF_DANO_TITLES){ //for_each(_BMF_DANO_TITLES.begin(), _BMF_DANO_TITLES.end(), [&](auto &dano_titles_itr){ // Правка связей
				if(TMs dano_titles = dano_titles_itr.second; dano_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if(TMs _dano_titles = dano_titles_itr.second; _dano_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_DANO_VALUES.end() == _BMF_DANO_VALUES.find(atoi(dano_titles.at("dano_values_id").c_str()))){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs _dano_values = _BMF_DANO_VALUES.at(atoi(dano_titles.at("dano_values_id").c_str())); _dano_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_dano_values.end() == _dano_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if(dano_titles.at("dano_values_id") = _dano_values.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(_dano_titles = bmf::fk("dano_titles", {{"id", dano_titles["id"]}}, {}, dano_titles); _dano_titles.empty()){ mpre(dano_titles, __LINE__, "ОШИБКА обновления заголовка");
				}else{ //mpre(_dano_titles, __LINE__, "Связь 1"); //mpre(dano_titles, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if([&](){ for(auto &itog_titles_itr:_BMF_ITOG_TITLES){ //for_each(_BMF_ITOG_TITLES.begin(), _BMF_ITOG_TITLES.end(), [&](auto &itog_titles_itr){ // Правка связей
				if(TMs itog_titles = itog_titles_itr.second; itog_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if(TMs _itog_titles = itog_titles_itr.second; _itog_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_ITOG_VALUES.end() == _BMF_ITOG_VALUES.find(atoi(itog_titles.at("itog_values_id").c_str()))){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs _itog_values = _BMF_ITOG_VALUES.at(atoi(itog_titles.at("itog_values_id").c_str())); _itog_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_itog_values.end() == _itog_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if(itog_titles["itog_values_id"] = _itog_values.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(TMs _itog_titles = bmf::fk("itog_titles", {{"id", itog_titles["id"]}}, {}, itog_titles); _itog_titles.empty()){ mpre(itog_titles, __LINE__, "ОШИБКА обновления заголовка");
				}else{ //mpre(_itog_titles, __LINE__, "Связь 1"); //mpre(itog_titles, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if(bmf::exec("COMMIT TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
		//}else if(int result = sqlite3_close_v2(bmf::db); (SQLITE_OK != result)){ mpre("Не удалось закрыть соединение с БД", __LINE__);
		//}else if(bmf::Close(0)){ mpre("ОШИБКА закрытия БД", __LINE__);
		} return false; }()){ mpre("ОШИБКА установки ключей", __LINE__);
	}else{ //std::cerr << endl; //mpre(_BMF_INDEX, __LINE__, "Сохранение");
	}
}

