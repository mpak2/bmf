int mpre(string mess, int line, string comment = ""){
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	}else if(std::cerr << line << "." << mess << (comment.empty() ? "" : " `") << comment << (comment.empty() ? "" : "`") << endl; false){ mpre("ОШИБКА вывода уведомления", __LINE__);
	}else if(int npos = mess.find("ОШИБКА"); (-1 == npos)){ //mpre("Вхождение ключевого слова", __LINE__);
	}else if([&](){ mpre("^^^ Критическое сообщение ^^^ Остановка выполнения программы", __LINE__); exit(0); int response = system("sleep 3"); return (0 != response); }()){ std::cerr << __LINE__ << " Остановка программы" << endl; exit(1);
	}else if(int response = system("sleep 3"); (0 != response)){ std::cerr << __LINE__ << " Остановка программы" << endl; exit(1);
	}else{ //mpre("Возвращенное значение"+ to_string(response), __LINE__);
	} return 1;
} int mpre(string mess, string comment, int line){
	return mpre(mess, line, comment);
}

int mpre(TMs row, int line, string comment = "", string prefix = "", string key = ""){
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
	}else if(string num = [&](string num = ""){ // Получение ключа
			if("" != key){ num = key;
			}else if(row.end() != row.find("id")){ num = row["id"];
			}else{ //mpre("Ни ключ ни идентификатор не задан", __LINE__);
			} return num;
		}(); false){ //mpre("ОШИБКА получения ключа", __LINE__);
	}else{
		std::cerr << prefix << num << " => " << "( // __" << to_string(line) << "__ " << comment << "\n";
		for(TMs::iterator itr = row.begin(); itr != row.end(); itr++){
			string field = (string) itr->first;
			string val = (string) itr->second;
			std::cerr << prefix << "\t[" << field << "]=>" << val << "," << endl;
		}; std::cerr << prefix << ")\n";
	} return 1;
} int mpre(TMs row, string comment, int line){
	return mpre(row, line, comment);
}

int mpre(TMMi TAB, int line, string comment = ""){
	std::cerr << "Array";
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
	}else if(line > 0){
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
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
	}else if(line > 0){
		std::cerr << "__" << std::to_string(line) << "__";
	}; std::cerr << "( // " << comment << "\n";
	for(auto itr = TAB.begin(); itr != TAB.end(); itr++){
		string key = itr->first;
		TMs row = itr->second;
		mpre(row, line, comment, "\t", key);
	} std::cerr << ")\n";
	return 1;
} int mpre(TMMs TAB, string comment, int line){
	return mpre(TAB, line, comment);
}


int mpre(TM3i& TABS, int line, string comment = ""){
	std::cerr << "Array";
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
	}else if(line > 0){
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



TMMi rb(TMMi &TAB, TMs values, bool debug = false){
	std::lock_guard<std::recursive_mutex> lock(mu);
	TMMi LIST;
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

TMs erb(TMMi& TAB, TMs values, bool debug = false){ // Поиск по неиднексированной таблице
	TMs row;
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	}else if(TMMi LIST = rb(TAB, values); false){ std::cerr << __LINE__ << " Получение списка подходящих под условия элементов" << endl;
	}else if(LIST.size() > 1){ mpre(values, __LINE__, "Условия"); mpre(LIST, __LINE__, "Список выборки"); std::cerr << __LINE__ << " Количество элементов в выборке "+ to_string(LIST.size())+ " более одного" << endl; mpre("ОШИБКА выборки", __LINE__); // data(LIST, __LINE__); return row;
	}else if(LIST.empty()){// std::cerr << __LINE__ << " Список выборки пуст" << endl; mpre(values, __LINE__); return row;
	}else if(LIST.begin() == LIST.end()){ std::cerr << __LINE__ << " ОШИБКА выборки первого элемента из списка выборки" << endl;
	}else if(row = LIST.begin()->second; false){ mpre("ОШИБКА установки расчетного значения", __LINE__);
	}else{ //mpre("Выборка единичного значения", __LINE__);
	} return row;
}

TMs erb_insert(TM3i& TABS, string id, TMs index){ //mpre("Добавление индекса "+ id, __LINE__);
	if(index.end() == index.find("id")){ mpre("ОШИБКА у элемента не найдено поле id", __LINE__);
	}else if(index.at("id") != id){ mpre("ОШИБКА значение id с полем морфа не совпадает", __LINE__);
	}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	}else if([&](){ for(auto& tabs_itr:TABS){ // Удаление индексов
			TMs fields; string values = "";
			if(string field = tabs_itr.first; (0 >= field.length())){ //mpre("Список значений", __LINE__);
			}else if(TABS.at("").end() == TABS.at("").find(stoi(id))){ //mpre("Элемент не найден не удаляем ключи", __LINE__);
			}else if(TMs _index = TABS.at("").at(stoi(id)); _index.empty()){ mpre("ОШИБКА выборки морфа из списка", __LINE__);
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
		if(TABS.at("").end() != TABS.at("").find(stoi(id))){ TABS.at("").at(stoi(id)) = index; //mpre("Изменение уже установленного "+ index.at("id"), __LINE__);
		}else if(TABS.at("").insert(make_pair(stoi(id), index)); index.empty()){ mpre("ОШИБКА новой установки элемента", __LINE__);
		}else{ //mpre("Новая установка "+ index.at("id"), __LINE__);
		} return index.empty(); }()){ mpre("ОШИБКА изменения списка морфов", __LINE__);
	}else if([&](){ // Уведомление
		TMs tabs; string field = "dano_values_id";
		if(true){ //mpre("Пропускаем сравнение", __LINE__);
		}else if(index.end() == index.find(field)){ //mpre("Новый элемент не содержит "+ field, __LINE__);
		}else if([&](){ // Старый элемент
			if(TABS.at("").end() == TABS.at("").find(stoi(id))){ //mpre("Список не содержит элемента редактирвоания "+ id, __LINE__);
			}else if([&](){ tabs = TABS.at("").at(stoi(id)); return tabs.empty(); }()){ mpre("ОШИБКА выборки локального элемента", __LINE__);
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

TMMi rb(TM3i& TABS, TMs values, bool debug = false){
	TMMi LIST; string fields = ""; //clock_t t;
	if(values.empty()){ mpre("ОШИБКА запрос с пустыми результатами", __LINE__);
	}else if(TABS.at("").empty()){ //mpre("Пропускаем пустые массивы", __LINE__);
	}else if(0 < fields.length()){ mpre("ОШИБКА значение полня должно быть пустым", __LINE__);
	}else if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА блокировки", __LINE__);
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
				}else if(TABS.at("").end() == TABS.at("").find(stoi(id))){ mpre("ОШИБКА элемент по индексному ключю не найден в списке", __LINE__);
				}else if(LIST.insert(make_pair(stoi(id), TABS.at("").at(stoi(id)))); LIST.empty()){ mpre("ОШИБКА добавления нового элемента в список результата", __LINE__);
				}else{ //mpre(TABS.at(fields).at(value_crc32), __LINE__, "Список индексных полей");
				}
			} return false; }()){ mpre("ОШИБКА формирования совпадений по crc32/md5", __LINE__);
		}else{ //mpre(LIST, __LINE__, "Результат совпадения с индексами");
		} return false; }()){ mpre("ОШИБКА формирования списка результатов", __LINE__);
	}else{ //mpre(LIST, __LINE__, "Результат выборки"); //mpre("ОШИБКА формирования списка индексов", __LINE__);
	} return LIST;
}

TMs erb(TM3i& TABS, TMs values, bool debug = false){ // Поиск по индексированной таблице
	TMs line;
	if(std::lock_guard<std::recursive_mutex> lock(mu); false){ mpre("ОШИБКА установки блокировки", __LINE__);
	}else if([&](){ // Получение значения выборки по идентификатору
			if(1 != values.size()){ //mpre("ОШИБКА полей для поиска больше одного", __LINE__);
			}else if(values.end() == values.find("id")){ //mpre("Выборка только по id", __LINE__);
			}else if(string id = values.at("id"); (0 >= id.length())){ //mpre("Значение id в выборке не установлено", __LINE__);
			}else if(TABS.at("").end() == TABS.at("").find(stoi(id))){ /*mpre(TABS.at(""), __LINE__, "Список");*/ mpre("ОШИБКА Запись по id не найдена "+ id, __LINE__);
			}else if(line = TABS.at("").at(stoi(id)); line.empty()){ mpre("ОШИБКА выборка по id пуста", __LINE__);
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
	} return line;
}

TMMi BMF_CALC = { // Расчеты
	{1, {{"calc_orient_id", "4"}, {"calc_pos_id", "1"}, {"calc_val_id", "1"}, {"id", "1"}, {"name", "0000"}, {"time", "1504896122"}, {"val", "0"}, {"value", "0"}}},
	{2, {{"calc_orient_id", "4"}, {"calc_pos_id", "1"}, {"calc_val_id", "2"}, {"id", "2"}, {"name", "0001"}, {"time", "1504896150"}, {"val", "0"}, {"value", "1"}}},
	{3, {{"calc_orient_id", "2"}, {"calc_pos_id", "1"}, {"calc_val_id", "3"}, {"id", "3"}, {"name", "0010"}, {"time", "1504896160"}, {"val", "0"}, {"value", "2"}}},
	{4, {{"calc_orient_id", "2"}, {"calc_pos_id", "1"}, {"calc_val_id", "4"}, {"id", "4"}, {"name", "0011"}, {"time", "1504896221"}, {"val", "1"}, {"value", "3"}}},
	{5, {{"calc_orient_id", "2"}, {"calc_pos_id", "2"}, {"calc_val_id", "1"}, {"id", "5"}, {"name", "0100"}, {"time", "1504896240"}, {"val", "0"}, {"value", "4"}}},
	{6, {{"calc_orient_id", "2"}, {"calc_pos_id", "2"}, {"calc_val_id", "2"}, {"id", "6"}, {"name", "0101"}, {"time", "1504896261"}, {"val", "1"}, {"value", "5"}}},
	{7, {{"calc_orient_id", "4"}, {"calc_pos_id", "2"}, {"calc_val_id", "3"}, {"id", "7"}, {"name", "0110"}, {"time", "1504896277"}, {"val", "1"}, {"value", "6"}}},
	{8, {{"calc_orient_id", "4"}, {"calc_pos_id", "2"}, {"calc_val_id", "4"}, {"id", "8"}, {"name", "0111"}, {"time", "1504896292"}, {"val", "1"}, {"value", "7"}}},
	{9, {{"calc_orient_id", "2"}, {"calc_pos_id", "3"}, {"calc_val_id", "1"}, {"id", "9"}, {"name", "1000"}, {"time", "1504896307"}, {"val", "1"}, {"value", "8"}}},
	{10, {{"calc_orient_id", "2"}, {"calc_pos_id", "3"}, {"calc_val_id", "2"}, {"id", "10"}, {"name", "1001"}, {"time", "1504896321"}, {"val", "0"}, {"value", "9"}}},
	{11, {{"calc_orient_id", "4"}, {"calc_pos_id", "3"}, {"calc_val_id", "3"}, {"id", "11"}, {"name", "1010"}, {"time", "1504896341"}, {"val", "0"}, {"value", "10"}}},
	{12, {{"calc_orient_id", "4"}, {"calc_pos_id", "3"}, {"calc_val_id", "4"}, {"id", "12"}, {"name", "1011"}, {"time", "1504896357"}, {"val", "0"}, {"value", "11"}}},
	{13, {{"calc_orient_id", "4"}, {"calc_pos_id", "4"}, {"calc_val_id", "1"}, {"id", "13"}, {"name", "1100"}, {"time", "1504896372"}, {"val", "1"}, {"value", "12"}}},
	{14, {{"calc_orient_id", "4"}, {"calc_pos_id", "4"}, {"calc_val_id", "2"}, {"id", "14"}, {"name", "1101"}, {"time", "1504896390"}, {"val", "1"}, {"value", "13"}}},
	{15, {{"calc_orient_id", "2"}, {"calc_pos_id", "4"}, {"calc_val_id", "3"}, {"id", "15"}, {"name", "1110"}, {"time", "1504896405"}, {"val", "1"}, {"value", "14"}}},
	{16, {{"calc_orient_id", "2"}, {"calc_pos_id", "4"}, {"calc_val_id", "4"}, {"id", "16"}, {"name", "1111"}, {"time", "1504896422"}, {"val", "0"}, {"value", "15"}}},
};

TMMi BMF_CALC_POS = { // Позиции
	{0, {{"bmf-calc", "1"}, {"bmf-calc_orient", "1"}, {"bmf-calc_pos", "3"}, {"bmf-calc_val", "1"}, {"bmf_calc_pos", "4"}, {"calc-0", "0"}, {"calc-1", "1"}, {"calc_amend_id", "3"}, {"calc_id", "4"}, {"calc_orient_id", "4"}, {"calc_pos_id", "2"}, {"calc_val_id", "4"}, {"dano", "1"}, {"description", "И"}, {"fm", "({-1}&{-0})"}, {"id", "1"}, {"itog", "1"}, {"name", "00"}, {"time", "1504459453"}, {"v0", "0"}, {"v1", "0"}, {"val-0", "0"}, {"val-1", "1"}, {"value", "0"}}},
	{1, {{"bmf-calc", "8"}, {"bmf-calc_orient", "1"}, {"bmf-calc_pos", "4"}, {"bmf-calc_val", "4"}, {"bmf_calc_pos", "3"}, {"calc-0", "1"}, {"calc-1", "0"}, {"calc_amend_id", "2"}, {"calc_id", "5"}, {"calc_orient_id", "2"}, {"calc_pos_id", "1"}, {"calc_val_id", "1"}, {"dano", "0"}, {"description", "ИЛИ"}, {"fm", "({-1}|{-0})"}, {"id", "2"}, {"itog", "0"}, {"name", "01"}, {"time", "1504459481"}, {"v0", "1"}, {"v1", "0"}, {"val-0", "1"}, {"val-1", "0"}, {"value", "1"}}},
	{2, {{"bmf-calc", "12"}, {"bmf-calc_orient", "3"}, {"bmf-calc_pos", "1"}, {"bmf-calc_val", "4"}, {"bmf_calc_pos", "2"}, {"calc-0", "0"}, {"calc-1", "1"}, {"calc_amend_id", "2"}, {"calc_id", "9"}, {"calc_orient_id", "4"}, {"calc_pos_id", "4"}, {"calc_val_id", "1"}, {"dano", "0"}, {"description", "НЕ ИЛИ"}, {"fm", "!({-1}|{-0})"}, {"id", "3"}, {"itog", "1"}, {"name", "10"}, {"time", "1504459493"}, {"v0", "0"}, {"v1", "1"}, {"val-0", "1"}, {"val-1", "0"}, {"value", "2"}}},
	{3, {{"bmf-calc", "13"}, {"bmf-calc_orient", "3"}, {"bmf-calc_pos", "2"}, {"bmf-calc_val", "3"}, {"bmf_calc_pos", "1"}, {"calc-0", "1"}, {"calc-1", "0"}, {"calc_amend_id", "3"}, {"calc_id", "16"}, {"calc_orient_id", "2"}, {"calc_pos_id", "3"}, {"calc_val_id", "4"}, {"dano", "1"}, {"description", "НЕ И"}, {"fm", "!({-1}&{-0})"}, {"id", "4"}, {"itog", "0"}, {"name", "11"}, {"time", "1504459501"}, {"v0", "1"}, {"v1", "1"}, {"val-0", "0"}, {"val-1", "1"}, {"value", "3"}}},
};

TMMi BMF_CALC_VAL = { // Знаки
	{1, {{"calc_val_id", "4"}, {"id", "1"}, {"name", "00"}, {"time", "1504459633"}, {"v", "0"}, {"v0", "0"}, {"v1", "0"}, {"value", "0"}}},
	{2, {{"calc_val_id", "3"}, {"id", "2"}, {"name", "01"}, {"time", "1504459642"}, {"v", ""}, {"v0", "1"}, {"v1", "0"}, {"value", "1"}}},
	{3, {{"calc_val_id", "2"}, {"id", "3"}, {"name", "10"}, {"time", "1504459648"}, {"v", ""}, {"v0", "0"}, {"v1", "1"}, {"value", "2"}}},
	{4, {{"calc_val_id", "1"}, {"id", "4"}, {"name", "11"}, {"time", "1504459656"}, {"v", "1"}, {"v0", "1"}, {"v1", "1"}, {"value", "3"}}},
};

TMMi BMF_CALC_ORIENT = { // Направления
	{1, {{"bmf-calc_pos", "2"}, {"calc_orient_id", "3"}, {"calc_pos_id", "1"}, {"calc_val_id", "3"}, {"description", "Прямая"}, {"id", "1"}, {"name", "Верх"}, {"time", "1514131564"}}},
	{2, {{"bmf-calc_pos", "2"}, {"calc_orient_id", "4"}, {"calc_pos_id", "4"}, {"calc_val_id", "1"}, {"description", "Положительная"}, {"id", "2"}, {"name", "Право"}, {"time", "1514131572"}}},
	{3, {{"bmf-calc_pos", "3"}, {"calc_orient_id", "1"}, {"calc_pos_id", "4"}, {"calc_val_id", "2"}, {"description", "Обратная"}, {"id", "3"}, {"name", "Низ"}, {"time", "1514131576"}}},
	{4, {{"bmf-calc_pos", "3"}, {"calc_orient_id", "2"}, {"calc_pos_id", "1"}, {"calc_val_id", "4"}, {"description", "Отрицательная"}, {"id", "4"}, {"name", "Лево"}, {"time", "1514131580"}}},
};

TMMi BMF_CALC_BALANCE = { // Балансировка
	{1, {{"bmf-calc_pos", "1"}, {"calc_orient_id", "2"}, {"calc_pos_id", "2"}, {"calc_val_id", "1"}, {"id", "1"}, {"val", "0"}}},
	{2, {{"bmf-calc_pos", "3"}, {"calc_orient_id", "2"}, {"calc_pos_id", "1"}, {"calc_val_id", "2"}, {"id", "2"}, {"val", "0"}}},
	{3, {{"bmf-calc_pos", "1"}, {"calc_orient_id", "2"}, {"calc_pos_id", "3"}, {"calc_val_id", "3"}, {"id", "3"}, {"val", "0"}}},
	{4, {{"bmf-calc_pos", "3"}, {"calc_orient_id", "2"}, {"calc_pos_id", "4"}, {"calc_val_id", "4"}, {"id", "4"}, {"val", "0"}}},
	{5, {{"bmf-calc_pos", "4"}, {"calc_orient_id", "4"}, {"calc_pos_id", "3"}, {"calc_val_id", "1"}, {"id", "5"}, {"val", "1"}}},
	{6, {{"bmf-calc_pos", "4"}, {"calc_orient_id", "4"}, {"calc_pos_id", "2"}, {"calc_val_id", "2"}, {"id", "6"}, {"val", "1"}}},
	{7, {{"bmf-calc_pos", "2"}, {"calc_orient_id", "4"}, {"calc_pos_id", "4"}, {"calc_val_id", "3"}, {"id", "7"}, {"val", "1"}}},
	{8, {{"bmf-calc_pos", "2"}, {"calc_orient_id", "4"}, {"calc_pos_id", "1"}, {"calc_val_id", "4"}, {"id", "8"}, {"val", "1"}}},
};

TMMi BMF_CALC_AMEND = { // Изменения
	{1, {{"calc_amend_id", "4"}, {"field", ""}, {"id", "1"}, {"name", "Любой"}, {"time", "1504460121"}, {"title", ""}, {"v", ""}, {"v0", "1"}, {"v1", "1"}, {"value", "3"}}},
	{2, {{"calc_amend_id", "3"}, {"field", "bmf-index"}, {"id", "2"}, {"name", "Младший"}, {"time", "1504460131"}, {"title", "⇓"}, {"v", "0"}, {"v0", "1"}, {"v1", "0"}, {"value", "1"}}},
	{3, {{"calc_amend_id", "2"}, {"field", "index_id"}, {"id", "3"}, {"name", "Старший"}, {"time", "1504460136"}, {"title", "⇑"}, {"v", "1"}, {"v0", "0"}, {"v1", "1"}, {"value", "2"}}},
	{4, {{"calc_amend_id", "1"}, {"field", ""}, {"id", "4"}, {"name", "Оба"}, {"time", "1504460139"}, {"title", "⇔"}, {"v", ""}, {"v0", "0"}, {"v1", "0"}, {"value", "0"}}},
};

TMMi BMF_INDEX_TYPE = { // Тип значения
	{1, {{"hide", "1"}, {"id", "1"}, {"name", "Строка"}, {"time", "1528581629"}}},
	{2, {{"hide", "0"}, {"id", "2"}, {"name", "Список"}, {"time", "1528581635"}}},
	{3, {{"hide", "1"}, {"id", "3"}, {"name", "Шестнадцатеричное число"}, {"time", "1528581652"}}},
	{4, {{"hide", "0"}, {"id", "4"}, {"name", "Число"}, {"time", "1528581661"}}},
	{5, {{"hide", "0"}, {"id", "5"}, {"name", "Сигнал"}, {"time", "1528581675"}}},
	{6, {{"hide", "1"}, {"id", "6"}, {"name", "Диапазон"}, {"time", "1528583408"}}},
};

