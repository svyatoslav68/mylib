/**********************************************************************
***** Реализация класса, подставляющего значение строковых ************
***** переменных из специального файла.                    ************
***** Имя файла - str_from_file.сpp                        ************
**********************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <tuple>
#include <cstring>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/any.hpp>
#include "str_from_file.hpp"

using std::string;
//using string::size_type;

namespace pt = boost::property_tree;

StrFromFile::StrFromFile(const char *_delimiter):delimiter(string(_delimiter)){
}

StrFromFile::StrFromFile(const char *_name_file, const char *_delimiter):name_file(string(_name_file)), delimiter(string(_delimiter)) {
}

bool StrFromFile::setNameFile(const char *_name_file){
	name_file = string(_name_file);
	return true;
}

string StrFromFile::getString(const char *key_str, const char *delimiter){
	/*Ищется строка в файле name_file, содержащая построку key_str. 
	 * подстрока может содержаться в любом месте строки, не обязательно
	 * в ключе. Возвращается часть строки после delimiter */
	std::ifstream file;
	const uint16_t max_str_length = 1000;
	file.open(name_file, std::ifstream::in);
	char buf[max_str_length];
	string result;
	string search_str(key_str); 
	if (file){
		file.getline(buf, max_str_length);
		int numstr = 0;
		while (*buf){
			string source(buf);
			if (search(source.begin(), source.end(), search_str.begin(), search_str.end()) != source.end()){
				string delimiter_str(delimiter);
				auto it = search(source.begin(), source.end(), delimiter_str.begin(), delimiter_str.end());
				std::insert_iterator<string> ii(result, result.begin());
				std::copy(it+delimiter_str.size(), source.end(), ii);
				return result;
			}
			file.getline(buf, max_str_length);
			numstr++;
		}
		throw StrNoFound();
	}
	else{
		std::cout << "Файл " << name_file << " не получилось открыть." << std::endl;
		throw FileNoFound();
	}
	file.close();
	return result;
}

void StrFromFile::printFile(){
	std::ifstream file;
	file.open(name_file, std::ifstream::in);
	if (file){
		std::cout << "name_file = " << name_file << std::endl;
		char c;
		while (file.get(c)) {
			std::cout.put(c);
		}
	}
	else
		std::cout << "Файл с именем " << name_file << " открыть не удалось." << std::endl;
	file.close();
}

string StrFromFile::getModifiedString(const char *key_str, const char *template_str, const char *replace_str, const char *delimiter){
	string result = getString(key_str, delimiter);
	size_t idx;
	while ((idx = result.find(template_str)) != std::string::npos){
		result.replace(idx, std::strlen(template_str), replace_str) ;
	}
	return result;
}

ValuesFromXML::ValuesFromXML(const char *_name_file):m_name_file(string(_name_file)){
	pt::read_xml(m_name_file, tree, boost::property_tree::xml_parser::trim_whitespace);
	/* Create tree of application settings (year, unit, etc...) */
	boost::property_tree::ptree tree_settings = tree.get_child("FILE.SettingsApplication");
	BOOST_FOREACH(auto param, tree_settings){
		if(param.first=="description") continue;
		//boost::any value = string(param.second);
		//settings_from_file.insert(std::pair<std::string, boost::any>(param.first, value));
		settings_from_file.insert(std::pair<std::string, std::string>(param.first, param.second.get<std::string>("")));
	}
}

void ValuesFromXML::print_settings(){
	using boost::any_cast;
	std::cout << "setting from xml-file " << m_name_file << std::endl;
	auto it = settings_from_file.cbegin();
	while (it != settings_from_file.cend()){
		//std::cout << it->first << "=" << any_cast<string>(it->second) << std::endl;
		//std::cout << it->first << "=" << it->second << std::endl;
		it++;
	}
}

string ValuesFromXML::getStrSQL(const char *part, const char *section, const char *key){
	boost::property_tree::ptree tree_sql = tree.get_child(part);
	string result;
	char str_argument[strlen(section)+strlen(key)+strlen("query")+2];
	strcpy(str_argument, section);
	strcat(str_argument, ".");
	strcat(str_argument, key);
	strcat(str_argument, ".");
	strcat(str_argument, "query");
	//str_argument = new char(100);
	result =  tree_sql.get<string>(str_argument);
	const char * no_space = " \n\t";
	string::size_type idx_first = result.find_first_not_of(no_space); // Первый не пробельный символ строки
	string::size_type idx_last = result.find_last_not_of(no_space);
	result = result.substr(idx_first, idx_last-idx_first+1);
	//delete str_argument;
	return result;
}

string ValuesFromXML::getStrValue(const char *key){
	string result;
	const auto it = settings_from_file.find(key);
	if (it != settings_from_file.cend()){
		result = it->second;
		const char no_space[] {" \n\t"};
		const string::size_type idx_first = result.find_first_not_of(no_space); // Первый не пробельный символ строки
		if (string::npos == idx_first){ return {}; }
		const string::size_type idx_last = result.find_last_not_of(no_space);
		result = result.substr(idx_first, idx_last-idx_first+1);
		return result;
	}
	else {
		std::cerr << "Не найден параметр " << key << std::endl;
		return string("");	
	}
}
int ValuesFromXML::getIntValue(const char *key){
	const auto it = settings_from_file.find(key);
	if (it != settings_from_file.cend()){
		try{
			string result = it->second;
			const char no_space[] {" \n\t"};
			const string::size_type idx_first = result.find_first_not_of(no_space); // Первый не пробельный символ строки
			if (string::npos == idx_first){ return {}; }
			const string::size_type idx_last = result.find_last_not_of(no_space);
			result = result.substr(idx_first, idx_last-idx_first+1);
			return boost::lexical_cast<int>(result);
		}
		catch (...){
			std::cerr << "Не могу преобразовать значения параметра \"" << it->first << "\"" << it->second << " в int\n";
			std::cout << "Продолжить? (Y/n)\n";
			char x;
			std::cin.get(x);
			if (x == 'n')
				exit(1);
			else
				return -1;
		}
	}
	else {
		std::cerr << "Не найден параметр " << key << std::endl;
		return -1;
	}
}

void ValuesFromXML::putIntValue(const char *key, const int value){
	auto it = settings_from_file.find(key);
	std::ostringstream oss;
	if (it != settings_from_file.end()){
		oss << std::dec << value;
		it->second = oss.str();
	}
	else
		std::cerr << "Не найден параметр " << key << std::endl;
}

void ValuesFromXML::saveApplicationSettings(const char *place) {
	/* Saving application setting into xml-file */
	std::cout << "Состояние settings_from_file перед сохранением:\n";
	BOOST_FOREACH(auto set, settings_from_file){
		//std::cout << set.first << " = " << set.second << std::endl;
		char key[strlen(place)+strlen(set.first.c_str())+2];
		strcpy(key, place);
		strcat(key, ".");
		strcat(key, set.first.c_str());
		tree.put(key, set.second);
	}
	boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
	pt::write_xml(m_name_file, tree, std::locale(), settings);
}

std::vector<std::string> ValuesFromXML::getNamesGroup(std::string name_part){
	/* Return vector of group names, loaded from conf-file, 
	id of group is sequence number*/
	std::vector<std::string> result;
	/*result.push_back(std::make_pair<int, std::string>(1, "All"));
	result.push_back(std::make_pair<int, std::string>(2, "Leaders"));*/
	BOOST_FOREACH(auto group, tree.get_child(name_part)){
		result.push_back(group.first);
	}
	return result;
}

std::vector<std::string> ValuesFromXML::getMembersGroup(std::string name_part, std::string name_group){
	/* Return vector of members group. */
	std::vector<std::string> result;
	BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child(name_part+"."+name_group)){
		if (v.first == "member")
			result.push_back(v.second.data());
	}
	return result;
}

std::vector<std::pair<std::string, uint8_t>> ValuesFromXML::getConditionsGroup(std::string name_part, std::string name_group){
	std::vector<std::pair<std::string, uint8_t>> result;
	BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child(name_part+"."+name_group)){
		if (v.first == "condition") {
			std::string str_condition(v.second.data());
			size_t idx = str_condition.find(" ");
			int number_persons = std::stoi(str_condition.substr(idx + 1));
			std::string name_condition = str_condition.substr(0, idx);
			//std::cout << str_condition << ":" << str_condition.substr(str_condition.find(" ") + 1) << ":" << number_persons << std::endl;
			//std::cout << str_condition << "="<< name_condition <<":" << number_persons << std::endl;
			//std::pair<char, uint8_t> p = make
			result.push_back(std::make_pair(name_condition, number_persons));
		}
	}
	return result;
}

void ValuesFromXML::saveGroups(const char *place, const std::vector<std::tuple<std::string, std::vector<int>, std::vector<std::pair<std::string, int>>>> &groups) {
	/* Функция сохраняет в xml-файл названия и содержимое групп,
	указанных в векторе names */
	pt::ptree &child_tree = tree.get_child(place);
	child_tree.clear();
	BOOST_FOREACH(auto group, groups){
		std::string name_group;
		std::vector<int> members_group;
		std::vector<std::pair<std::string, int>> conditions_group;
		std::tie ( name_group, members_group, conditions_group ) = group;
		std::cout << "Saving group \"" << name_group << "\"\n";
		child_tree.put(name_group, "");
		BOOST_FOREACH(auto member, members_group){
			child_tree.add(name_group+".member", member);
		}
		BOOST_FOREACH(auto condition, conditions_group){
			child_tree.add(name_group+".condition", condition.first + " " + boost::lexical_cast<std::string>(condition.second));
		}
	}
	boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
	pt::write_xml(m_name_file, tree, std::locale(), settings);
}

/* Class for filing data of employes */
PersonsFile::PersonsFile(const char *name_file):m_name_file(name_file){
	try {
		pt::read_xml(m_name_file, tree, boost::property_tree::xml_parser::trim_whitespace);
		std::cout << "Open file:" << m_name_file << "\n";
	}
	catch (pt::xml_parser::xml_parser_error e){
		std::cout << "Exception: " << e.what() << "\n";
	}
	catch(...){
		std::cout << "Unknown exception from constructor PersonsFile\n";
	}
}

std::vector<PersonsFile::holiday> PersonsFile::getHolidays(int number){
	std::vector<PersonsFile::holiday> result;
	boost::property_tree::ptree tree_holidays;
	try {
		std::string str_key = "FILE.Persons."+boost::lexical_cast<std::string>(number)+".Holidays";
		tree_holidays = tree.get_child(str_key);
	}
	catch (pt::ptree_bad_path e){
		std::cout << "Exception: " << e.what() << "\n";
		return result;
	}
	catch(...){
		std::cout << "Unknown exception from PersonsFile::getHolidays(...)\n";
	}
	return result;
}

std::vector<PersonsFile::employee> PersonsFile::getPersons(){
	std::vector<employee> result;
	boost::property_tree::ptree tree_persons;
	try {
		tree_persons = tree.get_child("FILE.Persons");
	}
	catch (pt::ptree_bad_path e){
		std::cout << "Exception: " << e.what() << "\n";
		return result;
	}
	catch(...){
		std::cout << "Unknown exception from PersonsFile::getPersons(...)\n";
	}
	BOOST_FOREACH(pt::ptree::value_type &v, tree_persons){
		//boost::property_tree::ptree person = tree.get_child("FILE.Persons."+v.first);
		//std::cout << v.first << ":" << tree.get<std::string>("FILE.Persons."+v.first+".fio") << std::endl;
		result.push_back(employee(tree.get<int>("FILE.Persons."+v.first+".id"), boost::lexical_cast<int>(v.first), tree.get<std::string>("FILE.Persons."+v.first+".fio"), tree.get<std::string>("FILE.Persons."+v.first+".position")));
	}
	return result;
}
void PersonsFile::savePersons(std::vector<std::tuple<int, std::string, std::string>> vectorPersons) {
	bool existException;
	do {
	existException = false;
	try {
			pt::ptree &child_tree = tree.get_child("FILE.Persons");
			child_tree.clear();
			int number = 0;
			std::cout << "From PersonsFile::savePersons(...)\n";
			for (auto [ id, position, fio ] : vectorPersons){
				std::cout << "fio = " << fio.c_str() << std::endl;
				std::string name_group = boost::lexical_cast<std::string>(++number);
				child_tree.put(name_group, "");
				child_tree.add(name_group+".id", id);
				child_tree.add(name_group+".position", position);
				child_tree.add(name_group+".fio", fio);
			}
			existException = false;
		}
		catch (pt::ptree_bad_path &e){
			std::cout << "Bad path exception: " << e.what() << "\n";
			tree.put("FILE.Persons", "");
			existException = true;
		}
		try {
			boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
			pt::write_xml(m_name_file, tree, std::locale(), settings);
			existException = false;
		}
		catch (std::exception &e){
			std::cout << "Bad name file: " << e.what() << std::endl;
			m_name_file = "holidays.xml";
			existException = true;
		}
	} while (existException == true); 
		return;
}

void PersonsFile::saveHolidays(int numRow, const std::vector<holiday> &vectorHolidays)
{
	bool existException;
	existException = false;
	do {
			std::string part_name = "FILE.Persons."+boost::lexical_cast<std::string>(numRow)+".Holidays";
			try {
				pt::ptree &child_tree = tree.get_child(part_name);
				child_tree.clear();
				std::cout << "From PersonsFile::saveHolidays(...)\n";
				for (auto holiday : vectorHolidays) {
					std::cout << "Holiday from " << holiday.str_date_begin << std::endl;
					child_tree.add(part_name+".begin", holiday.str_date_begin);
				}
				existException = false;
			}
			catch (pt::ptree_bad_path &e){
				std::cout << "Bad path exception: " << e.what() << "\n";
				tree.put(part_name, "");
				existException = true;
			}
			try {
				boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
				pt::write_xml(m_name_file, tree, std::locale(), settings);
				existException = false;
			}
			catch (std::exception &e){
				std::cout << "Bad name file: " << e.what() << std::endl;
				m_name_file = "holidays.xml";
				existException = true;
			}
	} while (existException == true); 
}


