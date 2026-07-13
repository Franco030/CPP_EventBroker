#include <iostream>
#include <orm/Database.hpp>
#include <orm/Operations.hpp>

#include "models/Event.hpp"
#include "models/Topic.hpp"
#include "server/TcpServer.hpp"

int main() {
  std::cout << "Starting C++ Event Broker..." << std::endl;

  try {
    // Initialize SQLite Database through our ORM
    orm::Database db("broker.db");

    // Ensure tables exist
    orm::create_table<broker::models::Topic>(db);
    orm::create_table<broker::models::Event>(db);

    // Let's create a default topic for testing if we need one
    try {
      orm::insert(db, broker::models::Topic{0, "default_topic"});
    } catch (...) {
      // Ignore if it already exists (primary key constraint)
    }

    broker::TcpServer server(8080, db);
    server.start();
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
