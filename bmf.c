int mpre(string mess, int line, string comment = ""){
	if(std::cerr << line << "." << mess << " " << (comment.empty() ? "" : "`") << comment << (comment.empty() ? "" : "`") << endl; false){ mpre("ОШИБКА вывода уведомления", __LINE__);
	}else if(int npos = mess.find("ОШИБКА"); (-1 == npos)){ //mpre("Вхождение ключевого слова", __LINE__);
	}else if(std::cerr << line << ".^^^ Критическое сообщение ^^^ Остановка выполнения программы" << std::endl; false){ mpre("ОШИБКА отображения информации об ошибке", __LINE__);
	}else if(int response = system("sleep 1"); true){ exit(mpre("Остановка выполнения программы", __LINE__));
	}else{ // Возвращаем 1 в случае ошибки
	}return 0;
} int mpre(string mess, string comment, int line){
	return mpre(mess, line, comment);
}

int mpre(TMs row, int line, string comment = "", string prefix = "", string key = ""){
	if(string id = [&](string id = ""){ // Получение ключа
		if("" != key){ id = key;
		}else if(row.end() != row.find("id")){ id = row["id"];
		}else{ //mpre("Ни ключ ни идентификатор не задан", __LINE__);
		}return id; }(); false){ //mpre("ОШИБКА получения ключа", __LINE__);
	}else if([&](){ // [37;41m ОШИБКА \x1b[0m
		std::cerr << prefix << id << " => " << "( // __" << to_string(line) << "__ " << comment << "\n";
		for(TMs::iterator itr = row.begin(); itr != row.end(); itr++){
			string field = (string) itr->first;
			string val = (string) itr->second;
			std::cerr << prefix << "\t[" << field << "]=>" << val << "," << endl;
		}; std::cerr << prefix << ")\n"; return false; }()){ err("Вывод массива");
	}else if(int npos = comment.find("ОШИБКА"); (-1 == npos)){ //mpre("Вхождение ключевого слова", __LINE__);
	}else if(std::cerr << line << ".^^^ Критическое сообщение ^^^ Остановка выполнения программы" << std::endl; false){ mpre("ОШИБКА отображения информации об ошибке", __LINE__);
	}else if(int response = system("sleep 1"); true){ exit(mpre("Остановка выполнения программы", __LINE__));
	}else{
	}return 1;
} int mpre(TMs row, string comment, int line){
	return mpre(row, line, comment);
}

int mpre(TMMi& TAB, int line, string comment = ""){
	std::cerr << "Array";
	if(line > 0){
		std::cerr << "__" << std::to_string(line) << "__";
	}; std::cerr << "( // " << comment << "\n";
	for(auto itr = TAB.begin(); itr != TAB.end(); itr++){
		int key = itr->first;
		TMs row = itr->second;
		mpre(row, line, comment, "\t", to_string(key));
	} std::cerr << ")\n";
	return 1;
} int mpre(TMMi TAB, string comment, int line){
	return mpre(TAB, line, comment);
}


int mpre(TMMs& TAB, int line, string comment = ""){
	std::cerr << "Array";
	if(line > 0){
		std::cerr << "__" << std::to_string(line) << "__";
	}; std::cerr << "( // " << comment << "\n";
	for(auto& itr:TAB){
		string key = itr.first;
		TMs row = itr.second;
		mpre(row, line, comment, "\t", key);
	} std::cerr << ")\n";
	return 1;
} int mpre(TMMs TAB, string comment, int line){
	return mpre(TAB, line, comment);
}


int mpre(TM3i& TABS, int line, string comment = ""){
	std::cerr << "Array";
	if(line > 0){
		std::cerr << "__" << std::to_string(line) << "__";
	}; std::cerr << "( // " << comment << "\n";
	for(auto &tab_itr:TABS){
		if(string keys = tab_itr.first; false){ mpre("ОШИБКА получения ключа очередного элемента", __LINE__);
		}else{
			std::cerr << "\tArray";
			if(line > 0){
				std::cerr << "\t" << keys << " => " ;
			}; std::cerr << "( // " << comment << "\n";
			for(auto itr = tab_itr.second.begin(); itr != tab_itr.second.end(); itr++){
				int key = itr->first;
				TMs row = itr->second;
				mpre(row, line, comment, "\t\t", to_string(key));
			} std::cerr << "\t)\n";
		}
	}; std::cerr << ")\n";
	return 1;
} int mpre(TM3i TABS, string comment, int line){
	return mpre(TABS, line, comment);
}



TMMi rb(TMMi& TAB, TMs values, bool debug = false, TMMi LIST = {}){
	for(auto &tab_itr:TAB){
		bool keep = true; int id = tab_itr.first;
		TMs row = tab_itr.second;
		for(auto &values_itr:values){
			string key, val;
			if([&](){ key = values_itr.first; return false; }()){ std::cerr << __LINE__ << " Ключ условия выборки << " << key << endl;
			}else if([&](){ val = values_itr.second; return false; }()){ std::cerr << __LINE__ << " Значения условия выборки << " << val << endl;
			}else if(row.find(key) == row.end()){ std::cerr << __LINE__ << " Поле в таблице не найдено [" << key << "]=" << val << endl; mpre(values, __LINE__); mpre(row, __LINE__); keep = false;
			}else if(val == row.find(key)->second){// std::cerr << __LINE__ << " Значение не соответствует уcловию << " << " " << it->second << ":" << values.find(key)->second << endl;
			}else{ keep = false; // std::cerr << "i=" << i << ";" << endl;
			}// std::cerr << __LINE__ << " i[" << i++ << "]={" << keys[i] << ":" << vals[i] << "}" << endl;
		};
		if(keep){// mpre(row, __LINE__);
			LIST.insert(make_pair(id, row));
		}// std::cerr << __LINE__ << " Количество элементов в выборке:" << LIST.size() << endl;
	}; return LIST;
}

int Crc32(const char *message) {
	int i, j;
	unsigned int byte, crc;
	i = 0;
	crc = 0xFFFFFFFF;
	while (message[i] != 0) {
		byte = message[i];    // Get next byte.
		crc = crc ^ byte;
		for(j = 7; j>=0; j--){
			crc = (crc >> 1) ^ (crc & 1 ? 0xEDB88320 : 0);
		} i = i + 1;
	} return ~crc;
}

TMs erb(TMMi& TAB, TMs values, bool debug = false, TMs row = {}){ // Поиск по неиднексированной таблице
	if(false){ mpre("ОШИБКА уведомления", __LINE__);
	//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	//}else if(mpre("Проверка", __LINE__); false){ mpre("ОШИБКА уведомления", __LINE__);
	}else if(TAB.empty()){ //mpre("Пустой список", __LINE__);
	}else if(TMMi LIST = rb(TAB, values); false){ std::cerr << __LINE__ << " Получение списка подходящих под условия элементов" << endl;
	}else if(LIST.size() > 1){ mpre(values, __LINE__, "Условия"); mpre(LIST, __LINE__, "Список выборки"); std::cerr << __LINE__ << " Количество элементов в выборке "+ to_string(LIST.size())+ " более одного" << endl; mpre("ОШИБКА выборки", __LINE__); // data(LIST, __LINE__); return row;
	}else if(LIST.empty()){// std::cerr << __LINE__ << " Список выборки пуст" << endl; mpre(values, __LINE__); return row;
	}else if(LIST.begin() == LIST.end()){ std::cerr << __LINE__ << " ОШИБКА выборки первого элемента из списка выборки" << endl;
	}else if(row = LIST.begin()->second; false){ mpre("ОШИБКА установки расчетного значения", __LINE__);
	}else{ //mpre("Выборка единичного значения", __LINE__);
	} return row;
}

TMs erb_insert(TM3i& TABS, string id, TMs index){ //mpre("Добавление индекса "+ id, __LINE__);
	if(TABS.end() == TABS.find("")){ //mpre("Данные не сохраняем", __LINE__);
	}else if(index.end() == index.find("id")){ mpre("ОШИБКА у элемента не найдено поле id", __LINE__);
	}else if(index.at("id") != id){ mpre("ОШИБКА значение id с полем морфа не совпадает", __LINE__);
	//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	}else if([&](){ for(auto& tabs_itr:TABS){ // Удаление индексов
			TMs fields; string values = "";
			if(string field = tabs_itr.first; (0 >= field.length())){ //mpre("Список значений", __LINE__);
			}else if(TABS.at("").end() == TABS.at("").find(atoi(id.c_str()))){ //mpre("Элемент не найден не удаляем ключи", __LINE__);
			}else if(TMs _index = TABS.at("").at(atoi(id.c_str())); _index.empty()){ mpre("ОШИБКА выборки морфа из списка", __LINE__);
			}else if([&](){ // Выборка списка полей
					string token; istringstream tokenStream(field);
					while(std::getline(tokenStream, token, ',')){
						fields[to_string(fields.size())] = token;
					}
				return (0 >= fields.size()); }()){ mpre("ОШИБКА получения списка полей", __LINE__);
			}else if(0 < values.length()){ mpre("ОШИБКА первоначальное значение списка должно быть пустым", __LINE__);
			}else if([&](){ for(auto& fields_itr:fields){ // Получение значений полей
					string _field;
					if([&](){ _field = fields_itr.second; return false; }()){ mpre("ОШИБКА получения поля", __LINE__);
					}else if(_index.end() == _index.find(_field)){ mpre("ОШИБКА Поле у морфа не найдено TODO првоерить на наличие поля", __LINE__);
					}else if([&](){ values += _index.at(_field)+ "\n"; return false; }()){ mpre("ОШИБКА добавления значения к суммарному списку", __LINE__);
					}else{ //mpre("Добавление значене поля "+ _field+ " к суммарному значению "+ value, __LINE__);
					}
				} return false; }()){ mpre("ОШИБКА получения списка значений", __LINE__);
			}else if(int values_crc32 = Crc32(values.c_str()); false){ mpre("ОШИБКА получения crc32 хеша значения", __LINE__);
			}else if(TABS.at(field).end() == TABS.at(field).find(values_crc32)){ mpre("Значение crc32 в индексе не найдено", __LINE__);
			}else if(TABS.at(field).at(values_crc32).end() == TABS.at(field).at(values_crc32).find(id)){ mpre("Значение элемента в списке crc32 не найдено", __LINE__);
			}else if(string values_md5 = md5(values); (0 >= values_md5.length())){ mpre("ОШИБКА расчета md5 строки значения", __LINE__);
			}else if(TABS.at(field).at(values_crc32).at(id) != values_md5){ mpre("ОШИБКА значение md5 существующего элемента не совпало с md5 ключа", __LINE__);
			}else if(TABS.at(field).at(values_crc32).erase(id); TABS.empty()){ mpre("ОШИБКА удаления значения из справочника", __LINE__);
			}else{ //mpre(fields, __LINE__, "Список полей "+ values); mpre("ОШИБКА Удаления индекса поле `"+ field+ "`", __LINE__);
			}
		} return false; }()){ mpre("ОШИБКА удаления индексов", __LINE__);
	}else if([&](){ for(auto& tabs_itr:TABS){ // Добавление индекса
			TMs fields; string values = "";
			if(string field = tabs_itr.first; (0 >= field.length())){ //mpre("Список значений", __LINE__);
			}else if([&](){ // Выборка списка полей
				string token; istringstream tokenStream(field);
				while(std::getline(tokenStream, token, ',')){
					fields[to_string(fields.size())] = token;
				} return (0 >= fields.size()); }()){ mpre("ОШИБКА получения списка полей", __LINE__);
			}else if(0 < values.length()){ mpre("ОШИБКА первоначальное значение списка должно быть пустым", __LINE__);
			}else if([&](){ for(auto& fields_itr:fields){ // Получение значений полей
					if(string _field = fields_itr.second; false){ mpre("ОШИБКА получения поля", __LINE__);
					}else if(index.end() == index.find(_field)){ mpre("ОШИБКА Поле у морфа не найдено TODO првоерить на наличие поля", __LINE__);
					}else if(values += index.at(_field)+ "\n"; false){ mpre("ОШИБКА добавления значения к суммарному списку", __LINE__);
					}else{ //mpre("Добавление значене поля "+ _field+ " к суммарному значению "+ value, __LINE__);
					}
				} return false; }()){ mpre("ОШИБКА получения списка значений", __LINE__);
			}else if(int values_crc32 = Crc32(values.c_str()); false){ mpre("ОШИБКА получения crc32 хеша значения", __LINE__);
			}else if([&](){ // Создание структуры crc32
				if(TABS.at(field).end() != TABS.at(field).find(values_crc32)){ //mpre("Структура уже создана", __LINE__);
				}else if(TABS.at(field).insert(make_pair(values_crc32, TMs({}))); TABS.at(field).empty()){ mpre("ОШИБКА создания crc32 структуры", __LINE__);
				}else{ //mpre("Создание структуры crc32="+ to_string(values_crc32)+ " для поля field="+ field, __LINE__);
				} return false; }()){ mpre("ОШИБКА создания crc32 структуры индексных ключей", __LINE__);
			}else if(string values_md5 = md5(values); (0 >= values_md5.length())){ mpre("ОШИБКА расчета md5 строки значения", __LINE__);
			}else if(TABS.at(field).at(values_crc32).insert(make_pair(index.at("id"), values_md5)); TABS.at(field).at(values_crc32).empty()){ mpre("ОШИБКА добавления нового значения ключа индекса", __LINE__);
			}else{ //mpre("Добавляем индекс "+ field, __LINE__);
			}
		} return false; }()){ mpre("ОШИБКА добавления индекса", __LINE__);
	}else if([&](){ // Установка морфа
		if(TABS.at("").end() != TABS.at("").find(atoi(id.c_str()))){ TABS.at("").at(atoi(id.c_str())) = index; //mpre("Изменение уже установленного "+ index.at("id"), __LINE__);
		}else if(TABS.at("").insert(make_pair(atoi(id.c_str()), index)); index.empty()){ mpre("ОШИБКА новой установки элемента", __LINE__);
		}else{ //mpre("Новая установка "+ index.at("id"), __LINE__);
		} return index.empty(); }()){ mpre("ОШИБКА изменения списка морфов", __LINE__);
	}else if([&](){ // Уведомление
		TMs tabs; string field = "dano_values_id";
		if(true){ //mpre("Пропускаем сравнение", __LINE__);
		}else if(index.end() == index.find(field)){ //mpre("Новый элемент не содержит "+ field, __LINE__);
		}else if([&](){ // Старый элемент
			if(TABS.at("").end() == TABS.at("").find(atoi(id.c_str()))){ //mpre("Список не содержит элемента редактирвоания "+ id, __LINE__);
			}else if([&](){ tabs = TABS.at("").at(atoi(id.c_str())); return tabs.empty(); }()){ mpre("ОШИБКА выборки локального элемента", __LINE__);
			}else{
			} return false; }()){ mpre("ОШИБКА выборки старого элемента", __LINE__);
		}else if([&](){ mpre("Изменение элемента "+ id, __LINE__); mpre(tabs, __LINE__, "Старый"); mpre(index, __LINE__, "Новый"); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
		}else if(tabs.end() == tabs.find(field)){ //mpre("Новый элемент не содержит "+ field, __LINE__);
		}else if(tabs.at(field) == index.at(field)){ //mpre("Значения поля совпадают "+ field, __LINE__);
		}else{ mpre("ОШИБКА изменения поля не совпадают "+ field, __LINE__);
		} return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
	}else{ //mpre(index, __LINE__, "Редкатирование [["+ id+ "]]"); //mpre(TABS, __LINE__, "Добавление записи в таблицу"); //mpre("ОШИБКА добавления строки к индексной таблице", __LINE__);
	} return index;
}

TMMi rb(TM3i& TABS, TMs values, bool debug = false, TMMi LIST = {}){
	if(values.empty()){ mpre("ОШИБКА запрос с пустыми результатами", __LINE__);
	}else if(TABS.end() == TABS.find("")){ //mpre("Данные отсутствуют", __LINE__);
	}else if(TABS.at("").empty()){ //mpre("Пропускаем пустые массивы", __LINE__);
	}else if(string fields = ""; 0 < fields.length()){ mpre("ОШИБКА значение полня должно быть пустым", __LINE__);
	//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
	}else if([&](){ for(auto& values_itr:values){ // Поле индекса
			if(string field = values_itr.first; (0 >= field.length())){ mpre("ОШИБКА получения имени поля", __LINE__);
			}else if(fields += (0 >= fields.length() ? "" : ",")+ field; (0 >= fields.length())){ mpre("ОШИБКА инкремента имени поля к строке полей", __LINE__);
			}else{ //mpre("Формирование списка полей "+ fields, __LINE__);
			}
		} return (0 >= fields.length()); }()){ mpre("ОШИБКА получения поля индекса", __LINE__);
	}else if([&](){ // Выборка по идентификатору
		if(1 < values.size()){ //mpre("Индексов меньше одного", __LINE__);
		}else if("id" != values.begin()->first){ //mpre("Не идентификатор", __LINE__);
		}else{ mpre("ОШИБКА выборки по идентификатору", __LINE__);
		} return false; }()){ mpre("ОШИБКА выборки по идентификатору", __LINE__);
	}else if([&](){ // Создание индекса
		if([&](){ // Пропуск проверки индекса по индектификатору
			if(1 > values.size()){ mpre("Индексов меньше одного", __LINE__);
			}else if("id" != values.begin()->first){ //mpre("Не идентификатор", __LINE__);
			}else if(!LIST.empty()){ mpre("ОШИБКА список уже не пуст", __LINE__);
			}else{ //mpre(values, __LINE__, "Запрос"); mpre("Пропускаем создание индексных полей", __LINE__);
				return true;
			} return false; }()){ //mpre("Запрос по индентификатору", __LINE__);
		}else if([&](){ TABS.insert(make_pair(fields, TMMi({}))); return (TABS.end() == TABS.find(fields)); }()){ mpre("ОШИБКА добавления нового ключа", __LINE__);
		}else if([&](){ // Индексация нового ключа
			if(TABS.end() == TABS.find(fields)){ //mpre("Ключ поиска не создан", __LINE__);
			}else if(TABS.at(fields).empty()){ //mpre("Ключ поиска пуст", __LINE__);
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
			} return false; }()){ mpre("ОШИБКА создание содержимого индекса", __LINE__);
		}else{ //mpre("Список полей `"+ fields, __LINE__); mpre(values, __LINE__, "Список полей"); mpre(TABS, __LINE__, "Массив"); mpre("ОШИБКА добавления списка полей", __LINE__);
		} return false; }()){ mpre("ОШИБКА проверки наличия индексов", __LINE__);
	}else if([&](){ // Список результатов
		if(TABS.end() == TABS.find(fields)){ mpre("ОШИБКА индексный ключ поля не найден "+ fields, __LINE__);
		}else if(string value = ""; 0 < value.length()){ mpre("ОШИБКА первоначальное значение не пусто", __LINE__);
		}else if([&](){ for(auto& values_itr:values){ value += values_itr.second+ "\n"; }; return false; }()){ mpre("ОШИБКА получения строки значений", __LINE__);
		}else if(int value_crc32 = Crc32(value.c_str()); (0 == value_crc32)){ mpre("ОШИБКА получения crc значения поля", __LINE__);
		}else if(string value_md5 = md5(value); (0 == value_md5.length())){ mpre("ОШИБКА получения crc значения поля", __LINE__);
		//}else if([&](){ mpre(values, __LINE__, "Условия"); mpre("value_crc32="+ to_string(value_crc32), __LINE__); mpre("value_md5="+ value_md5, __LINE__); return false; }()){ mpre("ОШИБКА уведомления", __LINE__);
		}else if(TABS.at(fields).end() == TABS.at(fields).find(value_crc32)){ //mpre("Списка ключей по запросу не найден field="+ fields+ " crc32="+ to_string(value_crc32) , __LINE__);
		}else if([&](){ for(auto& index_itr:TABS.at(fields).at(value_crc32)){ // Формируем список по ключам
				if(index_itr.second != value_md5){ mpre("ОШИБКА md5 не совпал для индекса (Коллизия crc32)", __LINE__);
				}else if(string id = index_itr.first; (0 >= id.length())){ mpre("ОШИБКА нулевая длинна ключа индекса "+ id, __LINE__);
				}else if(TABS.at("").end() == TABS.at("").find(atoi(id.c_str()))){ mpre("ОШИБКА элемент по индексному ключю не найден в списке", __LINE__);
				}else if(LIST.insert(make_pair(atoi(id.c_str()), TABS.at("").at(atoi(id.c_str())))); LIST.empty()){ mpre("ОШИБКА добавления нового элемента в список результата", __LINE__);
				}else{ //mpre(TABS.at(fields).at(value_crc32), __LINE__, "Список индексных полей");
				}
			} return false; }()){ mpre("ОШИБКА формирования совпадений по crc32/md5", __LINE__);
		}else{ //mpre(LIST, __LINE__, "Результат совпадения с индексами");
		} return false; }()){ mpre("ОШИБКА формирования списка результатов", __LINE__);
	}else{ //mpre(LIST, __LINE__, "Результат выборки"); //mpre("ОШИБКА формирования списка индексов", __LINE__);
	}return LIST;
}

TMs erb(TM3i& TABS, TMs values, bool debug = false, TMs line = {}){ // Поиск по индексированной таблице
	if(false){ mpre("ОШИБКА уведомления", __LINE__);
	//}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	}else if(TABS.empty()){ //mpre("Справочкник пуст", __LINE__);
	}else if([&](){ // Получение значения выборки по идентификатору
			if(1 != values.size()){ //mpre("ОШИБКА полей для поиска больше одного", __LINE__);
			}else if(values.end() == values.find("id")){ //mpre("Выборка только по id", __LINE__);
			}else if(string id = values.at("id"); (0 >= id.length())){ //mpre("Значение id в выборке не установлено", __LINE__);
			}else if(TABS.at("").end() == TABS.at("").find(atoi(id.c_str()))){ mpre(TABS.at(""), __LINE__, "Список"); mpre(values, "условия", __LINE__); mpre("ОШИБКА Запись по id не найдена "+ id, __LINE__);
			}else if(line = TABS.at("").at(atoi(id.c_str())); line.empty()){ mpre("ОШИБКА выборка по id пуста", __LINE__);
			}else{ //mpre("ОШИБКА Выборка по идентификатору "+ id, __LINE__);
			} return false;
		}()){ mpre("ОШИБКА получения значения выборки по id", __LINE__);
	}else if([&](){// mpre(values, __LINE__, "Запрос");
		if([&](){ // Проверка на запрос по id
			if(1 < values.size()){ //mpre("Больше одного значения", __LINE__);
			}else if(values.end() == values.find("id")){ //mpre("Нет значения id", __LINE__);
			}else{ return true;
			} return false; }()){ //mpre(values, __LINE__, "Значения"); mpre("Не выбираем по нескольким полям", __LINE__);
		}else if(TMMi LINE = rb(TABS, values); false){ mpre("ОШИБКА выборки списка полей", __LINE__);
		}else if(1 < LINE.size()){ mpre(values, __LINE__, "Условия выборки"); mpre(LINE, __LINE__, "Результат"); mpre("ОШИБКА список выбранных полей больше одного", __LINE__);
		}else if(line = (1 > LINE.size() ? line : LINE.begin()->second); false){ mpre("ОШИБКА выборки первого результата", __LINE__);
		}else{ //mpre(values, __LINE__, "Список требований"); mpre("Выборка erb", __LINE__);
		} return false; }()){ mpre("ОШИБКА выборки не по идентификатору", __LINE__);
	}else{ //mpre(values, __LINE__, "Значения"); mpre(line, __LINE__, "Результат"); mpre(TABS, __LINE__, "Список"); //mpre("ОШИБКА выборка из индексированной таблицы", __LINE__);
	}return line;
}
