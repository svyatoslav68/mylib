/**********************************************************************
***** Определение класса подставляющего значение строковых ************
***** переменных из специального файла.                    ************
***** Имя файла - str_from_file.hpp                        ************
**********************************************************************/
#ifndef STR_FROM_FILE_HPP
#define STR_FROM_FILE_HPP

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/any.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class StrFromFile {
/*Класс позволяет получать строки из конфигурационного тестового файла.
  Файл имеет формат "Ключ:Значение" */
public:
	StrFromFile(const char *_delimiter);
	StrFromFile(const char *_name_file, const char *_delimiter);
	bool setNameFile(const char *_name_file);
	std::string getString(const char *key_str, const char *delimiter_str = ":");
	std::string getModifiedString(const char *key_str, const char *template_str, const char *replace_str, const char *delimiter = ":");
// Функции тестирования
	void printFile();
	class StrNoFound{}; // Исключение вырабатывается, когда строка не найдена в файле
	class FileNoFound{};// Исключение вырабатывается, когда файл не найден
private:
	StrFromFile();
	std::string delimiter;
	std::string name_file;
	//std::ifstream file;
};

class ValuesFromXML {
/* Класс позволяет хранить необходимые данные в конфигурационном файле, 
   имеющем формат XML. */
public:
	ValuesFromXML(const char *_name_file);
	std::string getStrSQL(const char *part, const char *section, const char *key);
	std::string getStrValue(const char *key);
	int getIntValue(const char *key);
	void putIntValue(const char *key, const int value);
	std::vector<std::string> getNamesGroup(std::string name_part);
	std::vector<std::string> getMembersGroup(std::string name_part, std::string name_group);
	std::vector<std::pair<std::string, uint8_t>> getConditionsGroup(std::string name_part, std::string name_group);
	void saveApplicationSettings(const char *place);
	void saveGroups(const char *place, const std::vector<std::tuple<std::string, std::vector<int>, std::vector<std::pair<std::string, int>>>> &groups);
	void print_settings();
private:
	ValuesFromXML();
	boost::property_tree::ptree tree; 
	//std::map<std::string, boost::any> settings_from_file;
	std::map<std::string, std::string> settings_from_file;
	std::string m_name_file;
};

class PersonsFile{
public:
	struct employee {
		explicit employee( int id_employee, int number, std::string family, std::string position):
			id_employee(id_employee), number(number), family(family), position(position) {}
		int id_employee;
		int number;
		std::string family;
		std::string position;
	};
	struct holiday {
		explicit holiday(std::string str_date_begin, int duration, int days_travel):
			str_date_begin(str_date_begin), duration(duration), days_travel(days_travel) {}
		std::string str_date_begin;
		int duration;
		int days_travel;
	};
	PersonsFile(const char *name_file);
	std::vector<employee> getPersons();
	void savePersons(std::vector<std::tuple<int, std::string, std::string>> vectorPersons);
	void saveHolidays(int numRow, std::vector<std::tuple<int, int, int>> holidays);
private:
	std::string m_name_file;
	boost::property_tree::ptree tree;
	std::vector<PersonsFile::holiday> getHolidays(int number);
};

#endif //STR_FROM_FILE_HPP
