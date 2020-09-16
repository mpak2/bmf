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

//#define CL_HPP_MINIMUM_OPENCL_VERSION 110
//#define CL_HPP_TARGET_OPENCL_VERSION 210
//#define CL_HPP_CL_1_2_DEFAULT_BUILD
//	#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
//#include "CL/cl2.hpp"

#define pre(x) if(x){ printf("%d.%s\n", __LINE__, x); }
#define err(x) if(x){ printf("%d.\x1b[37;41m ОШИБКА \x1b[0m %s\n", __LINE__, x); exit(1); }

using json = nlohmann::json; json in;

using namespace std; //setlocale(LC_ALL, "ru_RU");

typedef std::map<string, string> TMs;
typedef std::map<string, TMs> TMMs;
typedef std::map<int, TMs> TMMi;
typedef std::map<string, TMMi> TM3i;

std::recursive_mutex mu;

#include "bmf.c"

TM3i BMF_INDEX_EX, BMF_DANO_EX, BMF_ITOG_EX;
TMMi BMF_CLUMP, DATABASES;//, BMF_INDEX, BMF_DANO, _BMF_DANO, _BMF_ITOG, BMF_ITOG;
TM3i BMF_DANO_VALUES_EX, BMF_ITOG_VALUES_EX; // Значения позиций морфов
TM3i BMF_DANO_TITLES_EX, BMF_ITOG_TITLES_EX; // Списки справочников


namespace bmf {
	int loop = 0; // Количество итераций скрипта
	string clump_id = ""; // Скопление если не указано то данные из php кода
	TMs bmf_clump; // Текущее скопление
	int values_length = 1024; // Ограничение размера истории
	int change_sum = 0; // Счетчик расчетов
	int size_max = 1e6; // Максимально допустимое количество морфов в расчете
	float perc = 0; // Процент правильных прогнозов в примере
	float pips_perc = 0; // Процент правильных прогнозов в бите
	TMMi CACHE; // Кеш результатов расчета
	TMs CALCULATE; // Список формул для быстрых расчетов для каждого из итогов
	TMs MAP; // Список карт
}

int main(int argc, char **argv){
	TMMs data[16];// int data_size;
	TMs databases; // Список баз данных
	TMs ARGV; // Массив параметров консоли
	TMs TEST; // Список тестов

	int timestamp = time(0); // Расчет выполнения участков кода

	TM3i _VAL, _CALC;


	std::function<sqlite3_stmt*(string)> exec; // Таблица с интервалами времени
	std::function<sqlite3_stmt*(string)> prepare; // Таблица с интервалами времени
	std::function<TMMi(string)> Tab; // Выборка таблицы из БД
	std::function<TMs(string,TMs,TMs,TMs)> fk; // Сохранение информации в базу
	std::function<time_t(string,string,int)> Timer; TM3i TIMER; // Таблица с интервалами времени
	std::function<TMMi(string,TMMi)> Save; // Сохранение информации в базу
	std::function<int(TMs,string,TM3i&)> Values; // Проверка наличия значений в БД
	std::function<int(TMs,string,int,TM3i&,TM3i&,TM3i&)> Vals; // Обучение
	std::function<int(TMs,int,string,TM3i&,TM3i&)> Learning; // Обучение
	std::function<int(json,int,int,int,TM3i&,TM3i&)> LearningAll; // Обучение
	std::function<string(double)> Dec2bin; // Перерасчет размерности размерности из десятеричной в двоичную
	std::function<string(TMs,string,int,string,TM3i&,TM3i&)> Learn; // Рерасчет морфа
	std::function<double(string)> Bin2dec; // Перерасчет размерности из двоичной в десятеричную
	std::function<string(TMs,string)> Val; // Расчет истории
	//std::function<TMs(TMs,bool)> Calc; // Расчет истории
	std::function<TMs(TMs)> CalcAll; // Расчет истории
	std::function<TMs(TMs)> Calc_new; // Расчет истории
	std::function<TMs(TMs,TMs,TM3i&,TM3i&)> Choice; // Расчет истории
	std::function<string(TMMi&)> Id; // Расчет локального идентификатора
	std::function<TMMi(TMMi&,string,string,string)> Dataset; // Выгрузка данных из базы с проверкой равенства
	std::function<bool(TMMi,int,string,string)> Data; // Отображение списка данных в коде
	std::function<bool(int,int)> Do; // Непосредственно расчет
	std::function<string(TMs,TM3i&,TM3i&)> History; // История морфа
	std::function<TMs(TMs)> Value; // История морфа
	std::function<TMs(TMs,TM3i&)> Tree; // История морфа
	std::function<std::string(TMs)> Calculate; // Расчет строки для расчета польской нотации
	std::function<std::string(std::string,TM3i&)> Calc; // Расчет строки польской нотации

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
		}else if([&](){ bmf::clump_id = (ARGV.end() == ARGV.find("-d") ? bmf::clump_id : ARGV.at("-d")); return false; }()){ mpre("ОШИБКА Параметр адресной строки с указанием БД не задан", __LINE__);
		}else{ //mpre("Список параметров консоли", ARGV, __LINE__);
		} return false; }()){ mpre("ОШИБКА получения данных", __LINE__);
	}else if([&](){ // Входной поток
		if(ARGV.end() == ARGV.find("-")){ //mpre("Данные для обучения не установлены", __LINE__);
		}else if([&](string str = ""){ while(getline(std::cin, str)){ ARGV.at("-") += ("\n"+ str); }; return (0 >= ARGV.at("-").length()); }()){ mpre("ОШИБКА входящий параметр не задан", __LINE__);
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
				std::cout << "bimotph v5.0" << endl;
				std::cout << "Copyright (C) 2017 биморф.рф" << endl;
				std::cout << "Нет НИКАКИХ ГАРАНТИЙ до степени, разрешённой законом." << endl << endl;
				std::cout << "Лицензия freemium https://ru.wikipedia.org/wiki/Freemium" << endl;
				std::cout << "Данная версия является условной бесплатной с граничением сети в "+ to_string(bmf::size_max)+ " морфов" << endl;
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
						}else if([&](){ mpre("Повторный запрос к БД через "+ to_string(sleep *= 2) +" сек.", __LINE__); mpre("Запрос "+ sql, __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
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
							int next_id = (row.end() == row.find("id") ? ++id : atoi(row["id"].c_str()));
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
						} return check; }()){ std::cerr << __LINE__ << " ОШИБКА Даныне в справочниках не совпадают" << endl; Data(_TAB, __LINE__, name, comment); exit(1);
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
			}else if(std::experimental::filesystem::perms p = std::experimental::filesystem::status(dbname).permissions(); ((p & std::experimental::filesystem::perms::owner_write) == std::experimental::filesystem::perms::none)){ mpre("ОШИБКА файл БД не доступен для записи $chmod u+w "+ dbname, __LINE__);
			}else if([&](){ // Установка соединения с БД
				if(SQLITE_OK != sqlite3_open(dbname.c_str(), &db)){ std::cerr << __LINE__ << " ОШИБКА открытия базы данных << " << dbname << endl;
				}else if([&](){ string f = argv[0]; return (f != "./bmf"); }()){ //mpre("Пропуск подключения родительсткой БД", __LINE__);
				}else if(string attach_database = "../../../../.htdb"; (0 >= attach_database.length())){ mpre("ОШИБКА имя доп БД не задано", __LINE__);
				}else if(access(attach_database.c_str(), F_OK) == -1){ mpre("Файл БД не найден "+ attach_database, __LINE__);
				}else if([&](){ exec("ATTACH DATABASE '../../../../.htdb' AS bmf"); return false; }()){ mpre("ОШИБКА Подключение основной базы с данными", __LINE__);
				}else{ std::cerr << __LINE__ << " Подключение родительской БД " << attach_database << endl;
				} return false; }()){ mpre("ОШИБКА подключения баз данных", __LINE__);
			}else if([&](){ // Получение пути до файла БД
				if(int pos = dbname.rfind("/"); (0 > pos)){ mpre("Слешей в пути до скопления не найдено", __LINE__);
				}else if(bmf::clump_id = dbname.substr(pos+1, dbname.length()); (0 >= bmf::clump_id.length())){ mpre("ОШИБКА сокращения пути до файла", __LINE__);
				}else{ //mpre("Путь до БД сокращен "+ clump_id, __LINE__);
				} return (0 >= bmf::clump_id.length()); }()){ mpre("ОШИБКА получения скопления", __LINE__);
			}else if([&](){ // Получение текущего скопления
				if("0" == bmf::clump_id){ mpre("Скопление не указано", __LINE__);
				}else if(DATABASES = Tab("PRAGMA database_list"); DATABASES.empty()){ mpre("ОШИБКА получения списка подключенных таблиц", __LINE__);
				}else if(databases = erb(DATABASES, {{"name", "bmf"}}); databases.empty()){ //mpre("Общая таблица не найдена", __LINE__);
				}else if(string sql = "SELECT 0 AS id, COUNT(*) AS cnt FROM bmf.sqlite_master WHERE type='table' AND name='mp_bmf_clump'"; (0 >= sql.length())){ mpre("ОШИБКА составления запроса проверки таблицы скоплений", __LINE__);
				}else if([&](){ TMMi CLUMP = Tab(sql); return ("0" == CLUMP.find(0)->second.find("cnt")->second); }()){ mpre("Таблица со скоплениями отсутсвует в базе", __LINE__);
				}else if(sql = "SELECT * FROM mp_bmf_clump WHERE id='"+ bmf::clump_id+ "'"; false){ mpre("Задайте номер скопления", __LINE__);
				}else if([&](){ BMF_CLUMP = Tab(sql); bmf::bmf_clump = (0 >= BMF_CLUMP.size() ? TMs({}) : BMF_CLUMP.begin()->second); return bmf::bmf_clump.empty(); }()){ //mpre("Информация о скоплении не установлена "+ sql, __LINE__);
				}else if(bmf::bmf_clump = fk("mp_bmf_clump", {{"id", bmf::clump_id}}, {}, {{"hide", "0"}}); bmf::bmf_clump.empty()){ mpre("ОШИБКА обновления видимости скопления", __LINE__);
				}else if(exec("UPDATE mp_bmf_clump SET hide=1 WHERE id<>"+ bmf::clump_id); false){ mpre("ОШИБКА скрытия не активных скопления", __LINE__);
				}else{ mpre("Скопление: `"+ bmf::bmf_clump["name"]+ "`", __LINE__);
				} return (0 >= bmf::clump_id.length()); }()){ mpre("ОШИБКА получения скопления", __LINE__);
			}else if([&](){ // Добавление таблиц в БД если они не созданы
				if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_index (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`clump_id` INTEGER,`itog_values_id` INTEGER, `depth` INTEGER, `dimension` INTEGER,`dano_id` INTEGER,`itog_id` TEXT,`calc_pos_id` INTEGER,`index_id` INTEGER, `bmf-index` INTEGER)"); false){ mpre("ОШИБКА создания таблицы морфов", __LINE__);
				}else if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_dano (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER,`uid` INTEGER,`clump_id` INTEGER,`dano_values_id` INTEGER,`name` TEXT,`val` INTEGER,`values` TEXT)"); false){ mpre("ОШИБКА создания таблицы морфов", __LINE__);
				}else if(exec("CREATE INDEX IF NOT EXISTS `bmf_dano-clump_id-dano_values_id` ON mp_bmf_dano(clump_id,dano_values_id);"); false){ mpre("ОШИБКА создания индекса", __LINE__);
				}else if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_itog (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER,`clump_id` INTEGER,`index_id` INTEGER,`itog_values_id` INTEGER,`name` TEXT,`val` INTEGER,`values` TEXT)"); false){ mpre("ОШИБКА создания таблицы морфов", __LINE__);
				}else if(exec("CREATE INDEX IF NOT EXISTS `bmf_itog-clump_id-itog_values_id` ON mp_bmf_itog(clump_id,itog_values_id);"); false){ mpre("ОШИБКА создания индекса", __LINE__);
				}else if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_dano_values (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER ,`uid` INTEGER ,`clump_id` INTEGER ,`index_type_id` INTEGER ,`name` TEXT ,`value` TEXT ,`old` INTEGER ,`shift` INTEGER ,`dano_values_option_id` INTEGER ,`bin` TEXT)"); false){ mpre("ОШИБКА создания значений дано", __LINE__);
				}else if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_itog_values (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER,`index_type_id` INTEGER,`clump_id` INTEGER,`name` TEXT,`value` INTEGER,`itog_values_option_id` INTEGER,`bin` INTEGER)"); false){ mpre("ОШИБКА создания значения итога", __LINE__);
				}else if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_itog_titles (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `clump_id` INTEGER, `itog_values_id` INTEGER, `value` INTEGER, `name` INTEGER, `text` TEXT)"); false){ mpre("ОШИБКА создания значения итога", __LINE__);
				}else if(exec("CREATE TABLE IF NOT EXISTS main.mp_bmf_dano_titles (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `clump_id` INTEGER, `dano_values_id` INTEGER, `value` INTEGER, `name` INTEGER, `text` TEXT)"); false){ mpre("ОШИБКА создания значения итога", __LINE__);
				}else{ return false;
				} return true; }()){ mpre("ОШИБКА подключения БД созвездия и создания таблиц", __LINE__);
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
					}else{ mpre("Очистка списка морфов по аргументу -c", __LINE__); //system("sleep 1");
					} return false;
				}()){ mpre("ОШИБКА выборки списка морфов", __LINE__);
			}else if([&](){ BMF_CALC = Dataset(BMF_CALC, "BMF_CALC", "mp_bmf_calc", "Расчеты"); return BMF_CALC_POS.empty(); }()){ mpre("ОШИБКА выборки позиций расчетов", __LINE__);
			}else if([&](){ BMF_CALC_ORIENT = Dataset(BMF_CALC_ORIENT, "BMF_CALC_ORIENT", "mp_bmf_calc_orient", "Направления"); return BMF_CALC_ORIENT.empty(); }()){ mpre("ОШИБКА выборки направлений расчетов", __LINE__);
			}else if([&](){ BMF_CALC_POS = Dataset(BMF_CALC_POS, "BMF_CALC_POS", "mp_bmf_calc_pos", "Позиции"); return BMF_CALC_POS.empty(); }()){ mpre("ОШИБКА выборки позиций расчетов", __LINE__);
			}else if([&](){ BMF_CALC_VAL = Dataset(BMF_CALC_VAL, "BMF_CALC_VAL", "mp_bmf_calc_val", "Знаки"); return BMF_CALC_VAL.empty(); }()){ mpre("ОШИБКА выборки значений расчетов", __LINE__);
			}else if([&](){ BMF_INDEX_TYPE = Dataset(BMF_INDEX_TYPE, "BMF_INDEX_TYPE", "mp_bmf_index_type", "Тип значения"); return BMF_INDEX_TYPE.empty(); }()){ mpre("ОШИБКА выборки типа значений", __LINE__);
			}else if([&](){ TMMi DANO = Tab("SELECT * FROM `mp_bmf_dano` -- WHERE `clump_id`='"+ bmf::clump_id+ "'"); BMF_DANO_EX[""] = DANO; return false; }()){ mpre("ОШИБКА получения дано скопления", __LINE__);
			}else if([&](){ TMMi DANO_VALUES = Tab("SELECT * FROM `mp_bmf_dano_values` -- WHERE `clump_id`='"+ bmf::clump_id+ "'"); BMF_DANO_VALUES_EX[""] = DANO_VALUES; return false; }()){ mpre("ОШИБКА получения дано значений", __LINE__);
			}else if([&](){ TMMi DANO_TITLES = Tab("SELECT * FROM `mp_bmf_dano_titles` -- WHERE `clump_id`='"+ bmf::clump_id+ "'"); BMF_DANO_TITLES_EX[""] = DANO_TITLES; return false; }()){ mpre("ОШИБКА получения дано справочника", __LINE__);
			}else if([&](){ TMMi ITOG_VALUES = Tab("SELECT * FROM `mp_bmf_itog_values` -- WHERE `clump_id`='"+ bmf::clump_id+ "'"); BMF_ITOG_VALUES_EX[""] = ITOG_VALUES; return false; }()){ mpre("ОШИБКА получения итогов значений", __LINE__);
			}else if([&](){ TMMi ITOG_TITLES = Tab("SELECT * FROM `mp_bmf_itog_titles` -- WHERE `clump_id`='"+ bmf::clump_id+ "'"); BMF_ITOG_TITLES_EX[""] = ITOG_TITLES; return false; }()){ mpre("ОШИБКА получения итогов справочника", __LINE__);
			}else if([&](){ TMMi ITOG = Tab("SELECT * FROM `mp_bmf_itog` -- WHERE `clump_id`='"+ bmf::clump_id+ "'"); BMF_ITOG_EX[""] = ITOG; return false; }()){ mpre("ОШИБКА получения итогов скопления", __LINE__);
			}else if([&](){ // Получение списка итогов
					if(ARGV.end() == ARGV.find("-i")){ //mpre("Режим расчета не указан итог", __LINE__);
					}else if(string::npos != ARGV.at("-i").find_first_not_of("0123456789")){ mpre("ОШИБКА формат исходного значения не число "+ ARGV.at("-i"), __LINE__);
					}else if("0" == ARGV.at("-i")){ mpre("Режим расчета указан итог 0", __LINE__);
					}else if(BMF_ITOG_EX.at("").end() != BMF_ITOG_EX.at("").find(atoi(ARGV.at("-i").c_str()))){ mpre("Указан итог для расчета "+ ARGV.at("-i"), __LINE__);
					}else{ mpre(BMF_ITOG_EX.at(""), "Список итогов", __LINE__); mpre("ОШИБКА указанный итог не найден "+ ARGV.at("-i"), __LINE__); exit(0);
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
				}else if(_CALC.at(calc["calc_pos_id"]).insert(make_pair(atoi(calc["calc_val_id"].c_str()), calc)); (_CALC.at(calc["calc_pos_id"]).end() == _CALC.at(calc["calc_pos_id"]).find(atoi(calc["calc_val_id"].c_str())))){ mpre("ОШИБКА добавления второго уровня", __LINE__);
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
				}else if([&](){ _VAL.at(val["v1"]).insert(make_pair(atoi(val["v0"].c_str()), val)); return (_VAL.at(val["v1"]).end() == _VAL.at(val["v1"]).find(atoi(val["v0"].c_str()))); }()){ mpre("ОШИБКА добавления второго уровня", __LINE__);
				}else{ //mpre("Добавление нового знака списку быстрого доступа", __LINE__);
				}
			} return _VAL.empty(); }()){ mpre("ОШИБКА получения массива быстрых значений", __LINE__);
		}else{ //mpre("Списки быстрого доступа", __LINE__);
		} return false; }()){ mpre("ОШИБКА составления списка быстрого доступа", __LINE__);
	}else if([&](){ // Установка функций
		/*if(Timer = ([&](string name, string title, int line){ // Получаем число возвращаем двойное числоa
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
			} return t; }); false){ mpre("ОШИБКА установки функции засечения времени", __LINE__);
		}else*/ if(Tree = ([&](TMs bmf_index, TM3i& _BMF_DANO_EX){ // Отображение дерева
			if(string after_char = "  "; (0 >= after_char.length())){ mpre("ОШИБКА установки префикса отображения", __LINE__);
			}else if(TMMi STAIRS = [&](TMMi STAIRS = {}){ // Заполнение буфера
				TMs stairs, calc_pos, dano; string after;
				if(bmf_index.end() == bmf_index.find("id")){ mpre("ОШИБКА у морфа не указан идентификатор", __LINE__);
				}else if([&](){ stairs["index_id"] = bmf_index.at("id"); return stairs.empty(); }()){ mpre("ОШИБКА Установка идентификатора морфа", __LINE__);
				}else if([&](){ STAIRS.insert(make_pair(STAIRS.size(), stairs)); return STAIRS.empty(); }()){ mpre("ОШИБКА установки первой ступени", __LINE__);
				}else if(string::npos != bmf_index.at("depth").find_last_not_of("0123456789")){ mpre("ОШИБКА формат глучины", __LINE__);
				}else if([&](){ int depth = atoi(bmf_index.at("depth").c_str()); while(depth--){ after += after_char; }; return false; }()){ mpre("ОШИБКА получения строки отступа", __LINE__);
				}else if([&](){ calc_pos = erb(BMF_CALC_POS, {{"id", bmf_index.at("calc_pos_id")}}); return calc_pos.empty(); }()){ mpre("ОШИБКА позиция родительского морфа не найдена", __LINE__);
				}else if([&](){ dano = erb(_BMF_DANO_EX, {{"id", bmf_index.at("dano_id")}}); return dano.empty(); }()){ mpre("ОШИБКА получения исходного значения", __LINE__);
				}else if([&](){ mpre(after+ "bmf_index["+ bmf_index["id"]+ "] "+ bmf_index.at("calc_pos_id")+ " dano["+ dano.at("id")+ "]="+ dano.at("val"), __LINE__); return false; }()){ mpre("ОШИБКА отображения родителя", __LINE__);
				}else{ //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); //mpre(stairs, __LINE__, "Устанавливаем начало лестницы", __LINE__);
				} return STAIRS; }(); false){ mpre("ОШИБКА добавления элемента в буфер", __LINE__);
			}else if([&](int rep = 0){
				do{ // Перебор ступеней лестницы
					if(STAIRS.empty()){ mpre("ОШИБКА лестница пуста", __LINE__);
					}else if(TMMi::reverse_iterator stairs_itr = STAIRS.rbegin(); (STAIRS.rend() == stairs_itr)){ mpre("ОШИБКА выборки итератора", __LINE__);
					}else if(TMs stairs = stairs_itr->second; stairs.empty()){ mpre("ОШИБКА выборки первой ступени", __LINE__);
					}else if(stairs.end() == stairs.find("index_id")){ mpre("ОШИБКА поле со ссылкой на морф у ступени не найдено", __LINE__);
					}else if(TMs index = erb(BMF_INDEX_EX, {{"id", stairs.at("index_id")}}); index.empty()){ mpre("ОШИБКА выборки морфа", __LINE__);
					}else if(index.end() == index.find("calc_pos_id")){ mpre("ОШИБКА поле позиции у морфа не установлено", __LINE__);
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
						if(string after = after_char; false){ mpre("Сохранение значения отступа", __LINE__);
						}else if(index_next.empty()){ //mpre("Пустой морф", __LINE__);
						}else if(index.end() == index.find("depth")){ mpre("ОШИБКА поле глубины у морфа не установлено", __LINE__);
						}else if(string::npos != index.at("depth").find_last_not_of("0123456789")){ mpre("ОШИБКА формат глучины", __LINE__);
						}else if([&](){ int depth = atoi(index.at("depth").c_str()); while(depth--){ after += after_char; }; return (0 >= after.length()); }()){ mpre("ОШИБКА получения строки отступа", __LINE__);
						}else if(index_next.end() == index_next.find("calc_pos_id")){ mpre("ОШИБКА следующий морф не содержит поля позиции", __LINE__);
						//}else if([&](){ calc_pos_next = erb(BMF_CALC_POS, {{"id", index_next.at("calc_pos_id")}}); return calc_pos_next.empty(); }()){ mpre(index_next, "Нижестоящий", __LINE__); mpre("ОШИБКА получения позиции нижестоящего", __LINE__);
						}else if(TMs dano_next = erb(_BMF_DANO_EX, {{"id", index_next.at("dano_id")}}); dano_next.empty()){ mpre("ОШИБКА получения исходника нижестоящего морфа", __LINE__);
						}else if(mpre(after+ "index["+ index_field+ "]="+ index_next.at("id")+ " "+ index_next.at("calc_pos_id")+ " dano["+ dano_next.at("id")+ "]="+ dano_next.at("val"), __LINE__); false){ mpre("ОШИБКА отображения", __LINE__);
						}else{
						} return false; }()){ mpre("Отображение морфа", __LINE__);
					}else if([&](){ // Добавление супени
						TMs stairs_next;
						if(index_next.empty()){ //mpre("Не добавляем пустую", __LINE__);
						}else if(index_next.end() == index_next.find("id")){ mpre("ОШИБКА идентификатор у морфа не задан", __LINE__);
						}else if(stairs_next["index_id"] = index_next.at("id"); stairs_next.empty()){ mpre("ОШИБКА создания ступени", __LINE__);
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
			}else if([&](){ for(auto dano_itr:_BMF_DANO_EX.at("")){ // История исходников
				if(TMs dano = dano_itr.second; dano.empty()){ mpre("ОШИБКА выборки исходника", __LINE__);
				}else{ mpre("Исходник "+ dano["id"]+ " "+ dano["values"], __LINE__);
				} } return false; }()){ mpre("История исходников", __LINE__);
			}else{
			} return bmf_index; }); false){ mpre("ОШИБКА функции отображения дерева", __LINE__);
		}else if(Id = ([&](TMMi& ROWS, int id = 0){ // Генерация локального идентификатора
			if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
			}else if(int id_max = (ROWS.empty() ? 0 : ROWS.rbegin()->first); false){ exit(mpre("ОШИБКА получения идентификатора последней записи: "+ to_string(id_max), __LINE__));
			}else if(int id_min = (ROWS.empty() ? 0 : ROWS.begin()->first); false){ exit(mpre("ОШИБКА получения идентификатора первой записи", __LINE__));
			}else if(int id_next = max(abs(id_min), abs(id_max))+1; (0 >= id_next)){ mpre("ОШИБКА получения максимального значения", __LINE__);
			}else if([&](){ id = (id_next)*-1; return (0 <= id); }()){ mpre("ОШИБКА устанвоки следующего id", __LINE__);
			}else if(ROWS.end() != ROWS.find(id)){ mpre("ОШИБКА Дублирование идентификаторов ["+ to_string(id_min)+ ":"+ to_string(id_max)+ "] "+ to_string(id), __LINE__);// exit(mpre(BMF_INDEX, __LINE__));
			}else{// mpre("Максимальный id_min: "+ to_string(id_min)+ " id_max: " + to_string(id_max) + " id_next: "+ to_string(id_next)+ " id:"+ id, __LINE__);
			} return to_string(id); }); false){ mpre("ОШИБКА создания функции локального идентификатора", __LINE__);
		}else if(Dec2bin = ([&Dec2bin](double decimal, string binary = ""){ // Получаем число возвращаем двойное число
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
		}else if(Bin2dec = ([](string binary, double decimal = 0){ // Получаем число возвращаем двойное число
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
				} return bin; }(); false){ mpre("ОШИБКА получения целой части числа", __LINE__);
			}else if(unsigned long long dec = bin2dec(bin); (0 > dec)){ mpre("ОШИБКА перевода в двоичную форму целой части числа", __LINE__);
			}else if(std::reverse(_bin.begin(), _bin.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
			}else if(_dec = to_string(bin2dec(_bin)); (0 > _dec.length())){ mpre("ОШИБКА перевода в двоичную форму дробной части числа", __LINE__);
			}else if(std::reverse(_dec.begin(), _dec.end()); false){ mpre("ОШИБКА переворачивания дробной части", __LINE__);
			}else if(decimal = stod(to_string(dec)+ "."+_dec); false){ mpre("ОШИБКА получения итогового двоичного числа", __LINE__);
			}else if(decimal *= ("-" == binary.substr(0, 1) ? -1 : 1); false){ mpre("ОШИБКА установки отрицательного значения", __LINE__);
			}else{ //mpre("Результат расчетов "+ binary+ " "+ bin+ "("+ to_string(dec)+ ")."+ _bin+ "("+ _dec+ ") >> "+ to_string(decimal), __LINE__);
			} return decimal; }); false){ mpre("ОШИБКА Функция перевода из двоичной в десятичную систему", __LINE__);
		}else if(Values = ([&](TMs value, string alias, TM3i &BMF_VALUES){ // Проверка наличия значений в БД и установка новых
			for(auto &value_itr:value){
				if(string _values = value_itr.first; (0 >= _values.length())){ mpre("ОШИБКА получения имени значения", __LINE__);
				}else if(string _val = value_itr.second; false){ mpre("ОШИБКА получения значения из данных", __LINE__);
				}else if([&](TMs values = {}){ // Добавление значения
					if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
					}else if(values = erb(BMF_VALUES, {{"name", _values}}); !values.empty()){ //mpre("Значение уже создано `"+ _values+ "`", __LINE__);
					}else if(values = {{"id", Id(BMF_VALUES.at(""))}, {"name", _values}, {"clump_id", bmf::clump_id}, {"value", _val}}; values.empty()){ mpre("ОШИБКА фонмирования нового значения", __LINE__);
					}else if(erb_insert(BMF_VALUES, values["id"], values); BMF_VALUES.empty()){ mpre("ОШИБКА добавления нового значения в справочник", __LINE__);
					}else{ //mpre("ОШИБКА Добавляем новое значение `"+ _values+ "` ("+ alias+ ")", __LINE__); //mpre("Добавление значения", __LINE__);
					} return values.empty(); }()){ mpre("ОШИБКА добавления значения", __LINE__);
				}else{
				}
			}; return false; }); false){ mpre("ОШИБКА установки функции установки значений", __LINE__);
		}else if(History = ([&](TMs index, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX, string values = ""){ //mpre("Получение истории морфа", __LINE__);
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
			}else if(string values_1 = (index_1.empty() ? dano.at("values") : History(index_1, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_1.length())){ mpre("ОШИБКА получения истории старшего морфа", __LINE__);
			}else if(string values_0 = (index_0.empty() ? dano.at("values") : History(index_0, _BMF_DANO_EX, _BMF_ITOG_EX)); (0 >= values_0.length())){ mpre("ОШИБКА получения истории младшего морфа", __LINE__);
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
			} return values; }); false){ mpre("ОШИБКА создания функции расчета истории", __LINE__);
		}else if(Choice = ([&](TMs index, TMs stairs, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX, TMs _dano = {}){ //mpre("Выбор итога для расширения", __LINE__);
			if(TMMi _DANO = _BMF_DANO_EX.at(""); _DANO.empty()){ mpre("ОШИБКА сохранения списка исходников", __LINE__);
			}else if(string list = stairs["list"]; false){ mpre("История не установлена", __LINE__);
			}else if(TMs index = [&](TMs index = {}){ // Получение текущего морфа
				if(stairs.end() == stairs.find("id")){ mpre("ОШИБКА идентификатор морфа в ступени не установлен", __LINE__);
				}else if(int index_id = atoi(stairs.at("id").c_str()); (0 == index_id)){ mpre(stairs, "Ступень", __LINE__); mpre("ОШИБКА выборки идентификатор морфа", __LINE__);
				}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(index_id)){ mpre("ОШИБКА морф не найден в справочнике", __LINE__);
				}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
				}else if(index = BMF_INDEX_EX.at("").at(index_id); index.empty()){ mpre("ОШИБКА выборки идентификатора из справочника", __LINE__);
				}else if(list += "," +index.at("dano_id") +";"; list.empty()){ mpre("ОШИБКА добавления исходника морфа в список", __LINE__);
				}else{ //mpre(index, "Идентификатор list"+ list, __LINE__);
				} return index; }(); index.empty()){ mpre("ОШИБКА получения морфа", __LINE__);
			}else if([&](int pos = 0, int loop = 6.5e4){ while((string::npos != (pos = list.find_last_of(","))) && (loop-- > 0) && (1 < _DANO.size())){ // Исключение повторяющихся исходников
					if(false){ mpre("Пропуск исключений повторений", __LINE__);
					//}else if(mpre("Позиция запятой loop=" +to_string(loop) +" pos=" + to_string(pos) +" list=" +list, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else if(int dano_id =atoi(list.substr(pos +1, list.length()-1).c_str()); (0 == dano_id)){ mpre("ОШИБКА получения идентификатор исходника " +list, __LINE__);
					}else if(list =list.substr(0, pos); false){ mpre("ОШИБКА сокращение длинны списка", __LINE__);
					}else if(_DANO.end() == _DANO.find(dano_id)){ mpre("ОШИБКА Исходник не найден в списке " +list +" "+ to_string(dano_id), __LINE__);
					}else if(_DANO.erase(dano_id); _DANO.empty()){ mpre("ОШИБКА не удаляем последнее значение", __LINE__);
					}else{ //mpre("Идетификатор исходника "+ to_string(dano_id) +" list=" +list +" следующая=" +to_string(list.find_last_of(",")) + " "+ (string::npos != list.find_last_of(",") ? "1" : "0") +" size=" +to_string(_DANO.size()), __LINE__);
					}
				} return _DANO.empty(); }()){ mpre("ОШИБКА список не должен оставаться пустым", __LINE__);
			}else if([&](int pos = 1, bool loop = false, bool erase = false){ do{
					for(auto dano_itr:_BMF_DANO_EX.at("")){ // Перебор исходных значений
						if(1 >= _DANO.size()){ //mpre("Не удаляем последнее значение", __LINE__);
						}else if(TMs dano = dano_itr.second; dano.empty()){ mpre("Уже удаленный исходник " +to_string(loop ? 1 : 0), __LINE__);
						}else if(_DANO.end() == _DANO.find(dano_itr.first)){ //mpre("Уже удален " +dano.at("id"), __LINE__);
						}else if(dano.end() == dano.find("values")){ mpre("ОШИБКА в исходнике не найдена история", __LINE__);
						}else if(string values = dano.at("values"); (0 >= values.length())){ mpre("ОШИБКА выборки истории исходника", __LINE__);
						}else if(int len = values.length(); (0 >= len)){ mpre("Пропуск расчетов", __LINE__);
						}else if(bool erase = [&](){ // Удаление неподходящего по длинне исходника
							if(pos < len){ return false; //mpre("Позиция символа в пределах истории pos="+ to_string(pos) +" len="+ to_string(len), __LINE__);
							}else if(_DANO.erase(dano_itr.first); false){ mpre("ОШИБКА удаления короткого значения истории", __LINE__);
							}else{ //mpre("Удаляем исходник с короткой историей " +dano.at("id"), __LINE__);
							} return true; }()){ //mpre("Удаления исходника по длинне "+ dano.at("id"), __LINE__);
						}else if(1 >= _DANO.size()){ //mpre("Не удаляем последнее значение", __LINE__);
						}else if([&](){ // Удаление исходника
							if(string char_last = values.substr(len -1, 1); (1 != char_last.length())){ mpre("ОШИБКА получения последнего символа", __LINE__);
							}else if(string char_cur = values.substr(len -pos -1, 1); (1 != char_cur.length())){ mpre("ОШИБКА получения текущего символа", __LINE__);
							}else if(char_last != char_cur){ //mpre("Подходящее значение", __LINE__);
							}else if(_DANO.erase(dano_itr.first); false){ mpre("ОШИБКА удаления короткого значения истории", __LINE__);
							}else{ //mpre("Удаление исходника " +dano_itr.first, __LINE__);
							} return false; }()){ mpre("ОШИБКА удаления по значению", __LINE__);
						}else{ loop = true;
						}
					}
				}while((pos++ < bmf::values_length) && !(loop = !loop)); return false; }()){ mpre("ОШИБКА перебора значений", __LINE__);
			}else if(_dano = [&](){ // Оставшееся значение
				if(1 != _DANO.size()){ mpre("ОШИБКА ожидается одно значение в остатке подходящих значений", __LINE__);
				}else if(auto dano_itr = _DANO.begin(); false){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
				}else if(_dano = dano_itr->second; _dano.empty()){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
				}else{ //mpre(_dano, "Оставшееся значение", __LINE__);
				} return _dano; }(); _dano.empty()){ mpre("ОШИБКА выборки оставшегося значения", __LINE__);
			}else{ //mpre(stairs, "Ступень", __LINE__); mpre(_BMF_DANO_EX.at(""), "Список", __LINE__); mpre(_DANO, "Выбор", __LINE__); mpre(_dano, "Результат", __LINE__); //mpre("ОШИБКА функции выбора исходного значения", __LINE__);
			} return _dano; }); false){ mpre("ОШИБКА установки функции выбора Исходного значения", __LINE__);
		}else{ return false; //mpre("Функции успешно установлены", __LINE__);
		} return true; }()){ mpre("ОШИБКА установки списка функций", __LINE__);
	}else if([&](){ // Основные функции
		if(false){ mpre("ОШИБКА уведомления", __LINE__);
		}else if(Vals = ([&](TMs VALUE, string alias, int key, TM3i& BMF_VALS, TM3i& BMF_VALUES, TM3i& BMF_TITLES){ // Установка входных значений
				if(false){ mpre("Пропуск установки значений", __LINE__);
				}else if(static TM3i DANO; false){ mpre("ОШИБКА создания статического списка кеша", __LINE__);
				}else if([&](){ // Выборка данных из справочника
					if(DANO.empty()){ //mpre("Кеш знаков пуст", __LINE__);
					}else if(DANO.end() == DANO.find(alias)){ //mpre("Структура алиаса не задана", __LINE__);
					}else if(DANO.at(alias).end() == DANO.at(alias).find(key)){ //mpre("Структура знаков не задана", __LINE__);
					}else if(BMF_VALS.at("").size() != DANO.at(alias).at(key).size()){ //mpre("Запуск переиндексации", __LINE__);
					}else if([&](){ for(auto& vals_itr:BMF_VALS.at("")){ // Восстановление знаков
							if(TMs vals = vals_itr.second; vals.empty()){ mpre("ОШИБКА получения знака", __LINE__);
							}else if(DANO.at(alias).at(key).end() == DANO.at(alias).at(key).find(vals.at("id"))){ mpre("Сохраненного знака не найдено", __LINE__);
							}else if(vals["val"] = DANO.at(alias).at(key).at(vals.at("id")); (1 != vals.at("val").length())){ mpre("ОШИБКА получения значения знака", __LINE__);
							}else if(string _values = vals.at("values")+ vals.at("val"); (0 >= _values.length())){ mpre("ОШИБКА получения истории изменений знака", __LINE__);
							}else if(vals["values"] = (bmf::values_length < _values.length() ? _values.substr(_values.length()-bmf::values_length, _values.length()) : _values); vals.empty()){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
							}else if(vals_itr.second = vals; vals.empty()){ mpre("ОШИБКА сохранения знака в справочнике", __LINE__);
							}else if([&](){ // Выборка значения
								if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
								}else if(string field = alias+ "_values_id"; false){ mpre("ОШИБКА установки значения поля", __LINE__);
								}else if(vals.end() == vals.find(field)){ mpre("ОШИБКА поле в знаке не найдено "+ field, __LINE__);
								}else if(BMF_VALUES.at("").end() == BMF_VALUES.at("").find(atoi(vals.at(field).c_str()))){ mpre("ОШИБКА значение в справочнике не найдено", __LINE__);
								}else if(TMs values = BMF_VALUES.at("").at(atoi(vals.at(field).c_str())); values.empty()){ mpre(BMF_VALUES.at(""), __LINE__, "Значения"); mpre(vals, __LINE__, "Знак"); mpre("ОШИБКА выборки значения из справочника "+ field, __LINE__);
								}else{ return false; //mpre(values, __LINE__, "Значение");
								} return true; }()){ mpre("ОШИБКА выборки значения", __LINE__);
							}else{ //mpre("Кеширование "+ alias+ "["+ vals.at("id")+ "] "+ values.at("name")+ "("+ vals.at("name")+ ") key="+ to_string(key)+ " val="+ vals.at("val"), __LINE__);
							}
						} return false; }()){ mpre("ОШИБКА загрузки значений знаков", __LINE__);
					}else{ return true;
					} return false; }()){ //mpre("Кеширование "+ to_string(key), __LINE__);
				}else if([&](){ // Расчет знаков
					for(auto &val_itr:VALUE){ //mpre("VALUE ", __LINE__);
						TMMi VALS;
						if(string value = val_itr.second; false){ mpre("ОШИБКА получения значения итератора", __LINE__);
						}else if(TMs values = [&](TMs values = {}){ // Выборка значения и если не найден добавляем
							if(string name = val_itr.first; (0 >= name.length())){ mpre("ОШИБКА значение имени не установлено", __LINE__);
							}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
							}else if(values = erb(BMF_VALUES, {{"name", name}}); !values.empty()){ //mpre("Значение уже создано `"+ val_itr.first+ "`", __LINE__);
							}else{ //mpre(BMF_VALUES, __LINE__, "Список"); mpre("Значение не найдено `"+ val_itr.first+ "` "+ val_itr.second, __LINE__);
							} return values; }(); values.empty()){ mpre("ОШИБКА Значение `"+ val_itr.first+ "` не найдено в списке "+ alias, __LINE__);
						}else if(TM3i* BMF; false){ mpre("ОШИБКА установки ссылки на список исходных знаков", __LINE__);
						}else if(TMs bmf_titles = [&](TMs bmf_titles = {}){ // Получение заголовка значения если нет то его создание
							if(regex b("^[0-9|.|-]+$"); regex_match(value, b)){ //mpre("Формат значения `"+ value_itr.first+ "` верен `"+ value_itr.second+ "`", __LINE__);
							}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
							}else if(bmf_titles = erb(BMF_TITLES, {{alias+ "_values_id", values["id"]}, {"name", value}}); !bmf_titles.empty()){ //mpre("Заголовок найден в справочнике "+ value, __LINE__);
							}else if(TMMi TITLES = rb(BMF_TITLES, {{alias+ "_values_id", values["id"]}}); false){ mpre("ОШИБКА получения полного списка", __LINE__);
							}else if(int nn = [&](int nn = 0){ //mpre(bmf_titles, __LINE__, "Заголовок"); // Получение номера следующего заголовка
								if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установка блокировки", __LINE__);
								}else if(TMMi NN; false){ mpre("ОШИБКА создания первоначальной структуры", __LINE__);
								}else if([&](){ for(auto &titles_itr:TITLES){ //for_each(TITLES.begin(), TITLES.end(), [&](auto &titles_itr){
									if(TMs titles = titles_itr.second; titles.empty()){ mpre("ОШИБКА получения заголовка из списка", __LINE__);
									}else if(titles.end() == titles.find("value")){ mpre("ОШИБКА поле value не установлено в заголовоке", __LINE__);
									}else if(string _nn = titles.at("value"); (0 >= _nn.length())){ mpre("ОШИБКА значение номера заголовока не задано", __LINE__);
									}else if(NN.insert(make_pair(atoi(_nn.c_str()), titles)); NN.empty()){ mpre("ОШИБКА добавления значения в список", __LINE__);
									}else{ //mpre(NN, __LINE__, "NN");
									} }; return false; }()){ mpre("ОШИБКА получени списка по номерам", __LINE__);
								}else if(nn = (NN.empty() ? -1 : NN.begin()->first-1); (0 <= nn)){ mpre("ОШИБКА нахождения следующего номера "+ to_string(nn), __LINE__);
								}else if(bmf_titles = {{"id", Id(BMF_TITLES.at(""))}, {"clump_id", bmf::clump_id}, {alias+ "_values_id", values["id"]}, {"value", to_string(nn)}, {"name", value}}; bmf_titles.empty()){ mpre("ОШИБКА получения нового заголовка", __LINE__);
								}else if(erb_insert(BMF_TITLES, bmf_titles["id"], bmf_titles); BMF_TITLES.empty()){ mpre("ОШИБКА добавления заголовка в справочник", __LINE__);
								}else{ //mpre(bmf_titles, __LINE__, "Значение следующего заголовка `"+ value+ "` "+ to_string(nn));
								} return nn; }(); (0 < nn)){ mpre("ОШИБКА определения номера заголовока `"+ value+ "`", __LINE__);
							}else{ //mpre(bmf_titles, __LINE__, "Заголовок"); mpre("ОШИБКА Установлено наличие заголовка", __LINE__);
							} return bmf_titles; }(); false){ mpre("ОШИБКА получения заголовка", __LINE__);
						}else if([&](){ // Расчитываем и устанавливаем значение
							if(value = (bmf_titles.empty() ? value : bmf_titles.at("value")); (0 >= value.length())){ mpre("ОШИБКА Значение не определено", __LINE__);
							}else if(values["value"] = value; values.empty()){ mpre("ОШИБКА установки результата значению", __LINE__);
							}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
							}else if(BMF_VALUES.at("").at(atoi(values["id"].c_str())) = values; (BMF_VALUES.find("values") != BMF_VALUES.end())){ mpre(BMF_VALUES, __LINE__, "Справочник"); mpre("ОШИБКА сохранения результата в справочнике", __LINE__);
							}else{ //mpre(values, __LINE__, "Устанавливаем значние"); //mpre("Установка результата значению `"+ values["name"]+ "` "+ value, __LINE__);
							} return values.empty(); }()){ mpre("ОШИБКА установки результата значению", __LINE__);
						}else if(string _value = Dec2bin(stod(value)); (0 >= _value.length())){ mpre("ОШИБКА получения строки сиволов двоичного розультата "+ value, __LINE__);
						}else if(string _value_ = ("-" == _value.substr(0, 1) ? _value : "+"+ _value); (0 >= _value.length())){ mpre("ОШИБКА получения значения без знака", __LINE__);
						}else if(int pos = _value_.rfind("."); false){ mpre("ОШИБКА получения позиции точки", __LINE__);
						}else if([&](){ // Глобавльный справочник
							if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
							}else if("dano" == alias){ BMF = &BMF_DANO_EX;
							}else if("itog" == alias){ BMF = &BMF_ITOG_EX;
							}else{ mpre("ОШИБКА выборки глобального справочника", __LINE__);
							} return (*BMF).empty(); }()){ mpre("ОШИБКА выборки глобального справочника", __LINE__);
						//}else if(TMs VALS; false){ mpre("Создание промеждуточных данных для истории", __LINE__);
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
									}else if(TMs vals = [&](TMs vals = {}){ // Проверка и выборка глобального знака
										if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
										}else if(vals = erb(*BMF, {{"clump_id", bmf::clump_id}, {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}}); !vals.empty()){ //mpre(vals, __LINE__, "Знак `"+ values["name"]+ "` уже добавлен "+ to_string(nn));
										}else if(("-" != _value.substr(0, 1)) && (0 == nn)){ //mpre("Не создаем положительный знак отрицания", __LINE__);
										//}else if(ARGV.end() != ARGV.find("-t")){ mpre(values, __LINE__, "Значение"); mpre("ОШИБКА в многопоточном режиме недопустимо создание новых знаков "+ alias+ "_values_id="+ values.at("id")+ " name="+ to_string(nn), __LINE__);
										}else if(vals = {{"id", Id((*BMF).at(""))}, {"clump_id", bmf::clump_id}, {alias+ "_values_id", values.at("id")}, {"name", to_string(nn)}, {"val", ""}, {"values", ""}}; vals.empty()){ mpre("ОШИБКА создания нового знака", __LINE__);
										}else if(string itog = [&](string itog = ""){ // Проверка вставки в многопоточном режиме
											if(ARGV.end() == ARGV.find("-i")){ //mpre("В многооконном режиме вставка знаков запрещена", __LINE__);
											}else if(string _itog = ARGV.at("-i"); "0" == _itog){ //mpre("ОШИБКА пустой итог", __LINE__);
											}else{ itog = _itog;
											} return itog; }(); !itog.empty()){ mpre("ОШИБКА в многопоточном режиме вставка знаков запрещена", __LINE__);
										}else if(erb_insert(*BMF, vals.at("id"), vals); vals.empty()){ mpre("ОШИБКА обновления справочника знаков", __LINE__);
										}else{ //mpre("ОШИБКА добавление глобального знака", __LINE__); //mpre(*BMF, __LINE__, "Список знаков"); mpre("Добавление глобального знака `"+ values["name"]+ "` ["+ vals.at("id")+ "] "+ to_string(nn), __LINE__); mpre(vals, __LINE__, "Знак"); //mpre("ОШИБКА добавления знака "+ alias+ " "+ vals.at("id"), __LINE__);
										} return vals; }(); false){ mpre("ОШИБКА Добавления нового знака", __LINE__);
									}else if(TMs _vals = [&](TMs _vals = {}){ // Синхронизация глобального значения с локальным
										if(vals.empty()){ //mpre("Не найден знак", __LINE__);
										}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА устанвоки блокировки", __LINE__);
										}else if(_vals = (BMF_VALS.at("").end() == BMF_VALS.at("").find(atoi(vals.at("id").c_str())) ? TMs({}) : BMF_VALS.at("").at(atoi(vals.at("id").c_str()))); !_vals.empty()){ //mpre("Знак есть в локальной базе", __LINE__);
										}else if(_vals = vals; _vals.empty()){ mpre("ОШИБКА копирования глобального значения", __LINE__); //mpre(_vals, __LINE__, "Знак `"+ values["name"]+ "` уже добавлен "+ to_string(nn));
										//}else if(ARGV.end() != ARGV.find("-i")){ mpre(_vals, "Знак", __LINE__); mpre("ОШИБКА в многооконном режиме вставка знаков запрещена", __LINE__);
										}else if(string itog = [&](string itog = ""){ // Проверка вставки в многопоточном режиме
											if(ARGV.end() == ARGV.find("-i")){ //mpre("В многооконном режиме вставка знаков запрещена", __LINE__);
											}else if(string _itog = ARGV.at("-i"); "0" == _itog){ //mpre("ОШИБКА пустой итог", __LINE__);
											}else{ itog = _itog;
											} return itog; }(); !itog.empty()){ mpre("ОШИБКА в многопоточном режиме вставка знаков запрещена", __LINE__);
										}else if(erb_insert(BMF_VALS, _vals.at("id"), _vals); _vals.empty()){ mpre("ОШИБКА обновления справочника знаков", __LINE__);
										}else{ //mpre("Добавление глобального знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); mpre(_vals, __LINE__, "Знак"); mpre("ОШИБКА добавления знака "+ alias+ " "+ _vals.at("id"), __LINE__);
										} return _vals; }(); false){ mpre("ОШИБКА Добавления локального знака", __LINE__);
									}else if([&](){ // Установка свойств знака
										if(_vals.empty()){ //mpre("ОШИБКА знак не найден", __LINE__);
										}else if(_vals.at("val") = val; (1 != val.length())){ mpre("ОШИБКА установки знака знака", __LINE__);
										}else if(string _values = _vals.at("values")+ val; (0 >= _values.length())){ mpre("ОШИБКА инкремента знака к истории", __LINE__);
										}else if(_vals.at("values") = (bmf::values_length < _values.length() ? _values.substr(_values.length()-bmf::values_length, _values.length()) : _values); (0 >= _values.length())){ mpre("ОШИБКА приведения истории к нужной длинне", __LINE__);
										}else{ //mpre("Установка свойст знака", __LINE__);
										} return false; }()){ mpre("ОШИБКА установки свойств нового знака", __LINE__);
									}else if([&](){ // Сохранение
										if(_vals.empty()){ //mpre("ОШИБКА пустое значение знака", __LINE__);
										}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
										}else if(BMF_VALS.at("").end() == BMF_VALS.at("").find(atoi(_vals["id"].c_str()))){ mpre("ОШИБКА знак не найден в списке", __LINE__);
										}else if(BMF_VALS.at("").at(atoi(_vals["id"].c_str())).at("val") = _vals.at("val"); false){ mpre("ОШИБКА установки значения создан индекс `vals`", __LINE__);
										}else if(BMF_VALS.at("").at(atoi(_vals["id"].c_str())).at("values") = _vals["values"]; false){ mpre("ОШИБКА установки значения создан индекс `values`", __LINE__);
										}else if(VALS.insert(make_pair(atoi(_vals.at("id").c_str()), _vals)); VALS.empty()){ mpre("ОШИБКА формирования списка измененных знаков", __LINE__);
										}else{ //mpre("Сохранение знака", __LINE__);
										} return false; }()){ mpre("ОШИБКА сохраенния знака", __LINE__);
									}else{ //mpre("Установка знака `"+ values["name"]+ "` "+ to_string(nn), __LINE__); //mpre(_vals, __LINE__, "Добавление нового знака "+ to_string(i));
									}
								}  return false;
							}()){ mpre("ОШИБКА добавления знаков значению", __LINE__);
						}else if([&](){ // Установка истории
							for(auto& vals_itr:rb(BMF_VALS, {{alias+ "_values_id", values["id"]}})){ // Установка истории
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
								}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
								}else if(BMF_VALS.at("").at(atoi(vals.at("id").c_str())).at("val") = vals.at("val"); vals.empty()){ mpre("ОШИБКА сохраенния расчетов", __LINE__);
								}else if(BMF_VALS.at("").at(atoi(vals.at("id").c_str())).at("values") = vals.at("values"); vals.empty()){ mpre("ОШИБКА сохраенния расчетов", __LINE__);
								}else{ //mpre("Скидывание значения "+ alias+ " _value_="+ _value_+ " "+ vals.at("id"), __LINE__);
								}
							} return false; }()){ mpre("ОШИБКА скидывания значения", __LINE__);
						}else{ //mpre(BMF_VALS.at(""), __LINE__, "Список знаков"); //mpre(BMF_VALS, __LINE__, "Знаки"); mpre("ОШИБКА Знака "+ alias, __LINE__); //mpre(values, __LINE__, "Значение"); mpre(VALS, __LINE__, "Значения");
						}
					}; return false; }()){ mpre("ОШИБКА расчета знаков", __LINE__);
				}else if([&](){ // Сохранение данных в статическом справочнике
					if(bool cache = false; !cache){ //mpre("Кеширование значений отключено " +to_string(key), __LINE__);
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
				} return false; }); false){ mpre("ОШИБКА установки значений", __LINE__);
		}else if(Learn = ([&](TMs bmf_index, string val = "", int key, string vmap, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX, TMs stair = {}){ //mpre("Обучение "+ index.at("id"), __LINE__);  //system("sleep 1"); // mpre(BMF_INDEX, __LINE__, "Список"); // if(remove){ mpre(row, __LINE__, "Ученик"); }
			if(int rep = 1e9; 0 >= rep){ mpre("ОШИБКА установки предельного размера обучения для сети", __LINE__);
			}else if(TMMi STAIRS = [&](TMMi STAIRS = {}){ // Требование к морфу
				if(bmf_index.end() == bmf_index.find("id")){ mpre("ОШИБКА идентификатор у морка не установлен", __LINE__);
				}else if(TMs stairs; false){ mpre("ОШИБКА создания первой ступени", __LINE__);
				}else if(stairs["id"] = bmf_index.at("id"); stairs.empty()){ mpre("ОШИБКА добавления номера идентификатора", __LINE__);
				}else if([&](){ // Установка требований к лестнице
					if(0 >= val.length()){ //mpre("Нет требований к лестнице", __LINE__);
					}else if(stairs["promise"] = val; (stairs.end() == stairs.find("promise"))){ mpre("ОШИБКА установки требований к лестнице", __LINE__);
					}else{ //mpre("Устанавливаем требования к лестнице " +stairs.at("promise"), __LINE__);
					} return false; }()){ mpre("ОШИБКА установки требований к лестнице", __LINE__);
				}else if(STAIRS.insert(make_pair(STAIRS.size(), stairs)); STAIRS.empty()){ mpre("ОШИБКА устанвоки первоначального значения лестнице", __LINE__);
				}else if(bmf_index.empty()){ mpre("ОШИБКА входное значение лестницы пусто", __LINE__);
				}else{ //mpre("Установка требований к лестнице promise="+ val, __LINE__);
				} return STAIRS; }(); STAIRS.empty()){ mpre("ОШИБКА установки первоначального значения лестницы", __LINE__);
			}else if(boost::dynamic_bitset<unsigned char> gmap = boost::dynamic_bitset<unsigned char>{vmap}; false){ mpre("ОШИБКА установка битовой маски ", __LINE__);
			}else if([&](){ do{ //mpre("Повтор "+ to_string(rep), __LINE__); // Ступени расчета
					if(STAIRS.empty()){ mpre("ОШИБКА лестница пуста", __LINE__);
					//}else if(mpre("Расчет начало "+ bmf_index.at("id"), __LINE__); false){ mpre("ОШИБКА уведомления о расчете", __LINE__);
					}else if(auto stairs_itr = STAIRS.rbegin(); stairs_itr == STAIRS.rend()){ mpre("ОШИБКА получения итаратора следующей ступени", __LINE__);
					}else if(TMs stairs = stairs_itr->second; stairs.empty()){ mpre(STAIRS, __LINE__, "Лестница"); mpre("ОШИБКА получения ступени", __LINE__);
					}else if(stairs.end() == stairs.find("id")){ mpre("ОШИБКА идентификатор морфа не задан", __LINE__);
					}else if(TMs index = [&](TMs index = {}){ // Морф ступени
						if(stairs.end() == stairs.find("id")){ mpre("ОШИБКА у ступени не указана ссылка на морф", __LINE__);
						}else if(string index_id = stairs.at("id"); 0 >= index_id.length()){ mpre("ОШИБКА получения идентификатора морфа ступени", __LINE__);
						}else if("" == index_id){ mpre("Пустой индекс ступени", __LINE__);
						}else if(string::npos != index_id.find_last_not_of("-0123456789")){ mpre("ОШИБКА форма идентификатора не число `" +index_id +"`", __LINE__);
						}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(atoi(index_id.c_str()))){ mpre("ОШИБКА морф не найден в списке", __LINE__);
						}else if(index = BMF_INDEX_EX.at("").at(atoi(index_id.c_str())); index.empty()){ mpre("ОШИБКА получения морфа ступени", __LINE__);
						}else{ //mpre(index, __LINE__, "Морф ступени");
						} return index; }(); index.empty()){ mpre("ОШИБКА получения морфа ступени", __LINE__);
					}else if(TMs index_1 = [&](TMs index_1 = {}){ // Получение старшего потомка
						if(index.end() == index.find("index_id")){ mpre("ОШИБКА у ступени отстствует поле связи со старшим морфом", __LINE__);
						}else if("" == index.at("index_id")){ //mpre("Не указан старший нижестоящий потомок `index_id`", __LINE__);
						}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(atoi(index.at("index_id").c_str()))){ mpre("ОШИБКА указанный старший потомок не найден в списке морфов", __LINE__);
						}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
						}else if(index_1 = BMF_INDEX_EX.at("").at(atoi(index.at("index_id").c_str())); index_1.empty()){ mpre("ОШИБКА выборки старшего потомка из списка морфов", __LINE__);
						}else{ //mpre("Старший потомок "+ index_1["id"], __LINE__);
						} return index_1; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
					}else if(TMs index_0 = [&](TMs index_0 = {}){ // Получение младшего потомка
						if(index.end() == index.find("bmf-index")){ mpre("ОШИБКА у ступени отстствует поле связи со старшим морфом", __LINE__);
						}else if("" == index.at("bmf-index")){ //mpre("Не указан младший нижестоящий потомок `bmf-index`", __LINE__);
						}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(atoi(index.at("bmf-index").c_str()))){ mpre("ОШИБКА указанный младший потомок не найден в списке морфов", __LINE__);
						}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
						}else if(index_0 = BMF_INDEX_EX.at("").at(atoi(index.at("bmf-index").c_str())); index_0.empty()){ mpre("ОШИБКА выборки младшего потомка из списка морфов", __LINE__);
						}else{ //mpre("Младший потомок "+ index_0["id"], __LINE__);
						} return index_0; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
					}else if(TMs dano = [&](TMs dano = {}){ // Получение младшего потомка
						if(index.end() == index.find("dano_id")){ mpre("ОШИБКА у ступени отстствует поле связи со старшим морфом", __LINE__);
						}else if(string dano_id = index.at("dano_id"); ("" == dano_id)){ mpre("ОШИБКА Не указано исходное значение", __LINE__);
						}else if(_BMF_DANO_EX.at("").end() == _BMF_DANO_EX.at("").find(atoi(dano_id.c_str()))){ mpre("ОШИБКА указанный младший потомок не найден в списке морфов", __LINE__);
						}else if(dano = _BMF_DANO_EX.at("").at(atoi(dano_id.c_str())); dano.empty()){ mpre("ОШИБКА выборки младшего потомка из списка морфов", __LINE__);
						}else{ //mpre(dano, "Исходное значение ", __LINE__);
						} return dano; }(); false){ mpre("ОШИБКА получения старшего потомка", __LINE__);
					}else if([&](){ // Установка статуса
						if(stairs_itr->second["status"] = stairs["status"] = (stairs.end() == stairs.find("status") ? "1" : to_string(atoi(stairs.at("status").c_str()) +1)); stairs.at("status").empty()){ mpre("ОШИБКА установки первоначального статуса", __LINE__);
						}else if(stairs.end() != stairs.find("calc_"+ stairs.at("status"))){ mpre("ОШИБКА поле расчета уже задано у ступени calc_"+ stairs.at("status"), __LINE__);
						}else{ //mpre("СТАТУС id=" +stairs.at("id") + " status=" +stairs.at("status"), __LINE__); //mpre(stairs["index_id"]+ " Статус STAIRS["+ to_string(size-1)+ "] status="+ stairs["status"]+ (stairs.end() == stairs.find("promise") ? "" : " promise="+ stairs["promise"])+ (stairs.end() == stairs.find("learn") ? "" : " learn="+ stairs["learn"]), __LINE__);
						} return (string::npos != stairs.at("status").find_last_not_of("1234567890")); }()){ mpre("ОШИБКА установки состояния", __LINE__);
					}else if(string val_1 = [&](string val_1 = ""){ // Старшее значение
						if(string learn_1 = stairs["1_learn"]; !learn_1.empty()){ val_1 = learn_1; //mpre("Значение из обучения", __LINE__);
						}else if(string cache_1 = stairs["1_cache"]; !cache_1.empty()){ val_1 = cache_1; //mpre("Значение из обучения", __LINE__);
						}else if(string new_1 = stairs["1_new"]; !new_1.empty()){ val_1 = new_1; //mpre("Значение из обучения", __LINE__);
						}else if(string calc_1 = stairs["1_calc"]; !calc_1.empty()){ val_1 = calc_1;
						}else if(string v_1 = stairs["1_v"]; !v_1.empty()){ val_1 = v_1;
						}else{ //mpre("ОШИБКА значение не найдено", __LINE__);
						} //mpre("Значение старшего морфа val_1=" +val_1, __LINE__);
						return val_1; }(); false){ mpre("ОШИБКА получения старшего значения", __LINE__);
					}else if(string val_0 = [&](string val_0 = ""){ // Старшее значение
						if(string learn_0 = stairs["0_learn"]; !learn_0.empty()){ val_0 = learn_0; //mpre("Значение из обучения", __LINE__);
						}else if(string cache_0 = stairs["0_cache"]; !cache_0.empty()){ val_0 = cache_0; //mpre("Значение из обучения", __LINE__);
						}else if(string new_0 = stairs["0_new"]; !new_0.empty()){ val_0 = new_0; //mpre("Значение из обучения", __LINE__);
						}else if(string calc_0 = stairs["0_calc"]; !calc_0.empty()){ val_0 = calc_0;
						}else if(string v_0 = stairs["0_v"]; !v_0.empty()){ val_0 = v_0;
						}else{ //mpre("ОШИБКА значение не найдено", __LINE__);
						} //mpre("Значение младшего морфа val_0=" +val_0, __LINE__);
						return val_0; }(); false){ mpre("ОШИБКА получения старшего значения", __LINE__);
					}else if([&](string val = ""){ // Расчет значения морфа
						if(index.end() == index.find("dano_id")){ mpre("ОШИБКА ссылка на исходное значение не указана", __LINE__);
						}else if(string dano_id = (index.at("dano_id")); (0 >= dano_id.length())){ mpre("ОШИБКА идентификатор исходного значения не указан", __LINE__);
						}else if(int calc_pos_id = atoi(index["calc_pos_id"].c_str()); (0 > calc_pos_id)){ mpre("ОШИБКА формата позисии морфа", __LINE__);
						}else if([&](){ // Расчет результата по одному направлению
							if(string::npos != stairs.at("status").find_last_not_of("2")){ //mpre("Только для первого статуса", __LINE__);
							}else if(val_1.length() == val_0.length()){ mpre(stairs, "Ступень", __LINE__); mpre(stairs, "Ступень", __LINE__); mpre("ОШИБКА размер значений одинаковый val_1=" +val_1 +" val_0=" +val_0, __LINE__);
							}else if(string calc = (val_1.length() ? "1" : "0"); (1 != calc.length())){ mpre("ОШИБКА получения поля значения", __LINE__);
							}else if(string _val = (val_1.length() ? val_1 : val_0); _val.empty()){ mpre("ОШИБКА получения значения", __LINE__);
							}else if(bool sign = calc == _val; false){ mpre("ОШИБКА получения совпадения сигнала", __LINE__);
							}else if(bool pos = calc_pos_id&2 == calc_pos_id&1; false){ mpre("ОШИБКА расчета совпадения позиции", __LINE__);
							}else if(sign == pos){ //mpre("Не можем вычислить значение по одной позиции calc_pos_id=" +to_string(calc_pos_id) +" calc_1=" +calc_1 +" calc_0=" +calc_0, __LINE__);
							}else if(val = (calc_pos_id&1 ? "1" : "0"); (1 != val.length())){ mpre("ОШИБКА расчета значения", __LINE__);
							}else if(stairs_itr->second["val"] = stairs["val"] = val; (1 != stairs["val"].length())){ mpre("ОШИБКА расчета значения", __LINE__);
							}else{ //mpre("Расчет результата по одному значению id=" + index.at("id")+ " calc_pos_id=" +to_string(calc_pos_id) +" calc_"+ calc+ "=" +_val, __LINE__);
							} return false; }()){ mpre("Результат расчитан по одному значению", __LINE__);
						}else if([&](){ // Расчет по двум значениям
							if(string::npos != stairs.at("status").find_last_of("2")){ //mpre("Только для первого статуса", __LINE__);
							}else if(string sign = (calc_pos_id&1 ? "1" : "0"); (1 != sign.length())){ mpre("ОШИБКА расчета общего сигнала", __LINE__);
							}else if(string shift = ("1" == sign ? "0" : "1"); (1 !=shift.length())){ mpre("ОШИБКА расчета исключительного сигнала", __LINE__);
							}else if(string vals = ((calc_pos_id&2 == calc_pos_id&1) ? "10" : "01"); (2 != vals.length())){ mpre("ОШИБКА получения исключительных значений", __LINE__);
							}else if(val = (vals == val_1 +val_0 ? shift : sign); (1 != val.length())){ mpre("ОШИБКА получения результата расчетов", __LINE__);
							}else if(stairs_itr->second["val"] = stairs["val"] = val; (1 != stairs["val"].length())){ mpre("ОШИБКА расчета значения", __LINE__);
							}else{ //mpre("Расчет результата по двум значениям id=" + index.at("id")+ " calc_pos_id=" + to_string(calc_pos_id) +" val_1=" +val_1 +" val_0=" +val_0 +" sign=" +sign +" vals=" +vals +" val=" +val, __LINE__);
							} return false; }()){ mpre("ОШИБКА расчета результата", __LINE__);
						}else if([&](){ // Сохранение результата в кеш
							if(string::npos != stairs.at("status").find_last_not_of("5")){ //mpre("Только для первого статуса", __LINE__);
							}else if(stairs.end() == stairs.find("val")){ //mpre("Значение еще не расчитано", __LINE__);
							}else if(index.end() == index.find("id")){ mpre("ОШИБКА идентификатор морфа не установлен", __LINE__);
							}else if(int index_id = atoi(index.at("id").c_str()); (0 == index_id)){ mpre("ОШИБКА получения идентификатора морфа", __LINE__);
							}else if(string map = [&](string map = ""){ // Выборка карты морфа
								if(bmf::MAP.end() == bmf::MAP.find(index.at("id"))){ //mpre("Нет битового расчета", __LINE__);
								}else if(string smap = bmf::MAP.at(index.at("id")); smap.empty()){ mpre("ОШИБКА карта бит пуста", __LINE__);
								}else if([&](){ // Удаление неактуальной карты
									if(smap.length() == vmap.length()){ //mpre("Карты совпадают по длинне", __LINE__);
									}else if(bmf::MAP.erase(index.at("id")); (bmf::MAP.end() != bmf::MAP.find(index.at("id")))){ mpre("ОШИБКА удаления карты", __LINE__);
									}else{ //mpre("Неактуальная карта удалена", __LINE__);
									} return (bmf::MAP.end() == bmf::MAP.find(index.at("id"))); }()){ //mpre("Карты не совпадают по длинне", __LINE__);
								}else if([&](){ for(int i = 0; i < vmap.length(); i++){
									if(string s = smap.substr(i, 1); (1 != s.length())){ mpre("ОШИБКА получения значения бита", __LINE__);
									}else if(string v = vmap.substr(i, 1); (1 != v.length())){ mpre("ОШИБКА получения значения результата", __LINE__);
									}else if(map += ("1" == s ? v : "-"); map.empty()){ mpre("ОШИБКА инкремента карты", __LINE__);
									}else{ //mpre("Перебор карты i="+ to_string(i) +" map=" +map, __LINE__);
									}} return false; }()){ mpre("ОШИБКА расчета карты", __LINE__);
								}else{ //mpre("ОШИБКА Карта морфа vmap=" + vmap+ " smap=" + smap+ " map=" +map, __LINE__);
								} return map; }(); map.empty()){ //mpre("Карта морфа не задана", __LINE__);
							}else if([&](){ // Создание массива морфа кеша
								if(bmf::CACHE.end() != bmf::CACHE.find(index_id)){ //mpre("Массив морфа уже создан", __LINE__);
								}else if(bmf::CACHE.insert(make_pair(index_id, TMs({}))); bmf::CACHE.empty()){ mpre("ОШИБКА создания массива морфа", __LINE__);
								}else{ //mpre("Создание массива кеша морфа "+ to_string(index_id), __LINE__);
								} return (bmf::CACHE.end() == bmf::CACHE.find(index_id)); }()){ mpre("ОШИБКА сохдания массива морфа", __LINE__);
							}else if(bmf::CACHE.at(index_id)[map] = stairs.at("val"); bmf::CACHE.empty()){ mpre("ОШИБКА установки значения", __LINE__);
							}else{ //mpre("ОШИБКА Сохранение данных значений от бит в кеш " +stairs.at("status") +" index_id=" +index.at("id") +" map=" +map, __LINE__);
							} return false; }()){ mpre("ОШИБКА сохранения результата в кеш", __LINE__);
						}else{ //mpre("ОШИБКА сохранения битового кеша", __LINE__);
						} return false; }()){ mpre("ОШИБКА расчета значения морфа", __LINE__);
					}else if(TMs transfer = [&](TMs transfer = {}){ // Расчет направления
						if(false){ mpre("Пропуск выбора направления для расчета", __LINE__);
						//}else if(string calc = "calc_"+ stairs.at("status"); (0 >= calc.length())){ mpre("ОШИБКА расчета поля статуса", __LINE__);
						}else if(TMs transfer_1 = TMs({{"next_id", (index_1.empty() ? "" : index_1.at("id"))}, {"field", "index_id"}, {"calc", "1"}, {"other_id", (index_0.empty() ? "" : index_0.at("id"))}}); transfer_1.empty()){ mpre("ОШИБКА установки левого перехода", __LINE__);
						}else if(TMs transfer_0 = TMs({{"next_id", (index_0.empty() ? "" : index_0.at("id"))}, {"field", "bmf-index"}, {"calc", "0"}, {"other_id", (index_1.empty() ? "" : index_1.at("id"))}}); transfer_0.empty()){ mpre("ОШИБКА установки правого перехода", __LINE__);
						}else if([&](){ // Выбор направления первого расчета
							if(string::npos != stairs.at("status").find_last_not_of("1")){ //mpre("Расчитываем только на 1 и 2 этапе", __LINE__);
							}else if(int depth_1 = (index_1.end() == index_1.find("depth") ? 0 : atoi(index_1.at("depth").c_str())); (0 > depth_1)){ mpre("ОШИБКА не указана глубина ветвей", __LINE__);
							}else if(int depth_0 = (index_0.end() == index_0.find("depth") ? 0 : atoi(index_0.at("depth").c_str())); (0 > depth_0)){ mpre("ОШИБКА не указана глубина ветвей", __LINE__);
							}else if(transfer = (depth_1 < depth_0 ? transfer_1 : transfer_0); transfer.empty()){ mpre("ОШИБКА установки перехода", __LINE__);
							}else{ //mpre("Выбор направления первого расчета " +index.at("id") +" > " +transfer.at("next_id") +" " +to_string(depth_1) +" <=> " +to_string(depth_0) +" " +transfer.at("field"), __LINE__);
							} return false; }()){ mpre("ОШИБКА выбора направления первого расчета", __LINE__);
						}else if([&](){ // Выбор направления второго расчета
							if(string::npos != stairs.at("status").find_last_not_of("2")){ //mpre("Расчитываем только на 1 и 2 этапе", __LINE__);
							}else if(string calc = (val_1.length() ? "1" : "0"); (1 != calc.length())){ mpre("ОШИБКА выборки расчитанного направления", __LINE__);
							}else if(transfer = ("0" == calc ? transfer_1 : transfer_0); transfer.empty()){ mpre("ОШИБКА определения второго направления", __LINE__);
							}else{ //mpre("Выбор направления второго расчета " +index.at("id") +" > " +transfer.at("next_id") +" calc_" +calc +"=" +stairs["calc_" +calc] +" " +transfer.at("field"), __LINE__);
							} return false; }()){ mpre("ОШИБКА выбора направления второго расчета", __LINE__);
						}else if([&](){ // Выбор направления обучения
							if(string::npos != stairs.at("status").find_last_not_of("34")){ //mpre("Обучаем только на 3 и 4 этапе", __LINE__);
							}else if(stairs.end() == stairs.find("promise")){ //mpre("Обещания к ступени не установлены", __LINE__);
							}else if(stairs.end() == stairs.find("val")){ mpre("ОШИБКА Расчет еще не произведен при выборе направления", __LINE__);
							}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА у исходного значения не установлен статус", __LINE__);
							}else if(string mask = "11321232"; (8 != mask.length())){ mpre("ОШИБКА формирования маски переходов", __LINE__);
							}else if(index.end() == index.find("calc_pos_id")){ mpre("ОШИБКА в морфе не указана позиция", __LINE__);
							}else if(int calc_pos_id = atoi(index.at("calc_pos_id").c_str()); (0 > calc_pos_id)){ mpre("ОШИБКА формата позисии морфа", __LINE__);
							}else if(string key = [&](string key = ""){ // Расчет позиции маски
								if(int dano_val = atoi(dano.at("val").c_str()); (0 > dano_val)){ mpre("ОШИБКА расчета ключа значения", __LINE__);
								//}else if(string calc_0 = (stairs["learn_0"].length() ? stairs["learn_0"] : stairs["calc_0"]); (1 != calc_0.length())){ mpre("ОШИБКА выборки значения старшей сети", __LINE__);
								}else if(int _val_1 = atoi(val_1.c_str()); (0 > _val_1)){ mpre("ОШИБКА расчета ключа значения", __LINE__);
								}else if(int _val_0 = atoi(val_0.c_str()); (0 > _val_0)){ mpre("ОШИБКА расчета ключа значения", __LINE__);
								}else if(int pos = (dano_val<<2) +(_val_1<<1) +_val_0; (0 > pos)){ mpre("ОШИБКА расчета позиции маски", __LINE__);
								}else if(key = mask.substr(pos, 1); (1 != key.length())){ mpre("ОШИБКА получения ключа направления", __LINE__);
								}else{ //mpre("Выбор по маске " +index.at("id") +" status=" +stairs.at("status") +" " +mask +" calc_pos_id=" +to_string(calc_pos_id) +" dano_val=" +to_string(dano_val) +" val_1=" +to_string(val_1) +" val_0=" +to_string(val_0) + " > " +key +"("+ to_string(pos) +")", __LINE__);
								} return key; }(); (1 != key.length())){ mpre("ОШИБКА расчета позиции маски", __LINE__);
							}else if(TMs _transfer = ("3" == stairs.at("status") ? transfer_1 : transfer_0); _transfer.empty()){ mpre("ОШИБКА установки направления обучения", __LINE__);
							}else if(int bit = ("3" == stairs.at("status") ? 2 : 1); (0 > bit)){ mpre("ОШИБКА выборки бита маски", __LINE__);
							}else if([&](){ // Проверка соответствия обещаний результатам
								if(stairs["val"] == stairs["promise"]){ //mpre("Результаты совпадают " +index.at("id") +" val="+ stairs.at("val"), __LINE__);
								}else if(transfer = (atoi(key.c_str())&bit ? _transfer : transfer); false){ mpre("ОШИБКА выборки направления по биту маски", __LINE__);
								}else{ //mpre(transfer, "Направление", __LINE__); mpre("Установка направления для обучения", __LINE__); //mpre("ОШИБКА установки направления обучения", __LINE__); //mpre(stairs, "Ступень calc_pos_id=" +index.at("calc_pos_id") + " dano=" + dano.at("val")+ " calc_1=" + to_string(val_1)+ " calc_0=" +to_string(val_0) +" mask=" +mask +" val=" +val +" shift=" +shift, __LINE__); mpre(transfer, "Направление обучения", __LINE__); //mpre("ОШИБКА Установка требований", __LINE__);
								} return false; }()){ mpre("ОШИБКА установки обещания", __LINE__);
							}else if([&](){ // Требования к обучению
								if(transfer.empty()){ //mpre("Направление не задано", __LINE__);
								}else if(stairs.end() == stairs.find("promise")){ mpre("Обещание у ступени не задано", __LINE__);
								}else if(1 != stairs.at("promise").length()){ mpre(stairs, "Ступень", __LINE__); mpre("ОШИБКА формат обещания указан не верно", __LINE__);
								//}else if(transfer["promise"] = ("1" == transfer["calc"] ? "0" : "1"); (1 != transfer["promise"].length())){ mpre("ОШИБКА расчета обещания", __LINE__);
								}else if(string calc = ("3" == stairs["status"] ? "1" : "0"); (1 != calc.length())){ mpre("ОШИБКА получения значения расчетов", __LINE__);
								}else if(string val = ("1" == calc ? val_1 : val_0); (1 != val.length())){ mpre(stairs, "Ступень", __LINE__); mpre("ОШИБКА получения текущего значения val_1=" +val_1 +" val_0=" +val_0 +" val=" +val, __LINE__);
								}else if(transfer["promise"] = ("1" == val ? "0" : "1"); (1 != transfer["promise"].length())){ mpre("ОШИБКА получения обновленного обещания", __LINE__);
								}else{ //mpre(stairs, "Ступень", __LINE__); mpre("Направление обучения " +index.at("id") +" > " +transfer.at("next_id") +" " +transfer.at("field") + " promise=" +transfer.at("promise"), __LINE__); mpre("Установка требований к нижестоящей ветви", __LINE__);
								} return false; }()){ mpre("ОШИБКА установки требований к обучению", __LINE__);
							}else{ //mpre(transfer, "Направление", __LINE__);
							} return false; }()){ mpre("ОШИБКА выборки направления расчета", __LINE__);
						}else{ //mpre(transfer, "Выбор направления", __LINE__);
						} return transfer; }(); false){ mpre("ОШИБКА расчета направления перехода", __LINE__);
					}else if([&](){ // Подстановка собственных значений
						if(string::npos != stairs.at("status").find_last_not_of("12")){ //mpre("Расчитываем только на 1 и 2 этапе", __LINE__);
						//}else if(mpre("Расчет морфа "+ index.at("id") +" status " +stairs.at("status") ,__LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
						}else if([&](){ // Подстановка расчета из кеша
							if(transfer.end() == transfer.find("next_id")){ mpre("ОШИБКА у направления не задан морф перехода", __LINE__);
							}else if(bmf::MAP.end() == bmf::MAP.find(transfer.at("next_id"))){ //mpre("Карта следующего морфа не найдена", __LINE__);
							}else if(string smap = bmf::MAP.at(transfer.at("next_id")); smap.empty()){ mpre("ОШИБКА выборки карты следующего морфа", __LINE__);
							}else if(smap.length() != vmap.length()){ //mpre("Значения карт не совпадают", __LINE__);
							}else if(string map = [&](string map = ""){ // Выборка или расчет карты
								if([&](){ // Поиск карты в ступени
									if(stairs.end() == stairs.find("map")){ //mpre(stairs, "Ступень без карты", __LINE__); mpre("Поле карты в ступени не найдено id=" +index.at("id") +" status" +stairs.at("status"), __LINE__);
									}else if(map = stairs.at("map"); map.empty()){ mpre("ОШИБКА карта из ступени пуста", __LINE__);
									}else{ //mpre("Карта из ступени " +map, __LINE__);
									} return !map.empty(); }()){ //mpre("Карта найдена в ступени", __LINE__);
								}else if([&](){ for(int i =0; i<vmap.length(); i++){
									if(string v = vmap.substr(i, 1); (1 != v.length())){ mpre("ОШИБКА получения знака", __LINE__);
									}else if(string s = smap.substr(i, 1); (1 != s.length())){ mpre("ОШИБКА получения бита", __LINE__);
									}else if(map += ("1" == s ? v : "-"); map.empty()){ mpre("ОШИБКА формирования карты", __LINE__);
									}else{ //mpre("Значение перебора "+ to_string(i), __LINE__);
									}} return map.empty(); }()){ mpre("ОШИБКА расчета карты значений", __LINE__);
								}else if(stairs_itr->second["map"] = stairs["map"] = map; false){ mpre("ОШИБКА установки каты в ступень", __LINE__);
								}else{ //mpre(stairs, "Ступень с картой" ,__LINE__); mpre("Расчет карты и установка в ступень id=" +index.at("id") +" " +map, __LINE__);
								} return map; }(); map.empty()){ mpre("ОШИБКА выборки сохраненной карты", __LINE__);
							}else if(int next_id = atoi(transfer.at("next_id").c_str()); (0 == next_id)){ //mpre(transfer, "Направление", __LINE__); mpre("ОШИБКА получения идентификатора морфа", __LINE__);
							}else if(bmf::CACHE.end() == bmf::CACHE.find(next_id)){ //mpre("Кеш для морфа не задан", __LINE__);
							}else if(bmf::CACHE.at(next_id).end() == bmf::CACHE.at(next_id).find(map)){ //mpre("Значения примера в кеше для морфа не найдено", __LINE__);
							}else if(string val = bmf::CACHE.at(next_id).at(map); (1 != val.length())){ mpre("ОШИБКА выборки значения из кеша", __LINE__);
							}else if(string calc = transfer.at("calc") +"_cache"; calc.empty()){ mpre("ОШИБКА расчета поля результата кеша", __LINE__);
							}else if(stairs_itr->second[calc] = stairs[calc] = val; false){ mpre("ОШИБКА установки кеша", __LINE__);
							}else{ //mpre("Подстановка значений из кеша id="+index.at("id") +" next_id=" +to_string(next_id) +" " +calc +"=" +val +" map=" +map +" vmap=" +vmap, __LINE__);
							} return false; }()){ mpre("ОШИБКА расчета кеша", __LINE__);
						}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА значение у исходника не задано", __LINE__);
						}else if(string calc = transfer.at("calc") +"_v"; calc.empty()){ mpre("ОШИБКА формирования имени поля результата", __LINE__);
						}else if(stairs_itr->second[calc] = stairs[calc] = dano.at("val"); (1 != stairs.at(calc).length())){ mpre("ОШИБКА установки исходного значения", __LINE__);
						}else{ //mpre("Устанавливаем значение из нижестоящего status=" +stairs.at("status") +" id=" +index.at("id") +" " +calc +"=" +stairs.at(calc) +" dano_id=" +dano.at("id"), __LINE__);
						} return false; }()){ mpre("ОШИБКА подстановки собственных значений", __LINE__);
					}else if(TMs _index = [&](TMs _index = {}){ // Добавление нового морфа
						if(string::npos != stairs.at("status").find_last_not_of("34")){ //mpre("Расчитываем только на 3 и 4 этапе", __LINE__);
						}else if(transfer.end() == transfer.find("next_id")){ //mpre("В направлении не задан следующий переход id="+ index.at("id"), __LINE__);
						//}else if("" != transfer.at("next_id")){ mpre("Следующий элемент для перехода не пуст", __LINE__);
						}else if(transfer.end() == transfer.find("field")){ mpre("ОШИБКА поле перехода не установлено", __LINE__);
						}else if(string field = transfer.at("field"); (0 >= field.length())){ mpre("ОШИБКА выборки поля перехода", __LINE__);
						}else if(index.end() == index.find(field)){ mpre("ОШИБКА у морфа не найдено поле перехода", __LINE__);
						}else if("" != index.at(field)){ //mpre("Поле перехода уже заполнено не обновляем", __LINE__);
						}else if(stairs.end() == stairs.find("val")){ mpre("ОШИБКА Расчет не задан. Не создаем морф", __LINE__);
						}else if(transfer.end() == transfer.find("promise")){ mpre("Не расширяем так как не задано обещание", __LINE__);
						}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА исходник не содержит значения", __LINE__);
						}else if(TMs _dano = [&](TMs _dano = {}){ // Новый морф
							if(false){ mpre("Пропуск расчета морфа", __LINE__);
							//}else if(_dano = Choice(index, stairs, _BMF_DANO_EX, _BMF_ITOG_EX); _dano.empty()){ mpre("ОШИБКА выборки из подходящих сигналов", __LINE__);
							}else if(!_dano.empty()){ //mpre("Морф уже выбран отдельно от смежного морфа", __LINE__);
							}else if("" == transfer.at("other_id")){ _dano = Choice(index, stairs, _BMF_DANO_EX, _BMF_ITOG_EX);
							}else if(TMs _index = erb(BMF_INDEX_EX, {{"id", transfer.at("other_id")}}); _index.empty()){ mpre("ОШИБКА выборки смежного морфа", __LINE__);
							}else if(_dano = erb(_BMF_DANO_EX, {{"id", _index.at("dano_id")}}); _dano.empty()){ mpre("ОШИБКА выбоки исходника смежного морфа", __LINE__);
							}else{ //mpre(_dano, "Выборка исходника смежного морфа", __LINE__);
							} return _dano; }(); _dano.empty()){ mpre("ОШИБКА выборки нового морфа", __LINE__);
						}else if(string depth = to_string(atoi(index.at("depth").c_str())+1); (0 >= depth.length())){ mpre("ОШИБКА расчета глубины нового морфа", __LINE__);
						}else if(transfer.end() == transfer.find("field")){ mpre("ОШИБКА не указано поле дальнейшего перехода", __LINE__);
						}else if(string calc_pos_id = ("index_id" == transfer.at("field") ? "0" : "3"); (1 != calc_pos_id.length())){ mpre("ОШИБКА расчета позиции", __LINE__);
						}else if(_index = {{"id", Id(BMF_INDEX_EX.at(""))}, {"clump_id", index.at("clump_id")}, {"calc_pos_id", calc_pos_id}, {"itog_values_id", index.at("itog_values_id")}, {"depth", depth}, {"dimension", "1"}, {"dano_id", _dano.at("id")}, {"itog_id", index.at("itog_id")}, {"index_id", ""}, {"bmf-index", ""}}; _index.empty()){ mpre("ОШИБКА создания нового морфа "+ index.at("id"), __LINE__);
						}else if(string dmap = [&](string dmap = ""){ for(auto dano_itr:_BMF_DANO_EX.at("")){ // Расчет карты дано
							if(dmap += (index.at("dano_id") == dano_itr.second.at("id") ? "1" : "0"); dmap.empty()){ mpre("Устанавливаем значение", __LINE__);
							}else{ //mpre("Значение исходной карты", __LINE__);
							}} return dmap; }(); (dmap.length() != _BMF_DANO_EX.at("").size())){ mpre("ОШИБКА расчета карты дано", __LINE__);
						}else if(index.end() == index.find(transfer.at("field"))){ mpre("ОШИБКА у морфа нет поля для дальнейшего перехода", __LINE__);
						}else if(erb_insert(BMF_INDEX_EX, _index.at("id"), _index); _index.empty()){ mpre("ОШИБКА добавления морфа в справочник", __LINE__);
						}else if(index.at(transfer.at("field")) = _index.at("id"); index.empty()){ mpre("ОШИБКА установки нового морфа к вышестоящему", __LINE__);
						}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА сохранения изменений родителя в справочник", __LINE__);
						}else if(transfer.end() == transfer.find("calc")){ mpre("ОШИБКА поле расчета ступени не задано", __LINE__);
						}else if(string calc = transfer.at("calc"); (0 >= calc.length())){ mpre("ОШИБКА нулевая длинна поля для значения stairs[calc]", __LINE__);
						}else if(string _val = ("0" == calc ? "1" : "0"); (0 >= _val.length())){ mpre("ОШИБКА нулевая длинна поля для значения stairs[calc]", __LINE__);
						}else if(stairs_itr->second[calc] = stairs[calc] = _val; (1 != stairs.at(calc).length())){ mpre("ОШИБКА установки значения нового морфа", __LINE__);
						}else if(transfer.end() == transfer.find("calc")){ mpre("ОШИБКА не установлено направление обучения", __LINE__);
						}else if(string _calc = ("3" == stairs["status"] ? "1_new" : "0_new"); _calc.empty()){ mpre("ОШИБКА расчета поля результата", __LINE__);
						}else if(stairs_itr->second[_calc] = stairs[_calc] = _val; (1 != stairs.at(calc).length())){ mpre("ОШИБКА установки нового значения морфа", __LINE__);
						}else if(stairs_itr->second["revision"] = stairs["revision"] = "+"; (1 != stairs.at(calc).length())){ mpre("ОШИБКА установки доп информации по обучению", __LINE__);
						}else if([&](){ // Удаление маски
							if(bmf::MAP[_index.at("id")] = dmap; bmf::MAP.empty()){ mpre("ОШИБКА установки карта", __LINE__);
							}else if(bmf::MAP.end() == bmf::MAP.find(index.at("id"))){ //mpre("Маски уже нет", __LINE__);
							}else if(bmf::MAP.erase(index.at("id")); (bmf::MAP.end() != bmf::MAP.find(index.at("id")))){ mpre("ОШИБКА удаления маски", __LINE__);
							}else{ //mpre("Удаления текущей маски " +index.at("id"), __LINE__);
							} return false; }()){ mpre("ОШИБКА удаления маски", __LINE__);
						}else{ //mpre("Добавление нового морфа " +index.at("id") +" > "+ _index.at("id") +" " +_calc +"=" +_val, __LINE__); //mpre(index, "Морф", __LINE__); mpre(stairs, "Ступень", __LINE__);
						} return _index; }(); false){ mpre("ОШИБКА добавления морфа", __LINE__);
					}else if(TMs stairs_next = [&](TMs stairs_next = {}){ // Добавление новой ступени
						if(!_index.empty()){ //mpre("Пропуск добавления ступени морф уже добавлен", __LINE__);
						}else if(transfer.end() == transfer.find("next_id")){ //mpre("Ступень не добавляем не указан следующий морф", __LINE__);
						}else if("" == transfer.at("next_id")){ //mpre("Не добавляем пустую ветвь", __LINE__);
						}else if(string cache = [&](string cache = ""){ // Проверка расчета из кеша
							if(string::npos != stairs.at("status").find_last_not_of("12")){ //mpre("Расчитываем только для 1 и 2 а не этап " +stairs.at("status"), __LINE__);
							}else if(transfer.end() == transfer.find("calc")){ mpre("ОШИБКА у направления не установлено направление", __LINE__);
							}else if(string calc = transfer.at("calc") +"_cache"; calc.empty()){ mpre("ОШИБКА расчета поля с кешем", __LINE__);
							}else if(stairs.end() == stairs.find(calc)){ //mpre("У ступени не задано поле расчета", __LINE__);
							}else if(!atoi(transfer.at("next_id").c_str())%100){ mpre("Отменяем пропуск нижестоящей ветви id=" +index.at("id"), __LINE__);
							//}else if(true){ mpre("Отмена пропуска расчета ступени id=" +index.at("id") +" > " +transfer.at("next_id") +" " +stairs.at("status"), __LINE__);
							}else if(cache = stairs.at(calc); cache.empty()){ //mpre("Значение еще не расчитано " +calc, __LINE__); //mpre("Значение расчета из кеша id=" +index.at("id") +" field=" +transfer.at("field") +" key=" +to_string(key) +" calc=" +calc +" val=" +val, __LINE__);
							}else{ //mpre("Пропускаем расчет нижестоящей ветви " +index.at("id") +" " +calc +"=" +val, __LINE__); //mpre("Значение расчета найдено в кеше "+ index.at("id"), __LINE__); return true;
							} return cache; }(); !cache.empty()){ //mpre("Пропускаем расчтет нижестоящей ветви", __LINE__);
						}else if(stairs_next = {{"id", transfer.at("next_id")}, {"parent", transfer["calc"]}}; stairs_next.empty()){ mpre("ОШИБКА установки формирования ступени", __LINE__);
						}else if([&](){ // Установка обещаний
							if(transfer.end() == transfer.find("promise")){ //mpre("Требования к нижестоящей не заданы", __LINE__);
							}else if(string::npos != stairs.at("status").find_last_not_of("34")){ //mpre("Расчитываем только для 3 и 4 а не этап " +stairs.at("status"), __LINE__);
							}else if(stairs_next["promise"] = transfer.at("promise"); stairs_next.empty()){ mpre("ОШИБКА установки обещаний к нижестоящей", __LINE__);
							}else{ //mpre("Установка обещаний к нижестоящему " +index.at("id") +" > " +transfer.at("next_id") +" promise=" +transfer.at("promise"), __LINE__);
							} return false; }()){ mpre("ОШИБКА формирования новой ступени", __LINE__);
						}else if([&](){ // Список исходников ветвей
							if(string::npos != stairs.at("status").find_last_not_of("34")){ //mpre("Расчитываем только для 3 и 4 а не этап " +stairs.at("status"), __LINE__);
							}else if(string list = stairs["list"]; false){ mpre("ОШИБКА получения истории", __LINE__);
							}else if(string cur = ","+ dano.at("id")+ ";"; cur.empty()){ mpre("ОШИБКА дополнения история текущим исходником", __LINE__);
							}else if([&](){ // Удаление элемента внутри списка
								if(std::size_t pos = list.find(cur); false){ mpre("ОШИБКА определения текущей позиции исходника в списке", __LINE__);
								}else if(string::npos == pos){ //mpre("ОШИБКА Элемент не найден в списке list=" +list +" cur=" +cur, __LINE__);
								}else if(list.erase(pos, cur.length()); list.empty()){ mpre("ОШИБКА удаления символа из строки", __LINE__);
								}else{ //mpre("ОШИБКА удаления символа list=" +list +" cur=" +cur, __LINE__);
								} return false; }()){ mpre("ОШИБКА удаления элемента внутри списка", __LINE__);
							}else if(stairs_next["list"] = list +cur; stairs_next.empty()){ mpre("ОШИБКА сохранения истории родителю", __LINE__);
							}else{ //mpre("История исхдоников "+ list, __LINE__);
							} return false; }()){ mpre("ОШИБКА установки истории исходников", __LINE__);
						}else if(STAIRS.insert(make_pair(STAIRS.size(), stairs_next)); STAIRS.empty()){ mpre("ОШИБКА добавления новой ступени", __LINE__);
						}else{ //mpre("Добавление ступени " +stairs.at("id") + " > "+ stairs_next.at("id"), __LINE__);
						} return stairs_next; }(); false){ mpre("ОШИБКА Добавления морфа в лестницу", __LINE__);
					}else if([&](){ // Удаление ступени
						if(false){ mpre(stairs, "Ступень", __LINE__); mpre("ОШИБКА значения цифры", __LINE__);
						}else if(stairs.end() == stairs.find("status")){ mpre("ОШИБКА статус ступени не найден", __LINE__);
						//}else if(string::npos != stairs.at("status").find_last_not_of("5")){ //mpre(STAIRS, "Лестница", __LINE__); mpre("Статус удаления не максимальный " +stairs.at("status"), __LINE__); //mpre(stairs["index_id"]+ " Не расчитываем результат для статуса "+ stairs["status"], __LINE__);
						}else if([&](){ // Пропуск удаления лестницы
							if("5" == stairs.at("status")){ return false; mpre("Всегда на пятом статусе", __LINE__);
							}else if("3" != stairs.at("status")){ //mpre("Всегда на пятом статусе", __LINE__);
							//}else if(string::npos != stairs.at("status").find_last_not_of("32")){ mpre("Статусы на которых результат может совпадать", __LINE__);
							}else if(stairs.end() == stairs.find("promise")){ return false; //mpre("Обучать нечему обещаний нет", __LINE__);
							//}else if(stairs.end() == stairs.find("val")){ //mpre("Обучать нечему обещаний нет", __LINE__);
							//}else if(stairs.at("promise") == stairs.at("val")){ return false; // Обучение совпало
							}else{ //mpre("Третья стадия, но найдены обещания", __LINE__);
							} return true; }()){ //mpre("Пропуск удаления лестницы", __LINE__);
						}else if([&](){ // Сохранение результатов родителю
							if(1 >= STAIRS.size()){ //mpre("Нет родителя не сохраняем результат", __LINE__);
							}else if(stairs.end() == stairs.find("parent")){ mpre("ОШИБКА номер результата родителя не указан", __LINE__);
							}else if(auto stairs_parent_itr = stairs_itr; false){ mpre("Ссылка на текущую ступень", __LINE__);
							}else if(TMs stairs_parent = (++stairs_parent_itr)->second; stairs_parent.empty()){ mpre("ОШИБКА выборки ступеньки родителя", __LINE__);
							}else if(string learn = stairs.at("parent") +"_learn"; (0 >= learn.length())){ mpre("ОШИБКА получения вышестоящего морфа", __LINE__);
							}else if([&](){ // Проверка совпадения результата с кешем
								if(stairs.end() != stairs.find("promise")){ //mpre("Расчитываем только для 1 и 2 а не этап " +stairs.at("status"), __LINE__);
								}else if(string calc = stairs.at("parent") +"_cache"; calc.empty()){ mpre("ОШИБКА расчета поля кеша", __LINE__);
								}else if(stairs_parent.end() == stairs_parent.find(calc)){ //mpre("У родителя не задан кеш id=" +index.at("id"), __LINE__);
								}else if(string cache = stairs_parent.at(calc); cache.empty()){ mpre("Обучающее значение у родителя создано, но не расчитано", __LINE__);
								}else if(cache != stairs.at("val")){ mpre(bmf::MAP, "Карта бит", __LINE__); mpre(bmf::CACHE, "Кеш", __LINE__); Tree(bmf_index, _BMF_DANO_EX); mpre("ОШИБКА кеш не равен расчетному значению status="+ stairs.at("status")+" " +stairs_parent.at("id") +" id=" +index.at("id") +" " +calc +"=" +cache +" val=" +stairs.at("val"), __LINE__);
								}else{ //mpre(stairs_parent, "Родитель", __LINE__); mpre("ОШИБКА обучающее значение у родителя уже задано " +learn, __LINE__);
								} return false; }()){ mpre("ОШИБКА значение родителя уже задано", __LINE__);
							}else if(stairs_parent[learn] = stairs["val"]; (1 != stairs_parent[learn].length())){ mpre("ОШИБКА установки значения вышестоящему морфу", __LINE__);
							}else if([&](){ // Установка изменений
								if(stairs.end() == stairs.find("revision")){ //mpre("Изменений не зафексировано", __LINE__);
								}else if(stairs_parent["revision"] = stairs.at("revision"); (0 >= stairs_parent.at("revision").length())){ mpre("ОШИБКА установки признака изменений", __LINE__);
								}else{ //mpre("Установка изменений", __LINE__);
								} return false; }()){ mpre("ОШИБКА установки ревизии", __LINE__);
							}else if([&](){ // Расчет карты исходников
								if(false){ mpre("Пропуск расчета карт", __LINE__);
								}else if(int size = _BMF_DANO_EX.at("").size(); (1 >= size)){ mpre("Только для дочерних", __LINE__);
								}else if(stairs_parent.end() == stairs_parent.find("id")){ mpre("ОШИБКА идентификатор родиетял в родительской ступени не указан", __LINE__);
								}else if(string parent_id = stairs_parent.at("id"); parent_id.empty()){ mpre("ОШИБКА получения идентификатора родителя", __LINE__);
								}else if(string mapt = [&](string mapt = ""){ // Строка карта родителя
									if(bmf::MAP.end() == bmf::MAP.find(parent_id)){ //mpre("Карта морфа не задана", __LINE__);
									}else if(mapt = bmf::MAP.at(parent_id); mapt.empty()){ mpre("ОШИБКА получения строки карты родителя", __LINE__);
									}else{ //mpre(MAP, "Карта", __LINE__);
									} return mapt; }(); mapt.empty()){ //mpre("Строки карты родителя пуста", __LINE__);
								}else if(boost::dynamic_bitset<unsigned char> dmapt = boost::dynamic_bitset<unsigned char>{mapt}; false){ mpre("ОШИБКА установка буфера ", __LINE__);
								}else if(string mapb = [&](string mapb = ""){ // Строка карта родителя
									if(stairs.end() == stairs.find("id")){ mpre("ОШИБКА идентификатор родиетял в ступени не указан", __LINE__);
									}else if(string index_id = stairs.at("id"); index_id.empty()){ mpre("ОШИБКА получения идентификатора", __LINE__);
									}else if(bmf::MAP.end() == bmf::MAP.find(index_id)){ //mpre("Карта морфа не задана", __LINE__);
									}else if(mapb = bmf::MAP.at(index_id); mapb.empty()){ mpre("ОШИБКА получения строки карты родителя index_id" +index_id, __LINE__);
									}else{ //mpre(MAP, "Карта", __LINE__);
									} return mapb; }(); mapb.empty()){ //mpre("Строки карты пуста", __LINE__);
								}else if(boost::dynamic_bitset<unsigned char> dmapb = boost::dynamic_bitset<unsigned char>{mapb}; false){ mpre("ОШИБКА установка битовой карты ", __LINE__);
								}else if(boost::dynamic_bitset<unsigned char> dmap = dmapt | dmapb; false){ mpre("ОШИБКА расчета пересечений родителя", __LINE__);
								}else if([&](){ // Отображение свойств карт
									if(string dvalt = [&](string dvalt = ""){ boost::to_string(dmapt, dvalt); return dvalt; }(); false){ mpre("ОШИБКА строки map=" +mapt +" " +dvalt +" value_size=" +to_string(dvalt.length()), __LINE__);
									}else if(string dvalb = [&](string dvalb = ""){ boost::to_string(dmapb, dvalb); return dvalb; }(); false){ mpre("ОШИБКА строки map=" +mapb +" " +dvalb +" value_size=" +to_string(dvalb.length()), __LINE__);
									}else{ //mpre("Свойства карт mapt=" +mapt +" dvalt=" +dvalt +" mapb=" +mapb +" dvalb=" +dvalb, __LINE__);
									} return false; }()){ mpre("ОШИБКА отображения свойств", __LINE__);
								}else if(string dval = [&](string dval = ""){ boost::to_string(dmap, dval); return dval; }(); false){ mpre("ОШИБКА строки " +dval +" value_size=" +to_string(dval.length()), __LINE__);
								}else if(bmf::MAP[parent_id] = dval; bmf::MAP.empty()){ mpre("ОШИБКА сохранения карты бит", __LINE__);
								}else{ //mpre("Уточнение расчета карт dval=" +dval +" parent_id=" +parent_id, __LINE__); //if(bitmap dmap = ){
								} return false; }()){ mpre("ОШИБКА расчета карты исходников", __LINE__);
							}else if(stairs_parent_itr->second = stairs_parent; stairs_parent.empty()){ mpre("ОШИБКА установки значения вышестоящему морфу", __LINE__);
							}else{ //mpre("Сохранения результата status=" + stairs.at("status")+ " родителю " +stairs_parent.at("id") +" < " +stairs.at("id") +" " +learn +"=" +stairs["val"], __LINE__);
							} return false; }()){ mpre("ОШИБКА сохранения результатов родителю", __LINE__);
						}else if(stair = [&](){ // Получение результатов расчета
							if(1 < STAIRS.size()){ //mpre("Ступень не последняя", __LINE__);
							}else if(1 > STAIRS.size()){ mpre("ОШИБКА в лестнице не осталось ступеней для результата", __LINE__);
							}else if(stair = STAIRS.begin()->second; stair.empty()){ mpre("ОШИБКА выборки результата из ступени", __LINE__);
							}else{ //mpre(stair, "Результат расчетов", __LINE__);
							} return stair; }(); false){ mpre("ОШИБКА получения результатов расчета", __LINE__);
						}else if([&](){ // Проверка результата ступени
							if(stairs.end() == stairs.find("promise")){ //mpre("Обещание в ступени установлено", __LINE__);
							}else if(stairs.end() == stairs.find("val")){ mpre("ОШИБКА значение в ступени не задано", __LINE__);
							}else if(stairs.at("promise") == stairs.at("val")){ //mpre("Обещаяние выполнено", __LINE__);
							}else{ mpre(stairs, "Ступень", __LINE__); mpre("ОШИБКА обещание ступени не выполнено " +index.at("id"), __LINE__);
							} return false; }()){ mpre("ОШИБКА проверки равенства результата и обещания", __LINE__);
						/*}else if([&](){ // Удаление кеша
							if(stairs.end() == stairs.find("promise")){ //mpre("Для удаления кеша не задан признак обучения " +index.at("id"), __LINE__);
							}else if(string promise = stairs.at("promise"); promise.empty()){ mpre("Значение обещания не установлено", __LINE__);
							}else if(int index_id = atoi(index.at("id").c_str()); (0 == index_id)){ mpre("ОШИБКА получения значения морфа", __LINE__);
							}else if(CACHE.end() == CACHE.find(index_id)){ //mpre("Кеш уже пуст", __LINE__);
							}else if(CACHE.empty()){ mpre("Кеш уже пуст", __LINE__);
							}else if(CACHE.at(index_id).clear(); !CACHE.at(index_id).empty()){ mpre("ОШИБКА очистки кеша морфа", __LINE__);
							}else{ //mpre("Обнуление кеша морфа " +index.at("id"), __LINE__); //mpre("ОШИБКА очистки кеша морфа", __LINE__);
							} return false; }()){ mpre("ОШИБКА удаления кеша", __LINE__);*/
						}else if(STAIRS.erase(stairs_itr->first); false){ mpre("Окончание пустая лестница", __LINE__);
						}else{ //mpre("Удаление ступени с лестницы id=" +stairs.at("id") +" status=" +stairs.at("status"), __LINE__);
						} return false; }()){ mpre("ОШИБКА удаления ступени из лестницы", __LINE__);
					//}else if(mpre("Расчет окончание "+ bmf_index.at("id"), __LINE__); false){ mpre("ОШИБКА уведомления о расчете", __LINE__);
					}else{ //mpre("Окончание статуса "+ stairs.at("status"), __LINE__); //mpre("Повтор "+ to_string(rep), __LINE__);
					}
				}while((0 < --rep) && (0 < STAIRS.size())); return false; }()){ mpre("ОШИБКА получения результатов расчета", __LINE__);
			}else if(string revision = [&](string revision = ""){ // Получение строки изменений
				if(stair.end() == stair.find("revision")){ //mpre("Изменения результата не указаны", __LINE__);
				}else if(revision = stair.at("revision"); false){ mpre("ОШИБКА получения строки изменений", __LINE__);
				}else{ //mpre("Строка изменений revision=" +revision, __LINE__);
				} return revision; }(); false){ mpre("ОШИБКА получения изменений", __LINE__);
			}else if([&](){ // Расчет результата
				if(stair.end() == stair.find("val")){ mpre("ОШИБКА получения результата лестницы val", __LINE__);
				}else if(stair["result"] = revision+ stair.at("val"); (0 >= stair.at("result").length())){ mpre("ОШИБКА формирования результата с изменениями", __LINE__);
				}else{ //mpre(stair, __LINE__, "Результат с изменениями _val="+ stair.at("result"));
				} return stair.end() == stair.find("result"); }()){ mpre("ОШИБКА расчета значения морфа "+ to_string(rep)+ " "+ bmf_index["id"], __LINE__);
			}else if(stair.end() == stair.find("result")){ mpre("ОШИБКА результат расчетов не найден", __LINE__);
			}else{ //Tree(bmf_index, _BMF_DANO_EX); //mpre(stair, __LINE__, "Результаты вычислений"); //mpre("Количество итераций "+ to_string(rep), __LINE__);
			} return stair["result"]; }); false){ mpre("ОШИБКА создания функции обучения", __LINE__);
		}else if(Calculate = ([&](TMs bmf_index, std::string calculate = ""){ // Функция строки польской нотации
			if(TMMi STAIRS = [&](TMMi STAIRS = {}){ // Заполнение лестницы первоначальным значением
				if(false){ pre("Пропуск первоначального заполнения лестницы");
				}else if(bmf_index.end() == bmf_index.find("id")){ err("Идентификатор морфа не указан");
				}else if(TMs stairs = {{"index_id", bmf_index.at("id")}}; stairs.empty()){ err("Получение первоначальной ступени");
				}else if(STAIRS.insert(make_pair(0, stairs)); STAIRS.empty()){ err("Добавление ступени в лестницу");
				}else{ //pre("Первоначальное заполнение лестницы");
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
					}else if(index.end() == index.find(field)){ //err("Поле перехода не найдено в морфе");
					}else if(index_id = index.at(field); !index_id.empty()){ //mpre("Нижестоящий морф установлен "+ index.at("id"), __LINE__);
					}else if(calculate += index.at("dano_id"); calculate.empty()){ err("Установка знака");
					}else if(std::string sign = ("index_id" == field ? "/" : ""); false){ err("Выборка знака");
					}else if(calculate += sign; calculate.empty()){ err("Установка знака");
					}else{ //mpre("Расчет строки " +index.at("id") +" calculate=" +calculate, __LINE__);
					} return index_id; }(); false){ err("Формирование строки");
				}else if(TMs _stairs = [&](TMs _stairs = {}){ // Добавление ступени
					if(index_id.empty()){ //pre("Нет морфа для добавления");
					}else if(_stairs = {{"index_id", index_id}}; _stairs.empty()){ err("Создание следующей ступени");
					}else if(STAIRS.insert(make_pair(STAIRS.size(), _stairs)); STAIRS.empty()){ err("Добавление новой ступени");
					}else{ //mpre("Добавление новой ступени " +index.at("id") +" > " +index_id, __LINE__);
					} return _stairs; }(); false){ err("Добавление ступени");
				}else if([&](){ // Удаление ступени
					if(!_stairs.empty()){ //pre("Уже добавлена нижестоящая");
					}else if("index_id" == field){ //pre("Не удаляем старшее направление");
					}else if(index.end() == index.find("dano_id")){ err("Поле исходника не найдено");
					}else if(std::string dano_id = index.at("dano_id"); dano_id.empty()){ err("Выборка идентификатора исодника");
					}else if(index.end() == index.find("calc_pos_id")){ err("Выборка позиции");
					}else if(int calc_pos_id = atoi(index.at("calc_pos_id").c_str()); 0 > calc_pos_id){ err("Выборка позиции морфа");
					}else if(TMs morpths = TMs({{"0", "<"}, {"1", ">"}}); morpths.empty()){ err("Установка знаков");
					}else if(std::string pos = std::to_string(calc_pos_id&1); (1 != pos.length())){ err("Определение позиции знака морфа");
					}else if(morpths.end() == morpths.find(pos)){ mpre("ОШИБКА Позияия морфа не установлена " +pos, __LINE__);
					}else if(std::string morpth = morpths.at(pos); (1 != morpth.length())){ err("Расчет морфа");
					}else if(calculate += morpth; calculate.empty()){ pre("Установка символа расчета морфа");
					}else if(STAIRS.erase(STAIRS.size()-1); STAIRS.empty()){ //err("Удаление последней ступени");
					}else{ //mpre("Удаление ступени " +index.at("id"), __LINE__);
					} return false; }()){ err("Удаление ступени");
				}else{ //mpre("Расчет лестницы "+ std::to_string(loop), __LINE__);
				}}while(0 < STAIRS.size()); return false; }()){ err("Перебор всех элементов дерева для составления строки расчетов");
			}else{ //Tree(bmf_index, BMF_INDEX_EX); mpre("Расчет строки польской нотации " +calculate, __LINE__);
			}return calculate; }); false){ err("Создание функции расчета польской нотации");
		}else if(Calc = ([&](std::string calculate, TM3i& _BMF_DANO_EX, std::string calc = ""){ // Расчет калькуляции
			if(false){ pre("Пропуск расчета калькуляции");
			}else if(std::stack<string> vals = [&](std::stack<string> vals = {}, std::string c = "", std::string num = ""){ for(char c:calculate){ // Перебор значенийa
				if(std::string str(1,c); false){ err("Преобразование символа к строке");
				}else if([&](){
					if(string::npos != str.find_last_not_of("-0123456789")){ //pre("Не число"); //mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);
					}else if(num += str; false){ err("Инкримент числа");
					}else{ //mpre("Чтение числа "+ num, __LINE__);
					} return false; }()){ err("Расчет числа");
				}else if([&](){
					if(string::npos != str.find_last_not_of("<>/")){ //pre("Не значения"); //mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);a
					}else if(num.empty()){ //pre("Расчет ранее сохраненных значений");
					}else if(_BMF_DANO_EX.at("").end() == _BMF_DANO_EX.at("").find(atoi(num.c_str()))){ mpre("ОШИБКА Значение в исходниках не найдено "+ num, __LINE__);
					}else if(TMs dano = _BMF_DANO_EX.at("").at(atoi(num.c_str())); dano.empty()){ err("Выборка дано");
					}else if(dano.end() == dano.find("val")){ err("Значение дано не задано");
					}else if(std::string _val = dano.at("val"); (1 != _val.length())){ err("Выборка значения дано");
					}else if(vals.push(_val); false){ err("Сохранение результата");
					}else if(std::string tmp = num; false){ pre("Для отладки");
					}else if(num = ""; false){ err("Обнуление числа");
					}else{ //mpre("Знак " +tmp +" "+ _val, __LINE__);
					} return false; }()){ err("Сохранение значения");
				}else if([&](){ // Первое значение
					if(string::npos != str.find_last_not_of("<>")){ //pre("Не морфы"); //mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);
					}else if(std::string _v0 = vals.top(); (1 != _v0.length())){ err("Старшее значение");
					}else if(vals.pop(); false){ err("Удаляем младший элемент");
					}else if(std::string _v1 = vals.top(); (1 != _v1.length())){ err("Младшее значение");
					}else if(vals.pop(); false){ err("Удаляем старший элемент");
					}else if(std::string smooth = ("<" == str ? "1" : "0"); (1 != smooth.length())){ err("Расчет прямого значения");
					}else if(std::string reverse = (">" == str ? "1" : "0"); (1 != reverse.length())){ err("Расчет прямого значения");
					}else if(std::string val = ("01" == _v1 +_v0 ? smooth : reverse); (1 != val.length())){ err("ОШИБКА расчета значения");
					}else if(vals.push(val); false){ err("Сохранение результата");
					}else{ //mpre("Расчет морфа " +_v1 + ":"+ _v0  +" " +str + " val=" + val, __LINE__);
					} return false; }()){ err("Установка первого значения");
				}else{ //mpre("расчет " +str, __LINE__);
				}}; return vals; }(); (0 > vals.size())){ mpre("ОШИБКА Расчет значений " +std::to_string(vals.size()), __LINE__);
			}else if(calc = (1 == vals.size() ? vals.top() : ""); false){ err("Получение значения расчетов");
			}else{ //mpre(_BMF_DANO_EX.at(""), "Дано", __LINE__); mpre("Расчет калькуляции "+ calc+ " "+ calculate, __LINE__);
			}return calc; }); false){ err("Расчет калькуляции");
		}else if(Learning = ([&](TMs itog, int key, string vmap, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX, int change = 0){ //mpre("Обновление", __LINE__); // Обучение
			if(itog.empty()){ mpre("ОШИБКА итог не задан", __LINE__);
			}else if(_BMF_ITOG_EX.at("").end() == _BMF_ITOG_EX.at("").find(atoi(itog.at("id").c_str()))){ mpre(itog, __LINE__, "Итог"); mpre("ОШИБКА итог в локальном списке не найден", __LINE__);
			}else if(itog.end() == itog.find("itog_values_id")){ mpre(itog, __LINE__, "Итог"); mpre("ОШИБКА формата итога", __LINE__);
			}else if(TMs index = [&](TMs index = {}){ // Добавление первоначального морфа
				if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
				}else if([&](){ // Выборка основного морфа
					if(itog.end() == itog.find("index_id")){ //mpre("ОШИБКА поле с оснвоным морфом у итога не найдено", __LINE__);
					}else if(int index_id = atoi(itog.at("index_id").c_str()); (0 == index_id)){ //mpre(itog, "Итог", __LINE__); mpre("ОШИБКА Морф отрицательный", __LINE__);
					}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(index_id)){ //mpre("Морф в справочнике не найден", __LINE__);
					}else if(index = BMF_INDEX_EX.at("").at(index_id); index.empty()){ mpre("ОШИБКА получения морфа", __LINE__);
					}else{ //mpre(index, "Родительский морф", __LINE__);
					} return !index.empty(); }()){ //mpre("Основной морф уже создан", __LINE__);
				}else if(TMs dano = _BMF_DANO_EX.at("").begin()->second; dano.empty()){ mpre("ОШИБКА выборки первого дано", __LINE__);
				}else if("" == itog.at("val")){ mpre(_BMF_ITOG_EX.at(""), __LINE__, "Список локальных итогов"); mpre(BMF_ITOG_EX.at(""), __LINE__, "Список глобальных итогов"); mpre("ОШИБКА у итога и установлено значение", __LINE__);
				}else if(index = {{"id", Id(BMF_INDEX_EX.at(""))}, {"clump_id", bmf::clump_id}, {"itog_values_id", itog.at("itog_values_id")}, {"depth","0"}, {"dimension", "1"}, {"dano_id", dano.at("id")}, {"itog_id", itog.at("id")}, {"calc_pos_id", "3"}, {"index_id", ""}, {"bmf-index", ""}}; index.empty()){ mpre("ОШИБКА формирования свойст нового морфа", __LINE__);
				}else if(erb_insert(BMF_INDEX_EX, index.at("id"), index); index.empty()){ mpre("ОШИБКА добавления морфа в справочник", __LINE__);
				}else if(itog["index_id"] = index.at("id"); itog.empty()){ mpre("ОШИБКА установки свойства связи итога с морфом", __LINE__);
				}else if(erb_insert(_BMF_ITOG_EX, itog.at("id"), itog); itog.empty()){ mpre("ОШИБКА индексирования итога", __LINE__);
				}else if(erb_insert(BMF_ITOG_EX, itog.at("id"), itog); itog.empty()){ mpre("ОШИБКА индексирования итога", __LINE__);
				}else{ //mpre(itog, "Итог", __LINE__); mpre(index, "Добавление первоначального морфа", __LINE__);
				} return index; }(); index.empty()){ mpre("ОШИБКА морф связи не найден", __LINE__);
			}else if(TMs values = [&](TMs values = {}){ // Выбор значения морфа
				if("" == index.at("itog_values_id")){ mpre("ОШИБКА значение у итога не указано", __LINE__);
				}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
				}else if(BMF_ITOG_VALUES_EX.at("").end() == BMF_ITOG_VALUES_EX.at("").find(atoi(index["itog_values_id"].c_str()))){ mpre("ОШИБКА значение не найдено", __LINE__);
				}else if(values = BMF_ITOG_VALUES_EX.at("").at(atoi(index.at("itog_values_id").c_str())); values.empty()){ mpre("ОШИБКА выборки значения итога", __LINE__);
				}else{ //pre("Выборки значения морфа");
				} return values; }(); values.empty()){ mpre("ОШИБКА получения значения", __LINE__);
			}else if(std::string calculate = [&](std::string calculate = ""){ // Быстрая проверка
				if(index.empty()){ err("Морф не найден");
				}else if(calculate = (bmf::CALCULATE.end() == bmf::CALCULATE.find(itog.at("id")) ? "" : bmf::CALCULATE.at(itog.at("id"))); false){ //pre("Удаленная ранее формула");
				}else if(calculate = ("" == calculate ? Calculate(index) : calculate); calculate.empty()){ err("Расчет формулы калькуляции");
				}else if(bmf::CALCULATE[itog.at("id")] = calculate; bmf::CALCULATE.empty()){ err("Сохранение значений калькуляции");
				}else{ //mpre("Быстрая проверка calculate="+ calculate, __LINE__);
				} return calculate; }(); false){ err("Расчет формулы"); //mpre("Пропуск быстрой проверки calc=" +calc +" itog.at(val)=" +itog.at("val"), __LINE__);
			}else if(std::string calc = [&](std::string calc = ""){ // Расчет совпадения
				if(calculate.empty()){ //pre("Структура не создана");
				}else if(calc = Calc(calculate, _BMF_DANO_EX); (1 != calc.length())){ mpre("ОШИБКА Расчета Калькуляции " +calc +" " +calculate, __LINE__);
				}else if(calc == itog.at("val")){ //pre("Не скидываем формулу, структура не меняется");
				}else if(bmf::CALCULATE[itog.at("id")] = ""; bmf::CALCULATE.empty()){ err("Сбрасывание значения при обучении");
				}else{ //pre("Обнуление формулы калькуляции");
				} return calc; }(); (calc == itog.at("val"))){ mpre("Сравнение "+ itog.at("id")+ " "+ values.at("name")+ " ("+ itog.at("name")+ ") index["+ index.at("id")+ "]="+ itog.at("val")+ " "+ calc , __LINE__);
			//}else if(true){ err("Остановка расчета");
			}else if([&](){ // Обучение если не режим расчета
				if(string _val = Learn(index, itog.at("val"), key, vmap, _BMF_DANO_EX, _BMF_ITOG_EX); (1 <= _val.length() >= 2)){ mpre("ОШИБКА обучения морфа значение не верная длинна результата", __LINE__);
				}else if(mpre("Расчет "+ itog.at("id")+ " "+ values.at("name")+ " ("+ itog.at("name")+ ") index["+ index.at("id")+ "]="+ itog.at("val")+ " "+ _val , __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(0 >= _val.length()){ mpre("ОШИБКА формат расчета не верный "+ _val, __LINE__);
				}else if(string val = _val.substr(_val.length()-1, 1); (1 != val.length())){ mpre("ОШИБКА получения результатов расчета", __LINE__);
				}else if(calc == val){ mpre(BMF_INDEX_EX, "Морфы", __LINE__); mpre("ОШИБКА быстрый расчет не совпал с реальным itog_id=" +itog.at("id") +" itog_val=" +itog.at("val") +" index_id=" +index.at("id") +" calc=" +calc +" val=" +val +" calculate=" +calculate, __LINE__);
				}else if(val != itog.at("val")){ mpre("ОШИБКА обещанный результат не совпал с ожидаемым "+ itog.at("val")+ " val=" +val +" _val=" +_val, __LINE__);
				}else if(change += (2 <= _val.length() ? 1 : 0); false){ mpre("ОШИБКА установки изменения", __LINE__);
				}else{ //mpre("Проверка", __LINE__); //Tree(index, _BMF_DANO_EX); //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); mpre(BMF_DANO_EX.at(""), __LINE__, "Справочник"); mpre("ОШИБКА тест "+ itog["val"]+ " "+ _val+ " count="+ to_string(BMF_INDEX_EX.at("").size()), __LINE__);
				} return false; }()){ mpre("ОШИБКА обучения морфа", __LINE__);
			}else{ //mpre("Окончание обучения морфа "+ itog["name"], __LINE__); //system("sleep 0.5");
			} return change; }); false){ mpre("ОШИБКА устанвоки функции расчета итога", __LINE__);
		}else if(LearningAll = ([&](json js, int thread, int _thread, int key, TM3i& _BMF_DANO_EX, TM3i& _BMF_ITOG_EX, int change = 0){ //mpre("ОбновлениеОбщее", __LINE__); // Обучение
			if(false){ mpre("Остановка выполнения LearningAll", __LINE__);
			}else if(string vmap = [&](string vmap = ""){ for(auto dano_itr:_BMF_DANO_EX.at("")){ // Список значений
					if(vmap += dano_itr.second.at("val"); vmap.empty()){ mpre("ОШИБКА выборки значений", __LINE__);
					}else{ //mpre(dano->second, "Значение дано", __LINE__);
					} //mpre(_BMF_DANO_EX.at(""), "Исходные значения", __LINE__);
				} return vmap; }(); (vmap.length() != _BMF_DANO_EX.at("").size())){ mpre("ОШИБКА расчета строки значений " +vmap, __LINE__);
			}else if([&](){ //for(auto itog_itr = BMF_ITOG.begin(); itog_itr != BMF_ITOG.end(); itog_itr++){// mpre("Итог", __LINE__);
				for(auto& itog_itr:_BMF_ITOG_EX.at("")){
					if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА получения итогового знака из итератора", __LINE__);
					}else if([&](){ // Возможность прерывания на создание итогов
						if(ARGV.end() == ARGV.find("-i")){ //mpre("Не многооконный режим", __LINE__);
						}else if(string itog = ARGV.at("-i"); (0 < atoi(itog.c_str()))){ //mpre("Расчитываем итоги", __LINE__);
						}else{ return true; mpre("Пропускаем расчеты (толко создание итогов)", __LINE__);
						} return false; }()){ mpre("Пропускаем обучение только создание итогов "+ itog.at("id"), __LINE__);
					}else if([&](){ // Пропускаем расчет
						if(ARGV.end() == ARGV.find("-i")){ //mpre("Нулевой параметр итога", __LINE__);
						}else if(ARGV.at("-i") == itog.at("id")){ //mpre("Указанный итог равен текущему "+ itog["id"], __LINE__);
						}else{ return true;
						} return false; }()){ //mpre("Пропускаем ["+ itog["id"]+ "] != "+ to_string(itog_id), __LINE__);
					//}else if(int index_id = atoi(itog.at("index_id").c_str()); (0 > index_id)){ mpre("ОШИБКА отрицательный итог", __LINE__);
					}else if([&](){ // Пропуск потоков
						if(ARGV.end() == ARGV.find("-t")){ // Беспотоковый режим
						}else if(abs(atoi(itog.at("id").c_str()))%thread == (_thread-1)){ //mpre("Расчет потока "+ to_string(thread)+ "%"+ to_string(_thread)+ " "+ itog.at("id"), __LINE__);
						}else{ //mpre("Пропускаем расчет потока "+ to_string(thread)+ "%"+ to_string(_thread)+ " "+ itog.at("id"), __LINE__);
							return true;
						} return false; }()){ //mpre("Пропускаем расчет потока "+ to_string(thread)+ "%"+ to_string(_thread)+ " "+ itog.at("id"), __LINE__); //mpre("Пропускаем поток", __LINE__);
					//}else if(mpre(BMF_ITOG_EX.at(""), "Список итогов до", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else if(change += Learning(itog, key, vmap, _BMF_DANO_EX, _BMF_ITOG_EX); false){ mpre("ОШИБКА запуска расчета", __LINE__);
					//}else if(mpre(BMF_ITOG_EX.at(""), "Список итогов после", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
					}else{ //mpre("Перебор связанных морфов окончен " +to_string(_thread), __LINE__);
					}
				}; return false; }()){ mpre("ОШИБКА перебора знаков для обучения", __LINE__);
			}else{// mpre("Обучение", __LINE__);
			} return change; }); false){ mpre("ОШИБКА установки функции обучения", __LINE__);
		}else if(Do = ([&](int thread, int _thread, int change = 0){ // Цикл повторений расчета
			if(false){ mpre("ОШИБКА остановка выполнения", __LINE__);
			}else if(int loop = bmf::loop = [&](int loop = 0){ // Проверка количества эпох из командной строки
				if(0 != loop){ mpre("ОШИБКА для проверки необходимо скинуть значения эпох", __LINE__);
				}else if(string _loop = (ARGV.end() == ARGV.find("-e") ? "1" : ARGV.at("-e")); (0 >= _loop.length())){ mpre("ОШИБКА количество эпох не задано", __LINE__);
				}else if(string::npos != _loop.find_last_not_of("0123456789")){ mpre("ОШИБКА формат количества эпох задан неправильно "+ _loop, __LINE__);
				}else if(loop = atoi(_loop.c_str()); (0 >= loop)){ mpre("ОШИБКА количество эпох не достаточно для продолжения", __LINE__);
				}else{ //mpre("Количество эпох установлено "+ to_string(loop), __LINE__);
				} return loop; }(); (0 > bmf::loop)){ mpre("ОШИБКА количество эпох задано не верно -e", __LINE__);
			}else if(TM3i _BMF_DANO_EX = BMF_DANO_EX; _BMF_DANO_EX.empty()){ mpre("ОШИБКА установки первоначальных исходников дано", __LINE__);
			}else if(TM3i _BMF_ITOG_EX = BMF_ITOG_EX; _BMF_ITOG_EX.empty()){ mpre("ОШИБКА установки первоначальных итогов дано", __LINE__);
			}else if(auto _in = in; _in.empty()){ mpre("ОШИБКА установки временного массива", __LINE__);
			//}else if(true){ mpre(BMF_DANO_VALUES_EX, __LINE__, "Список дано");
			}else if([&](bool proceed = false){ // Обучение до состояния без ошибок
					do{ // Расчет и обучение входящих параметров
						if(change = 0; false){ mpre("ОШИБКА скидывания флага изменений", __LINE__);
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
						//}else if(TMs CALCULATE = {}; false){ err("Установка расчетов");
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
								}else if(el["itog"].empty()){ //std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << __LINE__ << ".ОБУЧЕНИЕ не задано" << endl;
								}else if([&](){ std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << endl << __LINE__ << ".РАСЧЕТ " +to_string(_thread) +" \"dano\":" << el["dano"].dump().substr(0 ,89) << "..." << endl;  return false; }()){ mpre("ОШИБКА отображения информации", __LINE__);
								}else if(Values(dano, "dano", BMF_DANO_VALUES_EX); BMF_DANO_VALUES_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
								}else if(Vals(dano, "dano", key, _BMF_DANO_EX, BMF_DANO_VALUES_EX, BMF_DANO_TITLES_EX); _BMF_DANO_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
								}else if([&](){ std::lock_guard<std::recursive_mutex> locker(mu); std::cerr << __LINE__ << " ОБУЧЕНИЕ " +to_string(_thread) +" \"itog\":" << el["itog"].dump() << endl; return false; }()){ mpre("ОШИБКА вывода уведомления", __LINE__);
								}else if(TMs itog = TMs(el["itog"]); itog.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
								}else if(Values(itog, "itog", BMF_ITOG_VALUES_EX); BMF_ITOG_VALUES_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
								}else if(Vals(itog, "itog", key, _BMF_ITOG_EX, BMF_ITOG_VALUES_EX, BMF_ITOG_TITLES_EX); _BMF_ITOG_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
								}else if(string info = "Эпоха:"+ to_string(loop)+ " Размер:"+ to_string(in.size())+ " Пример:"+ js.key()+ " Изменений:"+ to_string(change)+ " Сеть:"+ to_string(BMF_INDEX_EX.at("").size())+ " Процент:"+ to_string(bmf::perc)+ "/"+ to_string(_perc)+ "% Время:"+ to_string(time(0)-timestamp); (0 >= info.length())){ mpre("ОШИБКА составления строки состояния", __LINE__);
								}else if([&](){ //mpre("Начало Items " +to_string(_thread), __LINE__); // Уведомление об изменениях
									if(int _change = LearningAll(el, thread, _thread, key, _BMF_DANO_EX, _BMF_ITOG_EX); false){ mpre("Расчет изменеений", __LINE__);
									}else if(pips_change += _change; false){ mpre("ОШИБКА получения количества пипсов", __LINE__);
									}else if(0 == _change){ //mpre("Расчет "+ info, __LINE__);
									}else if(0 >= ++change){ mpre("ОШИБКА установки флага изменения", __LINE__);
									}else{ //mpre("Несовпадение результатов "+ info, __LINE__);
									} return false; }()){ //mpre("Нет изменений "+ info, __LINE__);
								}else if([&](string mess = to_string(__LINE__)){ // Формирование и отображение информации
									if(mess += " Время: " +to_string(time(0)-timestamp) +" Выборка: " +to_string(loop) +"/" +to_string(in.size()) +"/" +to_string(count); false){ mpre("ОШИБКА добавления информации о выборке", __LINE__);
									}else if(mess += " Морфы: " +to_string(BMF_INDEX_EX.at("").size()) +"/" +to_string(change) +" " +to_string(bmf::perc) +"/" +to_string(_perc) +"%"; false){ mpre("ОШИБКА информация о морфе", __LINE__);
									}else if(mess += " Итоги: " +to_string(BMF_ITOG_EX.at("").size()) +"/" +to_string(pips_sum) +"/" +to_string(pips_change) +" " +to_string(bmf::pips_perc) +"/" +to_string(_pips_perc) +"%"; false){ mpre("ОШИБКА информация об итогах", __LINE__);
									}else if(std::lock_guard<std::recursive_mutex> locker(mu); false){ mpre("ОШИБКА блокировки вывода", __LINE__);
									}else{ std::cerr << mess << endl;
									} return false; }()){ mpre("ОШИБКА вывода информации о данных", __LINE__);
								}else{ //mpre("Окончение Items " +to_string(_thread), __LINE__); //mpre(BMF_INDEX_EX.at(""), __LINE__, "Справочник"); mpre(BMF_DANO_EX.at(""), __LINE__, "Дано"); //mpre(BMF_ITOG_VALUES_EX, __LINE__, "Список итогов");
								}
							}; return false; }()){ mpre("ОШИБКА перебора перетасованных значений входящих данных", __LINE__);
						}else if((0 < bmf::size_max) && (bmf::size_max < BMF_INDEX_EX.at("").size())){ mpre("Максимальное количество морфов "+ to_string(BMF_INDEX_EX.at("").size())+ " больше допустимого значения size_max = "+ to_string(bmf::size_max)+ ". Расчет прерван "+ to_string(change), __LINE__); exit(EXIT_FAILURE);
						}else if([&](){ bmf::perc = float(in.size()-change)*100.0/in.size(); return false; }()){ mpre("ОШИБКА расчета процента", __LINE__);
						}else if([&](){ bmf::pips_perc = float(pips_sum-pips_change)*100.0/pips_sum; return false; }()){ mpre("ОШИБКА расчета процента совпадения сигнала", __LINE__);
						}else if([&](){ bmf::change_sum += change; return false; }()){ mpre("ОШИБКА расчета итоговой суммы изменений", __LINE__);
						}else{ //mpre(BMF_ITOG_EX.at(""), "Итоги", __LINE__); //std::cerr << endl << __LINE__ << " ЦИКЛ ОБУЧЕНИЯ " << to_string(change) << " из " << to_string(in.size()) << " Процент " << perc << "%";
							proceed = true;
						}
					}while((change != 0) && (--loop > 0) && !(proceed = !proceed));
				return false; }()){ mpre("ОШИБКА цикла обучения", __LINE__);
			}else if(bmf::loop -= loop; false){ mpre("ОШИБКА установки остатка эпох", __LINE__);
			}else{ //mpre(BMF_ITOG_EX.at(""), __LINE__, "Итог"); mpre("ОШИБКА", __LINE__);
			} return false; }); false){ mpre("ОШИБКА создания функции непосредственно расчета", __LINE__);
		}else{
		} return false; }()){ mpre("ОШИБКА создания рабочих функций", __LINE__);
	}else if([&](){ // Запуск расчетов
		if(0 >= in.size()){ mpre("Данные для обучения не заданы", __LINE__); //mpre(in, __LINE__, "Данные");
		}else if(int thread = [&](int thread = -1){ // Расчет количества потоков
			if(string _thread = (ARGV.end() == ARGV.find("-t") ? "0" : ARGV.at("-t")); (0 >= _thread.length())){ mpre("Количество потоков не задано в консоли", __LINE__);
			}else if(string::npos != _thread.find_first_not_of("0123456789")){ mpre("ОШИБКА формат количества потоков не число "+ _thread, __LINE__);
			}else if(thread = atoi(_thread.c_str()); false){ mpre("ОШИБКА конвертации количества потоков в число", __LINE__);
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
	}else if(Value = ([&](TMs itog_values){ // Расчет итога
		if(TMMi ITOG = rb(BMF_ITOG_EX, {{"itog_values_id", itog_values.at("id")}}); ITOG.empty()){ mpre("ОШИБКА получения списка знаков значения", __LINE__); //mpre(itog_values, __LINE__, "Значение"); mpre(BMF_ITOG_EX.at(""), __LINE__, "ОШИБКА получения списка знаков значения");
		}else if(TMMi INDEX; false){ mpre("ОШИБКА создания списка морфов", __LINE__);
		}else if([&](){ for(auto itog_itr:ITOG){ //for_each(ITOG.begin(), ITOG.end(), [&](auto itog_itr){ // Сортировка списка
				if(TMs itog = itog_itr.second; itog.empty()){ mpre("ОШИБКА получения итога", __LINE__);
				}else if(itog.end() == itog.find("index_id")){ mpre("ОШИБКА поле index_id у итога не задано", __LINE__);
				}else if("" == itog.at("index_id")){ mpre(itog, "Итог", __LINE__); mpre("Пустое поле у итога index_id", __LINE__);
				}else if(BMF_INDEX_EX.at("").end() == BMF_INDEX_EX.at("").find(atoi(itog["index_id"].c_str()))){ mpre("Морф итога не указан", __LINE__);
				}else if(TMs index = erb(BMF_INDEX_EX, {{"id", itog.at("index_id")}}); index.empty()){ //mpre("ОШИБКА получения морфа итога", __LINE__);
				}else if(TMs dano = erb(BMF_DANO_EX, {{"id", index.at("dano_id")}}); dano.empty()){ mpre("ОШИБКА выборки исходника морфа", __LINE__);
				}else if(dano.end() == dano.find("val")){ mpre("ОШИБКА поле исходника val не устанволено", __LINE__);
				}else if("" == dano.at("val")){ mpre("ОШИБКА значение исходника морфа не задано", __LINE__);
				}else if(index["val"] = Learn(index, "", 0, "", BMF_DANO_EX, BMF_ITOG_EX); index.empty()){ mpre("ОШИБКА расчета значения морфа", __LINE__);
				}else if(mpre("Расчет "+ itog.at("id")+ " "+ itog_values.at("name")+ " ("+ itog.at("name")+ ") index["+ index.at("id")+ "]="+ index["val"] , __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(INDEX.insert(make_pair(atoi(itog.at("name").c_str()), index)); INDEX.empty()){ mpre("ОШИБКА добавления итогов к сортированному списку", __LINE__);
				}else{ //mpre(itog, __LINE__, "Итог");
				}
			}; return ITOG.empty(); }()){ mpre("ОШИБКА получения сортированного списка значений", __LINE__);
		//}else if(true){ mpre(INDEX, "Индекс", __LINE__);
		}else if(string bin = ""; false){ mpre("ОШИБКА создания двоичной переменной", __LINE__);
		}else if([&](){ for(auto &index_itr:INDEX){ //for_each(INDEX.begin(), INDEX.end(), [&](auto index_itr){ // Установка знаков в значение
				if(TMs index = index_itr.second; index.empty()){ mpre("ОШИБКА получения индекса", __LINE__);
				}else if(int pos = index_itr.first; false){ mpre("ОШИБКА получения позиции знака", __LINE__);
				}else if(int os = INDEX.rbegin()->first-pos; (0 > os)){ mpre("ОШИБКА получения смещения (ноль и больше)", __LINE__);
				}else if(bin += (os >= bin.length() ? std::string(os-bin.length()+1, '-') : ""); (os >= bin.length())){ mpre("ОШИБКА увеличения длинны строки до нужного размера", __LINE__);
				}else if(string _val = (0 == pos ? "." : index.at("val")); (0 >= _val.length())){ mpre("ОШИБКА получения символа знака", __LINE__);
				}else if([&](){ string _bin = bin; bin = bin.substr(0, os)+ _val+ bin.substr(os+1, bin.length()); return (0 >= bin.length()); }()){ mpre("ОШИБКА установки символа знака", __LINE__);
				}else{ //mpre("Расчеты позиции и смещения pos="+ to_string(pos)+ " os="+ to_string(os)+ " length="+ to_string(bin.length())+ " val="+ val, __LINE__);
					//mpre("Изменения "+ _bin+ " >> "+ bin, __LINE__);
				}
			}; return INDEX.empty(); }()){ mpre("Морфы значения не установлены `"+ itog_values["name"]+ "`", __LINE__);
		}else if(bin = ((INDEX.find(0) != INDEX.end()) && (INDEX.at(0).at("val") == "1") ? "-" : "")+ bin; (0 >= bin.length())){ mpre("ОШИБКА установки символа отрицания", __LINE__);
		}else if(double dec = Bin2dec(bin); false){ mpre("ОШИБКА конвертации двоичной в десятичную систему", __LINE__);
		}else if(dec = Bin2dec(bin); false){ mpre("ОШИБКА перевода двоичной строки в десятичное число", __LINE__);
		}else if(string value = to_string(dec); false){ mpre("ОШИБКА установки значения расчета", __LINE__);
		}else if(itog_values["value"] = value; itog_values.empty()){ mpre("ОШИБКА получения конвертируемого значения", __LINE__);
		}else{ //mpre("Значение bin="+ bin+ " dec="+ to_string(dec), __LINE__);
		} return itog_values; }); false){ mpre("ОШИБКА создания функции расчета итога", __LINE__);
	}else if([&](){ // Вывод итоговых значений
		if(ARGV.end() == ARGV.find("-j")){ //mpre("Данные для расчета не указаны", __LINE__);
		}else if(nlohmann::json _in = ("" == ARGV.at("-j") ? in : json::parse(ARGV.at("-j"))); false){ mpre("ОШИБКА разбора строки атрибута -j", __LINE__);
		}else if(0 < bmf::change_sum){ mpre("При ошибках результат не выводим", __LINE__);
		}else if(nlohmann::json j = [&](nlohmann::json j = {}){ for(auto& js:_in.items()){ // Расчет списка теста
				if(json el = js.value(); el.empty()){ mpre("ОШИБКА элемент не найден", __LINE__);
				}else if(string key = js.key(); (0 >= key.length())){ mpre("ОШИБКА расчета ключа", __LINE__);
				}else if(el.end() == el.find("dano")){ mpre("ОШИБКА исходные данные не могут быть пустыми", __LINE__);
				}else if(TMs dano = [&](TMs dano = {}){ TMs _dano = el.at("dano"); dano = _dano; return dano; }(); dano.empty()){ mpre("ОШИБКА получения входных знаков", __LINE__);
				}else if(TMs itog = [&](TMs itog = {}){ // Получение итога
					if(el.end() == el.find("itog")){ //mpre("Итог не задан", __LINE__);
					}else if([&](){ TMs _itog = el.at("itog"); itog = _itog; return itog.empty(); }()){ mpre("ОШИБКА получения итоговых знаков", __LINE__);
					}else{ //mpre(itog, __LINE__, "Итог");
					} return itog; }(); false){ mpre("ОШИБКА получения итога", __LINE__);
				}else if(mpre("РЕЗУЛЬТАТ: "+ el.dump(), __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(int cache = Crc32(el["dano"].dump().c_str()); false){ mpre("ОШИБКА расчета ключа кеша", __LINE__);
				}else if(Vals(dano, "dano", cache, BMF_DANO_EX, BMF_DANO_VALUES_EX, BMF_DANO_TITLES_EX); BMF_DANO_EX.empty()){ mpre("ОШИБКА установки входящих значений", __LINE__);
				}else if([&](){ for(auto &itog_values_itr:BMF_ITOG_VALUES_EX.at("")){ //for_each(BMF_ITOG_VALUES_EX.at("").begin(), BMF_ITOG_VALUES_EX.at("").end(), [&](auto &itog_values_itr){ // Расчет значений
						if(BMF_INDEX_EX.at("").empty()){ //mpre("Установка первоначальных морфов", __LINE__);
						}else if(TMs itog_values = itog_values_itr.second; itog_values.empty()){ mpre("ОШИБКА получения значения", __LINE__);
						}else if(itog_values = Value(itog_values); itog_values.empty()){ mpre("ОШИБКА расчета значения", __LINE__);
						}else if(itog_values_itr.second = itog_values; itog_values_itr.second.empty()){ mpre("ОШИБКА сохранения результатов расчета в справочник", __LINE__);
						}else{ //mpre(itog_values, __LINE__, "Расчетное значение"); //mpre("Расчетное значение `"+ itog_values["name"]+ "` "+ to_string(dec)+ " ("+ bin+ ")", __LINE__);
						}
					}; return false; }()){ mpre("Ошибка отображение результата расчета", __LINE__);
				//}else if(mpre(dano, "Дано", __LINE__); false){ mpre("ОШИБКА вывода уведомления", __LINE__);
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
	}else if(Save = ([&](string table, TMMi ROW, int count_insert = 0, int count_update = 0, int count_delete = 0){ // Расчет значения формулы
		for(auto index_itr = ROW.begin(); index_itr != ROW.end(); index_itr++){ // Сохранение данных справочника
			if(TMs index = index_itr->second; index.empty()){ mpre("ОШИБКА выборки значения итератора", __LINE__);
			}else if(int index_id = index_itr->first; (0 == index_id)){ mpre("ОШИБКА получения ключа строки", __LINE__);
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
				}else if(std::string itog_id = [&](std::string itog_id = ""){ // Проверка на изменения только свой итогa
					if("mp_bmf_itog" != table){ //mpre("Ограничение только для итогов", __LINE__);
					}else if(ARGV.end() == ARGV.find("-i")){ //mpre("Итоги не установлены", __LINE__);
					}else if("0" == ARGV.at("-i")){ mpre("Режим создания итогов", __LINE__);
					}else if(itog_id = ARGV.at("-i"); itog_id.empty()){ mpre("Режим просмотра списков итогов", __LINE__);
					}else{ //mpre("Указан итог " +itog_id, __LINE__);
					} return itog_id; }(); (!itog_id.empty() && (itog_id != index.at("id")))){ //mpre("Изменения итога ограничено " +index.at("id"), __LINE__);
				}else if(index = fk(table, {{"id", index["id"]}}, {}, index); index.empty()){ mpre(index, __LINE__, "ОШИБКА обновления морфа");
				}else{ //mpre(index, __LINE__, "Обновление морфа");
					count_update++;
				} return false; }()){ mpre("ОШИБКА обновления значения морфа", __LINE__);
			}else if([&](){ // Добавление
				if(0 < index_id){ //mpre("Не добавляем морф (создан ранее)", __LINE__);
				}else if("0" == index["id"]){ //mpre("Не обновляем удаленный морф", __LINE__);
				}else if(index = fk(table, {}, index, {}); index.empty()){ mpre("ОШИБКА сохранения нового значения в базу", __LINE__);
				}else{ //mpre(index, __LINE__, "Добавление нового морфа "+ to_string(index_id));
					count_insert++;
				} return false; }()){ mpre("ОШИБКА добавления нового морфа", __LINE__);
			}else if(ROW.find(index_id)->second = index; index.empty()){ mpre(index, __LINE__, "Сохраняем в справочник");
			}else{ //exit(mpre(ROW, __LINE__, "Обновление "+ to_string(index_id)));
			}
		} mpre("Данные таблицы `"+ table+ "` удалений: "+ to_string(count_delete)+ " обновлений: "+ to_string(count_update)+ " добавлений: "+ to_string(count_insert), __LINE__);
		return ROW; }); false){ mpre("ОШИБКС создания функции сохранения в базу данных", __LINE__);
	}else if([&](){ // Сохранение
		if(in.empty()){ mpre("Пустой массив обучающей выборки. Не сохраняем данные в БД", __LINE__);
		//}else if(std::experimental::filesystem::perms p = std::experimental::filesystem::status(dbname).permissions(); ((p & std::experimental::filesystem::perms::others_write) == std::experimental::filesystem::perms::none)){ mpre("ОШИБКА файл БД не доступен для записи $chmod u+w "+ dbname, __LINE__);
		}else if([&](){ //mpre("Сохранение результатов в тест", __LINE__);
			if(exec("CREATE TABLE IF NOT EXISTS mp_bmf_test (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,`time` INTEGER, `date` TEXT, `size` INTEGER, `args` TEXT, `change` INTEGER, `duration` INTEGER, `bmf` INTEGER, `loop` INTEGER, `perc` REAL, `pips` REAL, `clump` TEXT)"); false){ mpre("ОШИБКА создания значения итога", __LINE__);
			}else if(string date = [&](string date = ""){ char mbstr[100]; time_t t = time(nullptr); std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&t)); date = string(mbstr); return date; }(); (0 >= date.length())){ mpre("ОШИБКА расчета времени", __LINE__);
			}else if(nlohmann::json arg = [&](nlohmann::json arg = {}){ arg = ARGV; arg.erase("-"); arg.erase("-j"); return arg; }(); arg.empty()){ mpre("ОШИБКА получения строки аргументов", __LINE__);
			}else if(TMs test = fk("mp_bmf_test", {}, {{"time", to_string(timestamp)}, {"date", date}, {"size", to_string(in.size())}, {"change", to_string(bmf::change_sum)}, {"duration", to_string(time(0)-timestamp)}, {"clump", bmf::clump_id}, {"loop", to_string(bmf::loop)}, {"perc", to_string(bmf::perc)}, {"pips", to_string(bmf::pips_perc)}, {"bmf", to_string(BMF_INDEX_EX.at("").size())}, {"args", arg.dump()}}, {}); test.empty()){ mpre("ОШИБКА сохранения результатов тестов", __LINE__);
			}else{ //mpre("Сохранение статистики id "+ test["id"], __LINE__);
			} return false; }()){ mpre("ОШИБКА сохранения теста", __LINE__);
		}else if(exec("BEGIN TRANSACTION"); false){ mpre("ОШИБКА запуска начала транзакции", __LINE__);
		}else if(std::cerr << endl; false){ err("Отделяем отчет проелом");
		}else if(TMMi _BMF_DANO = Save("mp_bmf_dano", BMF_DANO_EX.at("")); _BMF_DANO.empty()){ mpre("ОШИБКА сохранения исходных данных", __LINE__);
		}else if(TMMi _BMF_ITOG = Save("mp_bmf_itog", BMF_ITOG_EX.at("")); false){ mpre("ОШИБКА сохранения итоговых данных", __LINE__);
		}else if(TMMi _BMF_DANO_VALUES = Save("mp_bmf_dano_values", BMF_DANO_VALUES_EX.at("")); _BMF_DANO_VALUES.empty()){ mpre("ОШИБКА сохранения изначальных значений", __LINE__);
		}else if(TMMi _BMF_ITOG_VALUES = Save("mp_bmf_itog_values", BMF_ITOG_VALUES_EX.at("")); _BMF_ITOG_VALUES.empty()){ mpre("ОШИБКА сохранения итоговых значений", __LINE__);
		}else if(TMMi _BMF_DANO_TITLES = Save("mp_bmf_dano_titles", BMF_DANO_TITLES_EX.at("")); false){ mpre("ОШИБКА сохранения изначальных заголовков", __LINE__);
		}else if(TMMi _BMF_ITOG_TITLES = Save("mp_bmf_itog_titles", BMF_ITOG_TITLES_EX.at("")); false){ mpre("ОШИБКА сохранения итоговых заголовков", __LINE__);
		}else if(TMMi _BMF_INDEX = Save("mp_bmf_index", BMF_INDEX_EX.at("")); false){ mpre("ОШИБКА сохранения справочника морфа", __LINE__);
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
				}else if([&](){ _itog = fk("mp_bmf_itog", {{"id", itog["id"]}}, {}, itog); return _itog.empty(); }()){ mpre(itog, __LINE__, "ОШИБКА обновления значений морфа");
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
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(atoi(id.c_str()))){ mpre("ОШИБКА морф в списке не найден "+ id, __LINE__);
					}else if(TMs _index_ = _BMF_INDEX.at(atoi(id.c_str())); _index_.empty()){ mpre("ОШИБКА морф не найден", __LINE__);
					}else if(index["bmf-index"] = _index_.at("id"); false){ mpre("ОШИБКА установки значения связи", __LINE__);
					}else{ //mpre(index, __LINE__, "Измененная связь");
					} return index.empty(); }()){ mpre("ОШИБКА установки старшего морфа", __LINE__);
				}else if([&](){ // Установка старшего морфа
					if(index.end() == index.find("index_id")){ mpre("ОШИБКА поле со старшим морфом не найдено", __LINE__);
					}else if(string id = index.at("index_id"); (0 >= id.length())){ //mpre("Идентификатор связи не найден", __LINE__);
					}else if(_BMF_INDEX.end() == _BMF_INDEX.find(atoi(id.c_str()))){ mpre("Морф в списке не найден "+ id, __LINE__);
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
				}else if(_index = fk("mp_bmf_index", {{"id", index["id"]}}, {}, index); _index.empty()){ mpre(index, __LINE__, "ОШИБКА обновления значений морфа");
				}else{ //mpre(_index, __LINE__, "Связь 1"); mpre(index, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
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
				}else if(_dano_titles = fk("mp_bmf_dano_titles", {{"id", dano_titles["id"]}}, {}, dano_titles); _dano_titles.empty()){ mpre(dano_titles, __LINE__, "ОШИБКА обновления заголовка");
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
				}else if(TMs _itog_titles = fk("mp_bmf_itog_titles", {{"id", itog_titles["id"]}}, {}, itog_titles); _itog_titles.empty()){ mpre(itog_titles, __LINE__, "ОШИБКА обновления заголовка");
				}else{ //mpre(_itog_titles, __LINE__, "Связь 1"); //mpre(itog_titles, __LINE__, "Коррекция ключей морфа "+ to_string(id));
				}
			} return false; }()){ mpre("ОШИБКА правки связей морфов", __LINE__);
		}else if(exec("COMMIT TRANSACTION"); false){ mpre("ОШИБКА начала сессии к БД", __LINE__);
		}else{ //mpre(_BMF_INDEX, __LINE__, "Сохранение");
		} return false; }()){ mpre("ОШИБКА установки ключей", __LINE__);
	}else{
	}
}

