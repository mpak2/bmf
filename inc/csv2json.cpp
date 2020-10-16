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
	}else if(std::string str = [&](std::string str = ""){
		if(2 >= argc){ mpre("ОШИБКА укажите имя поля итога", __LINE__);
		}else if(str = argv[2]; str.empty()){ mpre("ОШИБКА выборки имени поля итога", __LINE__);
		}else{ //mpre("Строка итога "+ str, __LINE__);
		} return str; }(); str.empty()){ mpre("ОШИБКА строка итога не указана", __LINE__);
	}else if(std::map<std::string, std::string> itog = [&](std::string str, std::map<std::string, std::string> itog = {}, int npos = 0){ do{ // Список итогов
			if(npos = str.find(","); false){ mpre("ОШИБКА получения позиии запятой в строке", __LINE__);
			}else if(std::string field = (std::string::npos == npos ? str : str.substr(0, npos)); field.empty()){ mpre("ОШИБКА получения части строки str=" +str, __LINE__);
			}else if(str = str.substr(npos+1, str.length()); false){ mpre("ОШИБКА удаления поля из строки", __LINE__);
			}else if(itog.insert(make_pair(field, field)); itog.empty()){ mpre("ОШИБКА добавления поля в список", __LINE__);
			}else{ //mpre("Поле итога field="+ field+ " str=" +str, __LINE__);
			}
		}while(std::string::npos != npos); return itog; }(str); itog.empty()){ mpre("ОШИБКА список итогов не сформирован", __LINE__);
	}else if(int line = (3 < argc ? atoi(argv[3]) : 0); (0 > line)){ mpre("ОШИБКА расчета количества строк", __LINE__);
	}else if(std::string hide = [&](std::string hide = ""){
		if(4 >= argc){ //mpre("Не указано поле скрытых полей", __LINE__);
		}else if(hide = argv[4]; hide.empty()){ mpre("ОШИБКА выборки имени поля итога", __LINE__);
		}else{ //mpre("Строка итога "+ str, __LINE__);
		} return hide; }(); false){ mpre("ОШИБКА строка скрытых полей не указана", __LINE__);
	}else if(mpre("Скрытые поля " +hide, __LINE__); false){ mpre("ОШИБКА", __LINE__);
	}else if(std::map<std::string, std::string> hidden = [&](std::string hide, std::map<std::string, std::string> hidden = {}, int npos = 0, bool skip = true){ do{ // Список итогов
			if(hide.empty()){ //mpre("Не указан список скрытых полей", __LINE__);
			}else if(npos = hide.find(","); false){ mpre("ОШИБКА получения позиии запятой в строке", __LINE__);
			}else if(std::string field = (std::string::npos == npos ? hide : hide.substr(0, npos)); field.empty()){ mpre("ОШИБКА получения части строки str=" +str, __LINE__);
			}else if(hide = hide.substr(npos+1, hide.length()); false){ mpre("ОШИБКА удаления поля из строки", __LINE__);
			}else if(hidden.insert(make_pair(field, field)); hidden.empty()){ mpre("ОШИБКА добавления поля в список", __LINE__);
			}else if(skip = !skip; false){ mpre("ОШИБКА Признак продолжения", __LINE__);
			}else{//mpre("Поле итога field="+ field+ " hide=" +str, __LINE__);
			}
		}while(!(skip = !skip)); return hidden; }(hide); false){ mpre("ОШИБКА список скрытых полей не сформирован", __LINE__);
	//}else if(dump(hidden, __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
	}else if(mpre("Файл CSV `" + file_name+ "` Итоговые поля `" +str +"` Разбить по строкам " +std::to_string(line) +" Скрыть поля `" +hide +"`", __LINE__)){ mpre("ОШИБКА уведомления", __LINE__);
	}else if(std::ifstream fin(file_name); !fin){ mpre("ОШИБКА Подключения файла "+ file_name, __LINE__);
	}else if(std::map<std::string,std::string> titles; false){ mpre("ОШИБКА получения заголовков полей", __LINE__);
	}else if(int pos = file_name.find("."); false){ mpre("ОШИБКА нахождения позиции точки в имени файла", __LINE__);
	}else if(std::string file_dir = (std::string::npos == pos ? file_name : file_name.substr(0, pos)); file_dir.empty()){ mpre("ОШИБКА Директория для записи", __LINE__);
	}else if([&](std::string MNIST = "", int loop = 2e9, int lines = 0){ while(!fin.eof() && (loop-- > 0)){ //mpre("Проверка", __LINE__);
			if(std::string ln; false){ mpre("Срока данных", __LINE__);
			}else if(getline(fin, ln); false){ pre("Пустая строка");
			}else if(std::istringstream iss(ln); false){ mpre("ОШИБКА получения строки", __LINE__);
			}else if(std::map<std::string, std::string> data = [&](std::map<std::string, std::string> data = {}){ std::string str; while(std::getline(iss, str, ',')){ data.insert(make_pair(std::to_string(data.size()), str)); } return data; }(); false){ mpre("ОШИБКА разделения строк", __LINE__);
			}else if([&](bool skip = false){ // Установка заголовков
				if(!titles.empty()){ //mpre("Заголовки уже установлены", __LINE__);
				}else if(titles = data; titles.empty()){ mpre("ОШИБКА установки заголовков", __LINE__);
				}else if(skip = !skip; false){ mpre("ОШИБКА установки признака пропуска", __LINE__);
				}else{ //pre(titles, __LINE__); mpre("Заголовки устанволены " +std::to_string(titles.size()), __LINE__);
				} return skip; }()){ //dump(titles, __LINE__); mpre("Установка заголовка", __LINE__);
			//}else if(dump(titles, __LINE__); false){ mpre("ОШИБКА", __LINE__);
			}else if(std::map<std::string, std::string> csv = [&](std::map<std::string, std::string> csv = {}){ for(auto titles_itr:titles){ // Установка пар
				if(data.empty()){ //mpre("Пустая строка", __LINE__);
				}else if(std::string key = titles_itr.second; key.empty()){ mpre("Ключ пары не найден", __LINE__);
				}else if(data.end() == data.find(titles_itr.first)){ mpre("ОШИБКА в csv массиве не найдено поле заголовока " +titles_itr.first, __LINE__);
				}else if(std::string val = data.at(titles_itr.first); key.empty()){ mpre("Ключ пары не найден", __LINE__);
				/*}else if([&](){ // Нормализация значения
					if(std::string::npos != val.find_first_not_of("-1.234567890")){ //mpre("Не число " +val, __LINE__);
					}else if(int npos = val.find("."); false){ mpre("ОШИБКА получения позиции точки", __LINE__);
					}else if(std::string::npos == npos){ //mpre("Не дробное число " +val, __LINE__);
					}else if(val = val.substr(0, npos); val.empty()){ mpre("ОШИБКА нормализации числа", __LINE__);
					}else{ //mpre("ОШИБКА Нормализация числа " +val, __LINE__);
					} return false; }()){ mpre("ОШИБКА нормализации", __LINE__);*/
				//}else if(std::replace(str.begin(), str.end(), 's', 'd'); false){ mpre("ОШИБКА замены переноса строки", __LINE__);
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
			}else if(nlohmann::json json; false){ pre("ОШИБКА загрузки в json");
			}else if([&](){ for(auto itog_itr:itog){ // Формирование списка итогов
					if(csv.empty()){ //mpre("Пустая строка", __LINE__);
					}else if(std::string field = itog_itr.second; field.empty()){ mpre("ОШИБКА получения поля итога", __LINE__);
					}else if(csv.end() == csv.find(field)){ mpre("ОШИБКА поле не найдено в csv `"+ field +"`", __LINE__);
					}else if(json["itog"][field] = csv.at(field); (json.end() == json.find("itog"))){ mpre("ОШИБКА добавления итога", __LINE__);
					}else if(csv.erase(field); csv.empty()){ pre("Удаление итога из данных");
					}else{ //mpre("Поле итога "+ field, __LINE__);
					}
				}return false; }()){ mpre("ОШИБКА добавления итоговых значений", __LINE__);
			}else if([&](){ for(auto hidden_itr:hidden){ // Удаление скрытых полей
					if(csv.empty()){ //mpre("Пустая строка", __LINE__);
					}else if(std::string field = hidden_itr.second; field.empty()){ mpre("ОШИБКА получения поля итога", __LINE__);
					}else if(csv.end() == csv.find(field)){ mpre("ОШИБКА поле `"+ field +"` не найдено в " +file_name, __LINE__);
					}else if(csv.erase(field); csv.empty()){ pre("ОШИБКА Удаление итога из данных");
					}else{ //mpre("Скрытие поля "+ field, __LINE__);
					}
				}return false; }()){ mpre("ОШИБКА добавления итоговых значений", __LINE__);
			}else if(json["dano"] = csv; json.empty()){ pre("Загрузка исходников");
			}else if(std::string mnist = json.dump(); mnist.empty()){ pre("Установка в строку");
			}else if(MNIST += (MNIST.empty() ? "\t" : ",\n\t") +(mnist); MNIST.empty()){ pre("ОШИБКА добавления в список");
			}else if([&](){ // Записываем в файл
				if(lines += 1; !lines){ pre("Количество линий");
				}else if([&](bool skip = false){ // Пропуск записи
					if(fin.eof()){ //mpre("Последняя строка", __LINE__);
					}else if((0 < line) && (0 != lines%line)){ mpre("Совпадение делителя", __LINE__);
					}else if(skip = !skip; false){ mpre("ОШИБКА изменения условия пропуска", __LINE__);
					}else{ //mpre("Расчет пропуска " +std::to_string(skip ? 1 : 0), __LINE__);
					} return skip; }()){ //mpre("Условие пропуска записи файла " +std::to_string(lines) + " size ", __LINE__);
				//}else if(true){ mpre("Записываем в файл "+ std::to_string(lines)+ " разбивка " +std::to_string(line) + " " +std::to_string(lines%line), __LINE__);
				}else if(std::string num = (0 < line ? std::to_string(lines/line) : ""); false){ mpre("ОШИБКА расчета номера файла", __LINE__);
				}else if(std::string filename = file_dir +(num.empty() ? "" : "/"+ num) +".json"; filename.empty()){ pre("ОШИБКА составления имени файла");
				}else if(std::ofstream fin(filename); !fin){ mpre("ОШИБКА Записи в файл "+ filename, __LINE__);
				}else if(fin << "[\n" << MNIST << "\n]\n"; false){ pre("Запись");
				}else if(fin.close(); false){ pre("ОШИБКА закрытия файла");
				}else if(MNIST.clear(); false){ pre("Обнуляем список");
				}else{ mpre("Записываем в файл " +filename, __LINE__); //std::cout << "[\n" << MNIST << "\n]";
				} return false; }()){ pre("Выгрузка данных");
			}else{ //pre(csv, __LINE__); //mpre("Загружаемая строка "+ line, __LINE__);
			}
		} return false; }()){ mpre("ОШИБКА обработки файла", __LINE__);
	//}else if(nlohmann::json JSON = CSV; false){ mpre("ОШИБКА обьявления данных json", __LINE__);
	/*}else if(std::string MNIST = [&](std::string MNIST = "", int lines = 0){ for(auto csv_itr:CSV){ // Формирование файла csv
		if(std::map<std::string, std::string> csv = csv_itr.second; csv.empty()){ pre("ОШИБКА получения данных строки");
		}else{
		}} return MNIST; }(); !MNIST.empty()){ pre("ОШИБКА записи данных в файл");*/
	}else{ mpre("Расчет окончен", __LINE__);
	}
}

