#include <iostream>
#include <algorithm>
#include <fstream>
//#include <cassert>
#include <iterator>
#include <sstream>
#include <map>
#include <ctime>

#include <sqlite3.h>
#include "json.hpp"

#define pre(x) if(x) printf("%d.%s\n", __LINE__, x)

int main(int argc, char **argv){
	if(std::function<int(std::string,int)> mpre = [&](std::string mess, int line){ // Функция вывода сообщений
		//if(printf("%d.%s\n", line, mess.c_str()); false){ printf("%d.ОШИБКА\n", __LINE__);
		if(std::cerr << line << "." << mess << std::endl; false){ printf("%d.ОШИБКА\n", __LINE__);
		}else{ //print_r("Сообщение об ошибке", __LINE__);
		} return false; }; false){ mpre("ОШИБКА установки функции", __LINE__);
	}else if(std::string dbname = [&](std::string file_name = ""){ // Получение имени файла csv из консоли
		if(1 >= argc){ mpre("Параметров консоли меньше двух", __LINE__);
		}else if(file_name = argv[1]; file_name.empty()){ mpre("ОШИБКА получения имени csv файла", __LINE__);
		}else{ //mpre("Файл csv `"+ file_name +"`", __LINE__);
		} return file_name; }(); dbname.empty()){ mpre("ОШИБКА имя файла не указано", __LINE__);
	//}else if(std::string str = [&](std::string str = ""){
	}else if(std::function<int(std::map<std::string, std::string>,int)> dump = [&](std::map<std::string, std::string> row, int line){ // Функция вывода сообщений
		if(std::cerr << " => " << "( // __" << std::to_string(line) << "__ " << "\n"; false){ mpre("ОШИБКА отображение заголовка", __LINE__);
		}else if([&](){ for(auto itr = row.begin(); itr != row.end(); itr++){
				std::cerr << "\t[" << itr->first << "]=>" << itr->second << "," << std::endl;
			}; std::cerr << ")\n"; return false; }()){ mpre("ОШИБКА отображения массива", __LINE__);
		}else{
		} return false; }; false){ mpre("ОШИБКА установки функции", __LINE__);
	}else if(std::map<int, std::map<std::string, std::string>> TAB = [&](std::map<int, std::map<std::string, std::string>> TAB = {}){ // Формирование файла ответа
		if(sqlite3* db = 0; false){ mpre("ОШИБКА установки идентификатора ошибки базы данных", __LINE__);
		}else if(std::string dbname = "submission.sqlite"; dbname.empty()){ mpre("ОШИБКА установки файла БД", __LINE__);
		//}else if(mpre("Загрузка из БД "+ dbname, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
		}else if(SQLITE_OK != sqlite3_open(dbname.c_str(), &db)){ mpre(" ОШИБКА открытия базы данных "+ dbname, __LINE__);
		}else if(sqlite3_stmt* stmt; false){ mpre("Ссылка на ошибки", __LINE__);
		}else if(std::string sql = "SELECT * FROM mp_bmf_submission ORDER BY _id;"; sql.empty()){ mpre("ОШИБКА составления запроса", __LINE__);
		}else if(int result = sqlite3_prepare(db, sql.c_str(), -1, &stmt, 0); (SQLITE_OK != result)){ mpre("ОШИБКА выполнения запроса "+ sql, __LINE__);
		}else if(int count = sqlite3_column_count(stmt); (0 >= count)){ mpre("ОШИБКА расчета количества записей в таблице", __LINE__);
		}else if([&](){ //sqlite3_step(stmt);
			while(SQLITE_ROW == sqlite3_step(stmt)){
				std::map<std::string, std::string> row;
				for(int num = 0; num < count; num++){
					std::string field = std::string((const char*)sqlite3_column_name(stmt, num));
					std::string val = std::string((const char*)sqlite3_column_text(stmt, num));
					std::string value = (val == "NULL" ? "" : val);
					row.insert(std::make_pair(field, value));
				} //dump(row, __LINE__);
				TAB.insert(std::make_pair(atoi(row["_id"].c_str()), row));
			}; return false; }()){ mpre("ОШИБКА получения данных запроса", __LINE__);
		}else{ //std::cout << TAB.size() << std::endl; //pre(tab_itr.second, __LINE__);
		} return TAB; }(); false){ mpre("ОШИБКА формирования CSV", __LINE__);
	}else if(std::string file_name = [&](std::string file_name = ""){ // Получение имени файла csv из консоли
		if(file_name = (2 < argc ? argv[2] : ""); !file_name.empty()){ //mpre("ОШИБКА получения имени csv файла", __LINE__);
		}else if(std::cout << "id,target" << std::endl; false){ mpre("ОШИБКА Вывод заголовка", __LINE__);
		}else if([&](){ for(auto tab_itr:TAB){ // Формирование выборки
			if(std::map<std::string, std::string> tab = tab_itr.second; tab.empty()){ mpre("ОШИБКА пустая строка в "+ dbname, __LINE__);
			}else{ std::cout << tab["_id"] << "," << tab["target"] << std::endl;
			}}return false; }()){ mpre("ОШИБКА формирования выборки", __LINE__);
		}else{ //mpre("Файл csv `"+ file_name +"`", __LINE__);
		} return file_name; }(); file_name.empty()){ //mpre("Имя файла теста не указан", __LINE__);
	}else if(std::ifstream fin(file_name); !fin){ mpre("ОШИБКА Подключения файла "+ file_name, __LINE__);
	}else if(std::map<std::string,std::string> titles; false){ mpre("ОШИБКА получения заголовков полей", __LINE__);
	}else if(std::map<int, std::map<std::string, std::string>> CSV = [&](std::map<int, std::map<std::string, std::string>> CSV = {}){ while(!fin.eof()){ //mpre("Проверка", __LINE__);
			if(std::string line; false){ mpre("Срока данных", __LINE__);
			}else if(getline(fin, line); line.empty()){ mpre("Пустая строка в файле " +file_name, __LINE__);
			}else if(std::istringstream iss(line); false){ mpre("ОШИБКА создания массива", __LINE__);
			}else if(std::map<std::string, std::string> data; false){ mpre("ОШИБКА Создание списка значений", __LINE__);
			}else if([&]{ std::string str; while(std::getline(iss, str, ',')){ data.insert(make_pair(std::to_string(data.size()), str)); } return data.empty(); }()){ mpre("ОШИБКА разделения строк", __LINE__);
			}else if([&](bool pass = false){ // Установка заголовков
				if(!titles.empty()){ //mpre("Заголовки уже установлены", __LINE__);
				}else if(pass = true; !pass){ mpre("ОШИБКА установки признака пропуска", __LINE__);
				}else if(std::map<std::string, std::string> arr; false){ mpre("Массив ассоциаций", __LINE__);
				}else if(titles = data; titles.empty()){ mpre("ОШИБКА установки заголовков", __LINE__);
				}else{ //dump(titles, __LINE__); mpre("Заголовки устанволены " +std::to_string(titles.size()), __LINE__);
				} return pass; }()){ //mpre("Установка заголовка", __LINE__);
			}else if(std::map<std::string, std::string> csv; false){ mpre("Массив пар значений", __LINE__);
			}else if([&](){ for(auto titles_itr:titles){ // Установка пар
				if(std::string key = titles_itr.second; key.empty()){ mpre("Ключ пары не найден", __LINE__);
				//}else if(data.end() == data.find(titles_itr.first)){ mpre("ОШИБКА в csv массиве не найдено поле заголовока", __LINE__);
				}else if(std::string val = data.at(titles_itr.first); key.empty()){ mpre("Ключ пары не найден", __LINE__);
				//}else if(tabs.end() == tabs.find(key)){ dump(tabs, __LINE__); mpre("ОШИБКА заголовок не найден в таблицу "+ key, __LINE__);
				//}else if(std::string tab = tabs.at(key); tab.empty()){ mpre("ОШИБКА пустой заголовок", __LINE__);
				}else if(csv.insert(make_pair(key, val)); csv.empty()){ mpre("ОШИБКА добавления пары в массив", __LINE__);
				}else{ //mpre("key=" +key +" val=" +val, __LINE__); //pre(csv, __LINE__); //mpre("Пара key=" + key+" val=" +val, __LINE__);
				}} return false; }()){ mpre("ОШИБКА устанвоки пар заголовок:значение", __LINE__);
			}else if(CSV.insert(make_pair(CSV.size(), csv)); CSV.empty()){ mpre("ОШИБКА добавления новой строки", __LINE__);
			}else{ //dump(csv, __LINE__); //mpre("Загружаемая строка "+ line, __LINE__);
			}
		} return CSV; }(); CSV.empty()){ mpre("ОШИБКА обработки файла", __LINE__);
	}else if(nlohmann::json JSON = CSV; false){ mpre("ОШИБКА обьявления данных json", __LINE__);
	}else if(sqlite3* db = 0; false){ mpre("ОШИБКА установки идентификатора ошибки базы данных", __LINE__);
	}else if([&](int loop = 3){ for(auto csv_itr:CSV){ // Поэлементная обработка csv
			if(false){ mpre("Пропуск расчетов", __LINE__);
			}else if(std::map<std::string, std::string> csv = csv_itr.second; csv.empty()){ //mpre("ОШИБКА получения массива данных", __LINE__);
			}else if(std::string id = [&](std::string id = ""){ // Выборка идентификатора
				if(csv.end() == csv.find("id")){ //dump(csv, __LINE__); mpre("ОШИБКА поле Id не найдено в свойствах примера", __LINE__);
				}else if(id = csv.at("id"); id.empty()){ dump(csv, __LINE__); mpre("ОШИБКА выборки идентификатора примера", __LINE__);
				}else{ //mpre("Идентификатор примера " +id, __LINE__);
				} return id; }(); id.empty()){ mpre("ОШИБКА выборки идентификатора", __LINE__);
			/*}else if([&](std::string list = ""){ // Принудительный пропуск
				if(5 >= argc){ //mpre("Нет условий на принудительный пропуск", __LINE__);
				}else if(list = argv[5]; list.empty()){ mpre("ОШИБКА выборки списка принудительного пропуска", __LINE__);
				}else{ return (std::string::npos == list.find(id)); mpre("Принудительный пропуск " +list, __LINE__);
				} return false; }()){ mpre("Принудительный пропуск " +id + " (" +argv[5] +")", __LINE__);
			}else if(int skip = [&](int id =0, int split =1, int inc = 0){ // Пропуск по split
				if(csv.end() == csv.find("id")){ mpre("ОШИБКА поле Id в списке не найдено", __LINE__);
				}else if(id = atoi(csv.at("id").c_str()); (0 == id)){ dump(csv, __LINE__); mpre("ОШИБКА выборки идентификатора примера", __LINE__);
				}else if(3 >= argc){ //mpre("Остаток не указан ", __LINE__); // assert(!("Длина не может быть отрицательна", 1011)); //assert(!"Проверяем на ноль"); //assert(!"ОШИБКА какая то"); //throw __LINE__;// ohrow "Ошибка - на 0 делить нельзя!!!!"; //std::perror("ОШИБКА"); //mpre("Делитель не задан", __LINE__);
				}else if(split = atoi(argv[3]); (0 > split)){ mpre("ОШИБКА выборки делителя", __LINE__);
				}else if(4 >= argc){ //mpre("Делитель не указан ", __LINE__); // assert(!("Длина не может быть отрицательна", 1011)); //assert(!"Проверяем на ноль"); //assert(!"ОШИБКА какая то"); //throw __LINE__;// ohrow "Ошибка - на 0 делить нельзя!!!!"; //std::perror("ОШИБКА"); //mpre("Делитель не задан", __LINE__);
				}else if(inc = atoi(argv[4]); false){ mpre("ОШИБКА расчета коррекции", __LINE__);
				}else{ //mpre("Проверка пропуска id=" + std::to_string(id)+ " split=" +std::to_string(split) +" " +std::to_string(0 == split ? 0 : (id+inc)%split), __LINE__);
				}return (0 == split ? 0 : (id+inc)%split); }(); (0 != skip)){ mpre("Пропуск выполнения "+ id+ "+" +(4 > argc ? "0" : argv[3]) +"%" +(3 > argc ? "" : argv[2]) +" => "+ std::to_string(skip), __LINE__);*/
			}else if(TAB.end() != TAB.find(atoi(id.c_str()))){ mpre("Пропускаем "+ id, __LINE__);
			}else if(mpre("Расчет "+ id, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(csv.end() == csv.find("id")){ mpre("ОШИБКА идентификатор в списке не найден", __LINE__);
			}else if(csv.erase("id"); false){ mpre("ОШИБКА удаления идентификатора", __LINE__);
			}else if(nlohmann::json json = csv; json.empty()){ mpre("ОШИБКА загрузки данных в json", __LINE__);
			}else if(std::string data = "{\""+ id+ "\":{\"dano\":" +json.dump() +"}}"; data.empty()){ mpre("ОШИБКА получения строки данных", __LINE__);
			}else if(std::string cmd = "./bimorph "+ dbname+ " -dano '" +data +"' 2>/dev/null"; cmd.empty()){ mpre("ОШИБКА формирования строки комманды расчета", __LINE__);
			//}else if(mpre("Расчет " +cmd, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(std::string result = [&](std::string result = ""){
				if(false){ mpre("Не расчитываем", __LINE__);
				//}else if(mpre("Расчет " +id +" "+ cmd, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
				}else if(std::array<char, 128> buffer; false){ mpre("ОШИБКА создания бефера", __LINE__);
				}else if(std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose); !pipe){ mpre("ОШИБКА запуска програмы в папе", __LINE__);
				}else if([&](){ while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){ result += buffer.data(); } return false; }()){ mpre("Чтение вывода программы", __LINE__);
				}else{ mpre(result, __LINE__);
				} return result; }(); result.empty()){ mpre("ОШИБКА данные расчета пусты "+ cmd, __LINE__);
			}else if(std::string price = [&](std::string price = ""){
				if(nlohmann::json json = nlohmann::json::parse(result); false){ mpre("ОШИБКА загрузки данных в json", __LINE__);
				}else if(json.end() == json.find(id)){ mpre("ОШИБКА поля id нет в результате", __LINE__);
				}else if(json.at(id).end() == json.at(id).find("target")){ mpre("ОШИБКА поля target нет в результате", __LINE__);
				}else if(price = json[id]["target"]; price.empty()){ mpre("ОШИБКА выборки расчетной цены", __LINE__);
				}else{ //mpre("Данные расчета json="json.dump(), __LINE__);
				}return price; }(); price.empty()){ mpre("ОШИБКА получения цены расчета", __LINE__);
			}else if([&](std::string sql = ""){ // Сохранение результатов
				if(sqlite3_stmt* stmt; false){ mpre("Ссылка на ошибки", __LINE__);
				}else if(std::string dbname = "submission.sqlite"; dbname.empty()){ mpre("ОШИБКА установки имени БД", __LINE__);
				}else if(SQLITE_OK != sqlite3_open(dbname.c_str(), &db)){ mpre(" ОШИБКА открытия базы данных "+ dbname, __LINE__);
				}else if(sql = "CREATE TABLE IF NOT EXISTS `mp_bmf_submission` (`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, `time` INTEGER, `_id` INTEGER, `target` INTEGER)"; sql.empty()){ mpre("ОШИБКА установки строки запроса", __LINE__);
				}else if(SQLITE_OK != sqlite3_exec(db, sql.c_str(), 0, &stmt, 0)){ mpre("ОШИБКА выполнения запроса " +sql, __LINE__);
				}else if(sql = "INSERT OR REPLACE INTO `mp_bmf_submission` (id, _id, time, target) VALUES ((SELECT `id` FROM `mp_bmf_submission` WHERE `_id`="+ id+ "), '"+ id+ "','"+ std::to_string(std::time(0))+ "','"+ price+ "');"; sql.empty()){ mpre("ОШИБКА установки строки запроса", __LINE__);
				}else if(SQLITE_OK != sqlite3_exec(db, sql.c_str(), 0, &stmt, 0)){ mpre("ОШИБКА выполнения запроса " +sql, __LINE__);
				}else if(SQLITE_OK != sqlite3_close(db)){ mpre(" ОШИБКА закрытия базы данных "+ dbname, __LINE__);
				}else{ mpre("Сохранение данных в файл бд: "+ dbname+ " таблицу: mp_bmf_submission", __LINE__);
				} return false; }()){ mpre("ОШИБКА сохранения результатов", __LINE__);
			}else{ mpre("Итоговая цена id=" +id +" " +price, __LINE__);
			}
		} return false; }()){ mpre("ОШИБКА обработки csv построчно", __LINE__);
	}else{ mpre("Количество CSV строк в файле "+ std::to_string(CSV.size()), __LINE__);
	}
}

