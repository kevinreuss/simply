#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <cctype>
#include "Variables.h"

using namespace std;

vector<Number*> variables;
vector<string> instructions;

std::regex allocation("(.*) is (.*)");
std::regex output("say (.*)");
std::regex input("ask for (.*)");

void read_simply_file(string filename) {
	string line;
	bool ignore_flag = false;

	ifstream simply_file(filename.c_str());

	while (getline(simply_file, line)) {

		if (line == "") continue;
		else if (line == "start ignore") {
			ignore_flag = true;
			continue;
		}
		else if ((ignore_flag == true) && (line == "end ignore")) {
			ignore_flag = false;
			continue;
		}
		if (ignore_flag) continue;
		instructions.push_back(line);
	}
}

void add_variable(string name, float value) {
	Number *tmp = new Number(name, value);
	variables.push_back(tmp);
}

bool interpret_instructions() {

	for (auto instruction : instructions) {
		try {
			std::smatch match;

			if (std::regex_search(instruction, match, output) && match.size() > 1) {
				
				vector<string> elements;
				string expr = match.str(1);
				int first = 0;
				int second = 0;
				for (int i = 0; i < expr.length() - 2; i++) {
					if ((expr.at(i) == 'a') && (expr.at(i + 1) == 'n') && (expr.at(i + 2) == 'd')){
						second = i;
						elements.push_back(expr.substr(first, second - first - 1));
						first = second + 4;
					}
				}
				elements.push_back(expr.substr(first, expr.length() - first));

				string output = "";

				for (auto e : elements) {
					if (e.at(0) == '\"') {
						cout << e.substr(1, e.length() - 2);
					}
					else {
						bool exist = false;
						for (int i = 0; i < variables.size(); i++) {
							if (variables[i]->name == e) {
								cout << to_string(variables[i]->value);
								exist = true;
								break;
							}
						}
						if (!exist) {
							cout << e << " is not defined" << endl;
							return false;
						}
					}
				}
				cout << endl;

				continue;
			}
			
			else if (std::regex_search(instruction, match, input) && match.size() > 1) {
				cout << match.str(1) << " ?" << endl;
				string input_string = "NULL";
				float input_float = 0.0;

				bool to_float = false;

				while (!to_float) {
					cin >> input_string;
					try {
						input_float = stof(input_string);
						to_float = true;
					}
					catch(const exception& e) {
						cout << input_string << " is not a number !" << endl;
					}
				}

				bool exist = false;

				for (int i = 0; i < variables.size(); i++) {
					if (variables[i]->name == match.str(1)) {
						variables[i]->value = input_float;
						exist = true;
						break;
					}
				}
				if (!exist) add_variable(match.str(1), input_float);
				continue;
			}

			else if (std::regex_search(instruction, match, allocation) && match.size() > 1) {
				
				vector<string> elements;
				string expr = match.str(2);
				int first = 0;
				int second = 0;
				for (int i = 0; i < expr.length(); i++) {
					if (isspace(expr.at(i))) {
						second = i;
						elements.push_back(expr.substr(first, second - first));
						first = second + 1;
					}
				}
				elements.push_back(expr.substr(first, expr.length() - first));

				float value = 0.0;

				for (auto e : elements) {
					if (e == "+") continue;

					try {
						float input_float = stof(e);
						value += input_float;
						continue;
					}
					catch(const exception& e) {}
					

					bool exist = false;
					for (auto v : variables) {
						if (e == v->name) {
							value += v->value;
							exist = true;
							break;
						}
					}
					if (!exist) {
						cout << e << " is not defined" << endl;
						return false;
					}
				}

				bool exist = false;

				for (int i = 0; i < variables.size(); i++) {
					if (variables[i]->name == match.str(1)) {
						variables[i]->value = value;
						exist = true;
						break;
					}
				}
				if (!exist) add_variable(match.str(1), value);
				continue;
			}

			cout << "ERROR : " << instruction << endl;
			return false;

		} catch (std::regex_error& e) {cout << "ERROR : REGEX" << endl; return false;}
	}
	return true;
}

int main(int argc, char* argv[]) {
	
	if (argc != 2) {
		std::cout << "usage : simply.exe filename" << endl;
		return true;
	}

	read_simply_file(argv[1]);

	cout << "START" << endl << endl;;
	if (interpret_instructions()) cout << endl << "END" << endl;

	return true;
}

