#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "nlohmann/json.hpp"

#include "my_db.hpp"
#include "my_ssr.hpp"

using json = nlohmann::json;

const std::string DB_PATH = "todo.db";

struct TodoData {
    int max_id;
    std::vector<Todo> items;
};

class MyTodo {
private:
    std::string m_name;

    public:
    explicit MyTodo(std::string str){}

    ~MyTodo() {}
    
    void todo_add_handler(std::string input_str){
        try{
            MyDb db_helper(DB_PATH);
            db_helper.add(input_str);
        } catch (const std::exception& e) {
            std::cout << "Error , main" << std::endl;
        }  
    }

    std::string todo_list_handler(){
        std::string ret = "";
        try{

            MyDb db_helper(DB_PATH);
            auto todos = db_helper.list("all");
            ret = db_helper.get_list_json(todos);   
            //std::cout << "todo_list_handler.ret=" << ret << std::endl;         
            return ret;
        } catch (const std::exception& e) {
            std::cout << "Error , main" << std::endl;
            return ret;
        }  
    }


    std::string todo_list_elem(){
        std::string ret = "";
        try{
            MyDb db_helper(DB_PATH);
            auto todos = db_helper.list("all");
            //std::string resp = renderTodoList(todos);
            //ret = resp;
            return ret;
        } catch (const std::exception& e) {
            std::cout << "Error , main" << std::endl;
            return ret;
        }  
    }

    void todo_delete_handler(int id){
        try{
            MyDb db_helper(DB_PATH);
            db_helper.remove(id);
        } catch (const std::exception& e) {
            std::cout << "Error , main" << std::endl;
        }  
    }


    std::string dialog_show(int id) {
        std::string ret = "";
        MyDb db_helper(DB_PATH);
        auto todos = db_helper.list("all");

        //TodoData data = load_data();
        if (todos.empty()) {
            std::cout << "TODO none\n";
            return ret;
        }
        Todo row;
        row.id = 0;
        row.title = "";
        for (const auto& item : todos) {
            if (item.id == id) {
                row.id = item.id;
                row.title = item.title;
            }
            std::cout << "[" << item.id << "] " << item.title << "\n";
        }
        MySsr sLib("");
        std::string out = sLib.renderDialog(row);
        ret = out;
        return ret;
    }       
};
