#pragma once
#include <iostream>
#include "./my_type.hpp"

class MySsr {
private:
    std::string m_name = "";

public:
    explicit MySsr(std::string str){}
    ~MySsr() {}

    std::string ssr_htm_top(){
        std::string ret = "";
        std::string text = R"(<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>HTMX Todo App</title>
    <script src="https://unpkg.com/htmx.org@1.9.12"></script>
    <script src="https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4"></script>
  </head>
  <body class="bg-stone-100 min-h-screen flex justify-center text-stone-900 font-sans">
    <div class="max-w-3xl mx-auto p-4 py-12" hx-get="/api/todos/list" hx-trigger="load" hx-swap="outerHTML">
      Top
      <div class="flex justify-center mt-12">
        <div class="animate-spin rounded-full h-8 w-8 border-b-2 border-stone-800"></div>
      </div>
    </div>
  </body>  
</html>
)";
        ret = text;
        return ret;
    }
    
    std::string html_replace(std::string html, std::string target, std::string change_str){
      size_t pos = html.find(target);
      if (pos != std::string::npos) {
          html.replace(pos, target.length(), change_str);
      }

      return html;
    }

    std::string renderTodoList(const std::vector<Todo>& todos) {
        std::string html = R"(
            <div class="bg-white w-3xl rounded-xl shadow-sm border border-stone-200 overflow-hidden" id="todo-container">
            <div>
                <a href="/" class="font-bold ms-4" >Home</a>
                <a href="/about" class="ms-4" >[ about ]</a>
                <hr class="my-2" />
            </div>
            
            <div class="p-6 border-b border-stone-200">
                <h1 class="text-2xl font-semibold text-stone-800">Todo List</h1>
                <form class="mt-4 flex gap-2" hx-post="/api/todos/create" hx-target="#todo-container" hx-swap="outerHTML">
                <input type="text" name="title" required placeholder="Add a new task..." class="flex-1 px-4 py-2 border border-stone-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-stone-500 focus:border-transparent" />
                <button type="submit" class="px-4 py-2 bg-stone-800 text-white rounded-lg hover:bg-stone-700 transition-colors font-medium">Add</button>
                </form>
            </div>
            <ul class="divide-y divide-stone-100">
        )";

        for (const auto& todo : todos) {
            html += "          <li class=\"group flex items-center justify-between p-4 hover:bg-stone-50 cursor-pointer transition-colors\" \n";
            html += "              hx-get=\"/api/todos/get/" + std::to_string(todo.id) + "\" \n";
            html += "              hx-target=\"#dialog-container\" \n";
            html += "              hx-swap=\"innerHTML\">\n";
            html += "            <div class=\"flex items-center gap-3\">\n";
            html += "              <span class=\"text-stone-800 font-medium";
            html += "\">" + todo.title + "</span>\n";
            html += "            </div>\n";
            html += "            <form class=\"mt-4 flex gap-2\" hx-post=\"/api/todos/delete\" hx-target=\"#todo-container\" hx-swap=\"outerHTML\">\n";
            html += "              <input type=\"hidden\" name=\"id\"  value=\"" + std::to_string(todo.id) + "\" />\n";
            html += R"(              <button class=\"text-stone-400 hover:text-red-500 opacity-0 group-hover:opacity-100 transition-opacity p-2\"
                        onclick=\"event.stopPropagation()\">Delete
                </button>
                </form>
            </li>
            )";
        }

            if (todos.empty()) {
                html += "        <li class=\"p-8 text-center text-stone-500\">No tasks yet. Add one above!</li>\n";
            }

            html += R"(      </ul>
            <div id="dialog-container"></div>
            </div>
        )";

        return html;
    }


    std::string renderDialog(const Todo todo) {
      std::string ret = "";

      std::string html = R"HTML(
      <div class="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/50" id="todo-modal" onclick="this.remove()">
        <div class="bg-white rounded-xl shadow-xl w-full max-w-md overflow-hidden" onclick="event.stopPropagation()">

          <div class="p-6">
            <div class="flex justify-between items-start mb-4">
              <h2 class="text-xl font-semibold text-stone-800">{replace_str1}</h2>
              <button class="text-stone-400 hover:text-stone-600 transition-colors" onclick="document.getElementById('todo-modal').remove()">
                <svg xmlns="http://www.w3.org/2000/svg" class="h-6 w-6" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            </div>
            
            <form hx-put="/api/todos/${todo.id}" hx-target="#todo-container" hx-swap="outerHTML">
              
              <div class="mt-6 flex justify-end gap-3">
                <button type="button" class="px-4 py-2 text-stone-700 hover:bg-stone-100 rounded-lg transition-colors font-medium" onclick="document.getElementById('todo-modal').remove()">Cancel</button>
              </div>
            </form>

          </div>
        </div>
      </div>      
      )HTML";
      std::string target1 = "{replace_str1}";
      std::string target2 = "{replace_str2}";
      html = html_replace(html , target1 , todo.title);
      html = html_replace(html , target2 , std::to_string(todo.id));

      ret = html;
      return ret;
    }
};
