/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Generates source code by performing transformations on one or more
 * input files. Directory in which to write generated files is passed
 * as sole command line argument.
 */

// TODO HIGH PRIORITY Use this instead of the Tcl script for generating
// "make_currencies_inc.hpp". Incorporate compilation of this into the
// prebuild step. This is desirable because it removes a dependency on
// Tcl.

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <jewel/assert.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::string;
using std::vector;
namespace algorithm = boost::algorithm;
namespace filesystem = boost::filesystem;

int main(int argc, char** argv)
{
	// process command line arguments
	if (argc != 2)
	{
		cerr << "Wrong number of command line parameters passed to "
		     << "prebuild program."
			 << endl;
		return 1;
	}
	JEWEL_HARD_ASSERT (argc >= 2);
	string const out_directory = argv[1];

	// construct input and output objects
	vector<string> row;
	ifstream infile("currencies.csv");
	filesystem::path const outfile_path =
		out_directory / "include" / "make_currencies_inc.hpp";
	ofstream outfile(outfile_path.string());
	
	// discard first line of input (column headers)
	if (!getline(infile, line))
	{
		cerr << "Error parsing CSV file in prebuild: too few rows." << endl;
		return 1;
	}
	if (line != "Currency,Symbol,Precision")
	{
		cerr << "Unexpected input from CSV file in prebuild: unexpected "
		     << "column headers"
			 << endl;
		return 1;
	}

	// generate C++ code on the basis of CSV contents
	for (string line; getline(infile, line); )
	{
		algorithm::split(row, line, [](char c){ return c == ','; });
		string::size_type const expected_num_columns = 3;
		if (line.size() != expected_num_columns)
		{
			cerr << "Unexpected input from CSV file in prebuild: unexpected "
			     << "number of columns in record ("
				 << "expected " << expected_num_columns
				 << " but found" << line.size() ")."
				 << endl;
			return 1;
		}
		for (auto& cell: row) algorithm::trim(cell);
		string const& name = row.at(0);
		string const& abbreviation = row.at(1);
		string const& precision = row.at(2);
		outfile << "\t\tvec.push_back(make_currency(dbc, "
		        << "L\"" << name << "\""
				<< "\"" << abbreviation << "\""
				<< precision
				<< "));";
	}

	return 0;
}
