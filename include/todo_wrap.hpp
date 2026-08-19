#pragma once
#include <drogon/drogon.h>
#include <iostream>
#include "./include/my_todo.hpp"
#include "./include/my_ssr.hpp"

using namespace drogon;

class TodoWrap {
private:
    std::string m_name = "";

public:
    explicit TodoWrap(std::string str){}
    ~TodoWrap() {}

    std::string todo_create(std::string title){
      std::string ret = "";
      try{        
          Json::Value result;
          result["message"] = "user created";
          result["ret"] = "OK";
          MyTodo tLib("");
          tLib.todo_add_handler(title);
          auto items = tLib.todo_list_elem();
          return items;
      } catch (const std::exception& e) {
          std::cout << "Error , todo_create" << std::endl;
          return ret;
      }
    }

     std::string todo_delete(int id){
      std::string ret = "";
      try{        
          Json::Value result;
          result["message"] = "todo_delete";
          result["ret"] = "OK";
          MyTodo tLib("");
          tLib.todo_delete_handler(id);
          auto items = tLib.todo_list_elem();
          return items;
      } catch (const std::exception& e) {
          std::cout << "Error , delete" << std::endl;
          return ret;
      }
    }   
};
