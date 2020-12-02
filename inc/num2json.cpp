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

#define pre(x) if(x){ std::cerr << __LINE__ << " " << x << std::endl; }

int main(int argc, char **argv){
	if(std::function<int(std::string,int)> mpre = [&](std::string mess, int line){ // Функция вывода сообщений
		if(std::cerr << line << " " << mess << std::endl; false){ printf("%d.ОШИБКА\n", __LINE__);
		}else{ //print_r("Сообщение об ошибке", __LINE__);
		} return false; }; false){ mpre("ОШИБКА установки функции", __LINE__);
	}else if(std::function<int(std::map<std::string, std::string>,int)> dump = [&](std::map<std::string, std::string> row, int line){ // Функция вывода сообщений
		if(std::cerr << " => " << "( // __" << std::to_string(line) << "__ " << "\n"; false){ mpre("ОШИБКА отображение заголовка", __LINE__);
		}else if([&](){ for(auto itr = row.begin(); itr != row.end(); itr++){
				std::cerr << "\t[" << itr->first << "]=>" << itr->second << "," << std::endl;
			}; std::cerr << ")\n"; return false; }()){ mpre("ОШИБКА отображения массива", __LINE__);
		}else{
		} return false; }; false){ mpre("ОШИБКА установки функции", __LINE__);
	}else if(std::string file_name = [&](std::string file_name = ""){ // Получение имени файла csv из консоли
		if(1 >= argc){ mpre("Параметров консоли меньше двух", __LINE__);
		}else if(file_name = argv[1]; file_name.empty()){ mpre("ОШИБКА получения имени csv файла", __LINE__);
		}else{ //mpre("Файл csv `"+ file_name +"`", __LINE__);
		} return file_name; }(); file_name.empty()){ mpre("ОШИБКА имя файла не указано", __LINE__);
	}else if(int line = (2 < argc ? atoi(argv[2]) : 0); (0 > line)){ mpre("ОШИБКА расчета количества строк", __LINE__);
	}else if(std::ifstream fin(file_name); !fin){ mpre("ОШИБКА Подключения файла "+ file_name, __LINE__);
	}else if(std::map<std::string,std::string> titles; false){ mpre("ОШИБКА получения заголовков полей", __LINE__);
	}else if(int pos = file_name.find("."); false){ mpre("ОШИБКА нахождения позиции точки в имени файла", __LINE__);
	}else if(std::string file_dir = (std::string::npos == pos ? file_name : file_name.substr(0, pos)); file_dir.empty()){ mpre("ОШИБКА Директория для записи", __LINE__);
	}else if([&](std::string JSON = "", int loop = 2e9, int lines = 0){ while(!fin.eof() && (loop-- > 0)){ //mpre("Проверка", __LINE__);
			if(std::string ln; false){ mpre("Срока данных", __LINE__);
			}else if(std::map<std::string, std::string> itog; false){ mpre("Список итогов", __LINE__);
			}else if(std::map<std::string, std::string> dano; false){ mpre("Список исходных значений", __LINE__);
			}else if(getline(fin, ln); false){ pre("Пустая строка");
			}else if(std::istringstream iss(ln); false){ mpre("ОШИБКА получения строки", __LINE__);
			}else if(std::map<std::string, std::string> data = [&](std::map<std::string, std::string> data = {}){ std::string str; while(std::getline(iss, str, ',')){ data.insert(make_pair(std::to_string(data.size()), str)); } return data; }(); false){ mpre("ОШИБКА разделения строк", __LINE__);
			}else if([&](bool skip = false){ // Установка заголовков
				if(!titles.empty()){ //mpre("Заголовки уже установлены", __LINE__);
				}else if(titles = data; titles.empty()){ mpre("ОШИБКА установки заголовков", __LINE__);
				}else if(skip = !skip; false){ mpre("ОШИБКА установки признака пропуска", __LINE__);
				}else{ //pre(titles, __LINE__); mpre("Заголовки устанволены " +std::to_string(titles.size()), __LINE__);
				} return skip; }()){ //dump(titles, __LINE__); mpre("Установка заголовка", __LINE__);
			}else if(std::map<std::string, std::string> _csv = [&](std::map<std::string, std::string> csv = {}){ for(auto titles_itr:titles){ // Установка пар
				if(data.empty()){ //mpre("Пустая строка", __LINE__);
				}else if(std::string key = titles_itr.second; key.empty()){ mpre("Ключ пары не найден", __LINE__);
				}else if(data.end() == data.find(titles_itr.first)){ mpre("ОШИБКА в csv массиве не найдено поле заголовока " +titles_itr.first, __LINE__);
				}else if(std::string val = data.at(titles_itr.first); key.empty()){ mpre("Ключ пары не найден", __LINE__);
				}else if([&](){ // Замена переноса строки в значении
					if(int npos = key.find("\r"); (std::string::npos == npos)){ //mpre("Перенос не найден", __LINE__);
					}else if(key.erase(npos, 1); false){ mpre("ОШИБКА замены переноса в ключе", __LINE__);
					}else{ //mpre("Замена переноса в ключе `" + key+"`", __LINE__);
					} return false; }()){ mpre("ОШИБКА замены переноса строки", __LINE__);
				}else if([&](){ // Замена переноса строки в значении
					if(int npos = val.find("\r"); (std::string::npos == npos)){ //mpre("Перенос не найден", __LINE__);
					}else if(val.erase(npos, 1); false){ mpre("ОШИБКА замены переноса в значении", __LINE__);
					}else{ //mpre("Замена переноса в значении `" + val+"`", __LINE__);
					} return false; }()){ mpre("ОШИБКА замены переноса строки", __LINE__);
				}else if(csv.insert(make_pair(key, val)); csv.empty()){ mpre("ОШИБКА добавления пары в массив", __LINE__);
				}else{ //dump(csv, __LINE__); //mpre("Пара key=" + key+" val=" +val, __LINE__);
				}} return csv; }(); false){ mpre("ОШИБКА устанвоки пар заголовок:значение", __LINE__);
			}else if(std::map<std::string, std::string> csv = [&](std::map<std::string, std::string> csv = {}){ for(auto num_itr:_csv){ // Делим числа по символьно
				if(std::string key = num_itr.first; key.empty()){ mpre("ОШИБКА получения ключа числа", __LINE__);
				}else if(std::string val = num_itr.second; val.empty()){ mpre("ОШИБКА получения ключа числа", __LINE__);
				}else if(int count = val.length(); !count){ mpre("ОШИБКА расчета длинны строки", __LINE__);
				}else if([&](){ for(int i = 80; i >= 0; i--){
					if(std::string str = (val.length() > i ? val.substr(val.length()-i-1, 1) : "0"); (1 != str.length())){ mpre("ОШИБКА получения символа числа", __LINE__);
					}else if(std::string _key = key +"_" +std::to_string(i); _key.empty()){ mpre("ОШИБКА получения имени поля", __LINE__);
					}else if(itog.end() != itog.find(_key)){ mpre("ОШИБКА Уже добавлен itog " +_key, __LINE__);
					}else if(dano.end() != dano.find(_key)){ mpre("ОШИБКА Уже добавлен dano " + _key, __LINE__);
					}else if(("num3" == key ? itog.insert(make_pair(_key, str)) : dano.insert(make_pair(_key, str))); false){ mpre("ОШИБКА добавления итога", __LINE__);
					}else{ //mpre("Добавляем число " +key +" " +_key +" " +str, __LINE__);
					}}return false; }()){ mpre("ОШИБКА установки числа", __LINE__);
				}else{ //mpre("Ключ "+ key +" значение " +val +" count " +std::to_string(count), __LINE__);
				}}return csv; }(); false){ dump(csv, __LINE__); mpre("ОШИБКА разделения чисел посимвольно", __LINE__);
			}else if(nlohmann::json json; false){ pre("ОШИБКА загрузки в json");
			}else if(json["itog"] = itog; json.empty()){ mpre("ОШИБКА добавления итога", __LINE__);
			}else if(json["dano"] = dano; json.empty()){ pre("ОШИБКА Загрузка исходников");
			}else if(json["csv"] = _csv; json.empty()){ pre("ОШИБКА Загрузка исходников");
			//}else if(dump(itog, __LINE__); true){ mpre("ОШИБКА уведомления", __LINE__);
			}else if(JSON += (!JSON.empty() && !json.empty() ? ",\n" : ""); false){ mpre("ОШИБКА добавления переноса", __LINE__);
			}else if(JSON += (json.empty() ? "" : "\t" +json.dump()); false){ mpre("ОШИБКА добавления строки", __LINE__);
			}else if([&](){ // Записываем в файл
				if(lines += 1; !lines){ pre("Количество линий");
				}else if([&](bool skip = false){ // Пропуск записи
					if(fin.eof()){ //mpre("Последняя строка", __LINE__)
					}else if((0 < line) && (0 == lines%line)){ //mpre("Совпадение делителя", __LINE__);
					}else if(skip = !skip; false){ mpre("ОШИБКА изменения условия пропуска", __LINE__);
					}else{ //mpre("Расчет пропуска line=" +std::to_string(line) + " " +std::to_string(skip ? 1 : 0), __LINE__);
					} return skip; }()){ //mpre("Условие пропуска записи файла " +std::to_string(lines) + " size ", __LINE__);
				}else if(std::string num = (0 < line ? std::to_string(lines/line) : ""); false){ mpre("ОШИБКА расчета номера файла", __LINE__);
				}else if(std::string filename = file_dir +(num.empty() ? "" : "/"+ num) +".json"; filename.empty()){ pre("ОШИБКА составления имени файла");
				}else if(JSON.empty()){ //mpre("Пустые данные для выгрузки " +filename, __LINE__);
				}else if(std::ofstream fin(filename); !fin){ mpre("ОШИБКА Записи в файл "+ filename, __LINE__);
				}else if(fin << "[\n" << JSON << "\n]\n"; false){ pre("Запись");
				}else if(fin.close(); false){ pre("ОШИБКА закрытия файла");
				}else if(JSON.clear(); false){ pre("Обнуляем список");
				}else{ mpre("Записываем в файл " +filename, __LINE__); //std::cout << "[\n" << JSON << "\n]";
				} return false; }()){ pre("Выгрузка данных");
			}else{ //pre(csv, __LINE__); //mpre("Загружаемая строка "+ line, __LINE__);
			}
		} return false; }()){ mpre("ОШИБКА обработки файла", __LINE__);
	}else{ mpre("Расчет окончен", __LINE__);
	}
}

