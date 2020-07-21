#include <stdio.h>
#include <iostream>
#include <utility>
#include <string>
#include <cctype>
#include <cstring> // strstr
#include <map> /* Ассоциативные массивы */
//#include <iomanip>
//#include <typeinfo>
//#include <new>
//#include <locale>
#include <array>
#include <ctime>
//#include <fstream>
#include <algorithm> // Количество символов в строке
#include <bitset> // Перевод десятичных чисел к двоичному виду
#include <cmath> // Возведение в степень
#include <unistd.h> // sleep
//#include <algorithm> // replace_if
//#include <stdlib.h>

#include <sqlite3.h>
//#include "MathExpr/MathExpr.h"
//#include "cparse/shunting-yard.h"

//#include "tinyexpr/tinyexpr.c"
//#include "tinyexpr/tinyexpr.h"
//#include "exprtk/exprtk.hpp"

#include <sys/file.h> // umask
#include <cstdlib>

//#include <vector> // CSVRow
//#include <sstream> //CSVRow
#include "inc/csv.h"

#include "inc/json.hpp"
#include <random>
#include <sys/time.h>
#include "inc/md5.h"
//#include "crc32/crc32.c"
//#include <ctime> // timestamp
//#include <thread>
#include <regex> // Регулярные выражения замена символов во входящей строке

#include <iomanip>
#include <ctime> // std::put_time Функции времени
#include <sys/time.h> // timeval Функции времени
//#include <fstream>
#include <experimental/filesystem> // Права доступа к файлу
#include <string.h> // strpos
#include <thread>
#include <mutex>
#include <future> // Асинхноррное выполение потоков
//#include <vector>
//#include <cctype>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#define CL_HPP_TARGET_OPENCL_VERSION 210
#define CL_HPP_CL_1_2_DEFAULT_BUILD
//#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include "CL/cl2.hpp"

using json = nlohmann::json;
json in;

using namespace std; //setlocale(LC_ALL, "ru_RU");

typedef std::map<string, string> TMs;
typedef std::map<string, TMs> TMMs;
typedef std::map<int, TMs> TMMi;
typedef std::map<string, TMMi> TM3i;

std::recursive_mutex mu;
//std::lock_guard<std::recursive_mutex> lock(mu);
//std::lock_guard<std::recursive_mutex> locker(mu);

#include "bmf.c"

TM3i BMF_INDEX_EX, BMF_DANO_EX, BMF_ITOG_EX;
TMMi BMF_CLUMP, DATABASES;//, BMF_INDEX, BMF_DANO, _BMF_DANO, _BMF_ITOG, BMF_ITOG;
TM3i BMF_DANO_VALUES_EX, BMF_ITOG_VALUES_EX; // Значения позиций морфов
TM3i BMF_DANO_TITLES_EX, BMF_ITOG_TITLES_EX; // Списки справочников

int loop = 0; // Количество итераций скрипта

int main(int argc, char **argv){
	TMMs data[16];// int data_size;
	TMs bmf_clump; // Текущее скопление
	TMs databases; // Список баз данных
	TMs ARGV; // Массив параметров консоли
	TMs TEST; // Список тестов

	string clump_id = ""; // Скопление если не указано то данные из php кода
	int timestamp = time(0), // Расчет выполнения участков кода
	values_length = 128, // Ограничение размера истории
	change_sum = -1, // Начальное значение количества расчетов
	size_max = 1e6; // Максимально допустимое количество морфов в расчете

	float perc = 0, pips_perc = 0; // Процент правильных ответов
	TM3i _VAL, _CALC;

	std::function<sqlite3_stmt*(string)> exec; // Таблица с интервалами времени
	std::function<sqlite3_stmt*(string)> prepare; // Таблица с интервалами времени
	std::function<TMMi(string)> Tab; // Выборка таблицы из БД
	std::function<TMs(string,TMs,TMs,TMs)> fk; // Сохранение информации в базу
	std::function<time_t(string,string,int)> Timer; TM3i TIMER; // Таблица с интервалами времени
	std::function<TMMi(string,TMMi)> Save; // Сохранение информации в базу
	std::function<int(TMs,string,TM3i&,string)> Values; // Проверка наличия значений в БД
	std::function<int(TMs,string,int,TM3i&,TM3i&,TM3i&,string)> Vals; // Обучение
	std::function<int(json,int,int,string,int,TM3i&,TM3i&)> LearningAll; // Обучение
	std::function<int(TMs,int,TM3i&,TM3i&)> Learning; // Обучение
	std::function<string(double)> Dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
	std::function<string(TMs,string,int,TM3i&,TM3i&)> Learn; // Рерасчет морфа
	std::function<double(string)> Bin2dec; // Перерасчет размерности из двоичной в десятеричную
	std::function<string(TMs,string)> Val; // Расчет истории
	std::function<TMs(TMs,bool)> Calc; // Расчет истории
	std::function<TMs(TMs)> CalcAll; // Расчет истории
	std::function<TMs(TMs)> Calc_new; // Расчет истории
	std::function<TMs(TMs,string,TM3i&,TM3i&)> Choice; // Расчет истории
	std::function<string(TMMi&)> Id; // Расчет локального идентификатора
	std::function<TMMi(TMMi&,string,string,string)> Dataset; // Выгрузка данных из базы с проверкой равенства
	std::function<bool(TMMi,int,string,string)> Data; // Отображение списка данных в коде
	std::function<bool(int,int)> Do; // Непосредственно расчет
	std::function<string(TMs,TM3i&,TM3i&)> History; // История морфа
	std::function<TMs(TMs)> Value; // История морфа
	std::function<TMs(TMs,TM3i&)> Tree; // История морфа

	if([&](){ // Список значений из консоли
		if([&](){ for(int i = 0; i < argc; i++){ // Параметры значений консоли
				if(0 >= i){ //mpre("Имя файла", __LINE__);
				}else if(string arg = argv[i]; (0 >= arg.length())){ mpre("ОШИБКА получения аргумента консоли", __LINE__);
				}else if([&](){ // Установка БД
					if("-" == arg){ //mpre("Вход", __LINE__);
					}else if("-" == arg.substr(0, 1)){ //mpre("Имя параметра", __LINE__);
					}else if(ARGV.end() != ARGV.find("-d")){ mpre("ОШИБКА атрибут консоли имени БД уже установлен `"+ arg+ "`", __LINE__);
					}else if([&](){ ARGV.insert(make_pair("-d", arg)); return (ARGV.end() == ARGV.find("-d")); }()){ mpre("ОШИБКА установки имени БД из консоли", __LINE__);
					}else if(ARGV.end() != ARGV.find("-d")){ //mpre("Имя БД установлено из атрибута "+ to_string(i), __LINE__);
					}else{ return true;
					} return false; }()){ mpre("ОШИБКА установки БД", __LINE__);
				}else if("-" != arg.substr(0, 1)){ //mpre("Аргумент не имя `"+ arg+ "`", __LINE__);
				}else if([&](){ ARGV.insert(make_pair(arg, "")); return ARGV.empty(); }()){ mpre("ОШИБКА установки нового артибута в массив", __LINE__);
				}else if("-" == arg){ //mpre("Вход", __LINE__);
				}else if("-h" == arg){ //mpre("Вход", __LINE__);
				}else if("-v" == arg){ //mpre("Вход", __LINE__);
				}else if("-a" == arg){ //mpre("Вход", __LINE__);
				}else if("-c" == arg){ //mpre("Вход", __LINE__);
				}else if(argc <= i+1){ //mpre("ОШИБКА параметр консоли не указан "+ arg, __LINE__);
				}else if(string val = argv[i+1]; (0 >= val.length())){ mpre("ОШИБКА получения значения параметра "+ arg + " "+ to_string(i+1), __LINE__);
				}else if([&](){ ARGV.at(arg) = val; return ARGV.empty(); }()){ mpre("ОШИБКА установки значения атрибута", __LINE__);
				}else if(!i++){ mpre("ОШИБКА прпоуска следующего атрибута", __LINE__);
				}else{ //mpre("Атрибут "+ to_string(i), __LINE__);
				}
			} return false; }()){ mpre(ARGV, __LINE__, "Аргументы консоли"); mpre("ОШИБКА получения массива параметров консоли", __LINE__);
		}else if([&](){ clump_id = (ARGV.end() == ARGV.find("-d") ? clump_id : ARGV.at("-d")); return false; }()){ mpre("ОШИБКА Параметр адресной строки с указанием БД не задан", __LINE__);
		}else{ //mpre("Список параметров консоли", ARGV, __LINE__);
		} return false; }()){ mpre("ОШИБКА получения данных", __LINE__);
	}else if([&](){ // Входной поток
		if(ARGV.end() == ARGV.find("-")){ //mpre("Стандартный вывод не задан", __LINE__);
		}else if([&](){ string str = ""; while(getline(std::cin, str)){ ARGV.at("-") += ("\n"+ str); }; return (0 >= ARGV.at("-").length()); }()){ mpre("ОШИБКА входящий параметр не задан", __LINE__);
		}else if([&](){ in = json::parse(ARGV.at("-")); return false; }()){ mpre("ОШИБКА Входящий параметр с обучающими данными пуст", __LINE__);
		}else{ //mpre("Данные из потока std::cin "+ in.dump(), __LINE__);
		} return false; }()){ mpre("ОШИБКА получения входного потока", __LINE__);
	}else if([&](){ // Остановки из консоли
		if([&](){ // Отображение атрибутов командной строки
			if(ARGV.end() == ARGV.find("-a")){ return false; //mpre("Пропускаем отображение версии", __LINE__);
			}else{ mpre(ARGV, __LINE__, "Атрибуты командной строки");
			} return true; }()){ mpre("Атрибуты параметров командной строки", __LINE__);
		}else if([&](){ // Раздел помощи
			if(string file = (1 <= argc ? argv[0] : "./bimorph"); (0 >= file.length())){ mpre("ОШИБКА получения имени файла", __LINE__);
			}else if(ARGV.end() == ARGV.find("-h")){ return false; //mpre("Пропускаем раздел помощи", __LINE__);
			}else{ std::cout << endl;
				std::cout << "Использование: "+ file+ " [КЛЮЧИ]… [SQLITE ФАЙЛ]… [КЛЮЧИ]…" << endl;
				std::cout << "Печатает слияние ФАЙЛ(ов) на стандартный вывод." << endl << endl;
				std::cout << "Если задан -, читает стандартный ввод или из строки как -j" << endl << endl;
				std::cout << "-e	количество эпох выполнения при полном совпадении процесс обучения останавливается" << endl;
				std::cout << "-d	файл sqlite базы данных в которых хранится модель" << endl;
				std::cout << "-i	номер итога для обучения" << endl;
				std::cout << "-s	делитель итога используется для распределения расчетов" << endl;
				std::cout << "-r	перемешивание списка для более равномерных результатов" << endl;
				std::cout << "-v	показать информацию о версии" << endl;
				std::cout << "-h	показать эту справку и выйти" << endl;
				std::cout << "-j	параметры расчета в формате json при пустом значении берет из стандартного ввода -" << endl;
				std::cout << "-t	количество потоков обучения" << endl;
				//std::cout << "-tt	установка тестовых данных из выборки" << endl;
				std::cout << "-c	обнулить результаты предыдущих обучений" << endl;
				std::cout << "-a	отображение атрибутов параметров консоли" << endl << endl;

				std::cout << "Пример использования "+ file+ " db.json -e 10 -j '[{\"dano\":{\"Параметр_0\":\"0\",\"Параметр_1\":\"1\"}, \"itog\":{\"Результат\":\"1\"}}]'" << endl << endl;

				std::cout << "Оперативная справка: http://биморф.рф/" << endl;
				std::cout << "Об ошибках в переводе сообщений «биморф» сообщайте по адресу: <bmf@mpak.su>" << endl;
				std::cout << "Полная документация: http://биморф.рф/" << endl << endl;
			} return true; return false; }()){ mpre("Раздел помощи", __LINE__);
		}else if([&](){ // Версия программы
			if(ARGV.end() == ARGV.find("-v")){ return false; //mpre("Пропускаем отображение версии", __LINE__);
			}else{ std::cout << endl;
				std::cout << "bimotph v4" << endl;
				std::cout << "Copyright (C) 2017 биморф.рф" << endl;
				std::cout << "Нет НИКАКИХ ГАРАНТИЙ до степени, разрешённой законом." << endl << endl;
				std::cout << "Лицензия freemium https://ru.wikipedia.org/wiki/Freemium" << endl;
				std::cout << "Данная версия является условной бесплатной с граничением сети в "+ to_string(size_max)+ " морфов" << endl;
				std::cout << "Морф — наименьшая осмысленная единица языка (логики)" << endl << endl;

				std::cout << "Авторы программы -- Кривошлыков Евгений Павлович +79582014736" << endl << endl;
			} return true; }()){ mpre("Информация о версии", __LINE__);
		}else{ return false;
		} return true; }()){ exit(mpre("Остановка выполнения", __LINE__));
	}else if(sqlite3* db = 0; false){ mpre("ОШИБКА установки идентификатора ошибки базы данных", __LINE__);
	}else if(string dbname = [&](string dbname = ""){ // Имя базы данных
		if(ARGV.end() == ARGV.find("-d")){ mpre("ОШИБКА БД для сохранения не задана -d", __LINE__);
		}else if(0 >= ARGV.at("-d").length()){ mpre("База данных для сохранения не указана", __LINE__);
		}else if(dbname = ARGV.at("-d"); (0 >= dbname.length())){ mpre("ОШИБКА имя файла для БД не задано", __LINE__);
		}else{ //mpre("База данных"+ string, __LINE__);
		} return dbname; }(); (0 >= dbname.length())){ mpre("ОШИБКА получения имени базы данных", __LINE__);
	}else if([&](){ // Подключение базы
		if([&](){ // Запрос к БД
			exec = ([&](string sql){ // Запрос к БД
				int result, sqlite_result, sleep = 30;
				sqlite3_stmt* stmt; // Запрос к базе данныхa
					do{
						if([&](){ sqlite_result = sqlite3_exec(db, sql.c_str(), 0, &stmt, 0); return (SQLITE_OK == sqlite_result); }()){ //mpre("Запрос выполнен без ошибок");
						}else if([&](){ mpre("Повторный запрос к БД через "+ to_string(sleep *= 2), __LINE__); mpre("Запрос "+ sql, __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
						}else if([&](){ int request = system(("sleep "+ to_string(sleep)).c_str()); return (0 != request); }()){ mpre("Выход из запроса", __LINE__); exit(1);
						}else{ //mpre("Повторный запрос к БД", __LINE__);
						}
					}while(SQLITE_OK != sqlite_result);
				return stmt;//sqlite3_last_insert_rowid(db);
			}); return false; }()){ mpre("ОШИБКА установки функции запроса к БД", __LINE__);
		}else if([&](){ // Запрос к БД
			prepare = ([&](string sql){ // Запрос к БД
				int result, sqlite_result, sleep = 30;
				sqlite3_stmt* stmt; // Запрос к базе данныхa
					do{
						if([&](){ sqlite_result = sqlite3_prepare(db, sql.c_str(), -1, &stmt, 0); return (SQLITE_OK == sqlite_result); }()){ //mpre("Запрос выполнен без ошибок");
						}else if([&](){ mpre("Повторная выбрка из БД через "+ to_string(sleep *= 2), __LINE__); mpre("Запрос "+ sql, __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
						}else if([&](){ int request = system(("sleep "+ to_string(sleep)).c_str()); return (0 != request); }()){ mpre("Выход из запроса", __LINE__); exit(1);
						}else{ //mpre("Повторный запрос к БД", __LINE__);
						}
					}while(SQLITE_OK != sqlite_result);
				return stmt;//sqlite3_last_insert_rowid(db);
			}); return false; }()){ mpre("ОШИБКА установки функции запроса к БД", __LINE__);
		}else if([&](){ // Обновление записей БД
			fk = ([&](string table, TMs where, TMs insert, TMs update){ // Обновление
				TMs row;
				sqlite3_stmt* stmt;
				if([&](){
					if(where.empty()){// std::cerr << __LINE__ << " Условие на выборку записей не указано" << endl;
					}else if(string sql = [&](string sql = ""){
							string values;
							sql = "SELECT * FROM `"+ table+ "`";
							for(TMs::iterator itr = where.begin(); itr != where.end(); itr++){
								string key = itr->first;
								string val = itr->second;
								string separ = (values.size() ? " AND " : "");
								values += separ+ "`"+ key+ "`='"+ val+ "'";
							} sql += " WHERE "+ values;
							return sql;
						}(); (0 >= sql.length())){ std::cerr << __LINE__ << " Запрос добавления выборку записи из БД" << endl;
					}else if([&](){
						if(TMMi TAB = Tab(sql); false){ // Выполнение запроса к базе дыннх
						}else if([&](){ return TAB.size() < 1; }()){// std::cerr << __LINE__ << " Размер выборки меньше нуля << " << sql << endl;
						}else if([&](){ row = TAB.begin()->second; return false; }()){ std::cerr << __LINE__ << "Список измененных данных" << endl;
						}else{// mpre(row, __LINE__); // std::cerr << __LINE__ << " Запрос к БД:" << sql << endl; // mpre(row);
						} return false; }()){ std::cerr << " Запрос на получения вновь установленной записи" << endl;
					}else{// std::cerr << __LINE__ << " Запрос на выборку значений: " << sql << endl; // mpre(where, __LINE__);
					} return false; }()){ std::cerr << __LINE__ << " Выборка записи по условию" << endl;
				}else if([&](){ int rowid; // Добавление
					if(insert.empty()){// std::cerr << __LINE__ << " Условия добавления новой записи не указаны" << endl;
					}else if(!row.empty()){// std::cerr << __LINE__ << " Запись уже найдена по условиям << " << row["id"] << endl; mpre(where, __LINE__); mpre(insert, __LINE__); mpre(update, __LINE__);
					}else if([&](){ insert.erase("id"); return insert.empty(); }()){ mpre("Устанавливаем нулевой идентификатор", __LINE__);
					}else if(string sql = [&](string sql = ""){
						string fields, values;
						sql = "INSERT INTO `"+ table+ "`";
						for(TMs::iterator itr = where.begin(); itr != where.end(); itr++){
							string key = itr->first;
							string val = itr->second;
							string separ = (fields.size() ? ", " : "");
							fields += separ+ "`"+ key+ "`";
							values += separ+ "'"+ val+ "'";
						} for(TMs::iterator itr = insert.begin(); itr != insert.end(); itr++){
							string key = itr->first;
							string val = itr->second;
							string separ = (fields.size() ? ", " : "");
							fields += separ+ "`"+ key+ "`";
							values += separ+ "'"+ val+ "'";
						} sql += "("+ fields+ ") VALUES ("+ values+ ")";
						return sql; }(); (0 >= sql.length())){ std::cerr << __LINE__ << " Запрос добавления новой записи в БД" << endl;
					}else if([&](){ stmt = exec(sql); return false; }()){ mpre("ОШИБКА выполнения запроса к БД", __LINE__); //std::cerr << __LINE__ << " ОШИБКА выполнения запроса к базе данных: " << sql << " >> " << sqlite3_errmsg(db) << endl;// return false;
					}else if([&](){ rowid = sqlite3_last_insert_rowid(db); return false; }()){ mpre("ОШИБКА получения идентификатора последней установленной записи", __LINE__); //std::cerr << __LINE__ << " Идентификатор последней установленной записи << " << rowid << endl;
					}else if([&](){
						if([&](){ sql = "SELECT * FROM `"+ table+ "` WHERE id="+ to_string(rowid); return false; }()){ std::cerr << __LINE__ << " Запрос на выборку внось установленной записи" << endl;
						}else if([&](){ TMMi TAB = Tab(sql); row = erb(TAB, {{"id", to_string(rowid)}}); return false; }()){ std::cerr << __LINE__ << "Список измененных данных" << endl;
						}else{// std::cerr << __LINE__ << " Запрос к БД:" << sql << endl; // mpre(row);
						} return false; }()){ std::cerr << " Запрос на получения вновь установленной записи" << endl;
					}else{// std::cerr << __LINE__ << " Добавление новой записи к базе `"+ table+ "` << " << rowid << " " << endl;
					} return false; }()){ std::cerr << __LINE__ << "ОШИБКА добавление запроса если не найден по условиям" << endl;
				}else if([&](){ // Обновление
					if(update.empty()){ //std::cerr << __LINE__ << " Условия обновления не указаны" << endl;
					}else if(row.empty()){ std::cerr << __LINE__ << " Запись для обновления не найдена " << endl;
					}else if(string sql = [&](string sql = ""){
						string fields, values;
						sql = "UPDATE `"+ table+ "`";
						for(TMs::iterator itr = update.begin(); itr != update.end(); itr++){
							string key = itr->first;
							if(key == "id"){ // Сохраняем идентификатор записи
							}else if(key.substr(0, 1) == "_"){ // Технические поля
							}else{
								string val = itr->second;
								string separ = (values.size() ? ", " : "");
								values += separ+ "`"+ key+ "`='"+ val+ "'";
							}
						} sql += " SET "+ values+ " WHERE id="+ row["id"];
						return sql; }(); (0 >= sql.length())){ std::cerr << __LINE__ << " Запрос добавления новой записи в БД" << endl;
					}else if([&](){ exec(sql); return false; }()){ mpre("ОШИБКА обновления записи", __LINE__); //std::cerr << __LINE__ << " ОШИБКА обновления записи" << endl;
					}else if([&](){ sql = "SELECT * FROM `"+ table+ "` WHERE id="+ row["id"]; return false; }()){ mpre("ОШИБКА запроса на выборку вновь установленных данных", __LINE__); //std::cerr << __LINE__ << " Запрос на выборку внось установленной записи" << endl;
					}else if([&](){ TMMi TAB = Tab(sql); row = erb(TAB, {{"id", row["id"]}}); return false; }()){ mpre("ОШИБКА полуения списка измененных данных", __LINE__); //std::cerr << __LINE__ << "Список измененных данных" << endl;
					}else{ //std::cerr << __LINE__ << " Обновляем запись: " << sql << endl;// mpre(update, __LINE__);
					} return false; }()){ std::cerr << __LINE__ << " Обновение найденной по условию записи" << endl;
				}else{// mpre(where, __LINE__); mpre(insert, __LINE__); mpre(update, __LINE__);
				} return row; }); return false; }()){ mpre("ОШИБКА обьявления функции Crc32", __LINE__);
			}else if([&](){ // Выборка таблицы
				Tab = ([&](string sql){ // Запрос к бд
					char *val; int count; TMMi TAB;
					sqlite3_stmt* stmt;
					//if(SQLITE_OK != sqlite3_prepare(db, sql.c_str(), -1, &stmt, 0)){ std::cerr << __LINE__ << " ОШИБКА выполнения запроса к базе данных: " << sql << endl;
					if([&](){ stmt = prepare(sql); return false; }()){ mpre("ОШИБКА запроса к БД", __LINE__);
					}else if([&](){ count = sqlite3_column_count(stmt); return false; }()){ std::cerr << " ОШИБКА расчета количества записей в таблице";
					}else{ int id = 0;
						while(SQLITE_ROW == sqlite3_step(stmt)){
							TMs row; string row_id;
							for(int num = 0; num < count; num++){
								string field = (string) sqlite3_column_name(stmt, num);
								val = (char*) sqlite3_column_text(stmt, num);
								string value = (val == NULL ? "" : val);
								row.insert(std::make_pair(field, value));
							} //exit(mpre(mask, __LINE__, "Маска"));
							int next_id = (row.end() == row.find("id") ? ++id : stoi(row["id"]));
							TAB.insert(std::make_pair(next_id, row));
						}// std::cerr << __LINE__ << " Запрос к БД " << sql << " размер: " << TAB.size() << endl;
					} return TAB;
				}); return false; }()){ mpre("ОШИБКА установки функции выборки списка таблциы", __LINE__);
			}else if([&](){ // Восстановление из БД с проверкой
				Dataset = ([&](TMMi& TAB, string name, string table, string comment){ // Функция восстановления из БД с проверкой
					TMMi _TAB; bool check = true;
					if([&](){ // Проверка наличия таблицы в базах
							if(DATABASES.empty()){ mpre("ОШИБКА не установлен список баз данных", __LINE__);
							}else if(string master = string(databases.empty() ? "" : "bmf.")+ "sqlite_master"; (0 >= master.length())){ mpre("ОШИБКА формирования имени таблицы проверки", __LINE__);
							}else if(string sql = "SELECT 0 AS id, count(*) AS cnt FROM "+ master+ " WHERE type='table' AND name='"+ table+ "'"; (0 >= sql.length())){ mpre("ОШИБКА составления запроса проверки таблицы скоплений", __LINE__);
							}else if([&](){ TMMi CLUMP; CLUMP = Tab(sql); return ("0" != CLUMP[0]["cnt"]); }()){ return false; //mpre("ОШИБКА Таблица `"+ table+ "` отсутсвует в базе", __LINE__);
							}else{ //mpre("Таблица `"+ table+ "` не найдена в (База скопления) и bmf (База сайта)", __LINE__);
							} return true;
						}()){ //mpre("Данные справочника "+ name+ " с "+ to_string(TAB.size())+ " позициями не перезагружается", __LINE__);
					}else if(string sql = "SELECT * FROM `"+ table+ "`"; false){ // Запрос на выбору данных
					}else if(_TAB = Tab(sql); false){ std::cerr << __LINE__ << " ОШИБКА выборки таблицы из базы данных";
					}else if(!check){ return TAB;
					}else if(_TAB.size() != TAB.size()){ mpre("Не совпадение размера данных в справочниках", __LINE__); //data(_TAB, __LINE__, name, comment); //std::cerr << " Не совпадение размера данных в справочниках" << endl; data(_TAB, __LINE__, name, comment);
					}else if([&](){
						bool check = false; TMs index, index_, row;
						for(auto itr = _TAB.begin(); itr != _TAB.end(); itr++){
							TMs _index = itr->second;
							if([&](){ index = erb(TAB, _index); row = erb(TAB, {{"id", _index["id"]}}); return index.empty(); }()){ std::cerr << __LINE__ << " Данные не совпадают" << endl; mpre(row, __LINE__); mpre(_index, __LINE__); check = true;
							}else{// std::cerr << __LINE__ << " Данные совпали << " << index["id"] << endl;
								//mpre(index, __LINE__); mpre(_index, __LINE__);
							}
						} return check; }()){ std::cerr << __LINE__ << " ОШИБКА Даныне в справочниках не совпадают" << endl; Data(_TAB, __LINE__, name, comment); system("sleep 1"); exit(1);
					}else{// std::cerr << __LINE__ << " (Выборка данных) Запрос: " << sql << endl;
					} return TAB;
				}); return false; }()){ mpre("ОШИБКА установки функции выборки из БД", __LINE__);
			}else if([&](){ // Формат данных в коде
				Data = ([&](TMMi TAB, int line, string tab, string comment){
					string sepor;
					std::cerr << line << " " << (tab == "" ? "" : "TMMi "+ tab+ " = ") << "{" << (comment == "" ? "" : " // "+comment) << endl;
					for(TMMi::iterator itr = TAB.begin(); itr != TAB.end(); itr++){
						int id = itr->first;
						TMs row = itr->second;
						std::cerr << "\t{"+ to_string(id)+ ", {";
						for(TMs::iterator it = row.begin(); it != row.end(); it++){
							string key = it->first;
							string val = it->second;
							sepor = (it == row.begin() ? "" : ", ");
							std::cerr << sepor+ "{\""+ key+ "\", \""+ val+ "\"}";
						} std::cerr << "}}," << endl;
					} std::cerr << "};" <<	endl;
					return false;
				}); return false; }()){ mpre("ОШИБКА установки функции отображения данных в коде", __LINE__);
			}else if([&](){ // Создание файла БД если его нет
				if(access(dbname.c_str(), F_OK) == 0){ //mpre("Файл БД уже создан "+ clump_id, __LINE__); system("sleep 10");
				}else if(system(("touch "+ dbname).c_str())){ mpre("ОШИБКА создания файла БД", __LINE__); system("pwd;");
				}else if(system(("chmod u+w "+ dbname).c_str())){ mpre("ОШИБКА назначения доступа файла БД", __LINE__);
				}else{ mpre("Создание файла БД "+ dbname, __LINE__);
				} return false; }()){ mpre("ОШИБКА создания файла БД", __LINE__);
			}else if([&](){ // Установка соединения с БД
				if(SQLITE_OK != sqlite3_open(dbname.c_str(), &db)){ std::cerr << __LINE__ << " ОШИБКА открытия базы данных << " << dbname << endl;
				}else if([&](){ string f = argv[0]; return (f != "./bmf"); }()){ mpre("Пропуск подключения родительсткой БД", __LINE__);
				}else if(string attach_database = "../../../../.htdb"; (0 >= attach_database.length())){ mpre("ОШИБКА имя доп БД не задано", __LINE__);
				}else if(access(attach_database.c_str(), F_OK) == -1){ mpre("Файл БД не найден "+ attach_database, __LINE__);
				}else if([&](){ exec("ATTACH DATABASE '../../../../.htdb' AS bmf"); return false; }()){ mpre("ОШИБКА Подключение основной базы с данными", __LINE__);
				}else{ std::cerr << __LINE__ << " Подключение родительской БД " << attach_database << endl;
				} return false; }()){ mpre("ОШИБКА подключения баз данных", __LINE__);
			}else if([&](){ // Получение пути до файла БД
					if(int pos = dbname.rfind("/"); (0 > pos)){ mpre("Слешей в пути до скопления не найдено", __LINE__);
					}else if([&](){ clump_id = dbname.substr(pos+1, dbname.length()); return (0 >= clump_id.length()); }()){ mpre("ОШИБКА сокращения пути до файла", __LINE__);
					}else{ mpre("Путь до БД сокращен "+ clump_id, __LINE__);
					} return (0 >= clump_id.length()); }()){ mpre("ОШИБКА получения скопления", __LINE__);
			}else if([&](){ // Получение текущего скопления
					string sql;
					if("0" == clump_id){ mpre("Скопление не указано", __LINE__);
					}else if([&](){ DATABASES = Tab("PRAGMA database_list"); return DATABASES.empty(); }()){ mpre("ОШИБКА получения списка подключенных таблиц", __LINE__);
					}else if([&](){ databases = erb(DATABASES, {{"name", "bmf"}}); return databases.empty(); }()){ mpre("Общая таблица не найдена", __LINE__);
					}else if([&](){ sql = "SELECT 0 AS id, COUNT(*) AS cnt FROM bmf.sqlite_master WHERE type='table' AND name='mp_bmf_clump'"; return (0 >= sql.length()); }()){ mpre("ОШИБКА составления запроса проверки таблицы скоплений", __LINE__);
					}else if([&](){ TMMi CLUMP = Tab(sql); return ("0" == CLUMP.find(0)->second.find("cnt")->second); }()){ mpre("Таблица со скоплениями отсутсвует в базе", __LINE__);
					}else if([&](){ sql = "SELECT * FROM mp_bmf_clump WHERE id='"+ clump_id+ "'"; return false; }()){ mpre("Задайте номер скопления", __LINE__);
					}else if([&](){ BMF_CLUMP = Tab(sql); bmf_clump = (0 >= BMF_CLUMP.size() ? TMs({}) : BMF_CLUMP.begin()->second); return bmf_clump.empty(); }()){ //mpre("Информация о скоплении не установлена "+ sql, __LINE__);
					}else if([&](){ bmf_clump = fk("mp_bmf_clump", {{"id", clump_id}}, {}, {{"hide", "0"}}); return bmf_clump.empty(); }()){ mpre("ОШИБКА обновления видимости скопления", __LINE__);
					}else if([&](){ exec("UPDATE mp_bmf_clump SET hide=1 WHERE id<>"+ clump_id); return false; }()){ mpre("ОШИБКА скрытия не активных скопления", __LINE__);
					}else{ mpre("Скопление: `"+ bmf_clump["name"]+ "`", __LINE__);
					} return (0 >= clump_id.length());
				}()){ mpre("ОШИБКА получения скопления", __LINE__);
			}else if([&](){ // Добавление таблиц в БД если они не созданы
					string path;
					if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_index (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`clump_id` INTEGER,`itog_values_id` INTEGER, `depth` INTEGER, `dimension` INTEGER,`dano_id` INTEGER,`itog_id` TEXT,`calc_pos_id` INTEGER,`index_id` INTEGER, `bmf-index` INTEGER)"); return false; }()){ mpre("ОШИБКА создания таблицы морфов", __LINE__);
					//}else if([&](){ exec("CREATE INDEX IF NOT EXISTS `bmf_index-up` ON mp_bmf_index(up);"); return false; }()){ mpre("ОШИБКА создания индекса", __LINE__);
					}else if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_dano (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER,`uid` INTEGER,`clump_id` INTEGER,`dano_values_id` INTEGER,`name` TEXT,`val` INTEGER,`values` TEXT)"); return false; }()){ mpre("ОШИБКА создания таблицы морфов", __LINE__);
					}else if([&](){ exec("CREATE INDEX IF NOT EXISTS `bmf_dano-clump_id-dano_values_id` ON mp_bmf_dano(clump_id,dano_values_id);"); return false; }()){ mpre("ОШИБКА создания индекса", __LINE__);
					}else if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_itog (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER,`uid` INTEGER,`clump_id` INTEGER,`index_id` INTEGER,`itog_values_id` INTEGER,`name` TEXT,`val` INTEGER,`values` TEXT,`shift` INTEGER,`shifting` TEXT)"); return false; }()){ mpre("ОШИБКА создания таблицы морфов", __LINE__);
					}else if([&](){ exec("CREATE INDEX IF NOT EXISTS `bmf_itog-clump_id-itog_values_id` ON mp_bmf_itog(clump_id,itog_values_id);"); return false; }()){ mpre("ОШИБКА создания индекса", __LINE__);
					}else if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_dano_values (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER ,`uid` INTEGER ,`clump_id` INTEGER ,`index_type_id` INTEGER ,`name` TEXT ,`value` TEXT ,`old` INTEGER ,`shift` INTEGER ,`dano_values_option_id` INTEGER ,`bin` TEXT)"); return false; }()){ mpre("ОШИБКА создания значений дано", __LINE__);
					}else if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_itog_values (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER,`index_type_id` INTEGER,`clump_id` INTEGER,`name` TEXT,`value` INTEGER,`itog_values_option_id` INTEGER,`bin` INTEGER)"); return false; }()){ mpre("ОШИБКА создания значения итога", __LINE__);
					}else if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_itog_titles (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `clump_id` INTEGER, `itog_values_id` INTEGER, `value` INTEGER, `name` INTEGER, `text` TEXT)"); return false; }()){ mpre("ОШИБКА создания значения итога", __LINE__);
					}else if([&](){ exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_dano_titles (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `clump_id` INTEGER, `dano_values_id` INTEGER, `value` INTEGER, `name` INTEGER, `text` TEXT)"); return false; }()){ mpre("ОШИБКА создания значения итога", __LINE__);
					//}else if([&](){ path = "/var/www/192.168.1.6/.htdb"; return (0 >= path.length()); }()){ mpre("ОШИБКА установки пути до файла БД", __LINE__);
					//}else if(exec("ATTACH DATABASE `"+ path+"` AS clump")){ mpre("ОШИБКА при подключении базы скопления "+ path, __LINE__);
					}else{ return false;
					} return true;
				}()){ mpre("ОШИБКА подключения БД созвездия и создания таблиц", __LINE__);
			}else if([&](){ // Очищение списка морфов
					if(ARGV.end() == ARGV.find("-c")){ //mpre("Сохраняем результат предыдущих расчетов", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_index"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_dano"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_dano_values"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_dano_titles"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_itog"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_itog_values"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					}else if([&](){ exec("DELETE FROM mp_bmf_itog_titles"); return false; }()){ mpre("ОШИБКА выполнения запроса на очистку морфорв", __LINE__);
					//}else if([&](){ BMF_INDEX_EX[""] = Tab("SELECT * FROM `mp_bmf_index` -- WHERE `clump_id`='"+ clump_id+ "'"); return false; }()){ mpre("ОШИБКА выборки списка морфов из базы", __LINE__);
					//}else if(0 < BMF_INDEX_EX.at("").size()){ mpre("ОШИБКА после удаления всех морфов в базе все еще остаются данные", __LINE__);
					}else{ mpre("Очистка списка морфов по аргументу -c", __LINE__); system("sleep 1");
					} return false;
				}()){ mpre("ОШИБКА выборки списка морфов", __LINE__);
			}else if([&](){ BMF_CALC = Dataset(BMF_CALC, "BMF_CALC", "mp_bmf_calc", "Расчеты"); return BMF_CALC_POS.empty(); }()){ mpre("ОШИБКА выборки позиций расчетов", __LINE__);
			}else if([&](){ BMF_CALC_ORIENT = Dataset(BMF_CALC_ORIENT, "BMF_CALC_ORIENT", "mp_bmf_calc_orient", "Направления"); return BMF_CALC_ORIENT.empty(); }()){ mpre("ОШИБКА выборки направлений расчетов", __LINE__);
			}else if([&](){ BMF_CALC_POS = Dataset(BMF_CALC_POS, "BMF_CALC_POS", "mp_bmf_calc_pos", "Позиции"); return BMF_CALC_POS.empty(); }()){ mpre("ОШИБКА выборки позиций расчетов", __LINE__);
			}else if([&](){ BMF_CALC_VAL = Dataset(BMF_CALC_VAL, "BMF_CALC_VAL", "mp_bmf_calc_val", "Знаки"); return BMF_CALC_VAL.empty(); }()){ mpre("ОШИБКА выборки значений расчетов", __LINE__);
			}else if([&](){ BMF_INDEX_TYPE = Dataset(BMF_INDEX_TYPE, "BMF_INDEX_TYPE", "mp_bmf_index_type", "Тип значения"); return BMF_INDEX_TYPE.empty(); }()){ mpre("ОШИБКА выборки типа значений", __LINE__);
			}else if([&](){ TMMi DANO = Tab("SELECT * FROM `mp_bmf_dano` -- WHERE `clump_id`='"+ clump_id+ "'"); BMF_DANO_EX[""] = DANO; return false; }()){ mpre("ОШИБКА получения дано скопления", __LINE__);
			}else if([&](){ TMMi DANO_VALUES = Tab("SELECT * FROM `mp_bmf_dano_values` -- WHERE `clump_id`='"+ clump_id+ "'"); BMF_DANO_VALUES_EX[""] = DANO_VALUES; return false; }()){ mpre("ОШИБКА получения дано значений", __LINE__);
			}else if([&](){ TMMi DANO_TITLES = Tab("SELECT * FROM `mp_bmf_dano_titles` -- WHERE `clump_id`='"+ clump_id+ "'"); BMF_DANO_TITLES_EX[""] = DANO_TITLES; return false; }()){ mpre("ОШИБКА получения дано справочника", __LINE__);
			}else if([&](){ TMMi ITOG_VALUES = Tab("SELECT * FROM `mp_bmf_itog_values` -- WHERE `clump_id`='"+ clump_id+ "'"); BMF_ITOG_VALUES_EX[""] = ITOG_VALUES; return false; }()){ mpre("ОШИБКА получения итогов значений", __LINE__);
			}else if([&](){ TMMi ITOG_TITLES = Tab("SELECT * FROM `mp_bmf_itog_titles` -- WHERE `clump_id`='"+ clump_id+ "'"); BMF_ITOG_TITLES_EX[""] = ITOG_TITLES; return false; }()){ mpre("ОШИБКА получения итогов справочника", __LINE__);
			}else if([&](){ TMMi ITOG = Tab("SELECT * FROM `mp_bmf_itog` -- WHERE `clump_id`='"+ clump_id+ "'"); BMF_ITOG_EX[""] = ITOG; return false; }()){ mpre("ОШИБКА получения итогов скопления", __LINE__);
			}else if([&](){ // Получение списка итогов
					if(ARGV.end() == ARGV.find("-i")){ //mpre("Режим расчета не указан итог", __LINE__);
					}else if(string::npos != ARGV.at("-i").find_first_not_of("0123456789")){ mpre("ОШИБКА формат исходного значения не число "+ ARGV.at("-i"), __LINE__);
					}else if("0" == ARGV.at("-i")){ mpre("Режим расчета указан итог 0", __LINE__);
					}else if(BMF_ITOG_EX.at("").end() != BMF_ITOG_EX.at("").find(stoi(ARGV.at("-i")))){ mpre("Указан итог для расчета "+ ARGV.at("-i"), __LINE__);
					}else{ mpre("ОШИБКА указанный итог не найден "+ ARGV.at("-i"), __LINE__); exit(0);
					}
				return false; }()){ mpre("ОШИБКА получения списка итогов", __LINE__);
			}else if([&](){ // Выборка морфов
					string sql;
					if([&](){ sql = "SELECT * FROM `mp_bmf_index` WHERE "+ (((ARGV.end() == ARGV.find("-i"))) ? "1=1" : "itog_id='"+ ARGV.at("-i")+ "'"); return (0 >= sql.length()); }()){ mpre("ОШИБКА составления запроса на выборку морфов", __LINE__);
					}else if([&](){ BMF_INDEX_EX[""] = Tab(sql); return false; }()){ mpre("ОШИБКА выборки списка морфов из базы", __LINE__);
					}else{ //mpre(sql, __LINE__); mpre(BMF_INDEX_EX, __LINE__, "Список морфов");
					}
				return false; }()){ mpre("ОШИБКА выборки списка морфов", __LINE__);
			}else{ //mpre(BMF_INDEX_EX.at(""), __LINE__, "Скопление");
			} return false;
		}()){ mpre("ОШИБКА подключения базы данных", __LINE__);
	}else if([&](){ // Списки значений быстрого доступа
		if(BMF_CALC.empty()){ mpre("ОШИБКА для составления массива быстрого расчета требуется таблица расчетов", __LINE__);
		}else if([&](){ for(auto &calc_itr:BMF_CALC){ //for_each(BMF_CALC.begin(), BMF_CALC.end(), [&](auto calc_itr){ // По расчетам добавляем в список быстрого доступа
				if(TMs calc = calc_itr.second; calc.empty()){ mpre("ОШИБКА получения очередного расчета", __LINE__);
				}else if([&](){ // Добавление списка второго уровня v0
					if(_CALC.end() != _CALC.find(calc["calc_pos_id"])){ //mpre("Уровень calc_pos_id уже установлен", __LINE__);
					}else if(_CALC.insert(make_pair(calc["calc_pos_id"], TMMi({}))); _CALC.empty()){ mpre("ОШИБКА добавления нового уровня calc_pos_id", __LINE__);
					}else{ //mpre("Добавление нового уровня calc_pos_id", __LINE__);
					} return false; }()){ mpre("ОШИБКА добавление списка calc_pos_id", __LINE__);
				}else if(_CALC.at(calc["calc_pos_id"]).insert(make_pair(stoi(calc["calc_val_id"]), calc)); (_CALC.at(calc["calc_pos_id"]).end() == _CALC.at(calc["calc_pos_id"]).find(stoi(calc["calc_val_id"])))){ mpre("ОШИБКА добавления второго уровня", __LINE__);
				}else{ //mpre("Добавление нового знака списку быстрого доступа "+ calc["name"], __LINE__);
				}
			} return _CALC.empty(); }()){ mpre("ОШИБКА получения быстрого расчета калькуляции", __LINE__);
		}else if(BMF_CALC_VAL.empty()){ mpre("ОШИБКА для составления массива быстрых значений требуется таблица значений", __LINE__);
		}else if([&](){ for(auto &val_itr:BMF_CALC_VAL){ //for_each(BMF_CALC_VAL.begin(), BMF_CALC_VAL.end(), [&](auto val_itr){ // Познакам добавляем в список быстрого доступа
				if(TMs val = val_itr.second; val.empty()){ mpre("ОШИБКА получения очередного знака", __LINE__);
				}else if([&](){ // Добавление списка второго уровня v0
					if([&](){ return (_VAL.end() != _VAL.find(val["v1"])); }()){ //mpre("Второй уровень v1 уже установлен", __LINE__);
					}else if([&](){ _VAL.insert(make_pair(val["v1"], TMMi({}))); return _VAL.empty(); }()){ mpre("ОШИБКА добавления нового уровня v0", __LINE__);
					}else{ //mpre("Добавление нового уровня v1", __LINE__);
					} return (false); }()){ mpre("ОШИБКА добавление списка v1", __LINE__);
				}else if([&](){ _VAL.at(val["v1"]).insert(make_pair(stoi(val["v0"]), val)); return (_VAL.at(val["v1"]).end() == _VAL.at(val["v1"]).find(stoi(val["v0"]))); }()){ mpre("ОШИБКА добавления второго уровня", __LINE__);
				}else{ //mpre("Добавление нового знака списку быстрого доступа", __LINE__);
				}
			} return _VAL.empty(); }()){ mpre("ОШИБКА получения массива быстрых значений", __LINE__);
		}else{ //mpre("Списки быстрого доступа", __LINE__);
		} return false; }()){ mpre("ОШИБКА составления списка быстрого доступа", __LINE__);
	}else if([&](){ // Установка функций
		if([&TIMER, &Timer](){ Timer = ([&](string name, string title, int line){ // Получаем число возвращаем двойное числоa
			TMMi timer; TMs tmr; time_t t; struct timeval tv; double microsec, duration; int next;
			if([&](){ gettimeofday(&tv, NULL); return false; }()){ mpre("Структура содержащая время и микровремя", __LINE__);
			}else if([&](){ microsec = tv.tv_usec/1e6+tv.tv_sec; return (0 >= microsec); }()){ mpre("ОШИБКА получения микросекунд", __LINE__);
			}else if([&](){ timer = (TIMER.end() == TIMER.find(name) ? timer : TIMER.at(name)); return false; }()){ mpre("ОШИБКА получения ввремени события", __LINE__);
			}else if([&](){ tmr = (timer.end() == timer.find(1) ? tmr : timer.at(1)); return false; }()){ mpre("ОШИБКА получения первого элемента списка", __LINE__);
			}else if([&](){ duration = (tmr.end() == tmr.find("microsec") ? 0 : microsec-stod(tmr.at("microsec"))); return (0 > t); }()){ mpre("ОШИБКА получения времени исполнения", __LINE__);
			}else if([&](){ next = timer.size()+1; return (0 >= next); }()){ mpre("ОШИБКА определения следующего идентификатора", __LINE__);
			}else if([&](){ tmr = {{"id", to_string(next)}, {"title", title}, {"microsec", to_string(microsec)}, {"duration", to_string(duration)}}; return tmr.empty(); }()){ mpre("ОШИБКА установки значений очередного тайа", __LINE__);
			}else if([&](){ timer.insert(make_pair(next, tmr)); return timer.empty(); }()){ mpre("ОШИБКА добавления нового изменения событию", __LINE__);
			}else if([&](){ // Обновление таблицы таймера
				if(TIMER.end() != TIMER.find(name)){ TIMER.at(name) = timer;
				}else if([&](){ TIMER.insert(make_pair(name, timer)); return TIMER.empty(); }()){ mpre("ОШИБКА добавления нового события в таймер", __LINE__);
				}else{ //mpre(TIMER, __LINE__, "Таймер");
				} return TIMER.empty(); }()){ mpre("ОШИБКА обновления таблицы таймера", __LINE__);
			}else{ std::cerr << line << "." << "Таймер `" << name << "` " << title << " " << to_string(duration) << endl;
			} return t; }); return false; }()){ mpre("ОШИБКА установки функции засечения времени", __LINE__);
		}else if([&](){ // Отображение дерева
			Tree = ([&](TMs bmf_index, TM3i& _BMF_DANO_EX){ // Отображение дерева
				TMMi STAIRS; int rep = 0; string after_char = "  ";
				if([&](){ // Заполнение буфера
					TMs stairs, calc_pos, dano; string after;
					if(bmf_index.end() == bmf_index.find("id")){ mpre("ОШИБКА у морфа не указан идентификатор", __LINE__);
					}else if([&](){ stairs["index_id"] = bmf_index.at("id"); return stairs.empty(); }()){ mpre("ОШИБКА Установка идентификатора морфа", __LINE__);
					}else if([&](){ STAIRS.insert(make_pair(STAIRS.size(), stairs)); return STAIRS.empty(); }()){ mpre("ОШИБКА установки первой ступени", __LINE__);
					}else if(string::npos != bmf_index.at("depth").find_last_not_of("0123456789")){ mpre("ОШИБКА формат глучины", __LINE__);
					}else if([&](){ int depth = stoi(bmf_index.at("depth")); while(depth--){ after += after_char; }; return false; }()){ mpre("ОШИБКА получения строки отступа", __LINE__);
					}else if([&](){ calc_pos = erb(BMF_CALC_POS, {{"id", bmf_index.at("calc_pos_id")}}); return calc_pos.empty(); }()){ mpre("ОШИБКА позиция родительского морфа не найдена", __LINE__);
					}else if([&](){ dano = erb(_BMF_DANO_EX, {{"id", bmf_index.at("dano_id")}}); return dano.empty(); }()){ mpre("ОШИБКА получения исходного значения", __LINE__);
					}else if([&](){ mpre(after+ "bmf_index["+ bmf_index["id"]+ "] "+ calc_pos.at("name")+ " dano["+ dano.at("id")+ "]="+ dano.at("val"), __LINE__); return false; }()){ mpre("ОШИБКА отображения родителя", __LINE__);
					}else{ //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); //mpre(stairs, __LINE__, "Устанавливаем начало лестницы", __LINE__);
					} return false; }()){ mpre("ОШИБКА добавления элемента в буфер", __LINE__);
				//}else if([&](){ mpre("============ Дерево ============", __LINE__); return false; }()){ mpre("ОШИБКА обозначения начала дерева", __LINE__);
				}else if([&](){ do{ // Перебор ступеней лестницы
					TMs stairs, index, calc_pos, index_next; string index_field; TMMi::reverse_iterator stairs_itr;
					if(STAIRS.empty()){ mpre("ОШИБКА лестница пуста", __LINE__);
					}else if([&](){ stairs_itr = STAIRS.rbegin(); return (STAIRS.rend() == stairs_itr); }()){ mpre("ОШИБКА выборки итератора", __LINE__);
					}else if([&](){ stairs = stairs_itr->second; return stairs.empty(); }()){ mpre("ОШИБКА выборки первой ступени", __LINE__);
					}else if(stairs.end() == stairs.find("index_id")){ mpre("ОШИБКА поле со ссылкой на морф у ступени не найдено", __LINE__);
					}else if([&](){ index = erb(BMF_INDEX_EX, {{"id", stairs.at("index_id")}}); return index.empty(); }()){ mpre("ОШИБКА выборки морфа", __LINE__);
					}else if(index.end() == index.find("calc_pos_id")){ mpre("ОШИБКА поле позиции у морфа не установлено", __LINE__);
					}else if([&](){ calc_pos = erb(BMF_CALC_POS, {{"id", index.at("calc_pos_id")}}); return calc_pos.empty(); }()){ mpre("ОШИБКА выборки позиции морфа", __LINE__);
					}else if([&](){ // Статус
						int status = 1;
						if(0 >= status){ mpre("ОШИБКА статус должен быть положительным", __LINE__);
						}else if([&](){ // Инкремент статуса
							if(stairs.end() == stairs.find("status")){ //mpre("Статус не установлен", __LINE__);
							}else if(string::npos != stairs.at("status").find_last_not_of("0123456789")){ mpre("ОШИБКА формат status="+ stairs["status"], __LINE__);
							}else if([&](){ status = stoi(stairs.at("status"))+1; return (0 >= status); }()){ mpre("ОШИБКА не допустимый статус", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Статус установка STAIRS["+ to_string(size-1)+ "] calc_pos="+ calc_pos.at("name")+ " status="+ to_string(status), __LINE__); //mpre(index["id"]+ " Обновленный статус "+ to_string(status), __LINE__);
							} return (0 >= status); }()){ mpre("ОШИБКА установки статуса", __LINE__);
						}else if([&](){ stairs_itr->second["status"] = stairs["status"] = to_string(status); return (0 >= stairs.at("status").length()); }()){ mpre("ОШИБКА установки статуса", __LINE__);
						}else{ //mpre(stairs["index_id"]+ " Установка STAIRS["+ to_string(STAIRS.size()-1)+ "] status="+ stairs["status"], __LINE__);
						} return (string::npos != stairs.at("status").find_last_not_of("123")); }()){ mpre("ОШИБКА установки состояния", __LINE__);
					}else if([&](){ // Направление дерева
						if(stairs.end() == stairs.find("status")){ mpre("ОШИБКА статус в ступени не установлен", __LINE__);
						}else if("1" == stairs.at("status")){ index_field = "index_id"; //mpre("Выбран старший переход", __LINE__);
						}else if("2" == stairs.at("status")){ index_field = "bmf-index"; //mpre("Выбрано младший переход", __LINE__);
						}else{ //mpre("", __LINE__);
						} return false; }()){ mpre("Определение направления", __LINE__);
					}else if([&](){ // Дочерний морф направления
						if("" == index_field){ //mpre("Направление не устанволено", __LINE__);
						}else if(index.end() == index.find(index_field)){ mpre("ОШИБКА поле с направления у морфа не найдены", __LINE__);
						}else if([&](){ index_next = erb(BMF_INDEX_EX, {{"id", index.at(index_field)}}); return false; }()){ mpre("ОШИБКА выборки морфа направления", __LINE__);
						}else{ //mpre(index_next, __LINE__, "Морф направления "+ index["id"]+ " "+ index_field);
						} return false; }()){ mpre("ОШИБКА морф выборки дочернего морфа направления", __LINE__);
					}else if([&](){ // Отображение
						string after = after_char; TMs calc_pos_next, dano_next;
						if(index_next.empty()){ //mpre("Пустой морф", __LINE__);
						}else if(index.end() == index.find("depth")){ mpre("ОШИБКА поле глубины у морфа не установлено", __LINE__);
						}else if(string::npos != index.at("depth").find_last_not_of("0123456789")){ mpre("ОШИБКА формат глучины", __LINE__);
						}else if([&](){ int depth = stoi(index.at("depth")); while(depth--){ after += after_char; }; return (0 >= after.length()); }()){ mpre("ОШИБКА получения строки отступа", __LINE__);
						}else if(index_next.end() == index_next.find("calc_pos_id")){ mpre("ОШИБКА следующий морф не содержит поля позиции", __LINE__);
						}else if([&](){ calc_pos_next = erb(BMF_CALC_POS, {{"id", index_next.at("calc_pos_id")}}); return calc_pos_next.empty(); }()){ mpre("ОШИБКА получения позиции нижестоящего", __LINE__);
						}else if([&](){ dano_next = erb(_BMF_DANO_EX, {{"id", index_next.at("dano_id")}}); return dano_next.empty(); }()){ mpre("ОШИБКА получения исходника нижестоящего морфа", __LINE__);
						}else if([&](){ mpre(after+ "index["+ index_field+ "]="+ index_next.at("id")+ " "+ calc_pos_next.at("name")+ " dano["+ dano_next.at("id")+ "]="+ dano_next.at("val"), __LINE__); return false; }()){ mpre("ОШИБКА отображения", __LINE__);
						}else{
						} return false; }()){ mpre("Отображение морфа", __LINE__);
					}else if([&](){ // Добавление супени
						TMs stairs_next;
						if(index_next.empty()){ //mpre("Не добавляем пустую", __LINE__);
						}else if(index_next.end() == index_next.find("id")){ mpre("ОШИБКА идентификатор у морфа не задан", __LINE__);
						}else if([&](){ stairs_next["index_id"] = index_next.at("id"); return stairs_next.empty(); }()){ mpre("ОШИБКА создания ступени", __LINE__);
						}else if([&](){ STAIRS.insert(make_pair(STAIRS.size(), stairs_next)); return STAIRS.empty(); }()){ mpre("ОШИБКА добавления новой ступени", __LINE__);
						}else{ //mpre("Добавление новой ступени "+ index_next["id"], __LINE__);
						} return false; }()){ mpre("Добавление новой ступени", __LINE__);
					}else if([&](){ // Удаление
						if(!index_next.empty()){ //mpre("Не удаляем с нижней ступенью", __LINE__);
						}else if(stairs.end() == stairs.find("status")){ mpre(stairs["index_id"]+ "ОШИБКА Статус последней ступени не установлен", __LINE__);
						}else if(string::npos == stairs.at("status").find_last_not_of("1")){ //mpre("Не удаляем статус "+ stairs.at("status"), __LINE__);
						}else if([&](){ STAIRS.erase(STAIRS.size()-1); return false; }()){ mpre("Окончание пустая лестница", __LINE__);
						}else{ //mpre("Удаление ступени "+ stairs.at("status"), __LINE__);
						} return false; }()){ mpre("ОШИБКА удаления ступени", __LINE__);
					}else{ //mpre(stairs, __LINE__, "Ступень"); mpre(index, __LINE__, "Морф");
					}}while((1e3 > ++rep) && (0 < STAIRS.size())); return false; }()){ mpre("ОШИБКА цикла", __LINE__);
				//}else if([&](){ mpre("================================", __LINE__); return false; }()){ mpre("ОШИБКА обозначения начала дерева", __LINE__);
				}else if([&](){ for(auto dano_itr:_BMF_DANO_EX.at("")){ // История исходников
					TMs dano;
					if([&](){ dano = dano_itr.second; return dano.empty(); }()){ mpre("ОШИБКА выборки исходника", __LINE__);
					}else{ mpre("Исходник "+ dano["id"]+ " "+ dano["values"], __LINE__);
					} } return false; }()){ mpre("История исходников", __LINE__);
				}else{
				} return bmf_index;
			}); return false; }()){ mpre("ОШИБКА функции отображения дерева", __LINE__);
		}else if([&Id](){ // Генерация локального идентификатора
			Id = ([&](TMMi& ROWS){
				std::lock_guard<std::recursive_mutex> lock(mu);
				int id;
				if(int id_max = (ROWS.empty() ? 0 : ROWS.rbegin()->first); false){ exit(mpre("ОШИБКА получения идентификатора последней записи: "+ to_string(id_max), __LINE__));
				}else if(int id_min = (ROWS.empty() ? 0 : ROWS.begin()->first); false){ exit(mpre("ОШИБКА получения идентификатора первой записи", __LINE__));
				}else if(int id_next = max(abs(id_min), abs(id_max))+1; (0 >= id_next)){ mpre("ОШИБКА получения максимального значения", __LINE__);
				}else if([&](){ id = (id_next)*-1; return (0 <= id); }()){ mpre("ОШИБКА устанвоки следующего id", __LINE__);
				}else if(ROWS.end() != ROWS.find(id)){ mpre("ОШИБКА Дублирование идентификаторов ["+ to_string(id_min)+ ":"+ to_string(id_max)+ "] "+ to_string(id), __LINE__);// exit(mpre(BMF_INDEX, __LINE__));
				}else{// mpre("Максимальный id_min: "+ to_string(id_min)+ " id_max: " + to_string(id_max) + " id_next: "+ to_string(id_next)+ " id:"+ id, __LINE__);
				} return to_string(id); // mpre("Возвращаемое значение: "+ id, __LINE__);
			}); return false; }()){ mpre("ОШИБКА создания функции локального идентификатора", __LINE__);
		}else if([&](){ // Преобразование десятеричной системы счисления в двоичную
			Dec2bin = ([&Dec2bin](double decimal){ // Получаем число возвращаем двойное число
				string binary, _dec, bin, _bin; 
				std::function<string(unsigned long long)> dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
				if([&](){ // Перевод целого числа в двоичную форму
					dec2bin = ([&](long long n){ // Получаем число возвращаем двойное число
						std::string r; long long i = n;
						do{ r=(i%2==0 ?"0":"1")+r;
						}while(0<(i/=2)); return (0 == n ? "0" : r);
					}); return false; }()){ mpre("ОШИБКА создания функции перевода из целого числа в двоичное", __LINE__);
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
				}else if(bin = dec2bin(stod(dec)); (0 >= bin.length())){ mpre("ОШИБКА перевода в двоичную форму целой части числа", __LINE__);
				}else if(std::reverse(_dec.begin(), _dec.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
				}else if(_bin = dec2bin(stod(_dec)); (0 >= _bin.length())){ mpre("ОШИБКА перевода в двоичную форму дробной части числа", __LINE__);
				}else if(std::reverse(_bin.begin(), _bin.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
				}else if(binary = bin+ ("0" == _bin ? "" : "."+_bin); (0 >= binary.length())){ mpre("ОШИБКА получения итогового двоичного числа", __LINE__);
				}else if(binary = (0 > decimal ? "-" : "")+ binary; (0 >= binary.length())){ mpre("ОШИБКА установки знака отрицания", __LINE__);
				}else{ //mpre("Исходное число "+ to_string(decimal)+ " "+ dec+ "."+ _dec, __LINE__); //mpre("Результат "+ bin+ "."+ _bin+ " => "+ binary, __LINE__);
				} return binary;
			}); return false; }()){ mpre("ОШИБКА формирования функции пересчета разрядности чисел", __LINE__);
		}else if([&](){ // Перевод из двоичной системы в десятичную
			Bin2dec = ([](string binary){ // Получаем число возвращаем двойное число
				double decimal; unsigned long long dec; string bin, _bin, _dec, _binary = binary;
				std::function<long long(string)> bin2dec; // Перерасчет размерности размерности из десятеричной в двоичную
					if([&](){ // Перевод целого числа в двоичную форму
					bin2dec = ([&](string binary){ // Получаем число возвращаем двойное число
						//mpre("Двоичный перевод "+ binary, __LINE__);
						unsigned long long decimal = 0;
						for(int i = 0; i < binary.length(); i++){
							if(int pos = binary.length()-i-1; (0 > pos)){ mpre("ОШИБКА получения позиции символа", __LINE__);
							}else if(int _pow = (unsigned long long)pow(2, i); (0 > _pow)){ mpre("ОШИБКА получения степени числа", __LINE__);
							}else if(string val = binary.substr(pos, 1); (1 != val.length())){ mpre("ОШИБКА получения символа", __LINE__);
							}else if(decimal += ("1" == val ? _pow : 0); (0 > decimal)){ mpre("ОШИБКА получения результата", __LINE__);
							}else{ //mpre("Смещение/степень i="+ to_string(i)+ " pos="+ to_string(pos)+ " val="+ val+ " _pow="+ to_string(_pow)+ " >> "+ to_string(decimal), __LINE__);
							}
						} return decimal;
					}); return false; }()){ mpre("ОШИБКА создания функции перевода из двоичного числа в десятичное", __LINE__);
				}else if(_binary = ("-" == _binary.substr(0, 1) ? _binary.substr(1, _binary.length()) : _binary); (0 >= _binary.length())){ mpre("ОШИБКА удаления символа отрицания", __LINE__);
				}else if([&](){ // Получение целой части числа
					if(int pos = _binary.rfind("."); false){ mpre("Число без дробной части", __LINE__);
					}else if(bin = (-1 == pos ? _binary.substr(0, _binary.length()) : _binary.substr(0, pos)); (0 >= bin.length())){ mpre("ОШИБКА нахождения целой части числа "+ binary+ " "+ _binary+ " "+ to_string(pos), __LINE__);
					}else if(0 > pos){ //mpre("Не расчитываем дробную часть", __LINE__);
					}else if(_bin = _binary.substr(pos+1, _binary.length()); (0 >= _bin.length())){ //mpre("Дробная часть не указана "+ _binary, __LINE__);
					}else if(pos = _bin.find_last_not_of('0'); false){ mpre("ОШИБКА Действительных чисел в дробной части не найдено", __LINE__);
					}else{ //mpre("Число с дробной частью в позиции "+ to_string(pos)+ " >> "+ _bin, __LINE__);
					} return false; }()){ mpre("ОШИБКА получения целой части числа "+ dec, __LINE__);
				}else if(dec = bin2dec(bin); (0 > dec)){ mpre("ОШИБКА перевода в двоичную форму целой части числа", __LINE__);
				}else if(std::reverse(_bin.begin(), _bin.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
				}else if(_dec = to_string(bin2dec(_bin)); (0 > _dec.length())){ mpre("ОШИБКА перевода в двоичную форму дробной части числа", __LINE__);
				}else if(std::reverse(_dec.begin(), _dec.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
				}else if(decimal = stod(to_string(dec)+ "."+_dec); false){ mpre("ОШИБКА получения итогового двоичного числа", __LINE__);
				}else if(decimal *= ("-" == binary.substr(0, 1) ? -1 : 1); false){ mpre("ОШИБКА установки отрицательного значения", __LINE__);
				}else{ //mpre("Результат расчетов "+ binary+ " "+ bin+ "("+ to_string(dec)+ ")."+ _bin+ "("+ _dec+ ") >> "+ to_string(decimal), __LINE__);
				} return decimal;
			}); return false; }()){ mpre("ОШИБКА Функция перевода из двоичной в десятичную систему", __LINE__);
		}else if([&](){ // Установка значений
			Values = ([&](TMs value, string alias, TM3i &BMF_VALUES, string clump_id){ // Проверка наличия значений в БД и установка новых
				//for_each(value.begin(), value.end(), [&](pair<string, string> value_itr){ // Устанавливаем значения
				for(auto &value_itr:value){
					if(string _values = value_itr.first; (0 >= _values.length())){ mpre("ОШИБКА получения имени значения", __LINE__);
					}else if(string _val = value_itr.second; false){ mpre("ОШИБКА получения значения из данных", __LINE__);
					}else if([&](){ // Добавление значения
						std::lock_guard<std::recursive_mutex> lock(mu);
						TMs values;
						if(values = erb(BMF_VALUES, {{"name", _values}}); !values.empty()){ //mpre("Значение уже создано `"+ _values+ "`", __LINE__);
						}else if(values = {{"id", Id(BMF_VALUES.at(""))}, {"name", _values}, {"clump_id", clump_id}, {"value", _val}}; values.empty()){ mpre("ОШИБКА фонмирования нового значения", __LINE__);
						}else if(erb_insert(BMF_VALUES, values["id"], values); BMF_VALUES.empty()){ mpre("ОШИБКА добавления нового значения в справочник", __LINE__);
						}else{ //mpre("ОШИБКА Добавляем новое значение `"+ _values+ "` ("+ alias+ ")", __LINE__); //mpre("Добавление значения", __LINE__);
						} return values.empty(); }()){ mpre("ОШИБКА добавления значения", __LINE__);
					}else{
					}
				}; return false;
			}); return false; }()){ mpre("ОШИБКА установки функции установки значений", __LINE__);
		}else if([&](){ // Установка значений
			Vals = ([&](TMs VALUE, string alias, int key, TM3i& BMF_VALS, TM3i& BMF_VALUES, TM3i& BMF_TITLES, string clump_id){ // Установка входных значений
				static TM3i DANO; bool cache = true;
				if(false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if([&](){ // Выборка данных из справочника
					if(DANO.empty()){ //mpre("Кеш знаков пустa", __LINE__);
					}else if(DANO.end() == DANO.find(alias)){ //mpre("Структура алиаса не задана", __LINE__);
					}else if(DANO.at(alias).end() == DANO.at(alias).find(key)){ //mpre("Структура знаков не задана", __LINE__);
					}else if(BMF_VALS.at("").size() != DANO.at(alias).at(key).size()){ //mpre("Запуск переиндексации", __LINE__);
					}else if([&](){ for(auto& vals_itr:BMF_VALS.at("")){ // Восстановление знаков
							TMs values, vals; string _values;
							if([&](){ vals = vals_itr.second; return vals.empty(); }()){ mpre("ОШИБКА получения знака", __LINE__);
							}else if(DANO.at(alias).at(key).end() == DANO.at(alias).at(key).find(vals.at("id"))){ mpre("Сохраненного знака не найдено", __LINE__);
							}else if(vals["val"] = DANO.at(alias).at(key).at(vals.at("id")); (1 != vals.at("val").length())){ mpre("ОШИБКА получения значения знака", __LINE__);
							}else if(_values = vals.at("values")+ vals.at("val"); (0 >= _values.length())){ mpre("ОШИБКА получения истории изменений знака", __LINE__);
							}else if(vals["values"] = (values_length < _values.length() ? _values.substr(_values.length()-values_length, _values.length()) : _values); vals.empty()){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
							}else if(vals_itr.second = vals; vals.empty()){ mpre("ОШИБКА сохранения знака в справочнике", __LINE__);
							}else if([&](){ // Выборка значения
								std::lock_guard<std::recursive_mutex> lock(mu);
								if(string field = alias+ "_values_id"; false){ mpre("ОШИБКА установки значения поля", __LINE__);
								}else if(vals.end() == vals.find(field)){ mpre("ОШИБКА поле в знаке не найдено "+ field, __LINE__);
								}else if(BMF_VALUES.at("").end() == BMF_VALUES.at("").find(stoi(vals.at(field)))){ mpre("ОШИБКА значение в справочнике не найдено", __LINE__);
								}else if(values = BMF_VALUES.at("").at(stoi(vals.at(field))); values.empty()){ mpre(BMF_VALUES.at(""), __LINE__, "Значения"); mpre(vals, __LINE__, "Знак"); mpre("ОШИБКА выборки значения из справочника "+ field, __LINE__);
								}else{ //mpre(values, __LINE__, "Значение");
								} return values.empty(); }()){ mpre("ОШИБКА выборки значения", __LINE__);
							}else{ //mpre("Кеширование "+ alias+ "["+ vals.at("id")+ "] "+ values.at("name")+ "("+ vals.at("name")+ ") key="+ to_string(key)+ " val="+ vals.at("val"), __LINE__);
							}
						} return false; }()){ mpre("ОШИБКА загрузки значений знаков", __LINE__);
					}else{ //mpre(DANO.at(alias).at(stoi(key)), __LINE__, "Список знаков "+ alias+ " "+ key); mpre(BMF_VALS.at(""), __LINE__, "Знаки"); mpre("ОШИБКА Загрузка значений знаков из кеша "+ key, __LINE__); //mpre(DANO.at(alias).at(stoi(key)), __LINE__, "знаки "+ key); mpre("ОШИБКА Загрузка кешей", __LINE__);
						return true;
					} return false; }()){ //mpre("Кеширование "+ to_string(key), __LINE__);
				}else if([&](){ // Расчет знаков
					for(auto &val_itr:VALUE){ //mpre("VALUE ", __LINE__);
						TMMi VALS; string val, _value, _value_; long long bin; int pos;
						if(string value = val_itr.second; false){ mpre("ОШИБКА получения значения итератора", __LINE__);
						//}else if([&](){ mpre("VALUE ("+ value+ ") "+ alias, __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
						}else if(TMs values = [&](TMs values = {}){ // Выборка значения и если не найден добавляем
							std::lock_guard<std::recursive_mutex> lock(mu);
							if([&](){ values = erb(BMF_VALUES, {{"name", val_itr.first}}); return !values.empty(); }()){ //mpre("Значение уже создано `"+ val_itr.first+ "`", __LINE__);
							}else{ mpre("ОШИБКА значение не найдено в списке `"+ val_itr.first+ "`", __LINE__);
							} return values; }(); values.empty()){ mpre("ОШИБКА выборки значения", __LINE__);
						}else if(TM3i* BMF; false){ mpre("ОШИБКА установки ссылки на список исходных знаков", __LINE__);
						}else if([&](){ // Глобавльный справочник
							std::lock_guard<std::recursive_mutex> lock(mu);
							if("dano" == alias){ BMF = &BMF_DANO_EX;
							}else if("itog" == alias){ BMF = &BMF_ITOG_EX;
							}else{ mpre("ОШИБКА выборки глобального справочника", __LINE__);
							} return (*BMF).empty(); }()){ mpre("ОШИБКА выборки глобального справочника", __LINE__);
						}else if(TMs bmf_titles = [&](TMs bmf_titles = {}){ // Получение заголовка значения если нет то его создание
							std::lock_guard<std::recursive_mutex> lock(mu);
							if(regex b("^[0-9|.|-]+$"); regex_match(value, b)){ //mpre("Формат значения `"+ value_itr.first+ "` верен `"+ value_itr.second+ "`", __LINE__);
							}else if(bmf_titles = erb(BMF_TITLES, {{alias+ "_values_id", values["id"]}, {"name", value}}); !bmf_titles.empty()){ //mpre("Заголовок найден в справочнике "+ value, __LINE__);
							}else if(TMMi TITLES = rb(BMF_TITLES, {{alias+ "_values_id", values["id"]}}); false){ mpre("ОШИБКА получения полного списка", __LINE__);
							}else if(int nn = [&](int nn = 0){ //mpre(bmf_titles, __LINE__, "Заголовок"); // Получение номера следующего заголовка
								std::lock_guard<std::recursive_mutex> lock(mu);
								if(TMMi NN; false){ mpre("ОШИБКА создания первоначальной структуры", __LINE__);
								}else if([&](){ for(auto &titles_itr:TITLES){ //for_each(TITLES.begin(), TITLES.end(), [&](auto &titles_itr){
									if(TMs titles = titles_itr.second; titles.empty()){ mpre("ОШИБКА получения заголовка из списка", __LINE__);
									}else if(titles.end() == titles.find("value")){ mpre("ОШИБКА поле value не установлено в заголовоке", __LINE__);
									}else if(string _nn = titles.at("value"); (0 >= _nn.length())){ mpre("ОШИБКА значение номера заголовока не задано", __LINE__);
									}else if(NN.insert(make_pair(stoi(_nn), titles)); NN.empty()){ mpre("ОШИБКА добавления значения в список", __LINE__);
									}else{ //mpre(NN, __LINE__, "NN");
									} }; return false; }()){ mpre("ОШИБКА получени списка по номерам", __LINE__);
								}else if(nn = (NN.empty() ? -1 : NN.begin()->first-1); (0 <= nn)){ mpre("ОШИБКА нахождения следующего номера "+ to_string(nn), __LINE__);
								}else if(bmf_titles = {{"id", Id(BMF_TITLES.at(""))}, {"clump_id", clump_id}, {alias+ "_values_id", values["id"]}, {"value", to_string(nn)}, {"name", value}}; bmf_titles.empty()){ mpre("ОШИБКА получения нового заголовка", __LINE__);
								}else if(erb_insert(BMF_TITLES, bmf_titles["id"], bmf_titles); BMF_TITLES.empty()){ mpre("ОШИБКА добавления заголовка в справочник", __LINE__);
								}else{ //mpre(bmf_titles, __LINE__, "Значение следующего заголовка `"+ value+ "` "+ to_string(nn));
								} return nn; }(); (0 < nn)){ mpre("ОШИБКА определения номера заголовока `"+ value+ "`", __LINE__);
							}else{ //mpre(bmf_titles, __LINE__, "Заголовок"); mpre("ОШИБКА Установлено наличие заголовка", __LINE__);
							} return bmf_titles; }(); false){ mpre("ОШИБКА получения заголовка", __LINE__);
						}else if([&](){ // Расчитываем и устанавливаем значение
							std::lock_guard<std::recursive_mutex> lock(mu);
							if(value = (bmf_titles.empty() ? value : bmf_titles.at("value")); (0 >= value.length())){ mpre("ОШИБКА Значение не определено", __LINE__);
							}else if(values["value"] = value; values.empty()){ mpre("ОШИБКА установки результата значению", __LINE__);
							}else if(BMF_VALUES.at("").at(stoi(values["id"])) = values; (BMF_VALUES.find("values") != BMF_VALUES.end())){ mpre(BMF_VALUES, __LINE__, "Справочник"); mpre("ОШИБКА сохранения результата в справочнике", __LINE__);
							}else{ //mpre(values, __LINE__, "Устанавливаем значние"); //mpre("Установка результата значению `"+ values["name"]+ "` "+ value, __LINE__);
							} return values.empty(); }()){ mpre("ОШИБКА установки результата значению", __LINE__);
						}else if(_value = Dec2bin(stod(value)); (0 >= _value.length())){ mpre("ОШИБКА получения строки сиволов двоичного розультата "+ value, __LINE__);
						}else if(_value_ = ("-" == _value.substr(0, 1) ? _value : "+"+ _value); (0 >= _value.length())){ mpre("ОШИБКА получения значения без знака", __LINE__);
						}else if(pos = _value_.rfind("."); false){ mpre("ОШИБКА получения позиции точки", __LINE__);
						}else if([&](){ // //mpre("Добавляем новый знак "+ values["name"]+ " "+ to_string(VALS.size())+ "=>"+ to_string(_value.length())+ " "+ _value+ " ("+ to_string(value)+ ")" , __LINE__); //system("sleep 1");
								for(int i = 0; i < _value_.length(); i++){ // Добавление знаков значению
									if(val = _value_.substr(i, 1); (1 != val.length())){ mpre("ОШИБКА получения символа "+ to_string(i), __LINE__);
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
									}else if(TMs vals = [&](TMs vals = {}){ // Проверка и выборка глобального знака
										std::lock_guard<std::recursive_mutex> lock(mu);
										if(vals = erb(*BMF, {{"clump_id", clump_id}, {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}}); !vals.empty()){ //mpre(vals, __LINE__, "Знак `"+ values["name"]+ "` уже добавлен "+ to_string(nn));
										}else if(("-" != _value.substr(0, 1)) && (0 == nn)){ //mpre("Не создаем положительный знак отрицания", __LINE__);
										//}else if(ARGV.end() != ARGV.find("-t")){ mpre(values, __LINE__, "Значение"); mpre("ОШИБКА в многопоточном режиме недопустимо создание новых знаков "+ alias+ "_values_id="+ values.at("id")+ " name="+ to_string(nn), __LINE__);
										}else if(vals = {{"id", Id((*BMF).at(""))}, {"clump_id", clump_id}, {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}, {"val", ""}, {"values", ""}}; vals.empty()){ mpre("ОШИБКА создания нового знака", __LINE__);
										}else if(erb_insert(*BMF, vals.at("id"), vals); vals.empty()){ mpre("ОШИБКА обновления справочника знаков", __LINE__);
										}else{ //mpre(*BMF, __LINE__, "Список знаков"); mpre("Добавление глобального знака `"+ values["name"]+ "` ["+ vals.at("id")+ "] "+ to_string(nn), __LINE__); mpre(vals, __LINE__, "Знак"); //mpre("ОШИБКА добавления знака "+ alias+ " "+ vals.at("id"), __LINE__);
										} return vals; }(); false){ mpre("ОШИБКА Добавления нового знака", __LINE__);
									}else if(TMs _vals = [&](TMs _vals = {}){ // Синхронизация глобального значения с локальным
										if(vals.empty()){ //mpre("Не найден знак", __LINE__);
										}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА устанвоки блокировки", __LINE__);
										}else if(_vals = (BMF_VALS.at("").end() == BMF_VALS.at("").find(stoi(vals.at("id"))) ? TMs({}) : BMF_VALS.at("").at(stoi(vals.at("id")))); !_vals.empty()){ //mpre("Знак есть в локальной базе", __LINE__);
										}else if(_vals = vals; _vals.empty()){ mpre("ОШИБКА копирования глобального значения", __LINE__); //mpre(_vals, __LINE__, "Знак `"+ values["name"]+ "` уже добавлен "+ to_string(nn));
										//}else if([&](){ mpre("Добавление знака "+ alias+ " `"+ values.at("name")+ "` ("+ to_string(nn)+ ") "+ vals.at("id"), __LINE__); return false; }()){ mpre("ОШИБКА", __LINE__);
										}else if(erb_insert(BMF_VALS, _vals.at("id"), _vals); _vals.empty()){ mpre("ОШИБКА обновления справочника знаков", __LINE__);
										}else{ //mpre("Добавление глобального знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); mpre(_vals, __LINE__, "Знак"); mpre("ОШИБКА добавления знака "+ alias+ " "+ _vals.at("id"), __LINE__);
										} return _vals; }(); false){ mpre("ОШИБКА Добавления локального знака", __LINE__);
									}else if([&](){ // Установка свойств знака
										if(_vals.empty()){ //mpre("ОШИБКА знак не найден", __LINE__);
										}else if(_vals.at("val") = val; (1 != val.length())){ mpre("ОШИБКА установки знака знака", __LINE__);
										}else if(string _values = _vals.at("values")+ val; (0 >= _values.length())){ mpre("ОШИБКА инкремента знака к истории", __LINE__);
										}else if(_vals.at("values") = (values_length < _values.length() ? _values.substr(_values.length()-values_length, _values.length()) : _values); (0 >= _values.length())){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
										}else{ //mpre("Установка свойст знака", __LINE__);
										} return false; }()){ mpre("ОШИБКА установки свойств нового знака", __LINE__);
									}else if([&](){ // Сохранение
										if(_vals.empty()){ //mpre("ОШИБКА пустое значение знака", __LINE__);
										}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
										}else if(BMF_VALS.at("").end() == BMF_VALS.at("").find(stoi(_vals["id"]))){ mpre("ОШИБКА знак не найден в списке", __LINE__);
										}else if(BMF_VALS.at("").at(stoi(_vals["id"])).at("val") = _vals.at("val"); false){ mpre("ОШИБКА установки значения создан индекс `vals`", __LINE__);
										}else if(BMF_VALS.at("").at(stoi(_vals["id"])).at("values") = _vals["values"]; false){ mpre("ОШИБКА установки значения создан индекс `values`", __LINE__);
										}else if(VALS.insert(make_pair(stoi(_vals.at("id")), _vals)); VALS.empty()){ mpre("ОШИБКА формирования списка измененных знаков", __LINE__);
										}else{ //mpre("Сохранение знака", __LINE__);
										} return false; }()){ mpre("ОШИБКА сохраенния знака", __LINE__);
									}else{ //mpre("Установка знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); //mpre(_vals, __LINE__, "Добавление нового знака "+ to_string(i));
									}
								}  return false;
							}()){ mpre("ОШИБКА добавления знаков значению", __LINE__);
						}else if([&](){ // Установка истории
							std::lock_guard<std::recursive_mutex> lock(mu);
							for(auto& vals_itr:rb(BMF_VALS, {{alias+ "_values_id", values["id"]}})){ // Установка истории
								if(TMs vals = vals_itr.second; vals.empty()){ mpre("ОШИБКА выборки знака", __LINE__);
								}else if([&](){ // Пропускаем проверяем на заполненность
									if(VALS.end() == VALS.find(stoi(vals.at("id")))){ //mpre("Знак не изменен", __LINE__);
									}else if(TMs _vals = VALS.at(stoi(vals.at("id"))); _vals.empty()){ mpre("ОШИБКА выборки измененного знака", __LINE__);
									}else if(_vals.end() == _vals.find("val")){ mpre("ОШИБКА у измененного знака не установлено поле val", __LINE__);
									}else if(1 != _vals.at("val").length()){ mpre("ОШИБКА формате значения измененного знака", __LINE__);
									}else{ return true;
									} return false; }()){ //mpre("Уже измененный морф", __LINE__);
								}else if(vals.at("val") = "0"; (1 != val.length())){ mpre("ОШИБКА установки знака знака", __LINE__);
								}else if(string _values = vals.at("values")+ val; (0 >= _values.length())){ mpre("ОШИБКА инкремента знака к истории", __LINE__);
								}else if(vals.at("values") = (values_length < _values.length() ? _values.substr(_values.length()-values_length, _values.length()) : _values); (0 >= _values.length())){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
								}else if(BMF_VALS.at("").at(stoi(vals.at("id"))).at("val") = vals.at("val"); vals.empty()){ mpre("ОШИБКА сохраенния расчетов", __LINE__);
								}else if(BMF_VALS.at("").at(stoi(vals.at("id"))).at("values") = vals.at("values"); vals.empty()){ mpre("ОШИБКА сохраенния расчетов", __LINE__);
								}else{ //mpre("Скидывание значения "+ alias+ " _value_="+ _value_+ " "+ vals.at("id"), __LINE__);
								}
							} return false; }()){ mpre("ОШИБКА скидывания значения", __LINE__);
						}else{ //mpre(BMF_VALS.at(""), __LINE__, "Список знаков"); //mpre(BMF_VALS, __LINE__, "Знаки"); mpre("ОШИБКА Знака "+ alias, __LINE__); //mpre(values, __LINE__, "Значение"); mpre(VALS, __LINE__, "Значения");
						}
					}; return false; }()){ mpre("ОШИБКА расчета знаков", __LINE__);
				}else if([&](){ // Сохранение данных в статическом справочнике
					if(!cache){ //mpre("Кеширование значений отключено "+ key, __LINE__);
					}else if([&](){ // Проверка и создание структуры alias
						if(DANO.end() != DANO.find(alias)){ //mpre("Структура уже сохдана", __LINE__);
						}else if(DANO.insert(make_pair(alias, TMMi({}))); DANO.empty()){ mpre("ОШИБКА добавления структуры алиаса", __LINE__);
						}else{ //mpre("Создание кеша стркутуры алиаса", __LINE__);
						} return false; }()){ mpre("ОШИБКА создания структуры alias", __LINE__);
					}else if([&](){ // Создание структуры знаков
						if(DANO.at(alias).end() != DANO.at(alias).find(key)){ //mpre("Структура знаков уже создана", __LINE__);
						}else if(DANO.at(alias).insert(make_pair(key, TMs({}))); DANO.at(alias).empty()){ mpre("ОШИБКА создания структуры знаков", __LINE__);
						}else{ //mpre("Создание структуры знаков "+ to_string(key), __LINE__);
						} return false; }()){ mpre("ОШИБКА создания структуры знаков", __LINE__);
					}else if([&](){ for(auto& vals_itr:BMF_VALS.at("")){ // Сохранение знаков в кеш
							if(TMs vals = vals_itr.second; vals.empty()){ mpre("ОШИБКА получения знака", __LINE__);
							}else if(vals.end() == vals.find("val")){ mpre("ОШИБКА значение у знака не задано", __LINE__);
							}else if(string val = vals.at("val"); (1 != val.length())){ mpre(BMF_VALS.at(""), __LINE__, "Список знаков"); mpre(VALUE, __LINE__, "Значения"); mpre(vals, __LINE__, "Знак"); mpre("ОШИБКА выборки значения знака", __LINE__);
							}else if(DANO.at(alias).at(key).insert(make_pair(vals.at("id"), val)); DANO.at(alias).at(key).empty()){ mpre("ОШИБКА сохранения значения знака", __LINE__);
							}else{ //mpre("Загрузка знака "+ vals.at("id")+ " val="+ val, __LINE__);
							}
						} return false; }()){ mpre("ОШИБКА сохранения знаков в кеш", __LINE__);
					}else{ //mpre("Сохранение кеша "+ to_string(key), __LINE__);
					} return false; }()){ mpre("Сохранение данных", __LINE__);
				}else{
				} return false;
			}); return false; }()){ mpre("ОШИБКА установки значений", __LINE__);
		}else if([&](){ // Расчет истории
				History = ([&](TMs index, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX){ //mpre("Получение истории морфа", __LINE__);
					TMs dano, index_1, index_0, calc_pos; string values, values_1, values_0;
					if(false){ mpre("ОШИБКА", __LINE__);
					}else if([&](){ // Старший потомок
						if("" == index.at("index_id")){ //mpre("Связь со старшим потомком не указана", __LINE__);
						}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
						}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(index.at("index_id")))){ mpre("ОШИБКА морф не найден в справочнике", __LINE__);
						}else if(index_1 = BMF_INDEX_EX.at("").at(stoi(index.at("index_id"))); index_1.empty()){ mpre("ОШИБКА выборки старшего морфа index_id", __LINE__);
						}else{ //mpre(index_1, __LINE__, "Старший морф");
						} return false; }()){ mpre("ОШИБКА получения старшего потомка", __LINE__);
					}else if([&](){ // Младший потомок
						if("" == index.at("bmf-index")){ //mpre("Связь с младшим потомком не указана", __LINE__);
						}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
						}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(index.at("bmf-index")))){ mpre("ОШИБКА морф не найден в справочнике", __LINE__);
						}else if(index_0 = BMF_INDEX_EX.at("").at(stoi(index.at("bmf-index"))); index_0.empty()){ mpre("ОШИБКА выборки младшего морфа bmf-index", __LINE__);
						}else{ //mpre(index_0, __LINE__, "Младший морф");
						} return false; }()){ mpre("ОШИБКА получения старшего потомка", __LINE__);
					}else if([&](){ // Исходник локального морфа
						if(_BMF_DANO_EX.at("").end() == _BMF_DANO_EX.at("").find(stoi(index.at("dano_id")))){ mpre("ОШИБКА связь морфа с дано не указана", __LINE__);
						}else if( dano = _BMF_DANO_EX.at("").at(stoi(index.at("dano_id"))); dano.empty()){ mpre("Морф не найден в локальных исходниках", __LINE__);
						}else{ //mpre("Исходник морфа", __LINE__);
						} return dano.empty(); }()){ mpre("ОШИБКА получения исходника морфа", __LINE__);
					}else if(values_1 = (index_1.empty() ? dano.at("values") : History(index_1, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_1.length())){ mpre("ОШИБКА получения истории старшего морфа", __LINE__);
					}else if(values_0 = (index_0.empty() ? dano.at("values") : History(index_0, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_0.length())){ mpre("ОШИБКА получения истории младшего морфа", __LINE__);
					}else if([&](){ // Позиция морфа
						if(BMF_CALC_POS.end() == BMF_CALC_POS.find(stoi(index.at("calc_pos_id")))){ mpre("ОШИБКА позиция морфа не найдена", __LINE__);
						}else if(calc_pos = BMF_CALC_POS.at(stoi(index.at("calc_pos_id"))); calc_pos.empty()){ mpre("ОШИБКА выборки позици морфа", __LINE__);
						}else{ //mpre(calc_pos, __LINE__, "Позиция морфа");
						} return calc_pos.empty(); }()){ mpre("ОШИБКА выборки позиции морфа", __LINE__);
					}else if([&](){ // Расчет истории морфа
						int pos = 0, size = min(values_1.length(), values_0.length());
						while(++pos <= size){
							if(string val_1 = values_1.substr(values_1.length()-size+pos-1, 1); (1 != val_1.length())){ mpre("ОШИБКА получения значения старшей истории", __LINE__);
							}else if(string val_0 = values_0.substr(values_0.length()-size+pos-1, 1); (1 != val_0.length())){ mpre("ОШИБКА получения значения младшей истории", __LINE__);
							}else if(string val = (((calc_pos.at("v1") == val_1) && (calc_pos.at("v1") == val_0)) ? calc_pos.at("calc-1") : calc_pos.at("calc-0")); (1 != val.length())){ mpre("ОШИБКА расчета значения", __LINE__);
							}else if(values += val; (0 >= values.length())){ mpre("ОШИБКА формирования истории", __LINE__);
							}else{ //mpre("Цикл "+ to_string(size)+ " "+ to_string(pos)+ " "+ values+ " ("+ values_1+ ","+ values_0+ ")", __LINE__);
								continue;
							} break;
						}; return false; }()){ mpre("ОШИБКА расчета истории морфа", __LINE__);
					}else{ //mpre(index, __LINE__, "Морф"); mpre("ОШИБКА нахождения истории "+ values, __LINE__);
					} return values;
				});
			return false; }()){ mpre("ОШИБКА создания функции расчета истории", __LINE__);
		}else if([&Choice, &Calc, &History, &values_length](){ // Выбор подходящего знака
				Choice = ([&](TMs index, string index_val, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX){ //mpre("Выбор итога для расширения", __LINE__);
					TMs dano, _dano, _dano_, index_1, index_0; string index_values; 
					if(1 > index_val.length()){ mpre("ОШИБКА выбора исходника значение не задано", __LINE__);
					}else if(index_val = ("0" == index_val ? "1" : "0"); (1 != index_val.length())){ mpre("ОШИБКА инвертирования значения", __LINE__);
					}else if([&](){ // Выборка старшего морфа
							if(index.end() == index.find("index_id")){ mpre("ОШИБКА у морфа отсутствует поле старшей связи", __LINE__);
							}else if("" == index.at("index_id")){ //mpre("Пустой морф", __LINE__);
							}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
							}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(index.at("index_id")))){ mpre("ОШИБКА указанный морф не найден в спиавочнике", __LINE__);
							}else if(index_1 = BMF_INDEX_EX.at("").at(stoi(index.at("index_id"))); false){ mpre("ОШИБКА выборки морфа", __LINE__);
							}else{ //mpre(index, __LINE__, "Морф"); mpre(BMF_INDEX_EX.at(""), __LINE__, "Спиравочник");
							}
						return false; }()){ mpre("ОШИБКА выборки старшего морфа", __LINE__);
					}else if([&](){ // Выборка старшего морфа
							if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА у морфа отсутствует поле младшей связи", __LINE__);
							}else if("" == index.at("bmf-index")){ //mpre("Пустой морф", __LINE__);
							}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
							}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(index.at("bmf-index")))){ mpre("ОШИБКА указанный морф не найден в спиавочнике", __LINE__);
							}else if(index_0 = BMF_INDEX_EX.at("").at(stoi(index.at("bmf-index"))); false){ mpre("ОШИБКА выборки морфа", __LINE__);
							}else{ //mpre(index, __LINE__, "Морф"); mpre(BMF_INDEX_EX.at(""), __LINE__, "Спиравочник");
							}
						return false; }()){ mpre("ОШИБКА выборки старшего морфа", __LINE__);
					}else if([&](){ // Выборка итога
							if(index.end() == index.find("dano_id")){ mpre("ОШИБКА у морфа отсутствует ссылка на источник", __LINE__);
							}else if("" == index.at("dano_id")){ mpre("Дано у морфа не указано", __LINE__);
							}else if(_BMF_DANO_EX.at("").end() == _BMF_DANO_EX.at("").find(stoi(index.at("dano_id")))){ mpre("ОШИБКА исходник в справочнике не указан", __LINE__);
							}else if(_dano_ = _BMF_DANO_EX.at("").at(stoi(index.at("dano_id"))); _dano_.empty()){ mpre("ОШИБКА выборки исходника", __LINE__);
							}else{ //mpre(_dano_, __LINE__, "Исходник");
							}
						return _dano_.empty(); }()){ mpre("ОШИБКА получения итога", __LINE__);
					}else if(string values_0 = (index_0.empty() ? _dano_.at("values") : History(index_0, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_0.length())){ mpre(_BMF_DANO_EX.at(""), __LINE__, "Дано"); mpre("ОШИБКА получения истории основного дано", __LINE__);
					}else if(string values_1 = (index_1.empty() ? _dano_.at("values") : History(index_1, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_1.length())){ mpre(_BMF_DANO_EX.at(""), __LINE__, "Дано"); mpre("ОШИБКА получения истории основного дано", __LINE__);
					}else if([&](){ // Расчет основной истории
							int offset = 1;
							do{ //mpre("Смещение "+ to_string(offset), __LINE__);
								if(values_0.length() < offset){ //mpre("ОШИБКА размер младшей истории больше смещения", __LINE__);
								}else if(values_1.length() < offset){ //mpre("ОШИБКА размер старшей истории больше смещения", __LINE__);
								}else if(string val_1 = values_1.substr(values_1.length()-offset, 1); (1 != val_1.length())){ mpre("ОШИБКА получения старшего знака истории", __LINE__);
								}else if(string val_0 = values_0.substr(values_0.length()-offset, 1); (1 != val_0.length())){ mpre("ОШИБКА получения младшего знака истории", __LINE__);
								}else if(TMs _calc_val = erb(BMF_CALC_VAL, {{"v1", val_1}, {"v0", val_0}}); _calc_val.empty()){ mpre("ОШИБКА получения значения сигналов", __LINE__);
								}else if(TMs _calc = erb(BMF_CALC, {{"calc_pos_id", index.at("calc_pos_id")}, {"calc_val_id", _calc_val.at("id")}}); _calc.empty()){ mpre("ОШИБКА выборки расчета морфа", __LINE__);
								}else if(index_values = _calc.at("val")+ index_values; (0 >= index_values.length())){ mpre("ОШИБКА инкремента знака истории", __LINE__);
								}else{ //mpre("Старший "+ val_1+ " младший "+ val_0+ " история "+ index_values, __LINE__);
								} //break;
							}while(++offset <= values_length);
							return (0 >= index_values.length());
						}()){ mpre("ОШИБКА расчета основной истории", __LINE__);
					}else if(0 >= index_values.length()){ mpre("ОШИБКА история морфа пуста values_index", __LINE__);
					}else if([&](){ // Зеркалируем последний знак морфа
							if(string _val = index_values.substr(index_values.length()-1, 1); (1 != _val.length())){ mpre("ОШИБКА получения последнего перевернутого сигнала", __LINE__);
							}else if(string _shift = ("0" == _val ? "1" : "0"); (1 != _shift.length())){ mpre("ОШИБКА получения перевернутого последнего значения", __LINE__);
							}else if(index_values = index_values.substr(0, index_values.length()-1)+ _shift; (0 >= index_values.length())){ mpre("ОШИБКА длинна расчетного значения не совпадает с исходным", __LINE__);
							}else{ //mpre(index, __LINE__, "Зеркальный последний сигнал морфа "+ values); system("sleep 1");
							} return (0 >= index_values.length());
						}()){ mpre(index, __LINE__, "ОШИБКА переворота последнего сигнала истории морфа");
					//}else if([&](){ mpre("Середина Choice "+ index.at("id"), __LINE__); return false; }()){ mpre("ОШИБКА", __LINE__);
					}else if(int _level = 0; false){ mpre("Установка счетчика уровня", __LINE__);
					}else if([&](){ for(auto &_dano_itr:_BMF_DANO_EX.at("")){ // Расчет максимального совпадения
							string _dano_val; bool _dano_equal; int _level_ = -1;
							if(TMs _dano_ = _dano_itr.second; _dano_.empty()){ mpre("ОШИБКА получения исходника из итератора", __LINE__);
							}else if(_dano_.at("id") == index.at("dano_id")){ //mpre("Пропускаем исходник морфа "+ index.at("dano_id"), __LINE__);
							}else if(string _dano_values = _dano_.at("values"); (0 >= _dano_values.length())){ mpre("ОШИБКА получения истории очереного исходника", __LINE__);
							}else if(0 >= _dano_values.length()){ mpre("ОШИБКА нулевая длинна истории исходника", __LINE__);
							}else if(string dano_val = _dano_values.substr(_dano_values.length()-1, 1); (1 != dano_val.length())){ mpre("ОШИБКА получения знака очередного исходника", __LINE__);
							}else if(bool dano_equal = (dano_val != index_val); false){ mpre("ОШИБКА нахождения эквивалентов значений исходников", __LINE__);
							}else if(int _min = min(index_values.length(), _dano_values.length()); (0 >= _min)){ mpre("ОШИБКА нахождения максимальной длинны истории", __LINE__);
							}else if([&](){ // Перебор значений история до момента несовпадения
									do{
										if(_level_ += 1; (0 > _level_)){ mpre("ОШИБКА инкремента смещения", __LINE__);
										}else if(_level_ >= _min){ //mpre("Уровень больше длинны минимального значения", __LINE__);
										}else if(string _index_val = index_values.substr(index_values.length()-_level_-1, 1); (1 != _index_val.length())){ mpre("ОШИБКА получения знака морфа", __LINE__);
										}else if(_dano_val = _dano_values.substr(_dano_values.length()-_level_-1, 1); (1 != _dano_val.length())){ mpre("ОШИБКА получения знака исходника", __LINE__);
										}else if(_dano_equal = (_index_val == _dano_val); false){ mpre("ОШИБКА расчета эквивалентности значений", __LINE__);
										}else{ //mpre("Смещение "+ to_string(_level_)+ " "+ _index_val+ ":"+ _dano_val+ " _dano_equal="+ (_dano_equal ? "1" : "0")+ " _equal="+ (_equal ? "1" : "0"), __LINE__);
										}
									}while((_min > _level_) && (dano_equal == _dano_equal)); return (0 > _level_);
								}()){ mpre("ОШИБКА получения уровня совпадения", __LINE__);
							}else if(_level > _level_){ //mpre("Уровень "+ to_string(_level_)+ " меньше или равен предыдущему "+ to_string(_level), __LINE__);
							}else if([&](){ // Сравнение с уровнем
								if(_level < _level_){ //mpre("Уровень больше", __LINE__);
								}else if(_dano.empty()){ //mpre("Нет результата", __LINE__);
								}else if((_level == _level_) && (clock()%2)){ //mpre("ОШИБКА Уровень совпадает _level="+ to_string(_level)+ " _level_="+ to_string(_level_)+ " clock("+ to_string(clock())+ ")", __LINE__);
								}else{ //mpre("Уровень меньше", __LINE__);
									return true;
								} return false; }()){ //mpre("Уровень меньше или равен", __LINE__);
							}else if(_level = _level_; (0 > _level)){ mpre("ОШИБКА сохранения максимального уровня", __LINE__);
							}else if(_dano = _dano_; _dano.empty()){ mpre("ОШИБКА установки подходящего исходника", __LINE__);
							}else{ //mpre("Выбор "+ to_string(_level_)+ " "+ _dano_values+ " min="+ to_string(_min), __LINE__);
							}
						} return _dano.empty(); }()){ mpre("ОШИБКА нахождения длинн совпадения история", __LINE__);
					}else if(_dano.empty()){ mpre("ОШИБКА нахождения подходящего результата", __LINE__);
					}else{ //mpre("Максимальный уровень "+ to_string(_level)+ " номер источника "+ _dano["id"], __LINE__);
					} return _dano;
				}); return false;
			}()){ mpre("ОШИБКА установки функции выбора знака", __LINE__);
		}else{ return false; //mpre("Функции успешно установлены", __LINE__);
		} return true; }()){ mpre("ОШИБКА установки списка функций", __LINE__);
	}else if([&](){ // Тесты
		if(1 > argc){ mpre("Параметров для теста не достатоно", __LINE__);
		}else if(true){ return false; //mpre("Безусловный пропуск тестов", __LINE__);
		}else if("./bmf" != argv[0]){ mpre("Пропуск тестов", __LINE__);
		}else if([&](){ // Тест работы ключей
			if(TMs index = {{"id", Id(BMF_INDEX_EX.at(""))}, {"index_id", "1"}, {"calc_pos_id", "2"}}; index.empty()){ mpre("ОШИБКА создания нового элемента", __LINE__);
			}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("Добавление элемента к списку", __LINE__);
			}else if(index = {{"id", Id(BMF_INDEX_EX.at(""))}, {"index_id", "3"}, {"calc_pos_id", "2"}}; index.empty()){ mpre("ОШИБКА создания нового элемента", __LINE__);
			}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("Добавление элемента к списку", __LINE__);
			}else if([&](){
				if(TMs values = {{"index_id", "1"}}; values.empty()){ mpre("ОШИБКА установки значений выборки", __LINE__);
				}else if([&](){ index = erb(BMF_INDEX_EX, values); return index.empty(); }()){ mpre("ОШИБКА выборки", __LINE__);
				}else{ mpre(values, __LINE__, "Условия"); mpre(index, __LINE__, "Результат"); mpre("Тест пройден корректно", __LINE__);
					return false;
				} return true; }()){ mpre("ОШИБКА тест не пройден", __LINE__);
			}else if([&](){
				if(TMs values = {{"calc_pos_id", "2"}}; values.empty()){ mpre("ОШИБКА установки значений выборки", __LINE__);
				}else if(TMMi INDEX = rb(BMF_INDEX_EX, values); INDEX.empty()){ mpre("ОШИБКА выборки", __LINE__);
				}else if(2 != INDEX.size()){ mpre("ОШИБКА количество значений не верное", __LINE__);
				}else{ mpre(values, __LINE__, "Условия"); mpre(INDEX, __LINE__, "Результат"); mpre("Тест пройден корректно", __LINE__);
					return false;
				} return true; }()){ mpre("ОШИБКА тест не пройден", __LINE__);
			}else if([&](){ index = erb(BMF_INDEX_EX, {{"id", "-1"}}); return index.empty(); }()){ mpre("ОШИБКА выборки элемента", __LINE__);
			}else if([&](){ index.at("calc_pos_id") = "4"; erb_insert(BMF_INDEX_EX, index.at("id"), index); return index.empty(); }()){ mpre("ОШИБКА переиндексирования", __LINE__);
			}else{ //mpre(index, __LINE__, "Морф"); ;
				return false;
			} return true; }()){ mpre("ОШИБКА Проверка тестов работы ключей", __LINE__);
		}else if([&](){ // Проверка работы функций конвертации
				string bin, _bin; float dec, _dec, rand;
				if([&]{ std::random_device rd; std::mt19937 g(rd()); rand = (float(rd()%2000000)-1000000)/1000; return (0 == rand); }()){ mpre("ОШИБКА получения случайного значения", __LINE__);
				}else if([&](){ bin = Dec2bin(dec = rand); return (dec != (_dec = Bin2dec(bin))); }()){ mpre("ОШИБКА пересчета "+ to_string(rand)+ " "+ to_string(dec)+ " >> "+ bin+ ">>"+ to_string(_dec) , __LINE__);
				}else{ //mpre("Тесты перевода оснований "+ to_string(rand)+ " пройдены корректно", __LINE__);
					return false;
				} return true;
			}()){ mpre("ОШИБКА проверки функиции перевода из двоичной в десятичную", __LINE__);
		}else{ //mpre("Все тесты пройдены успешно", __LINE__);
			return false;
		} return true; }()){ mpre("ОШИБКА не все тесты пройдены", __LINE__);
	}else if([&](){ // Основные функции
		if(false){ mpre("ОШИБКА уведомления", __LINE__);
		}else if([&](){ // Обучение
			Learn = ([&](TMs bmf_index, string val = "", int key, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX){ //mpre("Обучение "+ index.at("id"), __LINE__);  //system("sleep 1"); // mpre(BMF_INDEX, __LINE__, "Список"); // if(remove){ mpre(row, __LINE__, "Ученик"); }
				TMs stair;
				if(int rep = 1e9; 0 >= rep){ mpre("ОШИБКА установки предельного размера обучения для сети", __LINE__);
				}else if(static TMMi CACHE; false){ mpre("ОШИБКА создания кеша результата", __LINE__);
				}else if(TMMi STAIRS; false){ mpre("ОШИБКА задания списка ступеней", __LINE__);
				}else if([&](){ // Требование к морфу
					if(TMs stairs; false){ mpre("ОШИБКА создания первой ступени", __LINE__);
					}else if(stairs["index_id"] = bmf_index.at("id"); stairs.empty()){ mpre("ОШИБКА добавления номера идентификатора", __LINE__);
					}else if(stairs["bmf-calc_pos"] = stairs["calc_pos_id"] = bmf_index.at("calc_pos_id"); stairs.empty()){ mpre("ОШИБКА добавления номера идентификатора", __LINE__);
					}else if(0 >= val.length()){ //mpre("Нет требований к лестнице", __LINE__);
					}else if(stairs["promise"] = val; (stairs.end() == stairs.find("promise"))){ mpre("ОШИБКА установки требований к лестнице", __LINE__);
					}else if(stairs["learn"] = ""; false){ mpre("ОШИБКА установки признака обучения", __LINE__);
					}else if(STAIRS.insert(make_pair(STAIRS.size(), stairs)); STAIRS.empty()){ mpre("ОШИБКА устанвоки первоначального значения лестнице", __LINE__);
					}else{ //mpre("Установка требований к лестнице promise="+ val+ " learn=", __LINE__);
					} return bmf_index.empty(); }()){ mpre("ОШИБКА установки требований к морфу", __LINE__);
				}else if([&](){ do{ //mpre("Повтор "+ to_string(rep), __LINE__); // Ступени расчета
						if(STAIRS.empty()){ mpre("ОШИБКА лестница пуста", __LINE__);
						}else if(int size = STAIRS.size(); 0 >= size){ mpre("ОШИБКА расчета размера списка ступеней", __LINE__);
						}else if(auto stairs_itr = STAIRS.rbegin(); stairs_itr == STAIRS.rend()){ mpre("ОШИБКА получения итаратора следующей ступени", __LINE__);
						}else if(TMs stairs = stairs_itr->second; stairs.empty()){ mpre(STAIRS, __LINE__, "Лестница"); mpre("ОШИБКА получения ступени", __LINE__);
						}else if(stairs.end() == stairs.find("index_id")){ mpre("ОШИБКА идентификатор морфа не задан", __LINE__);
						}else if(TMs index = erb(BMF_INDEX_EX, {{"id", stairs.at("index_id")}}); index.empty()){ mpre(stairs, __LINE__, "Ступень"); mpre("ОШИБКА выборки морфа", __LINE__);
						}else if(TMs calc_pos = [&](TMs calc_pos = {}){ // Выборка позиции
							if(stairs.end() == stairs.find("bmf-calc_pos")){ mpre("ОШИБКА в ступени отсутствует ссылка на позицию морфа", __LINE__);
							}else if(BMF_CALC_POS.end() == BMF_CALC_POS.find(stoi(stairs.at("bmf-calc_pos")))){ mpre("ОШИБКА позиция не найдена", __LINE__);
							}else if(calc_pos = BMF_CALC_POS.at(stoi(stairs.at("bmf-calc_pos"))); calc_pos.empty()){ mpre("ОШИБКА выборки позиции морфа", __LINE__);
							}else{ //mpre(calc_pos, __LINE__, "Позиция морфа");
							} return calc_pos; }(); calc_pos.empty()){ mpre("ОШИБКА выборки позиции", __LINE__);
						}else if(TMs index_1 = [&](TMs index_1 = {}){ // Получение старшего потомка
							if(index.end() == index.find("index_id")){ mpre("ОШИБКА у ступени отстствует поле связи со старшим морфом", __LINE__);
							}else if("" == index.at("index_id")){ //mpre("Не указан старший нижестоящий потомок `index_id`", __LINE__);
							}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(index.at("index_id")))){ mpre("ОШИБКА указанный старший потомок не найден в списке морфов", __LINE__);
							}else if([&](){ std::lock_guard<std::recursive_mutex> lock(mu); index_1 = BMF_INDEX_EX.at("").at(stoi(index.at("index_id"))); return index_1.empty(); }()){ mpre("ОШИБКА выборки старшего потомка из списка морфов", __LINE__);
							}else{ //mpre("Старший потомок "+ index_1["id"], __LINE__);
							} return index_1; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
						}else if(TMs index_0 = [&](TMs index_0 = {}){ // Получение младшего потомка
							if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА у ступени отстствует поле связи со старшим морфом", __LINE__);
							}else if("" == index.at("bmf-index")){ //mpre("Не указан младший нижестоящий потомок `bmf-index`", __LINE__);
							}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(index.at("bmf-index")))){ mpre("ОШИБКА указанный младший потомок не найден в списке морфов", __LINE__);
							}else if([&](){ std::lock_guard<std::recursive_mutex> lock(mu); index_0 = BMF_INDEX_EX.at("").at(stoi(index.at("bmf-index"))); return index_0.empty(); }()){ mpre("ОШИБКА выборки младшего потомка из списка морфов", __LINE__);
							}else{ //mpre("Младший потомок "+ index_0["id"], __LINE__);
							} return index_0; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
						}else if([&](){ // Статус
							if(int status = [&](int status = 1){ // Инкремент статуса
								if(stairs.end() == stairs.find("status")){ //mpre("Статус не установлен", __LINE__);
								}else if(string::npos != stairs.at("status").find_last_not_of("123")){ mpre(STAIRS, __LINE__, "Лестница"); mpre(stairs, __LINE__, "Ступень"); mpre("ОШИБКА формат status="+ stairs["status"], __LINE__);
								}else if(status = stoi(stairs.at("status"))+1; (0 >= status)){ mpre("ОШИБКА не допустимый статус", __LINE__);
								}else{ //mpre(stairs["index_id"]+ " Статус установка STAIRS["+ to_string(size-1)+ "] calc_pos="+ calc_pos.at("name")+ " status="+ to_string(status), __LINE__); //mpre(index["id"]+ " Обновленный статус "+ to_string(status), __LINE__);
								} return status; }(); 0 >= status){ mpre("ОШИБКА установки статуса", __LINE__);
							}else if(stairs_itr->second["status"] = stairs["status"] = to_string(status); (0 >= stairs.at("status").length())){ mpre("ОШИБКА установки статуса", __LINE__);
							}else if(stairs.end() != stairs.find("calc_"+ status)){ mpre("ОШИБКА поле расчета уже задано у ступени calc_"+ status, __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Статус STAIRS["+ to_string(size-1)+ "] status="+ stairs["status"]+ (stairs.end() == stairs.find("promise") ? "" : " promise="+ stairs["promise"])+ (stairs.end() == stairs.find("learn") ? "" : " learn="+ stairs["learn"]), __LINE__);
							} return (string::npos != stairs.at("status").find_last_not_of("1234")); }()){ mpre("ОШИБКА установки состояния", __LINE__);
						}else if([&](){ // Результат
							if(stairs.end() == stairs.find("status")){ mpre("ОШИБКА статус не указан", __LINE__);
							//}else if(stairs.end() != stairs.find("val")){ mpre(stairs, __LINE__, "Расчет уже выполнен");
							}else if(string::npos != stairs.at("status").find_last_not_of("234")){ //mpre(stairs["index_id"]+ " Не расчитываем результат для статуса "+ stairs["status"], __LINE__);
							}else if(TMs _calc_pos = erb(BMF_CALC_POS, {{"id", stairs.at("bmf-calc_pos")}}); _calc_pos.empty()){ mpre("ОШИБКА получения позиции морфа", __LINE__);
							}else if([&](){ // Установка ключа кеша
								if(CACHE.end() != CACHE.find(stoi(index["id"]))){ //mpre("Кеш уже найден в таблице id="+ index["id"], __LINE__);
								}else if(CACHE.insert(std::make_pair(stoi(index["id"]), TMs({}))); CACHE.empty()){ mpre("ОШИБКА добавления Морфа", __LINE__);
								}else{ //mpre("Добавление морфа в таблицу кешей id="+ index["id"], __LINE__);
								} return false; }()){ mpre("ОШИБКА устанвоки ключа кеша", __LINE__);
							}else if([&](){ // Результат по одному значению
								if("2" != stairs.at("status")){ //mpre(stairs["index_id"]+ " По одному значению только для второго статуса", __LINE__);
								}else if(stairs.end() == stairs.find("calc_1")){ mpre(stairs, __LINE__, "Ступень"); mpre("ОШИБКА Для расчета нужен минимум calc_1", __LINE__);
								}else if(stairs.at("calc_1") == _calc_pos.at("val-1")){ //mpre(stairs["index_id"]+ " Результат расчетов calc_pos="+ _calc_pos["name"]+ " не достаточно calc_1="+ stairs["calc_1"], __LINE__);
								}else if(stairs_itr->second["val"] = stairs["val"] = _calc_pos.at("calc-0"); (1 != stairs.at("val").length())){ mpre("ОШИБКА результата по одному расчету", __LINE__);
								}else if(CACHE.at(stoi(index["id"])).insert(make_pair(to_string(key), stairs["val"])); CACHE.empty()){ mpre("Добавление значения в кеш", __LINE__);
								}else{ //mpre(stairs["index_id"]+ " Результат "+ _calc_pos["name"]+ " по одному значению calc_1="+ stairs.at("calc_1")+ " val="+ stairs["val"]+ (stairs.end() == stairs.find("promise") ? "" : " promise="+ stairs.at("promise")), __LINE__);
								} return false; }()){ mpre("ОШИБКА расчета по одному значению", __LINE__);
							}else if([&](){ // Расчет по нескольким значениям
								if(string::npos == stairs.at("status").find_last_of("34")){ //mpre(stairs["index_id"]+ " Не расчитываем по нескольким значениям статусы "+ stairs["status"], __LINE__);
								}else if(stairs.end() == stairs.find("calc_1")){ mpre("ОШИБКА Для расчета нужен минимум первый calc_1", __LINE__);
								}else if(stairs.end() == stairs.find("calc_2")){ mpre(stairs, __LINE__, "Ступень"); mpre("ОШИБКА Для расчета нужен минимум первый calc_2", __LINE__);
								}else if(stairs_itr->second["val"] = stairs["val"] = ((stairs.at("calc_1") == _calc_pos.at("val-1")) && (stairs.at("calc_2") == _calc_pos.at("val-1")) ? _calc_pos.at("calc-1") : _calc_pos.at("calc-0")); (1 != stairs.at("val").length())){ mpre("ОШИБКА расчета результата по нескольким значениям", __LINE__);
								}else if(CACHE.at(stoi(index["id"])).insert(make_pair(to_string(key), stairs["val"])); CACHE.empty()){ mpre("Добавление значения в кеш", __LINE__);
								}else{ //mpre(stairs["index_id"]+ " Результат "+ _calc_pos["name"]+ " по нескольким значениям calc_1="+ stairs.at("calc_1")+ " calc_2="+ stairs.at("calc_2")+ " val="+ stairs["val"]+ (stairs.end() == stairs.find("promise") ? "" : " promise="+ stairs.at("promise")), __LINE__);
								} return false; }()){ mpre("ОШИБКА расчета по нескольким значениям", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Результат val="+ (stairs.end() == stairs.find("val") ? "" : stairs.at("val")), __LINE__);
							} return false; }()){ mpre("ОШИБКА расчета результата", __LINE__);
						}else if([&](){ // Установка размера нижестоящих ветвей
							if(int dimension_1 = (index_1.empty() || string::npos == index_1.at("dimension").find_last_of("0123456789") ? 0 : stoi(index_1.at("dimension"))); dimension_1 < 0){ mpre("ОШИБКА расчета глубины другой ветви", __LINE__);
							}else if(int dimension_0 = (index_0.empty() || string::npos == index_0.at("dimension").find_last_of("0123456789") ? 0 : stoi(index_0.at("dimension"))); dimension_0 < 0){ mpre("ОШИБКА расчета глубины следующей ветки", __LINE__);
							}else if(int dimension = dimension_1 +dimension_0 +1; index.at("dimension") == to_string(dimension)){ //mpre("Не сохраняем значение так как не найдены изменения " +to_string(dimension_next) +" " +to_string(dimension_other), __LINE__);
							}else if(index.at("dimension") = to_string(dimension); 0 >= index.at("dimension").length()){ mpre("ОШИБКА установки размера нижестоящих ветвей", __LINE__);
							}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА сохранения изменений родителя в справочник", __LINE__);
							}else{ //mpre(index, __LINE__, "Морф"); mpre("ОШИБКА размер нижестоящих не соответствует действительности "+ index.at("id")+ "(" +to_string(dimension) +") " +to_string(dimension_1) +" " +to_string(dimension_0), __LINE__);
							} return false; }()){ mpre("ОШИБКА сохранения размера ветвей", __LINE__);
						}else if([&](){ // Прерывание обхода дерева
							if(stairs.end() == stairs.find("val")){ //mpre(index["id"]+ " Проверка совпадения результатат с требованием", __LINE__);
							}else if([&](){ // Проверка обучения
								if(stairs.end() == stairs.find("learn")){ //mpre(stairs["index_id"]+ " Прерывание Отсутствует признак обучения", __LINE__);
								}else if("" == stairs.at("learn")){ //mpre(stairs["index_id"]+ " Прерывание Определен статус обучения learn="+ stairs["learn"], __LINE__);
								}else if(stairs.at("status") == stairs.at("learn")){ mpre(stairs["id"]+ " Прерывание Не совпадает статус обучения status="+ stairs["status"]+ " learn="+ stairs["learn"], __LINE__);
								}else{ //mpre("Условия не прерывание по обучению не сработали", __LINE__);
									return true;
								} return false; }()){ //mpre(stairs["index_id"]+ " Продолжение по обучению", __LINE__);
							}else if([&](){ // Проверка требования
								if(stairs.end() == stairs.find("promise")){ //mpre(stairs["index_id"]+ " Прерывание Отсутствует требование", __LINE__);
								}else if(stairs.at("val") == stairs.at("promise")){ //mpre(stairs["index_id"]+ " Прерывание для совпавшего значения и обещаяния val="+ stairs["val"]+ " promise="+ stairs["promise"], __LINE__);
								}else{ //mpre("Условие на прерывание по обещанию не сработало", __LINE__);
									return true;
								} return false; }()){ //mpre(index["id"]+ " Продолжение по требованию promise="+ stairs["promise"]+ " val="+ stairs["val"], __LINE__);
							}else if(stairs_itr->second["status"] = stairs["status"] = "4"; stairs.empty()){ mpre("ОШИБКА обнуления ступени", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Прерывание обхода дерева val="+ stairs["val"]+ " status="+ stairs["status"], __LINE__);
							} return false; }()){ mpre("ОШИБКА прерывание обхода дерева при совпадении результата с требованиями", __LINE__);
						}else if(string index_field = ""; false){ mpre("ОШИБКА устанвоки имени поля для изменений", __LINE__);
						}else if(TMs index_other; false){ mpre("ОШИБКА установки смежного морфа", __LINE__);
						}else if(TMs index_next = [&](TMs index_next = {}){ // Расчет направления
							if(string::npos != stairs.at("status").find_last_not_of("123")){ //mpre(stairs["index_id"]+ " Не устанавливаем переход для статуса "+ stairs["status"], __LINE__);
							}else if(string orient = ("2" == stairs.at("status") ? calc_pos.at("calc-0") : calc_pos.at("calc-1")); (1 != orient.length())){ mpre("ОШИБКА выбора направления развития", __LINE__);
							}else if("1" == orient){ index_field = "index_id"; index_other = index_0; index_next = index_1; //mpre(stairs["index_id"]+ " Направление "+ calc_pos["name"]+ " старший переход index_id="+ (index_next.empty() ? "" : index_next["id"]), __LINE__);
							}else if("0" == orient){ index_field = "bmf-index"; index_other = index_1; index_next = index_0; //mpre(stairs["index_id"]+ " Направление "+ calc_pos["name"]+ " младший переход bmf-index="+ (index_next.empty() ? "" : index_next["id"]), __LINE__);
							}else{ mpre(stairs["index_id"]+ " ОШИБКА выбора направления перехода", __LINE__);
							} return index_next; }(); false){ mpre("ОШИБКА расчета направлениея", __LINE__);
						}else if([&](){ // Признак обучения
							if(string::npos == stairs.at("status").find_last_of("23")){ //mpre(stairs["index_id"]+ " Не устанавливаем переход для статуса "+ stairs["status"], __LINE__);
							}else if(stairs.end() == stairs.find("learn")){ //mpre(index["id"]+ " Обещание Обучение не установлено", __LINE__);
							}else if(stairs.end() == stairs.find("calc_1")){ mpre("ОШИБКА первый расчет не найден", __LINE__);
							}else if(stairs.end() == stairs.find("promise")){ mpre("ОШИБКА требование для обучения не указаны", __LINE__);
							}else if("" != stairs.at("learn")){ //mpre(index["id"]+ " Статус обучения установлен ранее status="+ stairs.at("status")+ " learn="+ stairs.at("learn"), __LINE__);
							}else if(stairs_itr->second["learn"] = stairs["learn"] = (stairs.at("calc_1") == calc_pos.at("val-1") ? stairs.at("status") : stairs.at("learn")); false){ mpre("ОШИБКА установки обучения для второго статуса", __LINE__);
							}else if(stairs_itr->second["learn"] = stairs["learn"] = ("3" == stairs.at("status") ? stairs.at("status") : stairs.at("learn")); false){ mpre("ОШИБКА установки обучения для третьего статуса", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Обещание выполнение learn="+ stairs.at("learn")+ " promise="+ stairs["promise"], __LINE__);
							} return false; }()){ mpre("ОШИБКА установки признака обучения", __LINE__);
						}else if([&](){ // Выборка значения из кеша
							if(index_next.empty()){ //mpre("Не найден следующий морф", __LINE__);
							}else if([&](){ // Пропуск признака обучения
								if(stairs.end() == stairs.find("learn")){ //mpre("Поле признака обучения не найдено", __LINE__);
								}else if(stairs.at("learn") != stairs.at("status")){ //mpre("Признак обучения не совпадает со статусом", __LINE__);
								}else{ return true;
								} return false; }()){ //mpre("Установлен признак обучения", __LINE__);
							}else if(string::npos != stairs.at("status").find_last_not_of("12")){ //mpre(stairs["index_id"]+ " Не устанавливаем переход для статуса "+ stairs["status"], __LINE__);
							}else if(CACHE.end() == CACHE.find(stoi(index_next["id"]))){ //mpre("Ключ в кеше не найден", __LINE__);
							}else if(string calc = "calc_"+ stairs.at("status"); (0 >= calc.length())){ mpre("ОШИБКА расчета ключа значения", __LINE__);
							}else if(stairs.end() != stairs.find(calc)){ mpre("ОШИБКА значение уже установлено в ступени "+ calc, __LINE__);
							}else if(CACHE.at(stoi(index_next["id"])).end() == CACHE.at(stoi(index_next["id"])).find(to_string(key))){ //mpre("Морф ключа в кеше не найден", __LINE__);
							}else if(string val = CACHE.at(stoi(index_next["id"])).at(to_string(key));  (1 != val.length())){ mpre("ОШИБКА расчета значения", __LINE__);

							}else if(stairs_itr->second[calc] = stairs[calc] = val; (1 != stairs[calc].length())){ mpre("ОШИБКА получения значения морфа", __LINE__);
							}else{ //mpre("Значение найдено key=" +to_string(key) +" status="+ stairs.at("status") +" index_id="+ index_next["id"]+ " val="+ val, __LINE__); mpre(stairs, __LINE__, "Ступень"); mpre(index, __LINE__, "Морф"); //mpre(CACHE.at(key), __LINE__, "Список значений key="+ to_string(key));
							} return false; }()){ mpre("ОШИБКА выборки значений ступени из кеша", __LINE__);
						}else if(TMs stairs_next = [&](TMs stairs_next = {}){ // Следующая ступень
							if(index_next.empty()){ //mpre("Не найден следующий морф", __LINE__);
							//}else if(string calc = "calc_"+ stairs.at("status"); (0 >= calc.length())){ mpre("ОШИБКА расчета ключа значения", __LINE__);
							//}else if(.end() != index_next.find(calc)){ mpre("Значение следующего уже расчитано "+ index.at("id") +" > "+ index_next.at("id") +" не добавляем следующую ступень", __LINE__);
							}else if(stairs_next["index_id"] = index_next.at("id"); stairs_next.empty()){ mpre("ОШИБКА формирования следующей ступени", __LINE__);
							}else if(stairs_next["bmf-calc_pos"] = stairs_next["calc_pos_id"] = index_next.at("calc_pos_id"); stairs_next.empty()){ mpre("ОШИБКА формирования следующей ступени", __LINE__);
							}else{ //mpre(stairs_next, __LINE__, "Следующая ступень");
							} return stairs_next; }(); false){ mpre("ОШИБКА формирования следующей ступени", __LINE__);
						/*}else if([&](){ // Балансировка дерева
							if(false){ mpre("Пропуск балансировки", __LINE__);
							}else if(stairs.end() == stairs.find("learn")){ //mpre(index["id"]+ " Значение не расчитано", __LINE__);
							}else if(stairs.at("learn") != "3"){ //mpre("Только на стадии обучения", __LINE__);
							}else if(stairs.end() == stairs.find("calc_2")){ mpre("ОШИБКА Балансировка только при наличии двух результатов", __LINE__);
							}else if(stairs.at("calc_pos_id") != stairs.at("bmf-calc_pos")){ //mpre("Позиция уже изменена", __LINE__);
							}else if(stairs.at("calc_1") != stairs.at("calc_2")){ //mpre("Балансировка только при совпадении результата", __LINE__);
							}else if(index_next.empty() || index_other.empty()){ //mpre("Пустые потомки", __LINE__);
							}else if(index_next.end() == index_next.find("dimension")){ mpre("ОШИБКА размер ветви не указан", __LINE__);
							}else if(index_other.end() == index_other.find("dimension")){ mpre("ОШИБКА размер ветви не указан", __LINE__);
							}else if(int dimension_next = (string::npos == index_next.at("dimension").find_last_of("0123456789") ? 0 : stoi(index_next.at("dimension"))); dimension_next <= 0){ mpre("ОШИБКА расчета размера слещующей ветви", __LINE__);
							}else if(int dimension_other = (string::npos == index_other.at("dimension").find_last_of("0123456789") ? 0 : stoi(index_other.at("dimension"))); dimension_other <= 0){ mpre("ОШИБКА расчета размера другой ветви", __LINE__);
							}else if(dimension_next <= dimension_other){ //mpre("Баланс соблюден", __LINE__);
							}else if(TMs calc_pos = erb(BMF_CALC_POS, {{"bmf-calc_pos", index.at("calc_pos_id")}}); calc_pos.empty()){ mpre("ОШИБКА выборки отрицательной текущей позиции", __LINE__);
							}else if(index.at("calc_pos_id") = calc_pos.at("id"); index.empty()){ mpre("ОШИБКА присвоения обновленной позиции морфу", __LINE__);
							}else if(stairs.at("calc_pos_id") = stairs.at("bmf-calc_pos") = index.at("calc_pos_id"); stairs.empty()){ mpre("ОШИБКА установки свойств ступени", __LINE__);
							}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА сохранения обновленной позиции в справочнике", __LINE__);
							}else if(TMs calc_pos_next = erb(BMF_CALC_POS, {{"calc_pos_id", index_next.at("calc_pos_id")}}); calc_pos_next.empty()){ mpre("ОШИБКА выборки отрицательной текущей позиции", __LINE__);
							}else if(index_next.at("calc_pos_id") = calc_pos_next.at("id"); index_next.empty()){ mpre("ОШИБКА присвоения обновленной позиции морфу", __LINE__);
							}else if(erb_insert(BMF_INDEX_EX, index_next.at("id"), index_next); index_next.empty()){ mpre("ОШИБКА сохранения обновленной позиции в справочнике", __LINE__);
							}else if(TMs calc_pos_other = erb(BMF_CALC_POS, {{"calc_pos_id", index_other.at("calc_pos_id")}}); calc_pos_other.empty()){ mpre("ОШИБКА выборки отрицательной текущей позиции", __LINE__);
							}else if(index_other.at("calc_pos_id") = calc_pos_other.at("id"); index_other.empty()){ mpre("ОШИБКА присвоения обновленной позиции морфу", __LINE__);
							}else if(erb_insert(BMF_INDEX_EX, index_other.at("id"), index_other); index_other.empty()){ mpre("ОШИБКА сохранения обновленной позиции в справочнике", __LINE__);
							}else if(std::swap(index_next, index_other); false){ mpre("ОШИБКА изменнеия мест переменных", __LINE__);
							}else if(index_field = ("index_id" == index_field ? "bmf-index" : "index_id"); 0 >= index_field.length()){ mpre("ОШИБКА изменения поля перехода", __LINE__);
							}else{ mpre("Балансировка дерева " +to_string(dimension_next) +" " +to_string(dimension_other) +" [" +stairs.at("calc_1") + "," +stairs.at("calc_2") +"]", __LINE__);
							} return false; }()){ mpre("ОШИБКА балансировки дерева", __LINE__);*/
						}else if(TMs _index = [&](TMs _index = {}){ // Изменения
							if(stairs.end() == stairs.find("promise")){ //mpre(stairs["index_id"]+ " Обещание не установлено", __LINE__);
							}else if(stairs.end() == stairs.find("learn")){ mpre("Связь не обучаем ступень без признака обучения", __LINE__);
							}else if("" == stairs.at("learn")){ //mpre(index["id"]+ " Изменение Статус обучения не задан learn=", __LINE__);
							}else if(stairs.at("status") != stairs.at("learn")){ //mpre(stairs["index_id"]+ " Связь обучение установлено для другого статуса status="+ stairs.at("status")+ " learn="+ stairs.at("learn"), __LINE__);
							}else if(string calc = "calc_"+ stairs.at("status"); (0 >= calc.length())){ mpre("ОШИБКА расчета ключа значения", __LINE__);
							}else if(stairs.end() != stairs.find(calc)){ //mpre("Значение уже расчитано "+ index_next.at("id"), __LINE__);
							}else if([&](){ // Обнуление кеша
								if(CACHE.end() == CACHE.find(stoi(index["id"]))){ //mpre("Кеш не найден", __LINE__);
								}else if(CACHE.at(stoi(index["id"])) = TMs({}); !CACHE.at(stoi(index["id"])).empty()){ mpre("ОШИБКА обнуления кеша предыщузих расчетов", __LINE__);
								}else{ //mpre("Удаление кеша предыдущих расчетов "+ index["id"], __LINE__);
								} return false; }()){ mpre("ОШИБКА обнуления кеша", __LINE__);
							}else if([&](){ // Изменение позиции
								if("3" != stairs.at("status")){ //mpre(stairs["index_id"]+ " Не устанавливаем переход для статуса "+ stairs["status"], __LINE__);
								}else if(stairs.at("calc_2") != calc_pos.at("val-0")){ //mpre(index["id"]+ " Позиция "+ calc_pos["name"]+ " Изменяем только обратную сверхпозицию calc-1="+ calc_pos.at("calc-1")+ " calc_2="+ stairs["calc_2"], __LINE__);
								}else if([&](){ // Изменение позиции
									if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
									}else if(TMs _calc_pos = erb(BMF_CALC_POS, {{"id", calc_pos.at("calc_pos_id")}}); _calc_pos.empty()){ mpre("ОШИБКА получения обновленной позиции", __LINE__);
									}else if(index["calc_pos_id"] = stairs["bmf-calc_pos"] = stairs_itr->second["bmf-calc_pos"] = _calc_pos.at("id"); index.empty()){ mpre("ОШИБКА установки обновленной позции ступени", __LINE__);
									}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА сохранения обновленной позиции в справочнике", __LINE__);
									}else if(stairs_itr->second["revision"] = stairs["revision"] = (stairs.end() == stairs.find("revision") ? "~" : stairs.at("revision")); false){ mpre("ОШИБКА установки значения", __LINE__);
									}else{ //mpre(stairs["index_id"]+ " Позиция "+ calc_pos["name"]+ " обновление calc_1="+ stairs.at("calc_1")+ " calc_2="+ stairs.at("calc_2")+ " "+ calc_pos["name"]+ ">"+ _calc_pos["name"]+ " promise="+ stairs.at("promise")+ " val="+ stairs.at("val"), __LINE__);
									} return false; }()){ mpre("ОШИБКА изменения позиции", __LINE__);
								}else{
								} return false; }()){ mpre("ОШИБКА обновления позиции", __LINE__);
							}else if([&](){ // Создание нового морфа
								if(!stairs_next.empty()){ //mpre("Только для пустых", __LINE__);
								}else if(0 >= index_field.length()){ mpre("ОШИБКА не задано поле для нового морфа", __LINE__);
								}else if(TMs dano = erb(_BMF_DANO_EX, {{"id", index.at("dano_id")}}); dano.empty()){ mpre("ОШИБКА получения исходника морфа", __LINE__);
								}else if(string promise = (calc_pos.at("calc-0") == stairs.at("promise") ? calc_pos.at("val-0") : calc_pos.at("val-1")); (1 != promise.length())){ mpre("ОШИБКА расчета необходимого значения", __LINE__);
								}else if(dano.at("val") == promise){ //mpre(index["id"]+ " Исходник соответствует обещаниям dano="+ dano.at("val")+ " promise="+ promise, __LINE__);
								}else if(TMs _dano = (index_other.empty() ? Choice(index, promise, _BMF_DANO_EX, _BMF_ITOG_EX) : _dano); false){ mpre("ОШИБКА выборки из подходящих сигналов", __LINE__);
								}else if([&](){ // Выбор подходящего исходного сигнала
									if(!_dano.empty()){ //mpre("Выбор сделан из подходящих исходных сигналов", __LINE__);
									}else if(_dano = erb(_BMF_DANO_EX, {{"id", index_other.at("dano_id")}}); _dano.empty()){ mpre("ОШИБКА выборки исхдного значения по смежному морфу", __LINE__);
									}else{ //mpre("Выбор исходного значения сделан по смежному морфу", __LINE__);
									} return _dano.empty(); }()){ mpre("ОШИБКА выборки подходящего исходного сигнала", __LINE__);
								}else if([&](){ // Создание морфа
									if(index.at("dano_id") == _dano.at("id")){ mpre("ОШИБКА При выборе источника найден уже установленный", __LINE__);
									}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
									}else if(TMs _calc_pos = erb(BMF_CALC_POS, {{"itog", promise}, {"dano", _dano.at("val")}}); _calc_pos.empty()){ mpre(_dano, __LINE__, "Новый знак"); mpre("ОШИБКА получения позиции нового морфа", __LINE__);
									}else if(index_next = _index = {{"id", Id(BMF_INDEX_EX.at(""))}, {"clump_id", index.at("clump_id")}, {"itog_values_id", index.at("itog_values_id")}, {"depth", to_string(stoi(index.at("depth"))+1)}, {"dimension", "1"}, {"dano_id", _dano.at("id")}, {"itog_id", index.at("itog_id")}, {"calc_pos_id", _calc_pos.at("id")}, {"index_id", ""}, {"bmf-index", ""}}; _index.empty()){ mpre("ОШИБКА создания нового морфа "+ index.at("id"), __LINE__);
									}else if(erb_insert(BMF_INDEX_EX, _index.at("id"), _index); _index.empty()){ mpre("ОШИБКА добавления морфа в справочник", __LINE__);
									}else if(index.at(index_field) = _index.at("id"); index.empty()){ mpre("ОШИБКА установки нового морфа к вышестоящему", __LINE__);
									}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА сохранения изменений родителя в справочник", __LINE__);
									}else if(stairs_itr->second["val"] = stairs["val"] = stairs.at("promise"); false){ mpre("ОШИБКА установки значения", __LINE__);
									}else if(stairs_itr->second["revision"] = stairs["revision"] = "+"; false){ mpre("ОШИБКА установки значения", __LINE__);
									}else if(string calc_field = ("2" == stairs.at("status") ? "calc_2" : "calc_1"); (0 >= calc_field.length())){ mpre("ОШИБКА формирования расчетоного поля родителя", __LINE__);
									}else if(stairs_itr->second[calc_field] = stairs[calc_field] = promise; stairs.empty()){ mpre("ОШИБКА установки значения", __LINE__);
									}else{ //mpre(index["id"]+ " Связь добавление нового морфа stairs["+ stairs["index_id"]+ "] "+ index_field+ "="+ promise+ " val="+ stairs.at("val")+ " promise="+ stairs.at("promise"), __LINE__);
									} return _index.empty(); }()){ mpre("ОШИБКА создания морфа", __LINE__);
								}else{
								} return false; }()){ mpre("ОШИБКА создания нового морфа", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Связь обучение learn="+ stairs.at("learn"), __LINE__);
							} return _index; }(); false){ mpre("ОШИБКА изменений", __LINE__);
						}else if([&](){ // Установка значения из исходника
							if(!stairs_next.empty()){ //mpre("Направление не пусто", __LINE__);
							}else if(!_index.empty()){ //mpre("Уже создан нижестоящий морф", __LINE__);
							}else if(string::npos == stairs.at("status").find_last_of("12")){ //mpre(stairs["index_id"]+ " Не устанавливаем исходник для статуса "+ stairs["status"], __LINE__);
							}else if(TMs dano; false){ mpre("ОШИБКА установки значения дано", __LINE__);
							}else if([&](){ // Выборка исходника
								if(index.end() == index.find("dano_id")){ mpre("ОШИБКА поле со ссылкой на исходник не найдено", __LINE__);
								}else if(dano = erb(_BMF_DANO_EX, {{"id", index.at("dano_id")}}); dano.empty()){ mpre("ОШИБКА выборки исходника из списка", __LINE__);
								}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА поле значения у исходника не указано", __LINE__);
								}else{ //mpre(stairs["index_id"]+ " Значение исходника val="+ dano["val"], __LINE__);
								} return dano.empty(); }()){ mpre("ОШИБКА выборки исходника", __LINE__);
							}else if(string calc = "calc_"+ stairs.at("status"); (0 >= calc.length())){ mpre("ОШИБКА расчета поля результата", __LINE__);
							}else if(stairs.end() != stairs.find(calc)){ mpre("ОШИБКА значение уже установлено "+ calc, __LINE__);
							}else if(stairs_itr->second[calc] = stairs[calc] = dano.at("val"); (1 != dano.at("val").length())){ mpre(dano, __LINE__, "Дано"); mpre("ОШИБКА установки результата исходника", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Исходник установка значения "+ calc+ "="+ dano["val"], __LINE__);
							} return false; }()){ mpre("ОШИБКА получения расчета направления", __LINE__);
						}else if([&](){ // Добавление новой ступени
							if(stairs_next.empty()){ //mpre("Пустое значение расчета", __LINE__);
							}else if(string calc = "calc_"+ stairs.at("status"); (0 >= calc.length())){ mpre("ОШИБКА расчета ключа значения", __LINE__);
							}else if(stairs.end() != stairs.find(calc)){ //mpre("Значение уже расчитано "+ index_next.at("id"), __LINE__);
							}else if(string::npos == stairs.at("status").find_last_of("123")){ //mpre(stairs["index_id"]+ " Не устанавливаем исходник для статуса "+ stairs["status"], __LINE__);
							}else if([&](){ // Признак обучения потомку
								if(stairs.end() == stairs.find("learn")){ //mpre("Обучение не задано", __LINE__);
								}else if("" == stairs.at("learn")){ //mpre(index["id"]+ " Статус обучения не установлен", __LINE__);
								}else if(stairs.at("status") != stairs.at("learn")){ //mpre(index["id"]+ " Статус обучения не совпадает", __LINE__);
								}else if(stairs_next["learn"] = ""; stairs_next.empty()){ mpre("ОШИБКА установки признака обучения", __LINE__);
								}else if(stairs.end() == stairs.find("promise")){ mpre("ОШИБКА требования к потомку не указаны", __LINE__);
								}else if(stairs_next["promise"] = (stairs.at("promise") == calc_pos.at("calc-1") ? calc_pos.at("val-1") : calc_pos.at("val-0")); (1 != stairs.at("promise").length())){ mpre("ОШИБКА расчета требования к потомку", __LINE__);
								}else{ //mpre(index["id"]+ " Установка требований к потомку "+ stairs_next["index_id"]+ " promise="+ stairs_next["promise"]+ " learn="+ stairs_next["learn"], __LINE__);
								} return false; }()){ mpre("ОШИБКА передачи признака обучения потомку", __LINE__);
							}else if([&](){ STAIRS.insert(make_pair(size, stairs_next)); return STAIRS.empty(); }()){ mpre("ОШИБКА добавления новой ступени", __LINE__);
							//}else if([&](){ mpre(stairs, __LINE__, "Ступень"); mpre(stairs_next, __LINE__, "Следующая"); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Ступень status="+ stairs["status"]+ " увеличение лестницы id="+ stairs_next["index_id"], __LINE__); //mpre("Добавление новой ступени лестницы", __LINE__);
							} return false; }()){ mpre("ОШИБКА Добавления морфа в лестницу", __LINE__);
						}else if([&](){ // Удаление
							if(auto stairs_parent_itr = stairs_itr; false){ mpre("ОШИБКА получения итератора очреди", __LINE__);
							}else if(TMs _stairs = STAIRS.rbegin()->second; _stairs.empty()){ mpre("ОШИБКА последней ступени не найдено", __LINE__);
							}else if(_stairs.end() == _stairs.find("status")){ //mpre(stairs["index_id"]+ " Статус последней ступени не установлен", __LINE__);
							}else if(string::npos == _stairs.at("status").find_last_not_of("12")){ //mpre("ОШИБКА формат статусу "+ _stairs.at("status"), __LINE__);
							}else if(stair = (1 == size ? STAIRS.begin()->second : stair); false){ mpre("ОШИБКА получения последней ступени", __LINE__);
							}else if(_stairs.end() == _stairs.find("val")){ mpre("ОШИБКА значение ступени не установлено "+ stairs["index_id"], __LINE__);
							}else if(TMs stairs_parent = (++stairs_parent_itr)->second; stairs_parent.empty()){ mpre("ОШИБКА выборки ступеньки родителя", __LINE__);
							}else if([&](){ // Изменения родителю
								if(stairs.end() == stairs.find("revision")){ //mpre("Изменения не установлены", __LINE__);
								}else if(stairs_parent_itr->second["revision"] = stairs_parent["revision"] = stairs.at("revision"); (1 != stairs_parent.at("revision").length())){ mpre("ОШИБКА переноса изменений", __LINE__);
								}else{ //mpre(index["id"]+ " Перенос изменений revision="+ stairs_parent["revision"], __LINE__);
								} return false; }()){ mpre("ОШИБКА переноса изменений родителю", __LINE__);
							}else if(stairs_parent.end() == stairs_parent.find("status")){ mpre("ОШИБКА статус у родителя не найден", __LINE__);
							}else if(string field = "calc_"+ stairs_parent.at("status"); (0 >= field.length())){ mpre("ОШИБКА составления расчетного поля родительской ступени", __LINE__);
							}else if(stairs_parent.end() != stairs_parent.find(field)){ mpre(stairs_parent, __LINE__, "Родительская ступень"); mpre("ОШИБКА расчетное поле уже присутствует у родителя "+ field, __LINE__);
							}else if(stairs_parent_itr->second[field] = stairs_parent[field] = _stairs.at("val"); (1 != _stairs.at("val").length())){ mpre("ОШИБКА установки расчета родителю", __LINE__);
							}else if(field = ("2" == stairs_parent.at("status") ? "calc_2" : "calc_1"); (0 >= field.length())){ mpre("ОШИБКА составления расчетного поля родительской ступени", __LINE__);
							}else if(stairs_parent_itr->second[field] = stairs_parent[field] = _stairs.at("val"); (1 != _stairs.at("val").length())){ mpre("ОШИБКА установки расчета родителю", __LINE__);
							}else if([&](){ // Передача значения вышестоящей ступени
								if(_stairs.end() == _stairs.find("promise")){ //mpre("Не установлены требования", __LINE__);
								}else if(_stairs.at("val") == _stairs.at("promise")){ //mpre("Требования совпадают", __LINE__);
								//}else if(TMs _index = erb(BMF_INDEX_EX, {{"id", _stairs.at("index_id")}}); _index.empty()){ mpre("ОШИБКА выборки морфа", __LINE__);
								}else{ mpre(_stairs, __LINE__, "Ступень"); mpre(index, __LINE__, "Морф");
									mpre("ОШИБКА расчета ступени", __LINE__);
								} return (1 != _stairs.at("val").length()); }()){ mpre("ОШИБКА установки расчета родителю", __LINE__);
							}else if(STAIRS.erase(STAIRS.size()-1); false){ mpre("Окончание пустая лестница", __LINE__);
							}else{ //mpre(stairs["index_id"]+ " Окончание Передача значения родителю "+ stairs_parent["index_id"]+ " "+ field+ "="+ _stairs["val"]+ (stairs_parent.end() == stairs_parent.find("revision") ? "" : " revision="+ stairs_parent["revision"]), __LINE__);
							} return false; }()){ mpre("ОШИБКА удаления ступени", __LINE__);
						}else{ //mpre(index, __LINE__, "Ступень"); //mpre("Повтор "+ to_string(rep), __LINE__);
						}
					}while((0 < --rep) && (0 < STAIRS.size())); return false; }()){ mpre("ОШИБКА получения результатов расчета", __LINE__);
				}else if([&](){ // Расчет результата
					if(stair.end() == stair.find("val")){ mpre(stair, __LINE__, "Ступень лестницы"); mpre("ОШИБКА получения результата лестницы val", __LINE__);
					}else if(stair["result"] = stair.at("val"); (1 != stair.at("result").length())){ mpre("ОШИБКА формирования результата расчета", __LINE__);
					}else if(stair.at("promise") != stair.at("result")){ mpre(stair, __LINE__, "Ступень"); mpre("ОШИБКА расчета ступени", __LINE__);
					}else if(stair.end() == stair.find("revision")){ //mpre("Изменения результата не указаны", __LINE__);
					}else if(stair.at("result") = stair.at("revision")+ stair.at("result"); (0 >= stair.at("result").length())){ mpre("ОШИБКА формирования результата с изменениями", __LINE__);
					}else{ //mpre(stair, __LINE__, "Результат с изменениями _val="+ stair.at("result"));
					} return (0 >= stair.at("result").length()); }()){ mpre("ОШИБКА расчета значения морфа "+ to_string(rep)+ " "+ bmf_index["id"], __LINE__);
				}else if(stair.end() == stair.find("result")){ mpre("ОШИБКА результат расчетов не найден", __LINE__);
				}else{ //mpre(stair, __LINE__, "Результаты вычислений"); //mpre("Количество итераций "+ to_string(rep), __LINE__);
				} return stair.at("result");
			}); return false; }()){ mpre("ОШИБКА создания функции обучения", __LINE__);
		}else if([&](){ // Обучение итога
			Learning = ([&](TMs itog, int key, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX){ //mpre("Обновление", __LINE__); // Обучение
				int change = 0;
				if(itog.empty()){ mpre("ОШИБКА итог не задан", __LINE__);
				}else if(_BMF_ITOG_EX.at("").end() == _BMF_ITOG_EX.at("").find(stoi(itog.at("id")))){ mpre(itog, __LINE__, "Итог"); mpre("ОШИБКА итог в локальном списке не найден", __LINE__);
				}else if(itog.end() == itog.find("itog_values_id")){ mpre(itog, __LINE__, "Итог"); mpre("ОШИБКА формата итога", __LINE__);
				}else if(TMs index = [&](TMs index = {}){ // Проверка на разные ссылки сетей у итогов
					if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
					}else if(BMF_ITOG_EX.at("").end() == BMF_ITOG_EX.at("").find(stoi(itog.at("id")))){ mpre("ОШИБКА локального морф не найден в глобальном списке", __LINE__);
					}else if(TMs bmf_itog = BMF_ITOG_EX.at("").at(stoi(itog.at("id"))); bmf_itog.empty()){ mpre("ОШИБКА выборки глобального морфа", __LINE__);
					}else if(index = erb(BMF_INDEX_EX ,{{"id", bmf_itog["index_id"]}}); !index.empty()){ //mpre("Связь с морфом", __LINE__);
					}else if(TMs dano = _BMF_DANO_EX.at("").begin()->second; dano.empty()){ mpre("ОШИБКА выборки первого дано", __LINE__);
					}else if("" == itog.at("val")){ mpre(_BMF_ITOG_EX.at(""), __LINE__, "Список локальных итогов"); mpre(BMF_ITOG_EX.at(""), __LINE__, "Список глобальных итогов"); mpre("ОШИБКА у итога и установлено значение", __LINE__);
					}else if(TMs calc_val = erb(BMF_CALC_VAL, {{"v1", dano.at("val")}, {"v0", dano.at("val")}}); calc_val.empty()){ mpre("ОШИБКА выборки значения", __LINE__);
					}else if(TMs calc_balance = erb(BMF_CALC_BALANCE, {{"calc_val_id", calc_val.at("id")}, {"val", itog.at("val")}}); calc_balance.empty()){ mpre(calc_val, __LINE__, "ОШИБКА выборки подходящего значения "+ itog["val"]);
					}else if(index = {{"id", Id(BMF_INDEX_EX.at(""))}, {"clump_id", clump_id}, {"itog_values_id", itog.at("itog_values_id")}, {"depth","0"}, {"dimension", "1"}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}, {"calc_pos_id", calc_balance.at("calc_pos_id")}, {"index_id", ""}, {"bmf-index", ""}}; index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
					}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА добавления морфа в справочник", __LINE__);
					}else if(itog["index_id"] = index.at("id"); itog.empty()){ mpre("ОШИБКА установки свойства связи итога с морфом", __LINE__);
					}else if(erb_insert(BMF_ITOG_EX, itog.at("id"), itog); itog.empty()){ mpre("ОШИБКА индексирования итога", __LINE__);
					}else{ //mpre("ОШИБКА добавления связи с морфом", __LINE__);
					} return index; }(); index.empty()){ mpre("ОШИБКА морф связи не найден", __LINE__);
				}else if(TMs values = [&](TMs values = {}){ // Значение
					if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
					}else if("" == index.at("itog_values_id")){ mpre("ОШИБКА значение у итога не указано", __LINE__);
					}else if(BMF_ITOG_VALUES_EX.at("").end() == BMF_ITOG_VALUES_EX.at("").find(stoi(index["itog_values_id"]))){ mpre("ОШИБКА значение не найдено", __LINE__);
					}else if(values = BMF_ITOG_VALUES_EX.at("").at(stoi(index.at("itog_values_id"))); values.empty()){ mpre("ОШИБКА выборки значения итога", __LINE__);
					}else{ //mpre(values, __LINE__, "Значение итога");
					} return values; }(); values.empty()){ mpre("ОШИБКА получения значения", __LINE__);
				}else if([&](){ // Обучение если не режим расчета
					if(string _val = Learn(index, itog.at("val"), key, _BMF_DANO_EX, _BMF_ITOG_EX); (1 <= _val.length() >= 2)){ mpre("ОШИБКА обучения морфа значение не верная длинна результата", __LINE__);
					}else if(mpre("Расчет "+ itog.at("id")+ " "+ values.at("name")+ " ("+ itog.at("name")+ ") index["+ index.at("id")+ "]="+ itog.at("val")+ " "+ _val , __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else if(0 >= _val.length()){ mpre("ОШИБКА формат расчета не верный "+ _val, __LINE__);
					}else if(string val = _val.substr(_val.length()-1, 1); (1 != val.length())){ mpre("ОШИБКА получения результатов расчета", __LINE__);
					}else if(val != itog.at("val")){ mpre("ОШИБКА обещанный результат не совпал с расчетным "+ itog.at("val")+ " val=" +val +" _val" +_val, __LINE__);
					}else if(change += (2 <= _val.length() ? 1 : 0); false){ mpre("ОШИБКА установки изменения", __LINE__);
					}else{ //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); mpre(BMF_DANO_EX.at(""), __LINE__, "Справочник"); mpre("ОШИБКА тест "+ itog["val"]+ " "+ _val+ " count="+ to_string(BMF_INDEX_EX.at("").size()), __LINE__);
					} return false; }()){ mpre("ОШИБКА обучения морфа", __LINE__);
				}else{ //mpre("Окончание обучения морфа "+ itog["name"], __LINE__); //system("sleep 0.5");
				} return change;
			}); return false; }()){ mpre("ОШИБКА устанвоки функции расчета итога", __LINE__);
		}else if([&](){ // ОБучение
			LearningAll = ([&](json js, int thread, int _thread, string clump_id, int key, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX){ //mpre("ОбновлениеОбщее", __LINE__); // Обучение
				int change = 0;
				if(false){ mpre("Остановка выполнения LearningAll", __LINE__);
				}else if([&](){ //for(auto itog_itr = BMF_ITOG.begin(); itog_itr != BMF_ITOG.end(); itog_itr++){// mpre("Итог", __LINE__);
					for(auto& itog_itr:_BMF_ITOG_EX.at("")){
						if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА получения итогового знака из итератора", __LINE__);
						}else if([&](){ // Пропуск потоков
							if(ARGV.end() == ARGV.find("-t")){ // Беспотоковый режим
							}else if(abs(stoi(itog.at("id")))%thread == (_thread-1)){ //mpre("Расчет потока "+ to_string(thread)+ "%"+ to_string(_thread)+ " "+ itog.at("id"), __LINE__);
							}else{ //mpre("Пропускаем расчет потока "+ to_string(thread)+ "%"+ to_string(_thread)+ " "+ itog.at("id"), __LINE__);
								return true;
							} return false; }()){ //mpre("Пропускаем расчет потока "+ to_string(thread)+ "%"+ to_string(_thread)+ " "+ itog.at("id"), __LINE__); //mpre("Пропускаем поток", __LINE__);
						}else if([&](){ // Пропускаем расчет
							if(ARGV.end() == ARGV.find("-i")){ //mpre("Нулевой параметр итога", __LINE__);
							}else if(ARGV.at("-i") == itog.at("id")){ //mpre("Указанный итог равен текущему "+ itog["id"], __LINE__);
							}else{ return true;
							} return false; }()){ //mpre("Пропускаем ["+ itog["id"]+ "] != "+ to_string(itog_id), __LINE__);
						}else if(change += Learning(itog, key, _BMF_DANO_EX, _BMF_ITOG_EX); false){ mpre("ОШИБКА запуска расчета", __LINE__);
						}else{ //mpre("Перебор связанных морфов окончен " +to_string(_thread), __LINE__);
						}
					}; return false; }()){ mpre("ОШИБКА перебора знаков для обучения", __LINE__);
				}else{// mpre("Обучение", __LINE__);
				} return change;
			}); return false; }()){ mpre("ОШИБКА установки функции обучения", __LINE__);
		}else if([&](){ // Непосредственно расчет
			Do = ([&](int thread, int _thread){ // Цикл повторений расчета
				int change;
				if(int loop_max = [&](int loop_max = 0){ // Проверка количества эпох из командной строки
					if(0 != loop_max){ mpre("ОШИБКА для проверки необходимо скинуть значения эпох", __LINE__);
					}else if(string _loop_max = (ARGV.end() == ARGV.find("-e") ? "1" : ARGV.at("-e")); (0 >= _loop_max.length())){ mpre("ОШИБКА количество эпох не задано", __LINE__);
					}else if(string::npos != _loop_max.find_last_not_of("0123456789")){ mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop_max, __LINE__);
					}else if(loop_max = stoi(_loop_max); (0 >= loop_max)){ mpre("ОШИБКА количество эпох не достаточно для продолжения", __LINE__);
					}else{ //mpre("Количество эпох установлено "+ to_string(loop_max), __LINE__);
					} return loop_max; }(); (0 > loop_max)){ mpre("ОШИБКА количество эпох задано не верно -e", __LINE__);
				}else if(TM3i _BMF_DANO_EX = BMF_DANO_EX; _BMF_DANO_EX.empty()){ mpre("ОШИБКА установки первоначальных исходников дано", __LINE__);
				}else if(TM3i _BMF_ITOG_EX = BMF_ITOG_EX; _BMF_ITOG_EX.empty()){ mpre("ОШИБКА установки первоначальных итогов дано", __LINE__);
				}else if(auto _in = in; _in.empty()){ mpre("ОШИБКА установки временного массива", __LINE__);
				}else if([&](){ // Обучение до состояния без ошибок
						do{ // Расчет и обучение входящих параметров
							if(loop += 1; false){ mpre("ОШИБКА увеличения итераций", __LINE__);
							}else if(change = 0; false){ mpre("ОШИБКА скидывания флага изменений", __LINE__);
							}else if(int pips_sum = 0; false){ mpre("ОШИБКА обнуления суммы пипсов", __LINE__);
							}else if(int pips_change = 0; false){ mpre("ОШИБКА обнуления изменений пипсов", __LINE__);
							}else if([&](){ // Перемешивание
								if(1 >= _in.size()){ //mpre("Не перемешиваем одно значение", __LINE__);
								}else if(_in.is_object()){ mpre("Обьект не перемешиваем", __LINE__);
								}else if(string random = (ARGV.end() == ARGV.find("-r") ? "1" : ARGV.at("-r")); (0 >= random.length())){ mpre("ОШИБКА аргумент перемешивания не задан -r", __LINE__);
								}else if(string::npos != random.find_last_not_of("0123456789")){ mpre("ОШИБКА формат перемешивания задан неправильно "+ random, __LINE__);
								}else if("0" == random){ mpre("Перемешивание отключено -r " +ARGV.at("-r"), __LINE__);
								}else if(std::random_device rd;  false){ mpre("ОШИБКА создания случайного значения", __LINE__);
								}else if(std::mt19937 g(rd()); false){ mpre("ОШИБКА инициации случайного значения", __LINE__);
								}else if(shuffle(_in.begin(), _in.end(), g); false){ mpre("ОШИБКА перемешивания массива", __LINE__);
								}else if([&](){ std::lock_guard<std::recursive_mutex> lock(mu); std::cerr << endl << __LINE__ << " ПЕРЕМЕШИВАНИЕ " << loop; return _in.empty(); }()){ mpre("ОШИБКА перемешивания массива входящих значений", __LINE__);
								}else{ //mpre("Перемешиваем список", __LINE__);
								} return false; }()){ mpre("ОШИБКА перемешивания списка", __LINE__);
							}else if(int count = 0; (count < 0)){ mpre("ОШИБКА установки счетчика расчетов", __LINE__);
							}else if([&](){ // Расчет каждого из входных значений
								for(auto &js:_in.items()){ // Расчет исходных данных
									if(false){ mpre("Остановка потока _in.items()", __LINE__);
									}else if(count += 1; (0 >= count)){ mpre("ОШИБКА увеличения примера", __LINE__);
									}else if(json el = js.value(); el.empty()){ mpre("ОШИБКА элемент не найден", __LINE__);
									}else if(int key = Crc32(el["dano"].dump().c_str()); (0 == key)){ mpre("ОШИБКА расчета crc32", __LINE__);
									}else if(pips_sum += BMF_ITOG_EX.at("").size(); false){ mpre("ОШИБКА расчета количества итогов", __LINE__);
									}else if(TMs dano = TMs(el["dano"]); dano.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
									}else if(float _perc = (0 >= in.size() ? : float(in.size()-change)*100.0/in.size()); false){ mpre("ОШИБКА расчета процента", __LINE__);
									}else if(float _pips_perc = (0 >= pips_sum ? 0 : float(pips_sum-pips_change)*100.0/pips_sum); false){ mpre("ОШИБКА расчета процента совпадения сигнала", __LINE__);
									}else if([&](){ std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << endl << __LINE__ << ".РАСЧЕТ " +to_string(_thread) +" \"dano\":" << el["dano"].dump().substr(0 ,89) << "..." << endl;  return false; }()){ mpre("ОШИБКА отображения информации", __LINE__);
									}else if(Values(dano, "dano", BMF_DANO_VALUES_EX, clump_id); BMF_DANO_VALUES_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
									}else if(Vals(dano, "dano", key, _BMF_DANO_EX, BMF_DANO_VALUES_EX, BMF_DANO_TITLES_EX, clump_id); _BMF_DANO_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
									}else if(el["itog"].empty()){ //std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << __LINE__ << ".ОБУЧЕНИЕ не задано" << endl;
									}else if([&](){ std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << __LINE__ << " ОБУЧЕНИЕ " +to_string(_thread) +" \"itog\":" << el["itog"].dump() << endl; return false; }()){ mpre("ОШИБКА вывода уведомления", __LINE__);
									}else if(TMs itog = TMs(el["itog"]); itog.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
									}else if(Values(itog, "itog", BMF_ITOG_VALUES_EX, clump_id); BMF_ITOG_VALUES_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
									}else if(Vals(itog, "itog", key, _BMF_ITOG_EX, BMF_ITOG_VALUES_EX, BMF_ITOG_TITLES_EX, clump_id); _BMF_ITOG_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
									}else if(string info = "Эпоха:"+ to_string(loop)+ " Размер:"+ to_string(in.size())+ " Пример:"+ js.key()+ " Изменений:"+ to_string(change)+ " Сеть:"+ to_string(BMF_INDEX_EX.at("").size())+ " Процент:"+ to_string(perc)+ "/"+ to_string(_perc)+ "% Время:"+ to_string(time(0)-timestamp); (0 >= info.length())){ mpre("ОШИБКА составления строки состояния", __LINE__);
									}else if([&](){ //mpre("Начало Items " +to_string(_thread), __LINE__); // Уведомление об изменениях
										if(int _change = LearningAll(el, thread, _thread, clump_id, key, _BMF_DANO_EX, _BMF_ITOG_EX); false){ mpre("Расчет изменеений", __LINE__);
										}else if(pips_change += _change; false){ mpre("ОШИБКА получения количества пипсов", __LINE__);
										}else if(0 == _change){ //mpre("Расчет "+ info, __LINE__);
										}else if(0 >= ++change){ mpre("ОШИБКА установки флага изменения", __LINE__);
										}else{ //mpre("Несовпадение результатов "+ info, __LINE__);
										} return false; }()){ //mpre("Нет изменений "+ info, __LINE__);
									}else if([&](){ std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << __LINE__ << " Время: " << (time(0)-timestamp) << " Выборка: " << loop << "/" << in.size() << "/" << count << " Морфы: " << BMF_INDEX_EX.at("").size() << "/" << change << " " << perc << "/" << _perc << "% Итоги: " << BMF_ITOG_EX.at("").size() << "/" << pips_sum << "/" << pips_change << " " << pips_perc << "/" << _pips_perc << "%" << endl; return false; }()){ mpre("ОШИБКА вывода информации о данных", __LINE__);
									}else{ //mpre("Окончение Items " +to_string(_thread), __LINE__); //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); mpre(BMF_DANO_EX.at(""), __LINE__, "Дано"); //mpre(BMF_ITOG_VALUES_EX, __LINE__, "Список итогов");
									}
								}; return false; }()){ mpre("ОШИБКА перебора перетасованных значений входящих данных", __LINE__);
							}else if((0 < size_max) && (size_max < BMF_INDEX_EX.at("").size())){ mpre("Максимальное количество морфов "+ to_string(BMF_INDEX_EX.at("").size())+ " больше допустимого значения size_max = "+ to_string(size_max)+ ". Расчет прерван "+ to_string(change), __LINE__); exit(EXIT_FAILURE);
							}else if([&](){ perc = float(in.size()-change)*100.0/in.size(); return false; }()){ mpre("ОШИБКА расчета процента", __LINE__);
							}else if([&](){ pips_perc = float(pips_sum-pips_change)*100.0/pips_sum; return false; }()){ mpre("ОШИБКА расчета процента совпадения сигнала", __LINE__);
							}else if([&](){ change_sum += change; return false; }()){ mpre("ОШИБКА расчета итоговой суммы изменений", __LINE__);
							}else{ //std::cerr << endl << __LINE__ << " ЦИКЛ ОБУЧЕНИЯ " << to_string(change) << " из " << to_string(in.size()) << " Процент " << perc << "%";
								//std::cerr << " Итогов " << to_string(pips_change) << " Изменений сигналов " << to_string(pips_change) << " Процент " << to_string(pips_perc) << endl;
							}
						}while(change != 0 && ((0 == loop_max) || (loop < loop_max)));
					return false; }()){ mpre("ОШИБКА цикла обучения", __LINE__);
				}else{ //mpre(BMF_DANO_EX.at(""), __LINE__, "Дано"); mpre(BMF_ITOG_EX.at(""), __LINE__, "Итог");
				} return false;
			}); return false; }()){ mpre("ОШИБКА создания функции непосредственно расчета", __LINE__);
		}else{
		} return false; }()){ mpre("ОШИБКА создания рабочих функций", __LINE__);
	}else if([&](){ // Запуск расчетов
		if(0 >= in.size()){ mpre("Данные для обучения не заданы", __LINE__);
		}else if(int thread = [&](int thread = -1){ // Расчет количества потоков
			if(string _thread = (ARGV.end() == ARGV.find("-t") ? "0" : ARGV.at("-t")); (0 >= _thread.length())){ mpre("Количество потоков не задано в консоли", __LINE__);
			}else if(string::npos != _thread.find_first_not_of("0123456789")){ mpre("ОШИБКА формат количества потоков не число "+ _thread, __LINE__);
			}else if(thread = stoi(_thread); false){ mpre("ОШИБКА конвертации количества потоков в число", __LINE__);
			}else{ //mpre("Количество потоков "+ _thread, __LINE__);
			} return thread; }(); (0 > thread)){ mpre("ОШИБКА расчета количества потоков", __LINE__);
		}else if(std::vector<std::thread> threads; false){ mpre("ОШИБКА создания списка потоков", __LINE__);
		}else if([&](){ for(int _thread = 1; _thread <= thread; _thread++){
				if(std::thread th = std::thread(Do, thread, _thread); false){ mpre("ОШИБКА запуска потока", __LINE__);
				}else if(threads.push_back(std::move(th)); (0 >= threads.size())){ mpre("ОШИБКА добавления потока в справочник", __LINE__);
				}else{ mpre("Запуск потока "+ to_string(_thread), __LINE__);
				}
			} return false; }()){ mpre("ОШИБКА запуска потока", __LINE__);
		}else if([&](){ for(std::thread& th: threads){ // Ожидание потоков
				if(!th.joinable()){ mpre("ОШИБКА поток не подключаемый", __LINE__);
				}else if(th.join(); false){ mpre("ОШИБКА отключения от потока", __LINE__);
				}else{ mpre("Завершение потока #"+ to_string(--thread), __LINE__);
				}
			} return false; }()){ mpre("ОШИБКА отвязывания от потоков", __LINE__);
		}else if([&](){ // Без потоков
			if(threads.size()){ mpre("Количество потоков "+ to_string(threads.size()), __LINE__);
			}else{ Do(0, 0);
			} return false; }()){ mpre("ОШИБКА запуска функции без потоков", __LINE__);
		}else{ //mpre("Ожидание закончилось", __LINE__); 
		} return false; }()){ mpre("ОШИБКА перебора всех входных значений", __LINE__);
	}else if(1 > BMF_ITOG_VALUES_EX.at("").size()){ mpre("Cписок значений не установлен", __LINE__);
	}else if([&](){ // Расчет итога
		Value = ([&](TMs itog_values){
			TMMi ITOG, INDEX; string bin = "", val, value; double dec;
			if([&](){ ITOG = rb(BMF_ITOG_EX, {{"itog_values_id", itog_values.at("id")}}); return ITOG.empty(); }()){ mpre("ОШИБКА получения списка знаков значения", __LINE__); //mpre(itog_values, __LINE__, "Значение"); mpre(BMF_ITOG_EX.at(""), __LINE__, "ОШИБКА получения списка знаков значения");
			//}else if(true){ mpre("ОШИБКА Расчет временно остановлен из за функции Calc", __LINE__);
			}else if([&](){ for(auto itog_itr:ITOG){ //for_each(ITOG.begin(), ITOG.end(), [&](auto itog_itr){ // Сортировка списка
					TMs itog, index, dano; int pos, os;
					if([&](){ itog = itog_itr.second; return itog.empty(); }()){ mpre("ОШИБКА получения итога", __LINE__);
					}else if(itog.end() == itog.find("index_id")){ mpre("ОШИБКА поле index_id у итога не задано", __LINE__);
					}else if("" == itog.at("index_id")){ mpre("Пустое поле у итога index_id", __LINE__);
					}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(stoi(itog["index_id"]))){ mpre("Морф итога не указан", __LINE__);
					}else if([&](){ index = erb(BMF_INDEX_EX, {{"id", itog.at("index_id")}}); return index.empty(); }()){ //mpre("ОШИБКА получения морфа итога", __LINE__);
					}else if([&](){ dano = erb(BMF_DANO_EX, {{"id", index.at("dano_id")}}); return dano.empty(); }()){ mpre("ОШИБКА выборки исходника морфа", __LINE__);
					}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА поле исходника val не устанволено", __LINE__);
					}else if("" == dano.at("val")){ mpre("ОШИБКА значение исходника морфа не задано", __LINE__);
					}else if([&](){ index["val"] = Learn(index, "", 0, BMF_DANO_EX, BMF_ITOG_EX);	return index.empty(); }()){ mpre("ОШИБКА расчета значения морфа", __LINE__);
					}else if([&](){ mpre("Расчет "+ itog.at("id")+ " "+ itog_values.at("name")+ " ("+ itog.at("name")+ ") index["+ index.at("id")+ "]="+ index["val"] , __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
					}else if([&](){ INDEX.insert(make_pair(stoi(itog.at("name")), index)); return INDEX.empty(); }()){ mpre("ОШИБКА добавления итогов к сортированному списку", __LINE__);
					}else{ //mpre(itog, __LINE__, "Итог");
					}
				}; return ITOG.empty(); }()){ mpre("ОШИБКА получения сортированного списка значений", __LINE__);
			//}else if(true){ mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); mpre("Прверка", __LINE__);
			//}else if(true){ mpre(INDEX, __LINE__, "Список");
			}else if([&](){ for(auto &index_itr:INDEX){ //for_each(INDEX.begin(), INDEX.end(), [&](auto index_itr){ // Установка знаков в значение
					TMs index; int pos, os; string _val, _bin;
					if([&](){ index = index_itr.second; return index.empty(); }()){ mpre("ОШИБКА получения индекса", __LINE__);
					}else if([&](){ pos = index_itr.first; return false; }()){ mpre("ОШИБКА получения позиции знака", __LINE__);
					}else if([&](){ os = INDEX.rbegin()->first-pos; return (0 > os); }()){ mpre("ОШИБКА получения смещения (ноль и больше)", __LINE__);
					}else if([&](){ bin += (os >= bin.length() ? std::string(os-bin.length()+1, '-') : ""); return (os >= bin.length()); }()){ mpre("ОШИБКА увеличения длинны строки до нужного размера", __LINE__);
					}else if([&](){ _val = (0 == pos ? "." : index.at("val")); return (0 >= _val.length()); }()){ mpre("ОШИБКА получения символа знака", __LINE__);
					}else if([&](){ _bin = bin; bin = bin.substr(0, os)+ _val+ bin.substr(os+1, bin.length()); return (0 >= bin.length()); }()){ mpre("ОШИБКА установки символа знака", __LINE__);
					}else{ //mpre("Расчеты позиции и смещения pos="+ to_string(pos)+ " os="+ to_string(os)+ " length="+ to_string(bin.length())+ " val="+ val, __LINE__);
						//mpre("Изменения "+ _bin+ " >> "+ bin, __LINE__);
					}
				}; return INDEX.empty(); }()){ mpre("Морфы значения не установлены `"+ itog_values["name"]+ "`", __LINE__);
			}else if([&](){ bin = ((INDEX.find(0) != INDEX.end()) && (INDEX.at(0).at("val") == "1") ? "-" : "")+ bin; return (0 >= bin.length()); }()){ mpre("ОШИБКА установки символа отрицания", __LINE__);
			}else if([&](){ dec = Bin2dec(bin); return false; }()){ mpre("ОШИБКА конвертации двоичной в десятичную систему", __LINE__);
			}else if([&](){ dec = Bin2dec(bin); return false; }()){ mpre("ОШИБКА перевода двоичной строки в десятичное число", __LINE__);
			}else if([&](){ value = to_string(dec); return false; }()){ mpre("ОШИБКА установки значения расчета", __LINE__);
			}else if([&](){ itog_values["value"] = value; return itog_values.empty(); }()){ mpre("ОШИБКА получения конвертируемого значения", __LINE__);
			}else{ //mpre("Значение bin="+ bin+ " dec="+ to_string(dec), __LINE__);
			} return itog_values;
		}); return false; }()){ mpre("ОШИБКА создания функции расчета итога", __LINE__);
	}else if([&](){ // Вывод итоговых значений
		if(ARGV.end() == ARGV.find("-j")){ //mpre("Данные для расчета не указаны", __LINE__);
		}else if(nlohmann::json _in = ("" == ARGV.at("-j") ? in : json::parse(ARGV.at("-j"))); false){ mpre("ОШИБКА разбора строки атрибута -j", __LINE__);
		}else if(0 < change_sum){ mpre("При ошибках результат не выводим", __LINE__);
		}else if(nlohmann::json j = [&](nlohmann::json j = {}){ for(auto& js:_in.items()){ // Расчет списка теста
				if(json el = js.value(); el.empty()){ mpre("ОШИБКА элемент не найден", __LINE__);
				}else if(string key = js.key(); (0 >= key.length())){ mpre("ОШИБКА расчета ключа", __LINE__);
				}else if(el.end() == el.find("dano")){ mpre("ОШИБКА исходные данные не могут быть пустыми", __LINE__);
				}else if(TMs dano = [&](TMs dano = {}){ TMs _dano = el.at("dano"); dano = _dano; return dano; }(); dano.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
				}else if(TMs itog = [&](TMs itog = {}){ // Получение итога
					if(el.end() == el.find("itog")){ //mpre("Итог не задан", __LINE__);
					}else if([&](){ TMs _itog = el.at("itog"); itog = _itog; return itog.empty(); }()){ mpre("ОШИБКА получения итоговых знаков", __LINE__);
					}else{ //mpre(itog, __LINE__, "Итог");
					} return itog; }(); itog.empty()){ mpre("ОШИБКА получения итога", __LINE__);
				}else if(mpre("РЕЗУЛЬТАТ: "+ el.dump(), __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(int cache = Crc32(el["dano"].dump().c_str()); false){ mpre("ОШИБКА расчета ключа кеша", __LINE__);
				}else if(Vals(dano, "dano", cache, BMF_DANO_EX, BMF_DANO_VALUES_EX, BMF_DANO_TITLES_EX, clump_id); BMF_DANO_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
				}else if([&](){ for(auto &itog_values_itr:BMF_ITOG_VALUES_EX.at("")){ //for_each(BMF_ITOG_VALUES_EX.at("").begin(), BMF_ITOG_VALUES_EX.at("").end(), [&](auto &itog_values_itr){ // Расчет значений
						if(BMF_INDEX_EX.at("").empty()){ mpre("Установка первоначальных морфов", __LINE__);
						}else if(TMs itog_values = itog_values_itr.second; itog_values.empty()){ mpre("ОШИБКА получения значения", __LINE__);
						}else if(itog_values = Value(itog_values); itog_values.empty()){ mpre("ОШИБКА расчета значения", __LINE__);
						}else if(itog_values_itr.second = itog_values; itog_values_itr.second.empty()){ mpre("ОШИБКА сохранения результатов расчета в справочник", __LINE__);
						}else{ //mpre(itog_values, __LINE__, "Расчетное значение"); //mpre("Расчетное значение `"+ itog_values["name"]+ "` "+ to_string(dec)+ " ("+ bin+ ")", __LINE__);
						}
					}; return false; }()){ mpre("Ошибка отображение результата расчета", __LINE__);
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
						}
					} return false; }()){ mpre("ОШИБКА расчета результата", __LINE__);
				}else{ //mpre("Итог "+ j.dump(), __LINE__);
				}
			} return j; }(); false){ mpre("ОШИБКА расчета списка теста", __LINE__);
		}else{ //mpre("j "+ j.dump(), __LINE__);
			std::cout << j.dump('\t');
		} return false; }()){ mpre("Ошибка отображение результата расчета", __LINE__);
	}else if([&](){ std::cerr << endl;
		Save = ([&](string table, TMMi ROW){ // Расчет значения формулы
			TMs index; int count_insert = 0, count_update = 0, count_delete = 0; string sql;
			for(auto index_itr = ROW.begin(); index_itr != ROW.end(); index_itr++){ // Сохранение данных справочника
				TMs index, _index; int id, index_id;
				if([&](){ index = index_itr->second; return index.empty(); }()){ mpre("ОШИБКА выборки значения итератора", __LINE__);
				}else if(index.empty()){ mpre("ОШИБКА пустой морф в справочнике", __LINE__);
				}else if([&](){ index_id = index_itr->first; return (0 == index_id); }()){ mpre("ОШИБКА получения ключа строки", __LINE__);
				}else if([&](){ // Обновление
					if("0" != index["id"]){ //mpre("Не удаляем идентификатор не пуст", __LINE__);
					}else if(0 > index_id){ //mpre("Морф еще не сохранен в таблицу", __LINE__);
					}else if([&](){ exec("DELETE FROM `"+ table+ "` WHERE id="+ to_string(index_id)); return false; }()){ mpre("ОШИБКА удаления морфа", __LINE__);
					}else{ //mpre(index, __LINE__, "Удаление морфа");
						count_delete++;
					} return false; }()){ mpre("ОШИБКА удаления морфа", __LINE__);
				}else if([&](){ // Обновление
					if(0 > index_id){ //mpre("Не обновляем вновь добавленный "+ to_string(id), __LINE__);
					}else if("0" == index["id"]){ //mpre("Не обновляем удаленный морф", __LINE__);
					}else if([&](){ index = fk(table, {{"id", index["id"]}}, {}, index); return index.empty(); }()){ mpre(index, __LINE__, "ОШИБКА обновления морфа "+ to_string(id));
					}else{ //mpre(index, __LINE__, "Обновление морфа");
						count_update++;
					} return false; }()){ mpre("ОШИБКА обновления значения морфа", __LINE__);
				}else if([&](){ // Добавление
					if(0 < index_id){ //mpre("Не добавляем морф (создан ранее)", __LINE__);
					}else if("0" == index["id"]){ //mpre("Не обновляем удаленный морф", __LINE__);
					}else if([&](){ index = fk(table, {}, index, {}); return index.empty(); }()){ mpre("ОШИБКА сохранения нового значения в базу", __LINE__);
					}else{ //mpre(index, __LINE__, "Добавление нового морфа "+ to_string(index_id));
						count_insert++;
					} return false; }()){ mpre("ОШИБКА добавления нового морфа", __LINE__);
				}else if([&](){ ROW.find(index_id)->second = index; return index.empty(); }()){ mpre(index, __LINE__, "Сохраняем в справочник");
				}else{ //exit(mpre(ROW, __LINE__, "Обновление "+ to_string(index_id)));
				}
			} mpre("Данные таблицы `"+ table+ "` удалений: "+ to_string(count_delete)+ " обновлений: "+ to_string(count_update)+ " добавлений: "+ to_string(count_insert), __LINE__);
		return ROW; }); return false; }()){ mpre("ОШИБКС создания функции сохранения в базу данных", __LINE__);
	}else if([&](){ // Сохранение
		TMMi _BMF_DANO_VALUES, _BMF_ITOG_VALUES, _BMF_DANO_TITLES, _BMF_ITOG_TITLES, _BMF_DANO, _BMF_ITOG;
		if(in.empty()){ mpre("Пустой массив обучающей выборки. Не сохраняем данные в БД", __LINE__);
		}else if([&](){ // Установка прав доступа к файлу
			std::experimental::filesystem::perms p; string cmd;
			if([&](){ p = std::experimental::filesystem::status(dbname).permissions(); return false; }()){ mpre("ОШИБКА Получение прав доступа к файлу", __LINE__);
			}else if((p & std::experimental::filesystem::perms::others_write) != std::experimental::filesystem::perms::none){ //mpre("Права доступа заданы верно", __LINE__);
			}else if([&](){ cmd = "chmod a+w "+ dbname; return (0 >= cmd.length()); }()){ mpre("ОШИБКА задания комманды установки прав к файлу", __LINE__);
			}else if([&](){ system(cmd.c_str()); return false; }()){ mpre("ОШИБКА установки полных прав доступа", __LINE__);
			}else{ mpre("Установка прав доступа "+ cmd, __LINE__);
			} return false; }()){ mpre("ОШИБКА установки прав досутпа к файлу", __LINE__);
		}else if([&](){ std::experimental::filesystem::perms p = std::experimental::filesystem::status(dbname).permissions(); return ((p & std::experimental::filesystem::perms::others_write) == std::experimental::filesystem::perms::none); }()){ mpre("ОШИБКА файл БД не доступен для записи $chmod u+w "+ dbname, __LINE__);
		}else if([&](){ //mpre("Сохранение результатов в тест", __LINE__);
			if([&](){ exec("CREATE TABLE IF NOT EXISTS mp_bmf_test (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `date` TEXT, `size` INTEGER, `args` TEXT, `change` INTEGER, `duration` INTEGER, `bmf` INTEGER, `loop` INTEGER, `perc` REAL, `pips` REAL, `clump` TEXT)"); return false; }()){ mpre("ОШИБКА создания значения итога", __LINE__);
			}else if(string date = [&](string date = ""){ char mbstr[100]; time_t t = time(nullptr); std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&t)); date = string(mbstr); return date; }(); (0 >= date.length())){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(nlohmann::json arg = [&](nlohmann::json arg = {}){ arg = ARGV; arg.erase("-"); arg.erase("-j"); return arg; }(); arg.empty()){ mpre("ОШИБКА получения строки аргументов", __LINE__);
			}else if(TMs test = fk("mp_bmf_test", {}, {{"time", to_string(timestamp)}, {"date", date}, {"size", to_string(in.size())}, {"change", to_string(change_sum)}, {"duration", to_string(time(0)-timestamp)}, {"clump", clump_id}, {"loop", to_string(loop)}, {"perc", to_string(perc)}, {"pips", to_string(pips_perc)}, {"bmf", to_string(BMF_INDEX_EX.at("").size())}, {"args", arg.dump()}}, {}); test.empty()){ mpre("ОШИБКА сохранения результатов тестов", __LINE__);
			}else{ //mpre("Сохранение статистики id "+ test["id"], __LINE__);
			} return false; }()){ mpre("ОШИБКА сохранения теста", __LINE__);
		}else if([&](){ exec("BEGIN TRANSACTION"); return false; }()){ mpre("ОШИБКА запуска начала транзакции", __LINE__);
		}else if(TMMi _BMF_INDEX = [&](TMMi _BMF_INDEX = {}){ // Сохранение данных в базу для получения идентификаторов
			if([&](){ _BMF_DANO = Save("mp_bmf_dano", BMF_DANO_EX.at("")); return _BMF_DANO.empty(); }()){ mpre("ОШИБКА сохранения исходных данных", __LINE__);
			}else if([&](){ _BMF_ITOG = Save("mp_bmf_itog", BMF_ITOG_EX.at("")); return false; }()){ mpre("ОШИБКА сохранения итоговых данных", __LINE__);
			}else if([&](){ _BMF_DANO_VALUES = Save("mp_bmf_dano_values", BMF_DANO_VALUES_EX.at("")); return _BMF_DANO_VALUES.empty(); }()){ mpre("ОШИБКА сохранения изначальных значений", __LINE__);
			}else if([&](){ _BMF_ITOG_VALUES = Save("mp_bmf_itog_values", BMF_ITOG_VALUES_EX.at("")); return _BMF_ITOG_VALUES.empty(); }()){ mpre("ОШИБКА сохранения итоговых значений", __LINE__);
			}else if([&](){ _BMF_DANO_TITLES = Save("mp_bmf_dano_titles", BMF_DANO_TITLES_EX.at("")); return false; }()){ mpre("ОШИБКА сохранения изначальных заголовков", __LINE__);
			}else if([&](){ _BMF_ITOG_TITLES = Save("mp_bmf_itog_titles", BMF_ITOG_TITLES_EX.at("")); return false; }()){ mpre("ОШИБКА сохранения итоговых заголовков", __LINE__);
			}else if([&](){ _BMF_INDEX = Save("mp_bmf_index", BMF_INDEX_EX.at("")); return false; }()){ mpre("ОШИБКА сохранения справочника морфа", __LINE__);
			}else{ std::cerr << endl << __LINE__ << " Сохранение списка морфов << Эпох: " << loop << " Морфов: " << BMF_INDEX_EX.at("").size() << " Время: " << time(0)-timestamp << endl << endl;
			} return _BMF_INDEX; }(); _BMF_INDEX.empty()){ mpre("Не сохраняем историю изменение заголовков и морфов", __LINE__);
		}else if([&](){ for(auto &itog_itr:_BMF_ITOG){ //for_each(_BMF_ITOG.begin(), _BMF_ITOG.end(), [&](auto itog_itr){ // Корректировка связи с итогом
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if(TMs _itog = itog_itr.second; _itog.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if("" == itog["index_id"]){ mpre(BMF_ITOG_EX.at(""), __LINE__, "Итог"); mpre("ОШИБКА Идентификатор морфа не указан", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(stoi(itog["index_id"]))){ //mpre("Морф не найден", __LINE__);
					}else if(TMs _index = _BMF_INDEX.at(stoi(itog["index_id"])); _index.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_index.end() == _index.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if([&](){ itog["index_id"] = _index.at("id"); return false; }()){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_ITOG_VALUES.end() == _BMF_ITOG_VALUES.find(stoi(itog.at("itog_values_id")))){ mpre("ОШИБКА морф не значение", __LINE__);
					}else if(TMs _itog_values = _BMF_ITOG_VALUES.at(stoi(itog.at("itog_values_id"))); _itog_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_itog_values.end() == _itog_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if([&](){ itog["itog_values_id"] = _itog_values.at("id"); return false; }()){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(itog == _itog){ //mpre("Изменений в итоге не найдено", __LINE__);
				}else if([&](){ _itog = fk("mp_bmf_itog", {{"id", itog["id"]}}, {}, itog); return _itog.empty(); }()){ mpre(itog, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(_index, __LINE__, "Связь 1"); mpre(index, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			}; return false; }()){ mpre("ОШИБКА корректировки свзи с итогом", __LINE__);
		}else if([&](){ for(auto &dano_itr:_BMF_DANO){ //for_each(_BMF_DANO.begin(), _BMF_DANO.end(), [&](auto dano_itr){ // Корректировка связи с итогом
				if(TMs dano = dano_itr.second; dano.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if(TMs _dano = dano_itr.second; _dano.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_DANO_VALUES.end() == _BMF_DANO_VALUES.find(stoi(dano.at("dano_values_id")))){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs _dano_values = _BMF_DANO_VALUES.at(stoi(dano.at("dano_values_id"))); _dano_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_dano_values.end() == _dano_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if([&](){ dano["dano_values_id"] = _dano_values.at("id"); return false; }()){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(dano == _dano){ //mpre("Изменений в итоге не найдено", __LINE__);
				}else if([&](){ _dano = fk("mp_bmf_dano", {{"id", dano["id"]}}, {}, dano); return _dano.empty(); }()){ mpre(dano, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(_index, __LINE__, "Связь 1"); mpre(index, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			}; return false; }()){ mpre("ОШИБКА корректировки свзи с итогом", __LINE__);
		}else if([&](){ for(auto &index_itr:_BMF_INDEX){ //for_each(_BMF_INDEX.begin(), _BMF_INDEX.end(), [&](auto &index_itr){ // Правка связей
				if(TMs index = index_itr.second; index.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if(TMs _index = index_itr.second; _index.empty()){ mpre("ОШИБКА получения морфа из итератора", __LINE__);
				}else if("0" == index["id"]){ //mpre("Не правим удаленные морфы", __LINE__);
				}else if([&](){ // Установка младшего морфа
					if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string id = index.at("bmf-index"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(stoi(id))){ mpre("ОШИБКА морф в списке не найден "+ id, __LINE__);
					}else if(TMs _index_ = _BMF_INDEX.at(stoi(id)); _index_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["bmf-index"] = _index_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка старшего морфа
					if(index.end() == index.find("index_id")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string id = index.at("index_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(stoi(id))){ mpre("Морф в списке не найден "+ id, __LINE__);
					}else if(TMs _index_ = _BMF_INDEX.at(stoi(id)); _index_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["index_id"] = _index_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка связи со значением
					if(index.end() == index.find("itog_values_id")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string id = index.at("itog_values_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_ITOG_VALUES.end() == _BMF_ITOG_VALUES.find(stoi(id))){ mpre("ОШИБКА морф в списке не найден "+ id, __LINE__);
					}else if(TMs _itog_values_ = _BMF_ITOG_VALUES.at(stoi(id)); _itog_values_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["itog_values_id"] = _itog_values_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка связи со значением
					if(index.end() == index.find("dano_id")){ mpre("ОШИБКА поле с исходным значением", __LINE__);
					}else if(string id = index.at("dano_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_DANO.end() == _BMF_DANO.find(stoi(id))){ //mpre("ОШИБКА исходник в списке не найден "+ id, __LINE__);
					}else if(TMs dano = _BMF_DANO.at(stoi(id)); dano.empty()){ mpre("ОШИБКА дано не найден", __LINE__);
					}else if(index["dano_id"] = dano.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка связи со значением
					if(index.end() == index.find("itog_id")){ mpre("ОШИБКА поле с итоговым значением", __LINE__);
					}else if(string id = index.at("itog_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_ITOG.end() == _BMF_ITOG.find(stoi(id))){ mpre("ОШИБКА итог в списке не найден "+ id, __LINE__);
					}else if(TMs itog = _BMF_ITOG.at(stoi(id)); itog.empty()){ mpre("ОШИБКА итог не найден", __LINE__);
					}else if(index.at("itog_id") = itog.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if(index == _index){ //mpre("Изменений не установлено", __LINE__);
				}else if([&](){ _index = fk("mp_bmf_index", {{"id", index["id"]}}, {}, index); return _index.empty(); }()){ mpre(index, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(_index, __LINE__, "Связь 1"); mpre(index, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if([&](){ for(auto &dano_titles_itr:_BMF_DANO_TITLES){ //for_each(_BMF_DANO_TITLES.begin(), _BMF_DANO_TITLES.end(), [&](auto &dano_titles_itr){ // Правка связей
				if(TMs dano_titles = dano_titles_itr.second; dano_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if(TMs _dano_titles = dano_titles_itr.second; _dano_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_DANO_VALUES.end() == _BMF_DANO_VALUES.find(stoi(dano_titles.at("dano_values_id")))){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs _dano_values = _BMF_DANO_VALUES.at(stoi(dano_titles.at("dano_values_id"))); _dano_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_dano_values.end() == _dano_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if(dano_titles.at("dano_values_id") = _dano_values.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if([&](){ _dano_titles = fk("mp_bmf_dano_titles", {{"id", dano_titles["id"]}}, {}, dano_titles); return _dano_titles.empty(); }()){ mpre(dano_titles, __LINE__, "ОШИБКА обновления заголовка");
				}else{ //mpre(_dano_titles, __LINE__, "Связь 1"); //mpre(dano_titles, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if([&](){ for(auto &itog_titles_itr:_BMF_ITOG_TITLES){ //for_each(_BMF_ITOG_TITLES.begin(), _BMF_ITOG_TITLES.end(), [&](auto &itog_titles_itr){ // Правка связей
				if(TMs itog_titles = itog_titles_itr.second; itog_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if(TMs _itog_titles = itog_titles_itr.second; _itog_titles.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if([&](){ // Установка ссылки на морф
					if(_BMF_ITOG_VALUES.end() == _BMF_ITOG_VALUES.find(stoi(itog_titles.at("itog_values_id")))){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(TMs _itog_values = _BMF_ITOG_VALUES.at(stoi(itog_titles.at("itog_values_id"))); _itog_values.empty()){ mpre("ОШИБКА выборки морфа по первой связи", __LINE__);
					}else if(_itog_values.end() == _itog_values.find("id")){ mpre("ОШИБКА получения идентификатора связи", __LINE__);
					}else if(itog_titles["itog_values_id"] = _itog_values.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre("Установка ссылки на морф", __LINE__);
					} return false; }()){ mpre("ОШИБКА установка ссылки на морф", __LINE__);
				}else if(TMs _itog_titles = fk("mp_bmf_itog_titles", {{"id", itog_titles["id"]}}, {}, itog_titles); _itog_titles.empty()){ mpre(itog_titles, __LINE__, "ОШИБКА обновления заголовка");
				}else{ //mpre(_itog_titles, __LINE__, "Связь 1"); //mpre(itog_titles, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if([&](){ exec("COMMIT TRANSACTION"); return false; }()){ mpre("ОШИБКА начала сессии к БД", __LINE__);
		}else{ //mpre(_BMF_INDEX, __LINE__, "Сохранение");
		} return false; }()){ mpre("ОШИБКА установки ключей", __LINE__);
	}else{
	}
}

