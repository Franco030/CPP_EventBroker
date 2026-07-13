#pragma once
#include <string>
#include <orm/Schema.hpp>

namespace broker {
namespace models {

struct Topic {
    int id;
    std::string name;
};

} // namespace models
} // namespace broker

namespace orm {
template <>
struct Schema<broker::models::Topic> {
    static constexpr std::string_view table_name = "topics";
    static constexpr auto columns = std::make_tuple(
        make_column("id", &broker::models::Topic::id),
        make_column("name", &broker::models::Topic::name)
    );
};
} // namespace orm
