//
// Create a new RG plugin project from RGTemplate
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <regex>
#include <algorithm>

using namespace std;
using namespace experimental::filesystem;

static void replace_matches(string& str, const string& rep)
{
	static const regex rex("RGTemplate");
	str = regex_replace(str, rex, rep);
}

static void replace_matches(path& path, const string& rep)
{
	string filename(path.filename().generic_string());
	replace_matches(filename, rep);
	path.replace_filename(filename);
}

static int copy_replace(const path& target, const path& source, const string& rep)
{
	ifstream is(source);
	if (is.bad())
	{
		cerr << "Can't open '" << source << "'\n";
		return (6);
	}
	ofstream os(target);
	if (os.bad())
	{
		cerr << "Can't open '" << target << "'\n";
		return (7);
	}
	string line;
	while (getline(is, line))
	{
		if (is.bad())
		{
			cerr << "Can't read '" << source << "'\n";
			return (8);
		}
		replace_matches(line, rep);
		os << line << '\n';
		if (os.bad())
		{
			cerr << "Can't write '" << target << "'\n";
			return (9);
		}
	}
	return(0);
}

static int create_plugin(path target, path source, const string& plugin_name)
{
	const path files[] = {"ReadMe.txt", "RGTemplate.cpp",
					"RGTemplate.vcxproj", "RGTemplate.vcxproj.filters"};
	for (const auto& file : files)
	{
		source.replace_filename(file);
		if (!exists(source))
		{
			cerr << "Template file '" << source << "' doesn't exist\n";
			return(5);
		}
	}
	for (const auto& file : files)
	{
		source.replace_filename(file);
		target.replace_filename(file);
		replace_matches(target, plugin_name);
		auto err = copy_replace(target, source, plugin_name);
		if (err) return err;
		cout << "Created '" << target << "' from '" << source << "'\n";
	}
	return(0);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: rgmkplugin pluginname\n";
		return (1);
	}
	string plugin_name(argv[1]);
	if(!all_of(plugin_name.begin(), plugin_name.end(), &isalnum))
	{
		cerr << "Plugin name '" << plugin_name << '\'' << " isn't alphanumeric\n";
		return (2);
	}
	if (plugin_name.find("MQ2") != 0) plugin_name.insert(0, "MQ2");
	plugin_name[3] = toupper(static_cast<unsigned char>(plugin_name[3]));
	path template_directory("RGTemplate");
	if (!exists(template_directory))
	{
		cerr << "The project template '" << template_directory << "' doesn't exist\n";
		return (3);
	}
	path plugin_directory(plugin_name);
	if (!create_directory(plugin_directory))
	{
		cerr << "Can't create directory '" << plugin_directory << "'\n";
		return(4);
	}
	auto err = create_plugin(plugin_directory, template_directory, plugin_name);
	if (err) remove_all(plugin_directory);
	return (err);
}