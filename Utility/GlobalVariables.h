#pragma once

#include "Vector3.h"
#include <variant>
#include <map>
#include <string>

class GlobalVariables {
public:
	void CreateGroup(const std::string& groupName);
	void Update();

	void SaveFile(const std::string& groupName);
	void LoadFile(const std::string& groupName);
	void LoadFiles();

	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, Vector3& value);

	static GlobalVariables* GetInstance();

	bool GetIsSaved() { return isSave_; }

	int32_t GetIntValue(const std::string& groupName, const std::string& key);
	float GetFloatValue(const std::string& groupName, const std::string& key);
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key);

	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, Vector3 value);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = default;
	GlobalVariables& operator=(const GlobalVariables& obj) = default;

public:
	struct Item {
		std::variant<int32_t, float, Vector3> value;
	};

	struct Group {
		std::map<std::string, Item> items;
	};

	std::map<std::string, Group> datas_;

	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	bool isSave_;
};