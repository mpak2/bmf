#include <iostream>
#include <algorithm>
#include <fstream>
//#include <cassert>
#include <iterator>
#include <sstream>
#include <map>
#include <ctime>

#include <sqlite3.h>
#include "inc/json.hpp"

#define pre(x) if(x){ std::cerr << __LINE__ << " " << x << std::endl; }

int main(int argc, char **argv){
	if(std::function<int(std::string,int)> mpre = [&](std::string mess, int line){ // Функция вывода сообщений
		if(std::cerr << line << " " << mess << std::endl; false){ printf("%d.ОШИБКА\n", __LINE__);
		}else{ //print_r("Сообщение об ошибке", __LINE__);
		} return false; }; false){ mpre("ОШИБКА установки функции", __LINE__);
	//}else if(std::string dbname = "house20.sqlite"; dbname.empty()){ mpre("ОШИБКА установки пути к БД", __LINE__);
	}else if(std::function<int(std::map<std::string, std::string>,int)> dump = [&](std::map<std::string, std::string> row, int line){ // Функция вывода сообщений
		if(std::cerr << " => " << "( // __" << std::to_string(line) << "__ " << "\n"; false){ mpre("ОШИБКА отображение заголовка", __LINE__);
		}else if([&](){ for(auto itr = row.begin(); itr != row.end(); itr++){
				std::cerr << "\t[" << itr->first << "]=>" << itr->second << "," << std::endl;
			}; std::cerr << ")\n"; return false; }()){ mpre("ОШИБКА отображения массива", __LINE__);
		}else{
		} return false; }; false){ mpre("ОШИБКА установки функции", __LINE__);
	/*}else if([&](){ // Формирование файла ответа
		if(1 < argc){ //mpre("Не выводим результат", __LINE__);
		}else if(std::map<int, std::map<std::string, std::string>> TAB; false){ mpre("ОШИБКА создания вложенного массива значений из БД", __LINE__);
		}else if(sqlite3* db = 0; false){ mpre("ОШИБКА установки идентификатора ошибки базы данных", __LINE__);
		}else if(std::string dbname = "submission.sqlite"; dbname.empty()){ mpre("ОШИБКА установки файла БД", __LINE__);
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
				} //mpre(row, __LINE__);
				TAB.insert(std::make_pair(TAB.size(), row));
			}; return false; }()){ mpre("ОШИБКА получения данных запроса", __LINE__);
		//}else if(std::cout << "Id,SalePrice" << std::endl; false){ mpre("ОШИБКА вывода заголовка файла", __LINE__);
		}else if(std::map<std::string, std::string> TEST = [&](std::map<std::string, std::string> TEST = {}){ // Формирование списка который должен быть
			for(int i = 1461; i <= 2919; i++){
				TEST.insert(make_pair(std::to_string(i), std::to_string(i)));
			} return TEST; }(); TEST.empty()){ mpre("ОШИБКА формирования списка нужных", __LINE__);
		//}else if(true){ dump(TAB, __LINE__);
		}else if(std::string text = [&](std::string text = "Id,SalePrice\n"){ for(auto tab_itr:TAB){
				if(tab_itr.second.end() == tab_itr.second.find("_id")){ mpre("ОШИБКА поле идентификатора не найдено в данных", __LINE__);
				}else if(std::string id = tab_itr.second.at("_id"); id.empty()){ mpre("ОШИБКА выборки идентификатора", __LINE__);
				}else if(tab_itr.second.end() == tab_itr.second.find("price")){ mpre("ОШИБКА поле цены не найдено в данных", __LINE__);
				}else if(std::string price = tab_itr.second.at("price"); id.empty()){ mpre("ОШИБКА выборки идентификатора", __LINE__);
				}else if(text += id+ "," +price +"\n"; text.empty()){ mpre("ОШИБКА добавления данных в содержимое csv", __LINE__);
				}else if(TEST.erase(id); false){ mpre("ОШИБКА удаление уже добавленых", __LINE__);
				//}else if(std::cout << id << "," << price << std::endl; false){ mpre("ОШИБКА формирования текста ответов csv", __LINE__);
				}else{ //pre(tab_itr.second, __LINE__); mpre("Данные из таблицы", __LINE__);
				}
			} return text; }(); false){ mpre("ОШИБКА отображения значений таблицы", __LINE__);
		}else if(!TEST.empty()){ std::cout << "./submission test.txt 0 0 "; for(auto test_itr:TEST){ std::cout << "," << test_itr.first; }; std::cout << std::endl; mpre("В табе не ватает полей size=" +std::to_string(TEST.size()), __LINE__);
		}else{ std::cout << text << std::endl; exit(0); //pre(tab_itr.second, __LINE__);
		} return (1 == argc); }()){ //mpre("ОШИБКА формирования CSV", __LINE__);
	}else if(std::map<std::string, std::string> tabs = [&](std::map<std::string, std::string> tabs = {{"Id", "Id"}}){ // Массив значений в полях итогов
		if(std::map<int, std::map<std::string, std::string>> TAB; false){ mpre("ОШИБКА создания вложенного массива значений из БД", __LINE__);
		}else if(sqlite3* db = 0; false){ mpre("ОШИБКА установки идентификатора ошибки базы данных", __LINE__);
		}else if(SQLITE_OK != sqlite3_open(dbname.c_str(), &db)){ mpre(" ОШИБКА открытия базы данных "+ dbname, __LINE__);
		}else if(sqlite3_stmt* stmt; false){ mpre("Ссылка на ошибки", __LINE__);
		}else if(std::string sql = "SELECT * FROM mp_bmf_dano_values"; sql.empty()){ mpre("ОШИБКА составления запроса", __LINE__);
		}else if(int result = sqlite3_prepare(db, sql.c_str(), -1, &stmt, 0); (SQLITE_OK != result)){ mpre("Запрос выполнен без ошибок", __LINE__);
		}else if(int count = sqlite3_column_count(stmt); (0 >= count)){ mpre("ОШИБКА расчета количества записей в таблице", __LINE__);
		}else if([&](){ //sqlite3_step(stmt);
			while(SQLITE_ROW == sqlite3_step(stmt)){
				std::map<std::string, std::string> row;
				for(int num = 0; num < count; num++){
					std::string field = std::string((const char*)sqlite3_column_name(stmt, num));
					std::string val = std::string((const char*)sqlite3_column_text(stmt, num));
					std::string value = (val == "NULL" ? "" : val);
					row.insert(std::make_pair(field, value));
				} TAB.insert(std::make_pair(TAB.size(), row));
			}; return false; }()){ mpre("ОШИБКА получения данных запроса", __LINE__);
		}else if([&](){ for(auto tab_itr:TAB){
				if(tab_itr.second.end() == tab_itr.second.find("name")){ mpre("ОШИБКА поле name в списке полей не найдено", __LINE__);
				}else if(std::string name = tab_itr.second.at("name"); name.empty()){ mpre("ОШИБКА выборки имени", __LINE__);
				}else if(std::size_t pos = name.find("("); std::string::npos == pos){ mpre("ОШИБКА скобка в имени не найдена", __LINE__);
				}else if(std::string title = name.substr(0, pos); title.empty()){ mpre("ОШИБКА получения заголовка", __LINE__);
				}else if(tabs.insert(make_pair(title, name)); tabs.empty()){ mpre("ОШИБКА добавления заголовка в массив", __LINE__);
				}else{ //pre(tab_itr.second, __LINE__); mpre("Имя " +name, __LINE__);
				}
			} return false; }()){ mpre("ОШИБКА сокращения имен из таблицы", __LINE__);
		}else{ //for(auto tab_itr:TAB){ dump(tab_itr.second, __LINE__); }; mpre("Количество строк "+ std::to_string(TAB.size()) ,__LINE__);
		} return tabs; }(); tabs.empty()){ mpre("ОШИБКА получения списка заголовков БД", __LINE__);*/
	}else if(std::string file_name = [&](std::string file_name = ""){ // Получение имени файла csv из консоли
		if(2 > argc){ mpre("Параметров консоли меньше двух", __LINE__);
		}else if(file_name = argv[1]; file_name.empty()){ mpre("ОШИБКА получения имени csv файла", __LINE__);
		}else{ //mpre("Файл csv `"+ file_name +"`", __LINE__);
		} return file_name; }(); file_name.empty()){ mpre("ОШИБКА имя файла не указано", __LINE__);
	}else if(int line = (2 < argc ? atoi(argv[2]) : 0); (0 > line)){ mpre("ОШИБКА расчета количества строк", __LINE__);
	}else if(mpre("Файл CSV `" + file_name+ "` разбить по строкам " +std::to_string(line), __LINE__)){ mpre("ОШИБКА уведомления", __LINE__);
	}else if(std::ifstream fin(file_name); !fin){ mpre("ОШИБКА Подключения файла "+ file_name, __LINE__);
	}else if(std::map<std::string,std::string> titles; false){ mpre("ОШИБКА получения заголовков полей", __LINE__);
	}else if(std::map<int, std::map<std::string, std::string>> CSV = [&](std::map<int, std::map<std::string, std::string>> CSV = {}, int loop = 1e9){ while(!fin.eof() && (loop-- > 0)){ //mpre("Проверка", __LINE__);
			if(std::string line; false){ mpre("Срока данных", __LINE__);
			}else if(getline(fin, line); line.empty()){ pre("Пустая строка");
			}else if(std::istringstream iss(line); false){ mpre("ОШИБКА создания массива", __LINE__);
			}else if(std::map<std::string, std::string> data; false){ mpre("ОШИБКА Создание списка значений", __LINE__);
			}else if([&]{ std::string str; while(std::getline(iss, str, ',')){ data.insert(make_pair(std::to_string(data.size()), str)); } return data.empty(); }()){ mpre("ОШИБКА разделения строк", __LINE__);
			}else if([&](bool pass = false){ // Установка заголовков
				if(!titles.empty()){ //mpre("Заголовки уже установлены", __LINE__);
				}else if(pass = true; !pass){ mpre("ОШИБКА установки признака пропуска", __LINE__);
				}else if(std::map<std::string, std::string> arr; false){ mpre("Массив ассоциаций", __LINE__);
				}else if(titles = data; titles.empty()){ mpre("ОШИБКА установки заголовков", __LINE__);
				}else{ //pre(titles, __LINE__); mpre("Заголовки устанволены " +std::to_string(titles.size()), __LINE__);
				} return pass; }()){ //mpre("Установка заголовка", __LINE__);
			}else if(std::map<std::string, std::string> csv; false){ mpre("Массив пар значений", __LINE__);
			}else if([&](){ for(auto titles_itr:titles){ // Установка пар
				if(std::string key = titles_itr.second; key.empty()){ mpre("Ключ пары не найден", __LINE__);
				}else if(data.end() == data.find(titles_itr.first)){ mpre("ОШИБКА в csv массиве не найдено поле заголовока", __LINE__);
				}else if(std::string val = data.at(titles_itr.first); key.empty()){ mpre("Ключ пары не найден", __LINE__);
				}else if(csv.insert(make_pair(key, val)); csv.empty()){ mpre("ОШИБКА добавления пары в массив", __LINE__);
				}else{ //pre(csv, __LINE__); //mpre("Пара key=" + key+" val=" +val, __LINE__);
				}} return false; }()){ mpre("ОШИБКА устанвоки пар заголовок:значение", __LINE__);
			}else if(CSV.insert(make_pair(CSV.size(), csv)); CSV.empty()){ mpre("ОШИБКА добавления новой строки", __LINE__);
			}else{ //pre(csv, __LINE__); //mpre("Загружаемая строка "+ line, __LINE__);
			}
		} return CSV; }(); CSV.empty()){ mpre("ОШИБКА обработки файла", __LINE__);
	}else if(nlohmann::json JSON = CSV; false){ mpre("ОШИБКА обьявления данных json", __LINE__);
	}else if(int pos = file_name.find("."); false){ mpre("ОШИБКА нахождения позиции точки в имени файла", __LINE__);
	}else if(std::string file_dir = (std::string::npos == pos ? file_name : file_name.substr(0, pos)); file_dir.empty()){ mpre("ОШИБКА Директория для записи", __LINE__);
	}else if(std::string MNIST = [&](std::string MNIST = "", int lines = 0){ for(auto csv_itr:CSV){ // Формирование файла csv
		if(std::map<std::string, std::string> csv = csv_itr.second; csv.empty()){ pre("ОШИБКА получения данных строки");
		}else if(nlohmann::json json; false){ pre("ОШИБКА загрузки в json");
		}else if(json["itog"] = {{"label", csv["label"]}}; json.empty()){ pre("Загрузка итога");
		}else if(csv.erase("label"); json.empty()){ pre("Удаление итога из данных");
		}else if(json["dano"] = csv; json.empty()){ pre("Загрузка исходников");
		}else if(std::string mnist = json.dump(); mnist.empty()){ pre("Установка в строку");
		}else if(MNIST += (MNIST.empty() ? "\t" : ",\n\t") +(mnist); MNIST.empty()){ pre("ОШИБКА добавления в список");
		}else if([&](){ // Записываем в файл
			if(lines += 1; !lines){ pre("Количество линий");
			//}else if((lines == CSV.size()) && (!line && !(lines%line))){
			}else if([&](bool pass = false){ // Пропуск записи
				if(lines == CSV.size()){ //mpre("Последняя строка", __LINE__);
				}else if(0 == line){ pass = true; //mpre("Не указан делитель", __LINE__);
				}else if(0 != lines%line){ pass = true; //mpre("Совпадение делителя", __LINE__);
				}else{ //mpre("ОШИБКА расчета пропуска " +(pass ? "Пропускаем" : "Записываем в файл"), __LINE__);
				} return pass; }()){ //mpre("Условие пропуска записи файла " +std::to_string(lines) + " size " +std::to_string(CSV.size()), __LINE__);
			//}else if(true){ mpre("Записываем в файл "+ std::to_string(lines)+ " разбивка " +std::to_string(line) + " " +std::to_string(lines%line), __LINE__);
			}else if(std::string num = (0 < line ? std::to_string(lines/line) : ""); false){ mpre("ОШИБКА расчета номера файла", __LINE__);
			}else if(std::string filename = file_dir +(num.empty() ? "" : "/"+ num) +".json"; filename.empty()){ pre("ОШИБКА составления имени файла");
			}else if(std::ofstream fin(filename); !fin){ mpre("ОШИБКА Записи в файл "+ filename, __LINE__);
			}else if(fin << "[\n" << MNIST << "\n]\n"; false){ pre("Запись");
			}else if(fin.close(); false){ pre("ОШИБКА закрытия файла");
			}else if(MNIST.clear(); false){ pre("Обнуляем список");
			}else{ mpre("Записываем в файл " +filename, __LINE__); //std::cout << "[\n" << MNIST << "\n]";
			} return false; }()){ pre("Выгрузка данных");
		}else{
		}} return MNIST; }(); !MNIST.empty()){ pre("ОШИБКА записи данных в файл");
	}else{ mpre("Количество CSV строк в файле "+ std::to_string(CSV.size()), __LINE__);
	}
}

